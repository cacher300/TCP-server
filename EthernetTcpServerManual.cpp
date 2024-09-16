#include "EthernetTcpServerManual.h"

unsigned char packetReceived[MAX_PACKET_LENGTH];
char packetToSend[MAX_PACKET_LENGTH]; 
bool usingDhcp = false;

Connector *const outputLEDs[6] = {
    &ConnectorIO0, &ConnectorIO1, &ConnectorIO2, &ConnectorIO3, &ConnectorIO4, &ConnectorIO5
};

EthernetTcpServer server(PORT_NUM);
EthernetTcpClient clients[NUMBER_OF_CLIENTS];

void InitServer() {
    // Set up serial communication
    ConnectorUsb.Mode(Connector::USB_CDC);
    ConnectorUsb.Speed(9600);
    ConnectorUsb.PortOpen();
    uint32_t timeout = 5000;
    uint32_t startTime = Milliseconds();
    while (!ConnectorUsb && Milliseconds() - startTime < timeout) {
        continue;
    }

    // Set connectors IO0-IO5 as digital outputs
    for (int i = 0; i < 6; i++) {
        outputLEDs[i]->Mode(Connector::OUTPUT_DIGITAL);
    }

    // Wait until Ethernet cable is connected
    while (!EthernetMgr.PhyLinkActive()) {
        ConnectorUsb.SendLine("The Ethernet cable is unplugged...");
        Delay_ms(1000);
    }

    // Configure with DHCP or static IP
    EthernetMgr.Setup();
    if (usingDhcp) {
        if (EthernetMgr.DhcpBegin()) {
            ConnectorUsb.Send("DHCP assigned IP: ");
            ConnectorUsb.SendLine(EthernetMgr.LocalIp().StringValue());
        } else {
            ConnectorUsb.SendLine("DHCP failed!");
            while (true) {
                continue;
            }
        }
    } else {
        IpAddress ip = IpAddress(192, 168, 1, 120);
        EthernetMgr.LocalIp(ip);
        ConnectorUsb.Send("Assigned IP: ");
        ConnectorUsb.SendLine(EthernetMgr.LocalIp().StringValue());
    }
    
    // Start the TCP server
    server.Begin();
    ConnectorUsb.SendLine("Server is listening for connections...");
}

void ManageClients() {
    EthernetTcpClient tempClient = server.Accept();
    if (tempClient.Connected()) {
        bool newClient = true;
        for (int i = 0; i < NUMBER_OF_CLIENTS; i++) {
            if (!clients[i].Connected()) {
                clients[i] = tempClient;
                ConnectorUsb.Send(clients[i].RemoteIp().StringValue());
                ConnectorUsb.SendLine(" connected.");
                outputLEDs[i]->State(true);
                newClient = false;
                break;
            }
        }
        if (newClient) {
            tempClient.Send("Server full. Closing connection.");
            tempClient.Close();
        }
    }

    for (int i = 0; i < NUMBER_OF_CLIENTS; i++) {
        if (clients[i].Connected() && clients[i].BytesAvailable()) {
            sprintf(packetToSend, "Hello client %s", clients[i].RemoteIp().StringValue());
            clients[i].Send(packetToSend);
        } else if (!clients[i].Connected()) {
            outputLEDs[i]->State(false);
            clients[i].Close();
        }
    }

    EthernetMgr.Refresh();
}

bool GetClientMessage(char *messageBuffer, int bufferLength) {
    for (int i = 0; i < NUMBER_OF_CLIENTS; i++) {
        if (clients[i].Connected() && clients[i].BytesAvailable()) {
            int bytesRead = clients[i].Read((unsigned char *)messageBuffer, bufferLength - 1);
            messageBuffer[bytesRead] = '\0';  // Null-terminate the string
            return true;  // Message received
        }
    }
    return false;  // No message received
}
