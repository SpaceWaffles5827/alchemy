#include <alchemy/inputManager.h>
#include <alchemy/global.h>
#include <alchemy/hotbar.h>

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
    glfwSetScrollCallback(GraphicsContext::getInstance().getWindow(), scroll_callback);
    glfwSetMouseButtonCallback(GraphicsContext::getInstance().getWindow(), mouse_button_callback);
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

    static GLuint originalTextureID = 0;
    static std::string originalItemName;
    static int originalSlotIndex = -1;

    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            if (Inventory::getInstance().getIsVisable() || HotBar::getInstance().getIsVisable()) {
                int slotIndex = Inventory::getInstance().getSlotIndexAt(worldX, worldY);
                if (slotIndex != -1) {
                    inputManager->setDraggingFromHotbar(false);
                }
                else {
                    slotIndex = HotBar::getInstance().getSlotIndexAt(worldX, worldY);
                    if (slotIndex != -1) {
                        inputManager->setDraggingFromHotbar(true);
                    }
                }

                if (slotIndex != -1 && !inputManager->isDragging) {
                    originalSlotIndex = slotIndex;
                    originalTextureID = inputManager->isDraggingFromHotbar() ? HotBar::getInstance().getHotBarSlots()[slotIndex].getTextureID()
                        : Inventory::getInstance().getInventorySlots()[slotIndex].getTextureID();
                    originalItemName = inputManager->isDraggingFromHotbar() ? HotBar::getInstance().getItemInSlot(slotIndex)
                        : Inventory::getInstance().getItemInSlot(slotIndex);

                    if (inputManager->isDraggingFromHotbar()) {
                        HotBar::getInstance().getHotBarSlots()[slotIndex].setTexture(0);
                        HotBar::getInstance().getHotBarSlots()[slotIndex].setItem("");
                    }
                    else {
                        Inventory::getInstance().getInventorySlots()[slotIndex].setTexture(0);
                        Inventory::getInstance().getInventorySlots()[slotIndex].setItem("");
                    }

                    InputManager::getInstance().setSelectedSlotIndex(slotIndex);
                    InputManager::getInstance().setDraggingTextureId(originalTextureID);
                    InputManager::getInstance().setDraggingItemName(originalItemName);
                    InputManager::getInstance().setIsDraggingItemVisable(true);

                    inputManager->isDragging = true;
                    InputManager::getInstance().setDraggingStartPos(glm::vec2(worldX, worldY));
                }
            }
        }
        else if (action == GLFW_RELEASE && inputManager->isDragging) {
            int targetSlotIndex = Inventory::getInstance().getSlotIndexAt(worldX, worldY);
            bool targetIsHotbar = false;

            if (targetSlotIndex == -1) {
                targetSlotIndex = HotBar::getInstance().getSlotIndexAt(worldX, worldY);
                targetIsHotbar = true;
            }

            if (targetSlotIndex != -1 && targetSlotIndex != originalSlotIndex) {
                auto& originalSlot = inputManager->isDraggingFromHotbar() ? HotBar::getInstance().getHotBarSlots()[originalSlotIndex]
                    : Inventory::getInstance().getInventorySlots()[originalSlotIndex];
                auto& targetSlot = targetIsHotbar ? HotBar::getInstance().getHotBarSlots()[targetSlotIndex]
                    : Inventory::getInstance().getInventorySlots()[targetSlotIndex];

                GLuint targetTextureID = targetSlot.getTextureID();
                std::string targetItemName = targetSlot.getItem();
                bool targetIsVisable = targetSlot.getIsVisable();

                // Swap the items
                targetSlot.setTexture(InputManager::getInstance().getDragTextureId());
                targetSlot.setItem(InputManager::getInstance().getDraggingItemName());

                targetSlot.setIsVisable(true);

                originalSlot.setTexture(targetTextureID);
                originalSlot.setItem(targetItemName);
                originalSlot.setIsVisable(targetIsVisable);
            }
            else {
                auto& originalSlot = inputManager->isDraggingFromHotbar() ? HotBar::getInstance().getHotBarSlots()[originalSlotIndex]
                    : Inventory::getInstance().getInventorySlots()[originalSlotIndex];
                originalSlot.setTexture(originalTextureID);
                originalSlot.setItem(originalItemName);

                if (inputManager->isDraggingFromHotbar()) {
                    originalSlot.setIsVisable(true);
                }
            }

            inputManager->isDragging = false;
            InputManager::getInstance().setSelectedSlotIndex(-1);
            InputManager::getInstance().setDraggingTextureId(0);
            InputManager::getInstance().setDraggingItemName("");
            InputManager::getInstance().setIsDraggingItemVisable(true);
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
            glm::vec4 worldCoords = glm::inverse(GraphicsContext::getInstance().getProjection()) * ndcCoords;

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

        // Handle chat-related input
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

        // Handle alphabetic keys (A-Z)
        for (int key = GLFW_KEY_A; key <= GLFW_KEY_Z; ++key) {
            if (glfwGetKey(GraphicsContext::getInstance().getWindow(), key) == GLFW_PRESS && keyReleased[key]) {
                bool shiftPressed = glfwGetKey(GraphicsContext::getInstance().getWindow(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(GraphicsContext::getInstance().getWindow(), GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;
                char c = static_cast<char>(key);
                if (!shiftPressed) {
                    c += 32;  // Convert to lowercase
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

        // Handle numeric keys (0-9)
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

        // Handle space key
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
        // Player movement and action handling
        std::shared_ptr<Player> player = World::getInstance().getPlayerById(game.getClientId());
        if (player) {
            static GLuint runningTextureID = GraphicsContext::getInstance().loadTexture("textures/player/playerRunning.png");
            static GLuint idleTextureID = GraphicsContext::getInstance().loadTexture("textures/player/playerIdle.png");

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
            Inventory::getInstance().setIsVisable(false);
            Chat::getInstance().setChatModeActive(false);
            bKeyReleased = false;
        }
        if (glfwGetKey(GraphicsContext::getInstance().getWindow(), GLFW_KEY_B) == GLFW_RELEASE) {
            bKeyReleased = true;
        }

        if (glfwGetKey(GraphicsContext::getInstance().getWindow(), GLFW_KEY_TAB) == GLFW_PRESS && tabKeyReleased) {
            Inventory::getInstance().setIsVisable(!Inventory::getInstance().getIsVisable());
            Chat::getInstance().setChatModeActive(false);
            tabKeyReleased = false;
        }
        if (glfwGetKey(GraphicsContext::getInstance().getWindow(), GLFW_KEY_TAB) == GLFW_RELEASE) {
            tabKeyReleased = true;
        }

        if (glfwGetKey(GraphicsContext::getInstance().getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS && escKeyReleased) {
            if (Inventory::getInstance().getIsVisable()) {
                Inventory::getInstance().setIsVisable(false);
            }
            else {
                Chat::getInstance().setChatModeActive(false);
            }
            escKeyReleased = false;
        }
        if (glfwGetKey(GraphicsContext::getInstance().getWindow(), GLFW_KEY_ESCAPE) == GLFW_RELEASE) {
            escKeyReleased = true;
        }

        // Handle number keys (1-9) for selecting hotbar slots
        for (int key = GLFW_KEY_1; key <= GLFW_KEY_9; ++key) {
            if (glfwGetKey(GraphicsContext::getInstance().getWindow(), key) == GLFW_PRESS && keyReleased[key]) {
                int slotIndex = key - GLFW_KEY_1;
                HotBar::getInstance().setSelectedSlotIndex(slotIndex);
                keyReleased[key] = false;
            }
            if (glfwGetKey(GraphicsContext::getInstance().getWindow(), key) == GLFW_RELEASE) {
                keyReleased[key] = true;
            }
        }
    }
}

void InputManager::handleWorldInteraction(double xpos, double ypos, int width, int height) {
    if (game.getGameMode() == Mode::LevelEdit) {
        float xNDC = static_cast<float>((2.0 * xpos) / width - 1.0);
        float yNDC = static_cast<float>(1.0 - (2.0 * ypos) / height);

        glm::vec4 ndcCoords = glm::vec4(xNDC, yNDC, 0.0f, 1.0f);
        glm::vec4 worldCoords = glm::inverse(GraphicsContext::getInstance().getProjection()) * ndcCoords;

        float snappedX = std::round(worldCoords.x);
        float snappedY = std::round(worldCoords.y);

        std::shared_ptr<GameObject> gameObjectAdding = std::make_shared<GameObject>(
            glm::vec3(snappedX, snappedY, 0.0f),
            glm::vec3(0.0f),
            1.0f,
            1.0f,
            GraphicsContext::getInstance().getTextureID2());

        gameObjectAdding->setTextureTile(0, 0, 8, 256, 256, 32, 32);
        World::getInstance().addObject(gameObjectAdding);
    }
}
