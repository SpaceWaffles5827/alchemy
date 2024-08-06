#include <alchemy/server.h>
#include <chrono>
#include <iostream>
#include <stdexcept>

Server::Server() {
    try {
        initializeWinSock();
        createSocket();
        bindSocket();
    }
    catch (const std::system_error& e) {
        std::cerr << "System error during server initialization: " << e.what() << std::endl;
        throw; // Re-throw the exception after logging
    }
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
            try {
                {
                    std::lock_guard<std::mutex> guard(mutex);
                    sendMovementUpdates();
                }
                checkHeartbeats(); // Check for players who have timed out
                lag -= tickRate;
            }
            catch (const std::system_error& e) {
                std::cerr << "System error during server tick: " << e.what() << std::endl;
                // Handle the error, potentially break the loop or continue
            }
        }
    }
}

void Server::initializeWinSock() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed." << std::endl;
        throw std::system_error(WSAGetLastError(), std::system_category(), "WSAStartup failed");
    }
}

void Server::createSocket() {
    serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed." << std::endl;
        WSACleanup();
        throw std::system_error(WSAGetLastError(), std::system_category(), "Socket creation failed");
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
        throw std::system_error(WSAGetLastError(), std::system_category(), "Bind failed");
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

        try {
            processIncomingPacket(packet, clientAddr);
        }
        catch (const std::system_error& e) {
            std::cerr << "Error processing incoming packet: " << e.what() << std::endl;
            // Handle specific packet processing errors if necessary
        }
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

void Server::checkHeartbeats() {
    std::lock_guard<std::mutex> guard(mutex); // Ensure thread safety

    auto now = std::chrono::steady_clock::now();
    auto it = playerPositions.begin();

    while (it != playerPositions.end()) {
        std::chrono::duration<double> elapsed = now - it->second.lastKeepAlive;
        std::cout << "Client " << it->first << " last heartbeat was " << elapsed.count() << " seconds ago.\n";
        if (elapsed.count() > HEARTBEAT_TIMEOUT) {
            std::cout << "Client " << it->first << " timed out due to no heartbeat.\n";
            it = playerPositions.erase(it); // Remove player from the list
        }
        else {
            ++it;
        }
    }
}

void Server::processIncomingPacket(const IncomingPacket& packet, const sockaddr_in& clientAddr) {
    auto now = std::chrono::steady_clock::now();

    switch (packet.type) {
    case PlayerMovementUpdates:
        playerPositions[packet.clientId].x = packet.movementData.x;
        playerPositions[packet.clientId].y = packet.movementData.y;
        playerPositions[packet.clientId].lastKeepAlive = now;
        // std::cout << "Updated position for client " << packet.clientId
        //     << " to (" << packet.movementData.x << ", " << packet.movementData.y << ")\n";
        break;
    case heartBeat:
        playerPositions[packet.clientId].lastKeepAlive = now;
        // std::cout << "Received heartbeat from client " << packet.clientId << "\n";
        break;
    default:
        // std::cerr << "Received unknown packet type from client " << packet.clientId << "\n";
        break;
    }
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
