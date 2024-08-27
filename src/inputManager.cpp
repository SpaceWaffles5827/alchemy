#include <alchemy/inputManager.h>
#include <alchemy/global.h>

InputManager& InputManager::getInstance() {
    static InputManager instance;
    return instance;
}

InputManager::InputManager() : isDragging(false) {
    for (int i = 0; i < GLFW_KEY_LAST; ++i) {
        keyReleased[i] = true;
    }
}

InputManager::~InputManager() {
    // Cleanup if needed
}

bool InputManager::getIsDragging() {
    return isDragging;
}

void InputManager::registerCallbacks() {
    glfwSetWindowUserPointer(GraphicsContext::getInstance().getWindow(), this);
    glfwSetFramebufferSizeCallback(GraphicsContext::getInstance().getWindow(), framebuffer_size_callback);
    glfwSetScrollCallback(GraphicsContext::getInstance().getWindow(), scroll_callback);
    glfwSetMouseButtonCallback(GraphicsContext::getInstance().getWindow(), mouse_button_callback);
}

void InputManager::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    GraphicsContext::getInstance().updateProjectionMatrix(width, height);
    TextRenderer::getInstance().updateScreenSize(width, height);
}

void InputManager::scroll_callback(GLFWwindow* window, double xOffset, double yOffset) {
    GraphicsContext::getInstance().setCameraZoom(GraphicsContext::getInstance().getCameraZoom() + yOffset * -0.1f);
    if (GraphicsContext::getInstance().getCameraZoom() < 0.1f) GraphicsContext::getInstance().setCameraZoom(0.1f);
    if (GraphicsContext::getInstance().getCameraZoom() > 99.0f) GraphicsContext::getInstance().setCameraZoom(99.0f);

    int width, height;
    glfwGetWindowSize(window, &width, &height);

    GraphicsContext::getInstance().updateProjectionMatrix(width, height);
}

void InputManager::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    InputManager* inputManager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));

    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    int width, height;
    glfwGetWindowSize(window, &width, &height);

    float worldX = static_cast<float>(xpos);
    float worldY = static_cast<float>(ypos);

    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            if (game.getDispalyInventory()) {
                int slotIndex = Inventory::getInstance().getSlotIndexAt(worldX, worldY);
                if (slotIndex != -1 && !inputManager->isDragging) {
                    game.setSelectedSlotIndex(slotIndex);
                    game.setDraggingTextureId(Inventory::getInstance().getInventorySlots()[slotIndex].getTextureID());
                    game.setDraggingItemName(Inventory::getInstance().getItemInSlot(slotIndex));
                    inputManager->isDragging = true;
                    game.setDraggingStartPos(glm::vec2(worldX, worldY));
                }
            }
        }
        else if (action == GLFW_RELEASE && inputManager->isDragging) {
            if (game.getDispalyInventory()) {
                int slotIndex = Inventory::getInstance().getSlotIndexAt(worldX, worldY);
                if (slotIndex != -1 && slotIndex != game.getSelectedSlotIndex()) {
                    auto& sourceSlot = Inventory::getInstance().getInventorySlots()[game.getSelectedSlotIndex()];
                    auto& targetSlot = Inventory::getInstance().getInventorySlots()[slotIndex];

                    GLuint tempTextureID = targetSlot.getTextureID();
                    targetSlot.setTexture(sourceSlot.getTextureID());
                    sourceSlot.setTexture(tempTextureID);

                    std::string tempItemName = targetSlot.getItem();
                    targetSlot.setItem(sourceSlot.getItem());
                    sourceSlot.setItem(tempItemName);
                }

                inputManager->isDragging = false;
                game.setSelectedSlotIndex(-1);
                game.setDraggingTextureId(0);
                game.setDraggingItemName("");
            }
        }
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        if (game.getGameMode() == Mode::LevelEdit) {
            double xpos, ypos;
            glfwGetCursorPos(GraphicsContext::getInstance().getWindow(), &xpos, &ypos);

            int width, height;
            glfwGetWindowSize(GraphicsContext::getInstance().getWindow(), &width, &height);

            float xNDC = static_cast<float>((2.0 * xpos) / width - 1.0);
            float yNDC = static_cast<float>(1.0 - (2.0 * ypos) / height);

            glm::vec4 ndcCoords = glm::vec4(xNDC, yNDC, 0.0f, 1.0f);
            glm::vec4 worldCoords = glm::inverse(game.getProjection()) * ndcCoords;

            float snappedX = std::round(worldCoords.x);
            float snappedY = std::round(worldCoords.y);

            World::getInstance().eraseObject(glm::vec3(snappedX, snappedY, 0.0f));
        }
    }
}

void InputManager::handleInput() {
    static bool tabKeyReleased = true;
    static bool escKeyReleased = true;

    if (Chat::getInstance().isChatModeActive()) {
        static bool enterKeyReleased = true;
        static bool backspaceKeyReleased = true;

        if (glfwGetKey(GraphicsContext::getInstance().getWindow(), GLFW_KEY_ENTER) == GLFW_PRESS && enterKeyReleased) {
            Chat::getInstance().addMessage(Chat::getInstance().getCurrentMessage());
            Chat::getInstance().setCurrentMessage("");
            Chat::getInstance().setChatModeActive(false);
            enterKeyReleased = false;
        }
        if (glfwGetKey(GraphicsContext::getInstance().getWindow(), GLFW_KEY_ENTER) == GLFW_RELEASE) {
            enterKeyReleased = true;
        }

        if (glfwGetKey(GraphicsContext::getInstance().getWindow(), GLFW_KEY_BACKSPACE) == GLFW_PRESS && backspaceKeyReleased) {
            std::string currentMessage = Chat::getInstance().getCurrentMessage();
            if (!currentMessage.empty()) {
                currentMessage.pop_back();
                Chat::getInstance().setCurrentMessage(currentMessage);
            }
            backspaceKeyReleased = false;
        }
        if (glfwGetKey(GraphicsContext::getInstance().getWindow(), GLFW_KEY_BACKSPACE) == GLFW_RELEASE) {
            backspaceKeyReleased = true;
        }

        if (glfwGetKey(GraphicsContext::getInstance().getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS && escKeyReleased) {
            Chat::getInstance().setCurrentMessage("");
            Chat::getInstance().setChatModeActive(false);
            escKeyReleased = false;
        }
        if (glfwGetKey(GraphicsContext::getInstance().getWindow(), GLFW_KEY_ESCAPE) == GLFW_RELEASE) {
            escKeyReleased = true;
        }

        if (glfwGetKey(GraphicsContext::getInstance().getWindow(), GLFW_KEY_TAB) == GLFW_PRESS && tabKeyReleased) {
            Chat::getInstance().selectSuggestion();
            tabKeyReleased = false;
        }
        if (glfwGetKey(GraphicsContext::getInstance().getWindow(), GLFW_KEY_TAB) == GLFW_RELEASE) {
            tabKeyReleased = true;
        }

        for (int key = GLFW_KEY_A; key <= GLFW_KEY_Z; ++key) {
            if (glfwGetKey(GraphicsContext::getInstance().getWindow(), key) == GLFW_PRESS && keyReleased[key]) {
                bool shiftPressed = glfwGetKey(GraphicsContext::getInstance().getWindow(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(GraphicsContext::getInstance().getWindow(), GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;
                char c = static_cast<char>(key);
                if (!shiftPressed) {
                    c += 32;
                }
                std::string currentMessage = Chat::getInstance().getCurrentMessage();
                currentMessage += c;
                Chat::getInstance().setCurrentMessage(currentMessage);
                keyReleased[key] = false;
            }
            if (glfwGetKey(GraphicsContext::getInstance().getWindow(), key) == GLFW_RELEASE) {
                keyReleased[key] = true;
            }
        }

        for (int key = GLFW_KEY_0; key <= GLFW_KEY_9; ++key) {
            if (glfwGetKey(GraphicsContext::getInstance().getWindow(), key) == GLFW_PRESS && keyReleased[key]) {
                char c = static_cast<char>(key);
                std::string currentMessage = Chat::getInstance().getCurrentMessage();
                currentMessage += c;
                Chat::getInstance().setCurrentMessage(currentMessage);
                keyReleased[key] = false;
            }
            if (glfwGetKey(GraphicsContext::getInstance().getWindow(), key) == GLFW_RELEASE) {
                keyReleased[key] = true;
            }
        }

        if (glfwGetKey(GraphicsContext::getInstance().getWindow(), GLFW_KEY_SPACE) == GLFW_PRESS && keyReleased[GLFW_KEY_SPACE]) {
            std::string currentMessage = Chat::getInstance().getCurrentMessage();
            currentMessage += ' ';
            Chat::getInstance().setCurrentMessage(currentMessage);
            keyReleased[GLFW_KEY_SPACE] = false;
        }
        if (glfwGetKey(GraphicsContext::getInstance().getWindow(), GLFW_KEY_SPACE) == GLFW_RELEASE) {
            keyReleased[GLFW_KEY_SPACE] = true;
        }
    }
    else {
        std::shared_ptr<Player> player = World::getInstance().getPlayerById(game.getClientId());
        if (player) {
            static GLuint runningTextureID = GraphicsContext::getInstance().loadTexture("aniwooRunning.png");
            static GLuint idleTextureID = GraphicsContext::getInstance().loadTexture("andiwooIdle.png");

            bool positionUpdated = false;
            bool isMoving = false;

            glm::vec3 position = player->getPosition();
            float speed = 0.12f;

            static int frame = 0;
            static double lastTime = glfwGetTime();
            double currentTime = glfwGetTime();
            double frameDuration = 0.075;

            static int lastDirection = 0;
            glm::vec3 direction(0.0f);

            if (currentTime - lastTime >= frameDuration) {
                frame = (frame + 1) % 8;
                lastTime = currentTime;
            }

            bool moveUp = glfwGetKey(GraphicsContext::getInstance().getWindow(), GLFW_KEY_W) == GLFW_PRESS;
            bool moveDown = glfwGetKey(GraphicsContext::getInstance().getWindow(), GLFW_KEY_S) == GLFW_PRESS;
            bool moveLeft = glfwGetKey(GraphicsContext::getInstance().getWindow(), GLFW_KEY_A) == GLFW_PRESS;
            bool moveRight = glfwGetKey(GraphicsContext::getInstance().getWindow(), GLFW_KEY_D) == GLFW_PRESS;

            if (moveUp && !moveDown) {
                direction.y += 1.0f;
                lastDirection = 3;
                isMoving = true;
            }
            else if (moveDown && !moveUp) {
                direction.y -= 1.0f;
                lastDirection = 0;
                isMoving = true;
            }

            if (moveLeft && !moveRight) {
                direction.x -= 1.0f;
                lastDirection = 1;
                isMoving = true;
            }
            else if (moveRight && !moveLeft) {
                direction.x += 1.0f;
                lastDirection = 2;
                isMoving = true;
            }

            if (isMoving) {
                if (direction.x != 0 || direction.y != 0) {
                    direction = glm::normalize(direction);
                    position += direction * speed;
                    positionUpdated = true;

                    player->setTexture(runningTextureID);
                    player->setTextureTile(frame, lastDirection, 8, 512, 512, 64, 128);
                }
            }
            else {
                player->setTexture(idleTextureID);
                player->setTextureTile(frame, lastDirection, 8, 512, 512, 64, 128);
            }

            if (positionUpdated) {
                World::getInstance().updatePlayerPosition(game.getClientId(), position);
                NetworkManager::getInstance().sendPlayerMovement(game.getClientId(), position.x, position.y);
            }
            else {
                NetworkManager::getInstance().sendHeatBeat(game.getClientId());
            }
        }
        else {
            std::cerr << "Player with ID " << game.getClientId() << " not found." << std::endl;
        }

        static bool tKeyReleased = true;
        static bool slashKeyReleased = true;
        static bool bKeyReleased = true;

        if (glfwGetKey(GraphicsContext::getInstance().getWindow(), GLFW_KEY_T) == GLFW_PRESS && tKeyReleased) {
            Chat::getInstance().setChatModeActive(true);
            tKeyReleased = false;
        }
        if (glfwGetKey(GraphicsContext::getInstance().getWindow(), GLFW_KEY_T) == GLFW_RELEASE) {
            tKeyReleased = true;
        }

        if (glfwGetKey(GraphicsContext::getInstance().getWindow(), GLFW_KEY_SLASH) == GLFW_PRESS && slashKeyReleased) {
            Chat::getInstance().setChatModeActive(true);
            std::string currentMessage = Chat::getInstance().getCurrentMessage();
            currentMessage += '/';
            Chat::getInstance().setCurrentMessage(currentMessage);
            slashKeyReleased = false;
        }
        if (glfwGetKey(GraphicsContext::getInstance().getWindow(), GLFW_KEY_SLASH) == GLFW_RELEASE) {
            slashKeyReleased = true;
        }

        if (glfwGetKey(GraphicsContext::getInstance().getWindow(), GLFW_KEY_B) == GLFW_PRESS && bKeyReleased) {
            game.setDispalyInventory(false);
            Chat::getInstance().setChatModeActive(false);
            bKeyReleased = false;
        }
        if (glfwGetKey(GraphicsContext::getInstance().getWindow(), GLFW_KEY_B) == GLFW_RELEASE) {
            bKeyReleased = true;
        }

        if (glfwGetKey(GraphicsContext::getInstance().getWindow(), GLFW_KEY_TAB) == GLFW_PRESS && tabKeyReleased) {
            game.setDispalyInventory(!game.getDispalyInventory());
            Chat::getInstance().setChatModeActive(false);
            tabKeyReleased = false;
        }
        if (glfwGetKey(GraphicsContext::getInstance().getWindow(), GLFW_KEY_TAB) == GLFW_RELEASE) {
            tabKeyReleased = true;
        }

        if (glfwGetKey(GraphicsContext::getInstance().getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS && escKeyReleased) {
            if (game.getDispalyInventory()) {
                game.setDispalyInventory(false);
            }
            else {
                Chat::getInstance().setChatModeActive(false);
            }
            escKeyReleased = false;
        }
        if (glfwGetKey(GraphicsContext::getInstance().getWindow(), GLFW_KEY_ESCAPE) == GLFW_RELEASE) {
            escKeyReleased = true;
        }
    }
}

void InputManager::handleWorldInteraction(double xpos, double ypos, int width, int height) {
    if (game.getGameMode() == Mode::LevelEdit) {
        float xNDC = static_cast<float>((2.0 * xpos) / width - 1.0);
        float yNDC = static_cast<float>(1.0 - (2.0 * ypos) / height);

        glm::vec4 ndcCoords = glm::vec4(xNDC, yNDC, 0.0f, 1.0f);
        glm::vec4 worldCoords = glm::inverse(game.getProjection()) * ndcCoords;

        float snappedX = std::round(worldCoords.x);
        float snappedY = std::round(worldCoords.y);

        std::shared_ptr<GameObject> gameObjectAdding = std::make_shared<GameObject>(
            glm::vec3(snappedX, snappedY, 0.0f),
            glm::vec3(0.0f),
            1.0f,
            1.0f,
            game.gettextureID2());

        gameObjectAdding->setTextureTile(0, 0, 8, 256, 256, 32, 32);
        World::getInstance().addObject(gameObjectAdding);
    }
}
