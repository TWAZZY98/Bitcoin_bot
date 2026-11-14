#include "include/client.h"
#include <arpa/inet.h>
#include <array>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <string>

// Returns the address of the Windows host when running under WSL2 (NAT mode).
// Override with: export BROKER_HOST=1.2.3.4
static std::string getHostIp() {
    if (const char* env = std::getenv("BROKER_HOST")) return env;

    std::unique_ptr<FILE, int (*)(FILE*)> pipe(
        popen("ip route show default | awk 'NR==1{print $3}'", "r"), pclose);

    std::string out;
    if (pipe) {
        std::array<char, 128> tmp{};
        while (fgets(tmp.data(), tmp.size(), pipe.get())) out += tmp.data();
    }
    while (!out.empty() && (out.back() == '\n' || out.back() == '\r')) out.pop_back();

    return out.empty() ? "127.0.0.1" : out;
}

void clientloop(PriceBuffer& buffer) {
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        perror("socket failed");
        return;
    }

    std::string host = getHostIp();

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    if (inet_pton(AF_INET, host.c_str(), &serverAddress.sin_addr) != 1) {
        std::cerr << "Invalid host address: " << host << "\n";
        close(clientSocket);
        return;
    }

    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        perror("connect failed");
        std::cerr << "Tried " << host << ":" << PORT << "\n";
        close(clientSocket);
        return;
    }
    std::cout << "connected to " << host << ":" << PORT << "\n";

    const char* message = "R";
    ssize_t sent = send(clientSocket, message, strlen(message), 0);
    std::cout << "sent R, bytes=" << sent << "\n";

    std::string pending;
    char buf[MAX_SIZE];

    while (true) {
        ssize_t n = recv(clientSocket, buf, MAX_SIZE, 0);
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
            std::cout << line << "\n";
        }
    }

    close(clientSocket);
}