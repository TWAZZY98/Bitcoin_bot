#ifndef CLIENT_H
#define CLIENT_H
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include "pricebuffer.h"

#define MAX_SIZE 64
#define PORT 5050

int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

sockaddr_in serverAddress;
serverAddress.sin_family = AF_INET;
serverAddress.sin_port = htons(5050);
serverAddress.sin_addr.s_addr = INADDR_ANY;

void clientloop(PriceBuffer& buffer);

#endif