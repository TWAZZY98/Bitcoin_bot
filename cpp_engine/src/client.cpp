#include "include/client.h"

void clientloop(PriceBuffer& buffer){

    connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
    const char* message = "R";
    send(clientSocket, message, strlen(message), 0);
    while(true){

    }
}