#include "include/client.h"

void clientloop(PriceBuffer& buffer){
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(5050);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

}