#ifndef ETHERNETTCPSERVERMANUAL_H
#define ETHERNETTCPSERVERMANUAL_H

#include "ClearCore.h"
#include "EthernetTcpServer.h"

#define PORT_NUM 8888
#define MAX_PACKET_LENGTH 100
#define NUMBER_OF_CLIENTS 6

extern bool usingDhcp;
extern unsigned char packetReceived[MAX_PACKET_LENGTH];
extern char packetToSend[MAX_PACKET_LENGTH];
extern Connector *const outputLEDs[6];

void InitServer();
void ManageClients();
bool GetClientMessage(char *messageBuffer, int bufferLength);

#endif 
