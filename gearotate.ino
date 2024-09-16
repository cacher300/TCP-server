#include "EthernetTcpServerManual.h"

void setup() {
    InitServer();  
}

void loop() {
    ManageClients();


    char messageBuffer[MAX_PACKET_LENGTH];
    if (GetClientMessage(messageBuffer, MAX_PACKET_LENGTH)) {

        ConnectorUsb.SendLine("Received message: ");
        ConnectorUsb.SendLine(messageBuffer);
    }
}
