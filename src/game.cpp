#include <alchemy/game.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <GLFW/glfw3.h>
#include <alchemy/networkManager.h>
#include <alchemy/textRenderer.h>
#include <fstream>

Game::Game(Mode mode)
    : window(nullptr), VAO(0), VBO(0), shaderProgram(0), redShaderProgram(0), clientId(std::rand()), tickRate(1.0 / 64.0),
    projection(1.0f), cameraZoom(1.0f), currentMode(mode), chat(800, 800), selectedTileX(0), selectedTileY(0),
    tileSelectionVisible(false), displayInventory(false), showFps(false) {

    for (int i = 0; i < GLFW_KEY_LAST; ++i) {
        keyReleased[i] = true;
    }

    networkManager.setupUDPClient();
}

Game::~Game() {
    cleanup();
}

void Game::init() {
    initGLFW();
    initGLEW();

    renderer.initialize();

    textRenderer = std::make_unique<TextRenderer>(800, 800);

    if (!textRenderer) {
        std::cerr << "ERROR::GAME: TextRenderer failed to initialize!" << std::endl;
    }
    else {
        std::cout << "TextRenderer initialized successfully!" << std::endl;
    }

    textRenderer->loadFont("fonts/minecraft.ttf", 24);

    textureID1 = loadTexture("aniwooRunning.png");
    std::shared_ptr<Player> clientPlayer = std::make_shared<Player>(clientId, glm::vec3(1.0f, 0.5f, 0.2f), 0.0f, 0.0f, 1.0f, 2.0f, textureID1);
    clientPlayer->setTextureTile(0, 0, 8, 512, 512, 64, 128);
    world.addPlayer(clientPlayer);

    textureID2 = loadTexture("spriteSheet.png");
    inventoryTextureID = loadTexture("inventory.png");

    playerInventory = Inventory(glm::vec3(400.0f, 400.0f, 0.0f), glm::vec3(0.0f), 176.0f * 3, 166.0f * 3, inventoryTextureID,
        glm::vec2(0.0f, 1.0f), glm::vec2(1.0f, 0.0f), 3, 9);

    // Change the texture of the first slot to a different texture
    std::vector<InventorySlot> & invSlot = playerInventory.getInventorySlots();

    GLuint specialTextureID = loadTexture("stone_bricks.png");
    invSlot[0].setTexture(specialTextureID);
    invSlot[0].setItem("Stone");
    invSlot[1].setTexture(specialTextureID);
    invSlot[1].setItem("Stone");
    invSlot[2].setTexture(specialTextureID);
    invSlot[2].setItem("Stone");
    invSlot[3].setTexture(specialTextureID);
    invSlot[3].setItem("Stone");

    world.initTileView(10, 10, 1.0f, textureID2, textureID2);
}

void Game::renderUI(int width, int height) {
    glm::mat4 projectionUI = glm::ortho(
        0.0f, static_cast<float>(width),
        static_cast<float>(height), 0.0f,
        -1.0f, 1.0f
    );

    if (displayInventory) {
        std::vector<std::shared_ptr<Renderable>> renderables;

        std::shared_ptr<Renderable> inventoryRenderable = std::make_shared<Inventory>(playerInventory);
        renderer.batchRenderGameObjects({ inventoryRenderable }, projectionUI);

        for (auto& slot : playerInventory.getInventorySlots()) {
            auto slotRenderable = std::make_shared<InventorySlot>(
                slot.getPosition(),
                slot.getRotation(),
                slot.getScale().x, slot.getScale().y,
                slot.getTextureID(),
                slot.getTextureTopLeft(),
                slot.getTextureBottomRight()
            );
            renderables.push_back(slotRenderable);
        }

        renderer.batchRenderGameObjects(renderables, projectionUI);

        // Render the dragged item if dragging
        if (isDragging) {
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);

            auto draggedItemRenderable = std::make_shared<InventorySlot>(
                glm::vec3(static_cast<float>(xpos), static_cast<float>(ypos), 0.0f),
                glm::vec3(0.0f),
                playerInventory.getInventorySlots()[selectedSlotIndex].getScale().x,
                playerInventory.getInventorySlots()[selectedSlotIndex].getScale().y,
                draggedTextureID,
                playerInventory.getInventorySlots()[selectedSlotIndex].getTextureTopLeft(),
                playerInventory.getInventorySlots()[selectedSlotIndex].getTextureBottomRight()
            );

            renderer.batchRenderGameObjects({ draggedItemRenderable }, projectionUI);
        }
    }
}

void Game::run() {
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

        if (fpsTime >= 1.0) {
            if (showFps) {
                double fps = frameCount / fpsTime;
                std::cout << "FPS: " << fps << " | Frame Time: " << (fpsTime / frameCount) * 1000.0 << " ms" << std::endl;
            }
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

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
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

    window = glfwCreateWindow(800, 800, "Tile Picker", NULL, NULL);
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
    Game* game = static_cast<Game*>(glfwGetWindowUserPointer(window));

    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    int width, height;
    glfwGetWindowSize(window, &width, &height);

    // Convert screen coordinates to the inventory UI coordinate system
    float worldX = static_cast<float>(xpos);
    float worldY = static_cast<float>(ypos);

    std::cout << "Clicking: " << worldX << ", " << worldY << "\n";

    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            if (game->displayInventory) {
                int slotIndex = game->playerInventory.getSlotIndexAt(worldX, worldY);
                if (slotIndex != -1 && !game->isDragging) {
                    game->selectedSlotIndex = slotIndex;
                    game->draggedTextureID = game->playerInventory.getInventorySlots()[slotIndex].getTextureID();
                    game->draggedItemName = game->playerInventory.getItemInSlot(slotIndex);
                    game->isDragging = true;
                    game->dragStartPosition = glm::vec2(worldX, worldY);
                }
            }
        }
        else if (action == GLFW_RELEASE && game->isDragging) {
            if (game->displayInventory) {
                int slotIndex = game->playerInventory.getSlotIndexAt(worldX, worldY);
                if (slotIndex != -1 && slotIndex != game->selectedSlotIndex) {
                    // Swap the items between the slots
                    auto& sourceSlot = game->playerInventory.getInventorySlots()[game->selectedSlotIndex];
                    auto& targetSlot = game->playerInventory.getInventorySlots()[slotIndex];

                    // Swap textures
                    GLuint tempTextureID = targetSlot.getTextureID();
                    targetSlot.setTexture(sourceSlot.getTextureID());
                    sourceSlot.setTexture(tempTextureID);

                    // Swap item names
                    std::string tempItemName = targetSlot.getItem();
                    targetSlot.setItem(sourceSlot.getItem());
                    sourceSlot.setItem(tempItemName);
                }

                // Reset dragging state
                game->isDragging = false;
                game->selectedSlotIndex = -1;
                game->draggedTextureID = 0;
                game->draggedItemName.clear();
            }
        }
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        game->handleRightClickInteraction();
    }
}

void Game::handleWorldInteraction(double xpos, double ypos, int width, int height) {
    if (currentMode == Mode::LevelEdit) {
        float xNDC = static_cast<float>((2.0 * xpos) / width - 1.0);
        float yNDC = static_cast<float>(1.0 - (2.0 * ypos) / height);

        glm::vec4 ndcCoords = glm::vec4(xNDC, yNDC, 0.0f, 1.0f);
        glm::vec4 worldCoords = glm::inverse(projection) * ndcCoords;

        float snappedX = std::round(worldCoords.x);
        float snappedY = std::round(worldCoords.y);

        std::shared_ptr<GameObject> gameObjectAdding = std::make_shared<GameObject>(
            glm::vec3(snappedX, snappedY, 0.0f),
            glm::vec3(0.0f),
            1.0f,
            1.0f,
            textureID2);

        gameObjectAdding->setTextureTile(selectedTileX, selectedTileY, 8, 256, 256, 32, 32);
        world.addObject(gameObjectAdding);
    }
}

void Game::handleRightClickInteraction() {
    if (currentMode == Mode::LevelEdit) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        int width, height;
        glfwGetWindowSize(window, &width, &height);

        float xNDC = static_cast<float>((2.0 * xpos) / width - 1.0);
        float yNDC = static_cast<float>(1.0 - (2.0 * ypos) / height);

        glm::vec4 ndcCoords = glm::vec4(xNDC, yNDC, 0.0f, 1.0f);
        glm::vec4 worldCoords = glm::inverse(projection) * ndcCoords;

        float snappedX = std::round(worldCoords.x);
        float snappedY = std::round(worldCoords.y);

        world.eraseObject(glm::vec3(snappedX, snappedY, 0.0f));
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

void Game::renderTileSelectionUI() {
    if (!tileSelectionVisible || currentMode != Mode::LevelEdit) return;

    const float uiSize = 200.0f;
    const int gridSizeX = 8; // Number of tiles in the X direction
    const int gridSizeY = 8; // Number of tiles in the Y direction

    // Calculate the tile size based on the UI size and grid size
    const float tileWidth = uiSize / gridSizeX;
    const float tileHeight = uiSize / gridSizeY;

    int width, height;
    glfwGetWindowSize(window, &width, &height);

    // Adjust start position to remove the gap at the bottom
    const float uiStartX = 0.0f; // Fixed pixel position from the left edge
    const float uiStartY = height - uiSize + tileHeight; // Adjust to align with the bottom edge

    std::vector<std::shared_ptr<Renderable>> tiles;

    for (int y = 0; y < gridSizeY; ++y) {
        for (int x = 0; x < gridSizeX; ++x) {
            // Convert pixel positions to normalized device coordinates (NDC)
            float xNDC = (uiStartX + x * tileWidth) / width * 2.0f - 1.0f;
            float yNDC = 1.0f - (uiStartY + y * tileHeight) / height * 2.0f;

            glm::vec3 position(xNDC, yNDC, 0.0f);

            auto tile = std::make_shared<GameObject>(
                position,
                glm::vec3(0.0f),
                tileWidth / width * 2.0f, tileHeight / height * 2.0f,
                textureID2
            );

            tile->setTextureTile(x, y, gridSizeX, 256, 256, 32, 32);
            tiles.push_back(tile);
        }
    }

    renderer.batchRenderGameObjects(tiles, glm::mat4(1.0f));
}

void Game::processInput() {
    static bool tabKeyReleased = true;
    static bool escKeyReleased = true;

    // Handle input when chat mode is active
    if (chat.isChatModeActive()) {
        static bool enterKeyReleased = true;
        static bool backspaceKeyReleased = true;

        if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS && enterKeyReleased) {
            chat.addMessage(chat.getCurrentMessage());
            chat.setCurrentMessage("");
            chat.setChatModeActive(false);
            enterKeyReleased = false;
        }
        if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_RELEASE) {
            enterKeyReleased = true;
        }

        if (glfwGetKey(window, GLFW_KEY_BACKSPACE) == GLFW_PRESS && backspaceKeyReleased) {
            std::string currentMessage = chat.getCurrentMessage();
            if (!currentMessage.empty()) {
                currentMessage.pop_back();
                chat.setCurrentMessage(currentMessage);
            }
            backspaceKeyReleased = false;
        }
        if (glfwGetKey(window, GLFW_KEY_BACKSPACE) == GLFW_RELEASE) {
            backspaceKeyReleased = true;
        }

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS && escKeyReleased) {
            chat.setCurrentMessage("");
            chat.setChatModeActive(false);
            escKeyReleased = false;
        }
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_RELEASE) {
            escKeyReleased = true;
        }

        if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS && tabKeyReleased) {
            chat.selectSuggestion(); // Handle tab in chat mode
            tabKeyReleased = false;
        }
        if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_RELEASE) {
            tabKeyReleased = true;
        }

        // Process letter keys for chat
        for (int key = GLFW_KEY_A; key <= GLFW_KEY_Z; ++key) {
            if (glfwGetKey(window, key) == GLFW_PRESS && keyReleased[key]) {
                bool shiftPressed = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;
                char c = static_cast<char>(key);
                if (!shiftPressed) {
                    c += 32; // Convert to lowercase if shift is not pressed
                }
                std::string currentMessage = chat.getCurrentMessage();
                currentMessage += c;
                chat.setCurrentMessage(currentMessage);
                keyReleased[key] = false;
            }
            if (glfwGetKey(window, key) == GLFW_RELEASE) {
                keyReleased[key] = true;
            }
        }

        // Process number keys for chat
        for (int key = GLFW_KEY_0; key <= GLFW_KEY_9; ++key) {
            if (glfwGetKey(window, key) == GLFW_PRESS && keyReleased[key]) {
                char c = static_cast<char>(key);
                std::string currentMessage = chat.getCurrentMessage();
                currentMessage += c;
                chat.setCurrentMessage(currentMessage);
                keyReleased[key] = false;
            }
            if (glfwGetKey(window, key) == GLFW_RELEASE) {
                keyReleased[key] = true;
            }
        }

        // Process space key for chat
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && keyReleased[GLFW_KEY_SPACE]) {
            std::string currentMessage = chat.getCurrentMessage();
            currentMessage += ' ';
            chat.setCurrentMessage(currentMessage);
            keyReleased[GLFW_KEY_SPACE] = false;
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
            keyReleased[GLFW_KEY_SPACE] = true;
        }
    }
    else { // Handle input when chat mode is not active
        std::shared_ptr<Player> player = world.getPlayerById(clientId);
        if (player) {
            player->handleInput();
        }
        else {
            std::cerr << "Player with ID " << clientId << " not found." << std::endl;
        }

        static bool tKeyReleased = true;
        static bool slashKeyReleased = true;
        static bool bKeyReleased = true;

        if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS && tKeyReleased) {
            chat.setChatModeActive(true);
            tKeyReleased = false;
        }
        if (glfwGetKey(window, GLFW_KEY_T) == GLFW_RELEASE) {
            tKeyReleased = true;
        }

        if (glfwGetKey(window, GLFW_KEY_SLASH) == GLFW_PRESS && slashKeyReleased) {
            chat.setChatModeActive(true);
            std::string currentMessage = chat.getCurrentMessage();
            currentMessage += '/';
            chat.setCurrentMessage(currentMessage);
            slashKeyReleased = false;
        }
        if (glfwGetKey(window, GLFW_KEY_SLASH) == GLFW_RELEASE) {
            slashKeyReleased = true;
        }

        if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && bKeyReleased) {
            tileSelectionVisible = !tileSelectionVisible;
            displayInventory = false;
            chat.setChatModeActive(false);
            bKeyReleased = false;
        }
        if (glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE) {
            bKeyReleased = true;
        }

        if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS && tabKeyReleased) {
            displayInventory = !displayInventory;
            chat.setChatModeActive(false);
            tileSelectionVisible = false;
            tabKeyReleased = false;
        }
        if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_RELEASE) {
            tabKeyReleased = true;
        }

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS && escKeyReleased) {
            if (displayInventory) {
                displayInventory = false;
            }
            else if (tileSelectionVisible) {
                tileSelectionVisible = false;
            }
            else {
                chat.setChatModeActive(false);
            }
            escKeyReleased = false;
        }
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_RELEASE) {
            escKeyReleased = true;
        }
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

    // Render the tile selection UI if visible
    // renderTileSelectionUI();

    // Render the UI elements (like inventory)
    if (displayInventory) {
        renderUI(width, height);
    }
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

    if (window) {
        glfwDestroyWindow(window);
        window = nullptr;
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
    return networkManager;
}

World& Game::getWorld() {
    return world;
}

GLFWwindow& Game::getWindow() {
    return *window;
}

TextRenderer& Game::getTextRender() {
    return *textRenderer;
}

void Game::saveLevel(const std::string& filename) {
    std::ofstream outFile(filename);
    if (outFile.is_open()) {
        std::cout << "Level saved to " << filename << std::endl;
        outFile.close();
    }
    else {
        std::cerr << "Failed to open file for saving: " << filename << std::endl;
    }
}

void Game::loadLevel(const std::string& filename) {
    std::ifstream inFile(filename);
    if (inFile.is_open()) {
        std::cout << "Level loaded from " << filename << std::endl;
        inFile.close();
    }
    else {
        std::cerr << "Failed to open file for loading: " << filename << std::endl;
    }
}

void Game::saveWorld(const std::string& filename) {
    std::ofstream outFile(filename);
    if (outFile.is_open()) {
        std::cout << "World saved to " << filename << std::endl;
        outFile.close();
    }
    else {
        std::cerr << "Failed to open file for saving: " << filename << std::endl;
    }
}

void Game::loadWorld(const std::string& filename) {
    std::ifstream inFile(filename);
    if (inFile.is_open()) {
        std::cout << "World loaded from " << filename << std::endl;
        inFile.close();
    }
    else {
        std::cerr << "Failed to open file for loading: " << filename << std::endl;
    }
}

void Game::updateUiProjectionMatrix(int width, int height) {
    float aspectRatio = static_cast<float>(width) / height;
    float viewWidth = 20.0f * cameraZoom;
    float viewHeight = viewWidth / aspectRatio;

    projection = glm::ortho(
        viewWidth / 2.0f, viewWidth / 2.0f,
        viewHeight / 2.0f, viewHeight / 2.0f,
        -1.0f, 1.0f
    );

    GLuint transformLoc = glGetUniformLocation(shaderProgram, "transform");
    glUseProgram(shaderProgram);
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

void Game::updateProjectionMatrix(int width, int height) {
    float aspectRatio = static_cast<float>(width) / height;
    float viewWidth = 20.0f * cameraZoom;
    float viewHeight = viewWidth / aspectRatio;

    auto player = world.getPlayerById(clientId);
    if (!player) return;

    glm::vec2 playerPos = player->getPosition();

    projection = glm::ortho(
        playerPos.x - viewWidth / 2.0f, playerPos.x + viewWidth / 2.0f,
        playerPos.y - viewHeight / 2.0f, playerPos.y + viewHeight / 2.0f,
        -1.0f, 1.0f
    );

    GLuint transformLoc = glGetUniformLocation(shaderProgram, "transform");
    glUseProgram(shaderProgram);
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

void Game::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height); // Update the OpenGL viewport to the new window size

    Game* game = static_cast<Game*>(glfwGetWindowUserPointer(window));

    // Recalculate the projection matrix with the new window size
    game->updateProjectionMatrix(width, height);

    // If a text renderer exists, update its screen size to match the new dimensions
    if (game->textRenderer) {
        game->textRenderer->updateScreenSize(width, height);
    }

    // If the tile selection UI is visible, ensure it's also updated
    if (game->tileSelectionVisible && game->currentMode == Mode::LevelEdit) {
        game->renderTileSelectionUI();  // Redraw or adjust the tile selection UI based on the new size
    }
}
