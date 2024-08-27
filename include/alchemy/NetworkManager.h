#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <iostream>
#include <cstring>
#include <cstdlib>
#include "player.h"
#include <unordered_map>
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
    heartBeat = 3,
};

struct OutGoingPacket {
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
        struct {
            bool alive;
        } heartBeat;
    };
};

struct PlayerPosition {
    int playerId;
    float x, y;
};

struct IncomingPacket {
    MessageType type;
    union {
        struct {
            int numPlayers;
            PlayerPosition players[BUFFER_SIZE / sizeof(PlayerPosition)];
        } movementUpdates;
        struct {
            char message[BUFFER_SIZE - sizeof(MessageType)];
        } chatData;
    };
};

class NetworkManager {
public:
    static NetworkManager& getInstance();

    // Deleted functions to ensure only one instance
    NetworkManager(const NetworkManager&) = delete;
    NetworkManager& operator=(const NetworkManager&) = delete;

    void setupUDPClient();
    void sendChatMessage(int clientId, const char* message);
    void sendPlayerMovement(int clientId, float x, float y);
    void sendHeatBeat(int clientId);
    bool receiveData(std::unordered_map<int, Player>& players);

private:
    NetworkManager();  // Private constructor
    ~NetworkManager(); // Private destructor

    SOCKET sock;
    struct sockaddr_in serv_addr, client_addr;
    char buffer[BUFFER_SIZE];
    int client_addr_len;
};

#endif
