#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <ctime>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#else
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#endif

#define SERVER_PORT 8080
#define BUFFER_SIZE 256

enum MessageType {
    PlayerMovement = 0,
    PlayerAttack = 1,
    ChatMessage = 2,
};

struct Packet {
    MessageType type;
    int clientId;
    union {
        struct {
            float x, y;
        } movementData;
        struct {
            int targetId;
            int attackPower;
        } attackData;
        struct {
            char message[BUFFER_SIZE - sizeof(MessageType) - sizeof(int)];
        } chatData;
    };
};

class NetworkManager {
public:
    NetworkManager();
    ~NetworkManager();

    void setupUDPClient();
    void sendChatMessage(int clientId, const char* message);
    void sendPlayerMovement(int clientId, float x, float y);
    bool receiveData(int& receivedClientId, float& receivedXPos, float& receivedYPos);

private:
    SOCKET sock;
    struct sockaddr_in serv_addr, client_addr;
    char buffer[BUFFER_SIZE];
    int client_addr_len;
};

#endif // NETWORK_MANAGER_H
