#ifndef CLIENT_H
#define CLIENT_H
#include <cstring>
#include <iostream>
#include <string>
#include <vector>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include "pricebuffer.h"

#define MAX_SIZE 256
#define PORT 5050

struct Kline {
    std::string symbol;
    std::string timestamp;
    double open, high, low, close, volume;
};

void clientloop(PriceBuffer& buffer);

#endif