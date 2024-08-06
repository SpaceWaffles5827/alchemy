#include <alchemy/server.h>

Server::Server() {
    initializeWinSock();
    createSocket();
    bindSocket();
}

Server::~Server() {
    if (receiverThread.joinable()) {
        receiverThread.join();
    }
    closesocket(serverSocket);
    WSACleanup();
}

void Server::run() {
    receiverThread = std::thread(&Server::receiveData, this);

    auto previousTime = std::chrono::high_resolution_clock::now();
    double lag = 0.0;

    while (true) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = currentTime - previousTime;
        previousTime = currentTime;
        lag += elapsed.count();

        while (lag >= tickRate) {
            std::lock_guard<std::mutex> guard(mutex);
            sendMovementUpdates();
            lag -= tickRate;
        }
    }
}

void Server::initializeWinSock() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed." << std::endl;
        exit(1);
    }
}

void Server::createSocket() {
    serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed." << std::endl;
        WSACleanup();
        exit(1);
    }
}

void Server::bindSocket() {
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(SERVER_PORT);
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed." << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        exit(1);
    }
    std::cout << "UDP server is listening on port " << SERVER_PORT << "..." << std::endl;
}

void Server::receiveData() {
    struct sockaddr_in clientAddr;
    int clientAddrLen = sizeof(clientAddr);
    char buffer[BUFFER_SIZE] = { 0 };

    while (true) {
        IncomingPacket packet;
        int bytesReceived = recvfrom(serverSocket, (char*)&packet, sizeof(IncomingPacket), 0, (struct sockaddr*)&clientAddr, &clientAddrLen);

        if (bytesReceived == SOCKET_ERROR) {
            int errorCode = WSAGetLastError();
            if (errorCode == WSAECONNRESET) {
                handleClientDisconnect(clientAddr);
            }
            else {
                std::cerr << "recvfrom failed with error code: " << errorCode << std::endl;
            }
            continue;
        }

        std::lock_guard<std::mutex> guard(mutex);
        clients.insert(clientAddr);
        processIncomingPacket(packet, clientAddr);
    }
}

void Server::handleClientDisconnect(const sockaddr_in& clientAddr) {
    std::lock_guard<std::mutex> guard(mutex);
    clients.erase(clientAddr);
    for (auto it = playerPositions.begin(); it != playerPositions.end(); ++it) {
        if (clientAddr.sin_port == it->first) {
            playerPositions.erase(it);
            break;
        }
    }
    std::cout << "Client disconnected. Removed from the list of players." << std::endl;
}

void Server::processIncomingPacket(const IncomingPacket& packet, const sockaddr_in& clientAddr) {
    playerPositions[packet.clientId] = { packet.movementData.x, packet.movementData.y };
}

void Server::sendMovementUpdates() {
    OutgoingPacket outgoingPacket;
    outgoingPacket.type = PlayerMovementUpdates;
    outgoingPacket.movementUpdates.numPlayers = 0;

    for (const auto& [id, position] : playerPositions) {
        if (outgoingPacket.movementUpdates.numPlayers < MAX_PLAYERS) {
            outgoingPacket.movementUpdates.players[outgoingPacket.movementUpdates.numPlayers++] = { id, position.x, position.y };
        }
    }

    for (const auto& client : clients) {
        int packetSize = sizeof(MessageType) + sizeof(int) + (outgoingPacket.movementUpdates.numPlayers * sizeof(PlayerPositionAndPlayer));
        int sentBytes = sendto(serverSocket, (char*)&outgoingPacket, packetSize, 0, (struct sockaddr*)&client, sizeof(client));
        if (sentBytes == SOCKET_ERROR) {
            std::cerr << "sendto failed." << std::endl;
        }
    }
}
