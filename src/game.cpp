#include <alchemy/game.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <GLFW/glfw3.h>
#include <alchemy/networkManager.h>
#include <alchemy/textRenderer.h>
#include <fstream>

Game::Game(Mode mode)
    : VAO(0), VBO(0), shaderProgram(0), redShaderProgram(0), clientId(std::rand()), tickRate(1.0 / 64.0),
    projection(1.0f), currentMode(mode), chat(800, 800),
    displayInventory(false), showFps(false) {
    NetworkManager::getInstance().setupUDPClient();
    GraphicsContext::getInstance().setCameraZoom(1.0f);
}

Game::~Game() {
    cleanup();
}

void Game::setProjectionMatrix(glm::mat4 projectionMatrix) {
    projection = projectionMatrix;
}

GLuint Game::gettextureID2() {
    return textureID2;
}

void Game::init() {
    GraphicsContext::getInstance().initialize();
    InputManager::getInstance().registerCallbacks();

    Render& renderer = Render::getInstance();
    renderer.initialize();

    TextRenderer& textRenderer = TextRenderer::getInstance();
    textRenderer.updateScreenSize(800, 800);

    textRenderer.loadFont("fonts/minecraft.ttf", 24);

    textureID1 = GraphicsContext::getInstance().loadTexture("aniwooRunning.png");
    std::shared_ptr<Player> clientPlayer = std::make_shared<Player>(clientId, glm::vec3(1.0f, 0.5f, 0.2f), 0.0f, 0.0f, 1.0f, 2.0f, textureID1);
    clientPlayer->setTextureTile(0, 0, 8, 512, 512, 64, 128);
    World& world = World::getInstance();
    world.addPlayer(clientPlayer);

    textureID2 = GraphicsContext::getInstance().loadTexture("spriteSheet.png");
    inventoryTextureID = GraphicsContext::getInstance().loadTexture("inventory.png");

    // Initialize and configure the singleton instance of Inventory
    Inventory& playerInventory = Inventory::getInstance();
    playerInventory.setPosition(glm::vec3(400.0f, 400.0f, 0.0f));
    playerInventory.setRotation(glm::vec3(0.0f));
    playerInventory.setDimensions(176.0f * 3, 166.0f * 3);
    playerInventory.setTexture(inventoryTextureID, glm::vec2(0.0f, 1.0f), glm::vec2(1.0f, 0.0f));
    playerInventory.setGridSize(3, 9);

    playerInventory.loadDefaults();

    world.initTileView(10, 10, 1.0f, textureID2, textureID2);
}

bool Game::getDispalyInventory() {
    return displayInventory;
}

void Game::setDispalyInventory(bool status) {
    displayInventory = status;
}

GLuint Game::getDragTextureId() {
    return draggedTextureID;
}

void Game::setSelectedSlotIndex(int index) {
    selectedSlotIndex = index;
}

void Game::setDraggingTextureId(GLuint textureId) {
    draggedTextureID = textureId;
}

void Game::setDraggingItemName(std::string name) {
    draggedItemName = name;
}

void Game::setDraggingStartPos(glm::vec2 position) {
    dragStartPosition = position;
}

int Game::getSelectedSlotIndex() {
    return selectedSlotIndex;
}

void Game::run() {
    double previousTime = glfwGetTime();
    double lag = 0.0;
    int frameCount = 0;
    double fpsTime = 0.0;

    while (!glfwWindowShouldClose(GraphicsContext::getInstance().getWindow())) {
        double currentTime = glfwGetTime();
        double elapsed = currentTime - previousTime;
        previousTime = currentTime;
        lag += elapsed;

        fpsTime += elapsed;
        frameCount++;

        if (fpsTime >= 1.0) {
            if (showFps) {
                double fps = frameCount / fpsTime;
                std::cout << "FPS: " << fps << " | Frame Time: " << (fpsTime / frameCount) * 1000.0 << " ms" << std::endl;
            }
            frameCount = 0;
            fpsTime = 0.0;
        }

        while (lag >= tickRate) {
            InputManager::getInstance().handleInput();
            lag -= tickRate;
        }

        update(elapsed);
        render();

        glfwSwapBuffers(GraphicsContext::getInstance().getWindow());
        glfwPollEvents();
    }

    cleanup();
}

int Game::getClientId() {
    return clientId;
}

Chat& Game::getChat() {
    return chat;
}

GLuint Game::getShaderProgram() {
    return shaderProgram;
}

void Game::update(double deltaTime) {
    if (NetworkManager::getInstance().receiveData(players)) {
        for (auto& pair : players) {
            int playerId = pair.first;
            Player& player = pair.second;

            glm::vec2 position = player.getPosition();
        }
    }
}

glm::mat4 Game::getProjection() {
    return projection;
}

Inventory& Game::getPlayerInventory() {
    return Inventory::getInstance();
}

Mode Game::getGameMode() {
    return currentMode;
}

void Game::render() {
    glClear(GL_COLOR_BUFFER_BIT);

    int width, height;
    glfwGetWindowSize(GraphicsContext::getInstance().getWindow(), &width, &height);

    GraphicsContext::getInstance().updateProjectionMatrix(width, height);
    World& world = World::getInstance();

    Render& renderer = Render::getInstance();

    // Render game world objects
    {
        std::vector<std::shared_ptr<Renderable>> renderables(world.getObjects().begin(), world.getObjects().end());
        renderer.batchRenderGameObjects(renderables, projection);
    }

    // Render player objects
    {
        std::vector<std::shared_ptr<Renderable>> renderables(world.getPlayers().begin(), world.getPlayers().end());
        renderer.batchRenderGameObjects(renderables, projection);
    }

    // Render the chat
    chat.render();

    // Render the UI elements (like inventory)
    if (displayInventory) {
        renderer.renderUI(width, height);
    }
}

InputManager& Game::getInputManager() {
    return InputManager::getInstance();
}

void Game::cleanup() {
    if (VAO) {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }

    if (VBO) {
        glDeleteBuffers(1, &VBO);
        VBO = 0;
    }

    if (shaderProgram) {
        glDeleteProgram(shaderProgram);
        shaderProgram = 0;
    }

    if (redShaderProgram) {
        glDeleteProgram(redShaderProgram);
        redShaderProgram = 0;
    }

    if (GraphicsContext::getInstance().getWindow()) {
        glfwDestroyWindow(GraphicsContext::getInstance().getWindow());
    }

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
    return NetworkManager::getInstance();
}

World& Game::getWorld() {
    World& world = World::getInstance();
    return world;
}

GraphicsContext& Game::getGraphicsContext() {
    return GraphicsContext::getInstance();
}

TextRenderer* Game::getTextRender() {
    TextRenderer& textRenderer = TextRenderer::getInstance();
    TextRenderer* testPointer = &textRenderer;
    return testPointer;
}
