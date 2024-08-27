#include <alchemy/NetworkManager.h>
#include <alchemy/player.h>
#include <unordered_map>
#include <sstream>
#include <unordered_set>

NetworkManager& NetworkManager::getInstance() {
    static NetworkManager instance;
    return instance;
}

NetworkManager::NetworkManager() : client_addr_len(sizeof(client_addr)) {
    std::srand(static_cast<unsigned int>(std::time(0)));
}

NetworkManager::~NetworkManager() {
#ifdef _WIN32
    closesocket(sock);
    WSACleanup();
#else
    close(sock);
#endif
}

void NetworkManager::setupUDPClient() {
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed." << std::endl;
        exit(EXIT_FAILURE);
    }
#endif

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET) {
        std::cerr << "Socket creation error" << std::endl;
        exit(EXIT_FAILURE);
    }

#ifdef _WIN32
    u_long mode = 1;
    ioctlsocket(sock, FIONBIO, &mode);
#else
    int flags = fcntl(sock, F_GETFL, 0);
    fcntl(sock, F_SETFL, flags | O_NONBLOCK);
#endif

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address / Address not supported" << std::endl;
        exit(EXIT_FAILURE);
    }
}

void NetworkManager::sendChatMessage(int clientId, const char* message) {
    OutGoingPacket packet;
    packet.type = ChatMessage;
    packet.clientId = clientId;
    strncpy_s(packet.chatData.message, message, sizeof(packet.chatData.message) - 1);
    packet.chatData.message[sizeof(packet.chatData.message) - 1] = '\0';

    sendto(sock, (char*)&packet, sizeof(OutGoingPacket), 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
}

void NetworkManager::sendPlayerMovement(int clientId, float x, float y) {
    OutGoingPacket packet;
    packet.type = PlayerMovement;
    packet.clientId = clientId;
    packet.movementData.x = x;
    packet.movementData.y = y;

    sendto(sock, (char*)&packet, sizeof(OutGoingPacket), 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
}

void NetworkManager::sendHeatBeat(int clientId) {
    OutGoingPacket packet;
    packet.type = heartBeat;
    packet.clientId = clientId;

    sendto(sock, (char*)&packet, sizeof(OutGoingPacket), 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
}

bool NetworkManager::receiveData(std::unordered_map<int, Player>& players) {
    IncomingPacket incomingPacket;
    int bytesReceived = recvfrom(sock, (char*)&incomingPacket, sizeof(IncomingPacket), 0, (struct sockaddr*)&client_addr, &client_addr_len);

    if (bytesReceived > 0) {
        if (incomingPacket.type == PlayerMovement && incomingPacket.movementUpdates.numPlayers > 0) {
            std::unordered_set<int> receivedPlayerIds;

            for (int i = 0; i < incomingPacket.movementUpdates.numPlayers; ++i) {
                const PlayerPosition& playerData = incomingPacket.movementUpdates.players[i];
                int playerId = playerData.playerId;
                float x = playerData.x;
                float y = playerData.y;

                receivedPlayerIds.insert(playerId);

                auto it = players.find(playerId);
                if (it != players.end()) {
                    it->second.setPosition(glm::vec3(x, y, 0));
                }
                else {
                    glm::vec3 defaultColor(1.0f, 1.0f, 1.0f);
                    Player newPlayer(playerId, defaultColor, x, y);
                    players[playerId] = newPlayer;
                }
            }

            for (auto it = players.begin(); it != players.end(); ) {
                if (receivedPlayerIds.find(it->first) == receivedPlayerIds.end()) {
                    it = players.erase(it);
                }
                else {
                    ++it;
                }
            }

            return true;
        }
        else {
            std::cerr << "Unexpected message type or no players in the update." << std::endl;
        }
    }
    else if (bytesReceived == SOCKET_ERROR) {
        int errorCode =
#ifdef _WIN32
            WSAGetLastError();
#else
            errno;
#endif
        // std::cerr << "recvfrom failed with error code: " << errorCode << std::endl;
    }
    return false;
}
