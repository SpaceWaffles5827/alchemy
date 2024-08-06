#include <alchemy/game.h>
#include <iostream>
#include <alchemy/player.h>

const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"uniform mat4 transform;\n"
"void main()\n"
"{\n"
"   gl_Position = transform * vec4(aPos, 1.0);\n"
"}\0";

const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\0";

const char* redFragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);\n"
"}\0";

Game::Game()
    : window(nullptr), VAO(0), VBO(0), shaderProgram(0), redShaderProgram(0),
    player1(0, glm::vec3(1.0f, 0.5f, 0.2f)), clientId(std::rand()), tickRate(1.0 / 64.0) {
    networkManager.setupUDPClient();
    player1 = Player(clientId, glm::vec3(1.0f, 0.5f, 0.2f));
}

Game::~Game() {
    cleanup();
}

void Game::run() {
    initGLFW();
    initGLEW();
    setupShaders();
    setupBuffers();

    double previousTime = glfwGetTime();
    double lag = 0.0;

    while (!glfwWindowShouldClose(window)) {
        double currentTime = glfwGetTime();
        double elapsed = currentTime - previousTime;
        previousTime = currentTime;
        lag += elapsed;

        while (lag >= tickRate) {
            processInput();
            lag -= tickRate;
        }

        update(elapsed);
        render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    cleanup();
}

void Game::initGLFW() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW!" << std::endl;
        std::exit(-1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(800, 800, "Moving Square", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        std::exit(-1);
    }

    glfwMakeContextCurrent(window);
}

void Game::initGLEW() {
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW!" << std::endl;
        std::exit(-1);
    }
}

void Game::setupShaders() {
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    GLuint redFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(redFragmentShader, 1, &redFragmentShaderSource, NULL);
    glCompileShader(redFragmentShader);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    redShaderProgram = glCreateProgram();
    glAttachShader(redShaderProgram, vertexShader);
    glAttachShader(redShaderProgram, redFragmentShader);
    glLinkProgram(redShaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glDeleteShader(redFragmentShader);
}

void Game::setupBuffers() {
    GLfloat vertices[] = {
        -0.1f, -0.1f, 0.0f,
         0.1f, -0.1f, 0.0f,
         0.1f,  0.1f, 0.0f,
         0.1f,  0.1f, 0.0f,
        -0.1f,  0.1f, 0.0f,
        -0.1f, -0.1f, 0.0f
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Game::processInput() {
    bool positionUpdated = false;
    float speed = 0.02f;
    glm::vec2 position = player1.getPosition();

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        position.y += speed;
        positionUpdated = true;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        position.y -= speed;
        positionUpdated = true;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        position.x -= speed;
        positionUpdated = true;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        position.x += speed;
        positionUpdated = true;
    }

    if (positionUpdated) {
        player1.updatePosition(position.x, position.y);
        networkManager.sendPlayerMovement(clientId, position.x, position.y);
    }
    else {
        networkManager.sendHeatBeat(clientId);
    }
}

void Game::update(double deltaTime) {
    if (networkManager.receiveData(players)) {
        for (auto& pair : players) { 
            int playerId = pair.first;    
            Player& player = pair.second;  

            glm::vec2 position = player.getPosition();
            std::cout << "Player ID: " << playerId << " Position: (" << position.x << ", " << position.y << ")\n";

        }
    }
}

void Game::render() {
    glClear(GL_COLOR_BUFFER_BIT);
    player1.render(shaderProgram, VAO);
    for (const auto& pair : players) {
        const Player& player = pair.second;
        player.render(shaderProgram, VAO);
    }
}

void Game::cleanup() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    glDeleteProgram(redShaderProgram);
    glfwTerminate();
}
