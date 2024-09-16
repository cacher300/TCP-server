#include "EthernetTcpServerManual.h"

void setup() {
    InitServer();  // Initialize the TCP server
}

void loop() {
    ManageClients();  // Continuously manage client connections

    // Check if a client has sent a message
    char messageBuffer[MAX_PACKET_LENGTH];
    if (GetClientMessage(messageBuffer, MAX_PACKET_LENGTH)) {
        // Print the received message to the serial monitor
        ConnectorUsb.SendLine("Received message: ");
        ConnectorUsb.SendLine(messageBuffer);
    }
}
