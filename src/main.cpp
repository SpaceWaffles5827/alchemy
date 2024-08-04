#include <GLEW/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <iomanip> // Include this header for std::setw, std::fixed, std::setprecision, and std::showpos
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
#else
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#endif

#define SERVER_PORT 8080
#define BUFFER_SIZE 256

// Vertex Shader source code
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"uniform mat4 transform;\n"
"void main()\n"
"{\n"
"   gl_Position = transform * vec4(aPos, 1.0);\n"
"}\0";

// Fragment Shader source code
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\0";

// Function to handle key input, update position, and send it to the server
void processInputAndSendToServer(GLFWwindow* window, SOCKET& sock, sockaddr_in& serv_addr, int clientId, float& xPos, float& yPos)
{
    bool positionUpdated = false;
    float speed = 0.01f; // Speed of movement

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        yPos += speed;
        positionUpdated = true;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        yPos -= speed;
        positionUpdated = true;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        xPos -= speed;
        positionUpdated = true;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        xPos += speed;
        positionUpdated = true;
    }

    if (positionUpdated)
    {
        char buffer[BUFFER_SIZE];
        snprintf(buffer, BUFFER_SIZE, "%d %.2f %.2f", clientId, xPos, yPos);
        sendto(sock, buffer, (int)strlen(buffer), 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

        // Output client position with formatting
        std::cout << "Client " << clientId << " position: ("
            << std::showpos << std::fixed << std::setw(6) << std::setprecision(2) << xPos << ", "
            << std::setw(6) << std::setprecision(2) << yPos << std::noshowpos << ")" << std::endl;
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

    float xPos = 0.0f, yPos = 0.0f;

    // Generate a random client ID
    std::srand(static_cast<unsigned int>(std::time(0)));
    int clientId = std::rand();

    // Initialize GLFW for rendering
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(800, 800, "Moving Square", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* Initialize GLEW */
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Error initializing GLEW!" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Define vertices for a square (two triangles)
    GLfloat vertices[] = {
        // First triangle
        -0.1f, -0.1f, 0.0f,
         0.1f, -0.1f, 0.0f,
         0.1f,  0.1f, 0.0f,
         // Second triangle
          0.1f,  0.1f, 0.0f,
         -0.1f,  0.1f, 0.0f,
         -0.1f, -0.1f, 0.0f
    };

    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // Bind the Vertex Array Object
    glBindVertexArray(VAO);

    // Bind VBO and load data into vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Configure vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Unbind VBO and VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Create and compile shaders
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Link shaders to create a program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Delete shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Process input and send updated position to server */
        processInputAndSendToServer(window, sock, serv_addr, clientId, xPos, yPos);

        /* Receive updated positions from server */
        int bytesReceived = recvfrom(sock, buffer, BUFFER_SIZE - 1, 0, (struct sockaddr*)&client_addr, &client_addr_len);
        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0'; // Null-terminate the buffer
            std::cout << "Received: " << buffer << std::endl;

            // Process the received data (e.g., update positions on the client-side)
            std::istringstream iss(buffer);
            int receivedClientId;
            float receivedXPos, receivedYPos;
            if (!(iss >> receivedClientId >> receivedXPos >> receivedYPos)) {
                std::cerr << "Error parsing received data." << std::endl;
                continue;
            }

            // Output received position with formatting
            std::cout << "Received Client " << receivedClientId << " position: ("
                << std::showpos << std::fixed << std::setw(6) << std::setprecision(2) << receivedXPos << ", "
                << std::setw(6) << std::setprecision(2) << receivedYPos << std::noshowpos << ")" << std::endl;

            // Handle the position data as needed, e.g., updating a game object's position
        }

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw the square
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);

        // Update the position of the square based on offset
        GLuint transformLoc = glGetUniformLocation(shaderProgram, "transform");
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(xPos, yPos, 0.0f));
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    glfwTerminate();

#ifdef _WIN32
    closesocket(sock);
    WSACleanup();
#else
    close(sock);
#endif

    return 0; // Ensure the main function returns 0
}
