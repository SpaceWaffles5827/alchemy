#include <alchemy/game.h>
#include <alchemy/player.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <GLEW/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec2 aTexCoord;\n"
"out vec2 TexCoord;\n"
"uniform mat4 transform;\n"
"void main()\n"
"{\n"
"   gl_Position = transform * vec4(aPos, 1.0);\n"
"   TexCoord = aTexCoord;\n"
"}\0";

const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec2 TexCoord;\n"
"uniform sampler2D ourTexture;\n"
"void main()\n"
"{\n"
"   FragColor = texture(ourTexture, TexCoord);\n"
"}\0";

const char* redFragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);\n"
"}\0";

Game::Game()
    : window(nullptr), VAO(0), VBO(0), shaderProgram(0), redShaderProgram(0), clientId(std::rand()), tickRate(1.0 / 64.0), clientPlayer(clientId, glm::vec3(1.0f, 0.5f, 0.2f)) {
    networkManager.setupUDPClient();

    initGLFW();
    initGLEW();

    if (!clientPlayer.loadTexture("wizard.png")) {
        std::cerr << "Failed to load texture 'wizard.png'" << std::endl;
    }
}

Game::~Game() {
    cleanup();
}

void Game::run() {
    setupShaders();
    setupBuffers();

    double previousTime = glfwGetTime();
    double lag = 0.0;

    // Create a GameObject using shared_ptr
    std::shared_ptr<GameObject> gameObject1 = std::make_shared<GameObject>(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f));

    // Set position and rotation
    gameObject1->setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    gameObject1->setRotation(glm::vec3(0.0f, 0.0f, 0.0f));

    // Add gameObject1 to world
    world.addObject(gameObject1);

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
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW!" << std::endl;
        std::exit(-1);
    }

    // Enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Game::setupShaders() {
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    checkCompileErrors(vertexShader, "VERTEX");

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    checkCompileErrors(fragmentShader, "FRAGMENT");

    GLuint redFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(redFragmentShader, 1, &redFragmentShaderSource, NULL);
    glCompileShader(redFragmentShader);
    checkCompileErrors(redFragmentShader, "FRAGMENT");

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    checkCompileErrors(shaderProgram, "PROGRAM");

    redShaderProgram = glCreateProgram();
    glAttachShader(redShaderProgram, vertexShader);
    glAttachShader(redShaderProgram, redFragmentShader);
    glLinkProgram(redShaderProgram);
    checkCompileErrors(redShaderProgram, "PROGRAM");

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glDeleteShader(redFragmentShader);
}

void Game::setupBuffers() {
    GLfloat vertices[] = {
        -0.1f, -0.1f, 0.0f,  0.0f, 0.0f,
         0.1f, -0.1f, 0.0f,  1.0f, 0.0f,
         0.1f,  0.1f, 0.0f,  1.0f, 1.0f,
         0.1f,  0.1f, 0.0f,  1.0f, 1.0f,
        -0.1f,  0.1f, 0.0f,  0.0f, 1.0f,
        -0.1f, -0.1f, 0.0f,  0.0f, 0.0f
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Texture coordinates attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Game::processInput() {
    bool positionUpdated = false;
    float speed = 0.02f;
    glm::vec2 position = clientPlayer.getPosition();

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
        clientPlayer.updatePosition(position.x, position.y);
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
            if (!player.isTextureLoaded()) {
                if (!player.loadTexture("wizard.png")) {
                    std::cerr << "Failed to load texture for player " << playerId << std::endl;
                }
            }
        }
    }
}

void Game::render() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Render world objects with the red shader program
    glUseProgram(redShaderProgram);
    world.render(redShaderProgram, VAO);

    // Render client player with the main shader program
    glUseProgram(shaderProgram);
    clientPlayer.render(shaderProgram, VAO);

    // Render other players with the main shader program
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

void Game::checkCompileErrors(GLuint shader, std::string type) {
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "| ERROR::SHADER: Compile-time error: Type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "| ERROR::Program: Link-time error: Type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}
