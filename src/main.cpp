#include <GLEW/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <iomanip>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cstdlib>
#include <ctime>
#include <sstream>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#endif

#include <alchemy/player.h>

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

// Vertex Shader source code
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"uniform mat4 transform;\n"
"void main()\n"
"{\n"
"   gl_Position = transform * vec4(aPos, 1.0);\n"
"}\0";

// Fragment Shader source code for the first square
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\0";

// Fragment Shader source code for the red square
const char* redFragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);\n"
"}\0";

// Function to send a chat message to the server
void sendChatMessage(SOCKET& sock, sockaddr_in& serv_addr, int clientId, const char* message) {
    Packet packet;
    packet.type = ChatMessage;
    packet.clientId = clientId;
    strncpy_s(packet.chatData.message, message, sizeof(packet.chatData.message) - 1);
    packet.chatData.message[sizeof(packet.chatData.message) - 1] = '\0';

    sendto(sock, (char*)&packet, sizeof(Packet), 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
}

// Function to handle key input, update position, and send it to the server
void processInputAndSendToServer(GLFWwindow* window, SOCKET& sock, sockaddr_in& serv_addr, int clientId, Player& player)
{
    bool positionUpdated = false;
    float speed = 0.01f; // Speed of movement
    glm::vec2 position = player.getPosition();

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        position.y += speed;
        positionUpdated = true;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        position.y -= speed;
        positionUpdated = true;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        position.x -= speed;
        positionUpdated = true;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        position.x += speed;
        positionUpdated = true;
    }

    if (positionUpdated)
    {
        player.updatePosition(position.x, position.y);

        Packet packet;
        packet.type = PlayerMovement;
        packet.clientId = clientId;
        packet.movementData.x = position.x;
        packet.movementData.y = position.y;

        sendto(sock, (char*)&packet, sizeof(Packet), 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    }
}

// Function to setup UDP connection
void setupUDPClient(SOCKET& sock, struct sockaddr_in& serv_addr)
{
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed." << std::endl;
        exit(EXIT_FAILURE);
    }
#endif

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
    {
        std::cerr << "Socket creation error" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Set the socket to non-blocking mode
#ifdef _WIN32
    u_long mode = 1;
    ioctlsocket(sock, FIONBIO, &mode);
#else
    int flags = fcntl(sock, F_GETFL, 0);
    fcntl(sock, F_SETFL, flags | O_NONBLOCK);
#endif

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        std::cerr << "Invalid address / Address not supported" << std::endl;
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char** argv)
{
    SOCKET sock;
    struct sockaddr_in serv_addr, client_addr;
    setupUDPClient(sock, serv_addr);
    char buffer[BUFFER_SIZE];
    int client_addr_len = sizeof(client_addr);

    std::srand(static_cast<unsigned int>(std::time(0)));
    int clientId = std::rand();

    Player player1(clientId, glm::vec3(1.0f, 0.5f, 0.2f)); // First square
    Player player2(clientId + 1, glm::vec3(1.0f, 0.0f, 0.0f)); // Red square

    GLFWwindow* window;

    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(800, 800, "Moving Square", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
    {
        std::cout << "Error initializing GLEW!" << std::endl;
        glfwTerminate();
        return -1;
    }

    GLfloat vertices[] = {
        -0.1f, -0.1f, 0.0f,
         0.1f, -0.1f, 0.0f,
         0.1f,  0.1f, 0.0f,
         0.1f,  0.1f, 0.0f,
        -0.1f,  0.1f, 0.0f,
        -0.1f, -0.1f, 0.0f
    };

    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    GLuint redFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(redFragmentShader, 1, &redFragmentShaderSource, NULL);
    glCompileShader(redFragmentShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    GLuint redShaderProgram = glCreateProgram();
    glAttachShader(redShaderProgram, vertexShader);
    glAttachShader(redShaderProgram, redFragmentShader);
    glLinkProgram(redShaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glDeleteShader(redFragmentShader);

    // Game loop
    const double tickRate = 1.0 / 64.0; // 64Hz
    double previousTime = glfwGetTime();
    double lag = 0.0;

    while (!glfwWindowShouldClose(window))
    {
        double currentTime = glfwGetTime();
        double elapsed = currentTime - previousTime;
        previousTime = currentTime;
        lag += elapsed;

        // Process input and update state at a fixed tick rate
        while (lag >= tickRate) {
            processInputAndSendToServer(window, sock, serv_addr, clientId, player1);
            lag -= tickRate;
        }

        // Receive server updates
        int bytesReceived = recvfrom(sock, buffer, BUFFER_SIZE - 1, 0, (struct sockaddr*)&client_addr, &client_addr_len);
        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0';

            std::istringstream iss(buffer);
            int receivedClientId;
            float receivedXPos, receivedYPos;
            if (!(iss >> receivedClientId >> receivedXPos >> receivedYPos)) {
                std::cerr << "Error parsing received data." << std::endl;
                continue;
            }

            player2.updatePosition(receivedXPos, receivedYPos);
        }

        glClear(GL_COLOR_BUFFER_BIT);

        player1.render(shaderProgram, VAO);

        player2.render(redShaderProgram, VAO);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    glDeleteProgram(redShaderProgram);
    glfwTerminate();

#ifdef _WIN32
    closesocket(sock);
    WSACleanup();
#else
    close(sock);
#endif

    return 0;
}
