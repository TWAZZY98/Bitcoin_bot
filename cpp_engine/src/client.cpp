#include "include/client.h"

void clientloop(PriceBuffer& buffer){
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        perror("socket failed");
        return;
    }

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    serverAddress.sin_addr.s_addr = inet_addr("172.20.10.13");

    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        perror("connect failed");
        close(clientSocket);
        return;
    }
    std::cout << "connected\n";

    const char* message = "R";
    ssize_t sent = send(clientSocket, message, strlen(message), 0);
    std::cout << "sent R, bytes=" << sent << "\n";

    std::string pending;
    char buf[MAX_SIZE];

    while (true) {
        ssize_t n = recv(clientSocket, buf, MAX_SIZE, 0);
        std::cout << "recv returned " << n << "\n";
        if (n <= 0) {
            if (n < 0) perror("recv");
            break;
        }
        pending.append(buf, n);

        size_t nl;
        while ((nl = pending.find('\n')) != std::string::npos) {
            std::string line = pending.substr(0, nl);
            pending.erase(0, nl + 1);
            buffer.write_to_buffer(line);
            cout<< line << "\n";
        }
    }

    close(clientSocket);
}