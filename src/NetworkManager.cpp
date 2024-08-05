#include <alchemy/networkManager.h>

#include <sstream>

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
    Packet packet;
    packet.type = ChatMessage;
    packet.clientId = clientId;
    strncpy_s(packet.chatData.message, message, sizeof(packet.chatData.message) - 1);
    packet.chatData.message[sizeof(packet.chatData.message) - 1] = '\0';

    sendto(sock, (char*)&packet, sizeof(Packet), 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
}

void NetworkManager::sendPlayerMovement(int clientId, float x, float y) {
    Packet packet;
    packet.type = PlayerMovement;
    packet.clientId = clientId;
    packet.movementData.x = x;
    packet.movementData.y = y;

    sendto(sock, (char*)&packet, sizeof(Packet), 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
}

bool NetworkManager::receiveData(int& receivedClientId, float& receivedXPos, float& receivedYPos) {
    int bytesReceived = recvfrom(sock, buffer, BUFFER_SIZE - 1, 0, (struct sockaddr*)&client_addr, &client_addr_len);
    if (bytesReceived > 0) {
        buffer[bytesReceived] = '\0';

        std::istringstream iss(buffer);
        if (!(iss >> receivedClientId >> receivedXPos >> receivedYPos)) {
            std::cerr << "Error parsing received data." << std::endl;
            return false;
        }
        return true;
    }
    return false;
}
