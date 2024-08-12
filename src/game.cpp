#include <alchemy/game.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <GLFW/glfw3.h>
#include <alchemy/networkManager.h>

const char* Game::vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec2 aTexCoord;\n"
"out vec2 TexCoord;\n"
"uniform mat4 transform;\n"
"void main()\n"
"{\n"
"   gl_Position = transform * vec4(aPos, 1.0);\n"
"   TexCoord = aTexCoord;\n"
"}\0";

const char* Game::fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec2 TexCoord;\n"
"uniform sampler2D ourTexture;\n"
"void main()\n"
"{\n"
"   FragColor = texture(ourTexture, TexCoord);\n"
"}\0";

const char* Game::redFragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);\n"
"}\0";

Game::Game(Mode mode)
    : window(nullptr), VAO(0), VBO(0), shaderProgram(0), redShaderProgram(0), clientId(std::rand()), tickRate(1.0 / 64.0),
    projection(1.0f), cameraZoom(1.0f), currentMode(mode) {

    networkManager.setupUDPClient();
}


Game::~Game() {
    cleanup();
}

void Game::init() {
    // Initialize GLFW, GLEW, and renderer in the constructor
    initGLFW();
    initGLEW();

    renderer.initialize();

    // Initialize clientPlayer within the world
    GLuint textureID1 = loadTexture("aniwooRunning.png");
    std::shared_ptr<Player> clientPlayer = std::make_shared<Player>(clientId, glm::vec3(1.0f, 0.5f, 0.2f), 0.0f, 0.0f, 1.0f, 2.0f, textureID1);
    clientPlayer->setTextureTile(0, 0, 8, 512, 512, 64, 128);
    world.addPlayer(clientPlayer);

    GLuint textureID2 = loadTexture("spriteSheet.png");

    world.initTileView(200, 200, 1.0f, textureID2, textureID2);
}


void Game::run() {
    setupShaders();
    setupBuffers();

    double previousTime = glfwGetTime();
    double lag = 0.0;
    int frameCount = 0;
    double fpsTime = 0.0;

    while (!glfwWindowShouldClose(window)) {
        double currentTime = glfwGetTime();
        double elapsed = currentTime - previousTime;
        previousTime = currentTime;
        lag += elapsed;

        fpsTime += elapsed;
        frameCount++;

        // Calculate FPS every second
        if (fpsTime >= 1.0) {
            double fps = frameCount / fpsTime;
            std::cout << "FPS: " << fps << " | Frame Time: " << (fpsTime / frameCount) * 1000.0 << " ms" << std::endl;
            frameCount = 0;
            fpsTime = 0.0;
        }

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

GLuint Game::loadTexture(const char* path) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // S (X-axis) wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // T (Y-axis) wrapping
    // Set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load the image
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // Flip the image vertically (optional)
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cerr << "Failed to load texture: " << path << std::endl;
    }
    stbi_image_free(data);

    return textureID;
}

void Game::initGLFW() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW!" << std::endl;
        std::exit(-1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    window = glfwCreateWindow(800, 800, "Moving Square", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        std::exit(-1);
    }

    glfwMakeContextCurrent(window);

    glfwSetWindowUserPointer(window, this);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glfwSetScrollCallback(window, scroll_callback);

    glfwSetMouseButtonCallback(window, mouse_button_callback);
}

void Game::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        Game* game = static_cast<Game*>(glfwGetWindowUserPointer(window));

        if (game->currentMode == Mode::LevelEdit) {
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);

            int width, height;
            glfwGetWindowSize(window, &width, &height);

            float xNDC = static_cast<float>((2.0 * xpos) / width - 1.0);
            float yNDC = static_cast<float>(1.0 - (2.0 * ypos) / height);

            glm::vec4 ndcCoords = glm::vec4(xNDC, yNDC, 0.0f, 1.0f);
            glm::vec4 worldCoords = glm::inverse(game->projection) * ndcCoords;

            float snappedX = std::round(worldCoords.x);
            float snappedY = std::round(worldCoords.y);

            float tileWidth = 1.0f;
            float tileHeight = 1.0f;

            std::shared_ptr<GameObject> gameObjectAdding = std::make_shared<GameObject>(
                glm::vec3(snappedX, snappedY, 0.0f),
                glm::vec3(0.0f),
                tileWidth,
                tileHeight,
                game->textureID1
                );

            int randomTileX = rand() % 8; // Random x between 0 and 7
            int randomTileY = rand() % 8; // Random y between 0 and 7

            std::cout << "Placing on: (" << randomTileX << ", " << randomTileY << ")" << std::endl;

            game->world.addObject(gameObjectAdding);
        }
    }
}

void Game::scroll_callback(GLFWwindow* window, double xOffset, double yOffset) {
    Game* game = static_cast<Game*>(glfwGetWindowUserPointer(window));

    game->cameraZoom += yOffset * -0.1f;
    if (game->cameraZoom < 0.1f) game->cameraZoom = 0.1f;
    if (game->cameraZoom > 99.0f) game->cameraZoom = 99.0f;

    int width, height;
    glfwGetWindowSize(window, &width, &height);

    game->updateProjectionMatrix(width, height);
}

void Game::initGLEW() {
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW!" << std::endl;
        std::exit(-1);
    }

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

    int width, height;
    glfwGetWindowSize(window, &width, &height);

    updateProjectionMatrix(width, height);
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

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Game::processInput() {
    std::shared_ptr<Player> player = world.getPlayerById(clientId);

    if (player) {
        player->handleInput();
    }
    else {
        std::cerr << "Player with ID " << clientId << " not found." << std::endl;
    }
}

void Game::update(double deltaTime) {
    if (networkManager.receiveData(players)) {
        for (auto& pair : players) {
            int playerId = pair.first;
            Player& player = pair.second;

            glm::vec2 position = player.getPosition();
        }
    }
}

void Game::render() {
    glClear(GL_COLOR_BUFFER_BIT);

    int width, height;
    glfwGetWindowSize(window, &width, &height);

    updateProjectionMatrix(width, height);

    renderer.batchRenderGameObjects(world.getObjects(), projection);

    auto gameObjects = std::vector<std::shared_ptr<GameObject>>(world.getPlayers().begin(), world.getPlayers().end());
    renderer.batchRenderGameObjects(gameObjects, projection);

    for (const auto& pair : players) {
        const Player& player = pair.second;
        player.render(shaderProgram, VAO, projection);
    }
}

void Game::cleanup() {
    // Check if VAO was generated before attempting to delete it
    if (VAO) {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }

    // Check if VBO was generated before attempting to delete it
    if (VBO) {
        glDeleteBuffers(1, &VBO);
        VBO = 0;
    }

    // Check if the shader programs were created before attempting to delete them
    if (shaderProgram) {
        glDeleteProgram(shaderProgram);
        shaderProgram = 0;
    }

    if (redShaderProgram) {
        glDeleteProgram(redShaderProgram);
        redShaderProgram = 0;
    }

    // Properly terminate GLFW if the window is valid
    if (window) {
        glfwDestroyWindow(window);
        window = nullptr;
    }

    // Terminate GLFW if it's been initialized
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

NetworkManager& Game::getNetworkManager() {
    return networkManager; 
}

World& Game::getWorld() {
    return world;
}

GLFWwindow& Game::getWindow() {
    return *window;
}

void Game::updateProjectionMatrix(int width, int height) {
    float aspectRatio = static_cast<float>(width) / height;
    float viewWidth = 20.0f * cameraZoom;
    float viewHeight = viewWidth / aspectRatio;

    auto player = world.getPlayerById(clientId);
    if (!player) return; // Ensure the player exists

    glm::vec2 playerPos = player->getPosition();

    projection = glm::ortho(
        playerPos.x - viewWidth / 2.0f, playerPos.x + viewWidth / 2.0f,
        playerPos.y - viewHeight / 2.0f, playerPos.y + viewHeight / 2.0f,
        -1.0f, 1.0f
    );

    GLuint transformLoc = glGetUniformLocation(shaderProgram, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

void Game::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);

    Game* game = static_cast<Game*>(glfwGetWindowUserPointer(window));

    game->updateProjectionMatrix(width, height);
}
