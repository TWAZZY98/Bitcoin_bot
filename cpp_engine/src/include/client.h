#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include "pricebuffer.h"

void clientloop(PriceBuffer& buffer);