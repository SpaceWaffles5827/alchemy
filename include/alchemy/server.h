#ifndef SERVER_H
#define SERVER_H

#include <winsock2.h>       
#include <ws2tcpip.h>         
#include <iostream>         
#include <unordered_map>      
#include <unordered_set>      
#include <string>            
#include <functional>         
#include <chrono>          
#include <thread>         
#include <mutex>             
#include <queue>               
#include <cstring>   
#include <ws2tcpip.h>
#include <chrono>

#pragma comment(lib, "Ws2_32.lib")

#define SERVER_PORT 8080
#define BUFFER_SIZE 1024

#define HEARTBEAT_TIMEOUT 5.0 // Timeout in seconds

class Server {
public:
    Server();
    ~Server();
    void run();

private:
    enum MessageType {
        PlayerMovementUpdates = 0,
        PlayerAttack = 1,
        ChatMessage = 2,
        heartBeat = 3,
    };

    struct PlayerInfo {
        float x;
        float y;
        std::chrono::steady_clock::time_point lastKeepAlive;

        PlayerInfo(float x = 0.0f, float y = 0.0f)
            : x(x), y(y), lastKeepAlive(std::chrono::steady_clock::now()) {}
    };

    struct IncomingPacket {
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
            struct
            {
                bool alive;
            } heartBeat;
        };
    };

    struct PlayerPositionAndPlayer {
        int playerId;
        float x, y;
    };

#define MAX_PLAYERS (BUFFER_SIZE - sizeof(MessageType) - sizeof(int)) / sizeof(PlayerPositionAndPlayer)

    struct OutgoingPacket {
        MessageType type;
        union {
            struct {
                int numPlayers;
                PlayerPositionAndPlayer players[MAX_PLAYERS];
            } movementUpdates;
            struct {
                char message[BUFFER_SIZE - sizeof(MessageType)];
            } chatData;
        };
    };

    struct sockaddr_in_hash {
        std::size_t operator()(const sockaddr_in& addr) const {
            std::size_t h1 = std::hash<int>()(addr.sin_family);
            std::size_t h2 = std::hash<int>()(addr.sin_port);
            std::size_t h3 = std::hash<int>()(addr.sin_addr.s_addr);
            return h1 ^ (h2 << 1) ^ (h3 << 1);
        }
    };

    struct sockaddr_in_equal {
        bool operator()(const sockaddr_in& lhs, const sockaddr_in& rhs) const {
            return lhs.sin_family == rhs.sin_family &&
                lhs.sin_port == rhs.sin_port &&
                lhs.sin_addr.s_addr == rhs.sin_addr.s_addr;
        }
    };

    void initializeWinSock();
    void createSocket();
    void bindSocket();
    void receiveData();
    void handleClientDisconnect(const sockaddr_in& clientAddr);
    void checkHeartbeats();
    void processIncomingPacket(const IncomingPacket& packet, const sockaddr_in& clientAddr);
    void sendMovementUpdates();

    SOCKET serverSocket;
    sockaddr_in serverAddr;
    std::unordered_set<sockaddr_in, sockaddr_in_hash, sockaddr_in_equal> clients;
    std::unordered_map<int, PlayerInfo> playerPositions;
    std::mutex mutex;
    std::thread receiverThread;
    const double tickRate = 1.0 / 64.0;
};

#endif // SERVER_H
