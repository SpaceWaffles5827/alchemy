#include <alchemy/game.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <GLFW/glfw3.h>
#include <alchemy/networkManager.h>
#include <alchemy/textRenderer.h>
#include <fstream>
#include <alchemy/fpsDisplay.h>
#include <alchemy/hotbar.h>

Game::Game(Mode mode)
    : clientId(std::rand()), tickRate(1.0 / 64.0), currentMode(mode) {
    GraphicsContext::getInstance().setProjectionMatrix(1.0f);
    NetworkManager::getInstance().setupUDPClient();
    GraphicsContext::getInstance().setCameraZoom(1.0f);
}

Game::~Game() {
    cleanup();
}

void Game::init() {
    GraphicsContext::getInstance().initialize();
    InputManager::getInstance().registerCallbacks();

    Render& renderer = Render::getInstance();
    renderer.initialize();

    TextRenderer& textRenderer = TextRenderer::getInstance();
    textRenderer.updateScreenSize(800, 800);

    textRenderer.loadFont("fonts/minecraft.ttf", 24);

    GraphicsContext::getInstance().setTextureID1(GraphicsContext::getInstance().loadTexture("aniwooRunning.png"));
    std::shared_ptr<Player> clientPlayer = std::make_shared<Player>(clientId, glm::vec3(1.0f, 0.5f, 0.2f), 0.0f, 0.0f, 1.0f, 2.0f, GraphicsContext::getInstance().getTextureID1());
    clientPlayer->setTextureTile(0, 0, 8, 512, 512, 64, 128);
    World& world = World::getInstance();
    world.addPlayer(clientPlayer);

    GraphicsContext::getInstance().setTextureID2(GraphicsContext::getInstance().loadTexture("spriteSheet.png"));
    GraphicsContext::getInstance().setInventoryTextureID(GraphicsContext::getInstance().loadTexture("inventory.png"));
    GraphicsContext::getInstance().setHotbarTextureId(GraphicsContext::getInstance().loadTexture("textures/ui/hotbar.png"));

    Inventory& playerInventory = Inventory::getInstance();
    playerInventory.setPosition(glm::vec3(400.0f, 400.0f, 0.0f));
    playerInventory.setRotation(glm::vec3(0.0f));
    playerInventory.setDimensions(176.0f * 3, 166.0f * 3);
    playerInventory.setTexture(GraphicsContext::getInstance().getInventoryTextureID(), glm::vec2(0.0f, 1.0f), glm::vec2(1.0f, 0.0f));
    playerInventory.setGridSize(3, 9);
    playerInventory.loadDefaults();

    HotBar& playerHotbar = HotBar::getInstance();
    playerHotbar.setPosition(glm::vec3(400.0f, 765.0f, 0.0f));
    playerHotbar.setRotation(glm::vec3(0.0f));
    playerHotbar.setDimensions(183.0f * 3, 23.0f * 3);
    playerHotbar.setTexture(GraphicsContext::getInstance().getHotbarTextureId(), glm::vec2(0.0f, 1.0f), glm::vec2(1.0f, 0.0f));
    playerHotbar.loadDefaults();


    world.initTileView(10, 10, 1.0f, GraphicsContext::getInstance().getTextureID2(), GraphicsContext::getInstance().getTextureID2());

    std::shared_ptr<Mob> mobPtr = std::make_shared<Mob>();
    world.addMob(mobPtr);
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

        FPSDisplay::getInstance().update(); // Update the FPS counter

        if (fpsTime >= 1.0) {
            if (FPSDisplay::getInstance().isVisable()) {
                FPSDisplay::getInstance().render();
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

void Game::update(double deltaTime) {
    if (NetworkManager::getInstance().receiveData(players)) {
        for (auto& pair : players) {
            int playerId = pair.first;
            Player& player = pair.second;

            glm::vec2 position = player.getPosition();
        }
    }

    // Update all mobs with the deltaTime for smooth movement
    for (auto& mobPtr : World::getInstance().getMobs()) {
        if (mobPtr) {
            mobPtr->update(static_cast<float>(deltaTime));
        }
    }
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
        renderer.batchRenderGameObjects(renderables, GraphicsContext::getInstance().getProjection());
    }

    // Render player objects
    {
        std::vector<std::shared_ptr<Renderable>> renderables(world.getPlayers().begin(), world.getPlayers().end());
        renderer.batchRenderGameObjects(renderables, GraphicsContext::getInstance().getProjection());
    }

    // Render mobs
    {
        std::vector<std::shared_ptr<Renderable>> renderables(world.getMobs().begin(), world.getMobs().end());
        renderer.batchRenderGameObjects(renderables, GraphicsContext::getInstance().getProjection());
    }

    renderer.renderUI(width, height);

    Chat::getInstance().render();


    if (FPSDisplay::getInstance().isVisable()) {
        FPSDisplay::getInstance().render();
    }
}

void Game::cleanup() {
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
