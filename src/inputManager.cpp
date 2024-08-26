#include <alchemy/inputManager.h>
#include <alchemy/global.h>

InputManager::InputManager() {
    for (int i = 0; i < GLFW_KEY_LAST; ++i) {
        keyReleased[i] = true;
    }
    isDragging = false; // Initialize isDragging
}

InputManager::~InputManager() {
}

bool InputManager::getIsDragging() {
    return isDragging;
}

void InputManager::registerCallbacks() {
    glfwSetWindowUserPointer(game.getGraphicsContext().getWindow(), this); // Set the InputManager instance as the user pointer
    glfwSetFramebufferSizeCallback(game.getGraphicsContext().getWindow(), framebuffer_size_callback);
    glfwSetScrollCallback(game.getGraphicsContext().getWindow(), scroll_callback);
    glfwSetMouseButtonCallback(game.getGraphicsContext().getWindow(), mouse_button_callback);
}

void InputManager::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);

    // Recalculate the projection matrix with the new window size
    game.getGraphicsContext().updateProjectionMatrix(width, height);

    // If a text renderer exists, update its screen size to match the new dimensions
    if (game.getTextRender()) {
        game.getTextRender()->updateScreenSize(width, height);
    }
}

void InputManager::scroll_callback(GLFWwindow* window, double xOffset, double yOffset) {
    game.setCameraZoom(game.getCameraZoom() + yOffset * -0.1f);
    if (game.getCameraZoom() < 0.1f) game.setCameraZoom(0.1f);
    if (game.getCameraZoom() > 99.0f) game.setCameraZoom(99.0f);

    int width, height;
    glfwGetWindowSize(window, &width, &height);

    game.getGraphicsContext().updateProjectionMatrix(width, height);
}

void InputManager::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    InputManager* inputManager = static_cast<InputManager*>(glfwGetWindowUserPointer(window)); // Get the InputManager instance

    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    int width, height;
    glfwGetWindowSize(window, &width, &height);

    // Convert screen coordinates to the inventory UI coordinate system
    float worldX = static_cast<float>(xpos);
    float worldY = static_cast<float>(ypos);

    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            if (game.getDispalyInventory()) {
                int slotIndex = game.getPlayerInventory().getSlotIndexAt(worldX, worldY);
                if (slotIndex != -1 && !inputManager->isDragging) {
                    game.setSelectedSlotIndex(slotIndex);
                    game.setDraggingTextureId(game.getPlayerInventory().getInventorySlots()[slotIndex].getTextureID());
                    game.setDraggingItemName(game.getPlayerInventory().getItemInSlot(slotIndex));
                    inputManager->isDragging = true;
                    game.setDraggingStartPos(glm::vec2(worldX, worldY));
                }
            }
        }
        else if (action == GLFW_RELEASE && inputManager->isDragging) {
            if (game.getDispalyInventory()) {
                int slotIndex = game.getPlayerInventory().getSlotIndexAt(worldX, worldY);
                if (slotIndex != -1 && slotIndex != game.getSelectedSlotIndex()) {
                    // Swap the items between the slots
                    auto& sourceSlot = game.getPlayerInventory().getInventorySlots()[game.getSelectedSlotIndex()];
                    auto& targetSlot = game.getPlayerInventory().getInventorySlots()[slotIndex];

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
            glfwGetCursorPos(game.getGraphicsContext().getWindow(), &xpos, &ypos);

            int width, height;
            glfwGetWindowSize(game.getGraphicsContext().getWindow(), &width, &height);

            float xNDC = static_cast<float>((2.0 * xpos) / width - 1.0);
            float yNDC = static_cast<float>(1.0 - (2.0 * ypos) / height);

            glm::vec4 ndcCoords = glm::vec4(xNDC, yNDC, 0.0f, 1.0f);
            glm::vec4 worldCoords = glm::inverse(game.getProjection()) * ndcCoords;

            float snappedX = std::round(worldCoords.x);
            float snappedY = std::round(worldCoords.y);

            game.getWorld().eraseObject(glm::vec3(snappedX, snappedY, 0.0f));
        }
    }
}

void InputManager::handleInput() {
    static bool tabKeyReleased = true;
    static bool escKeyReleased = true;

    if (game.getChat().isChatModeActive()) {
        static bool enterKeyReleased = true;
        static bool backspaceKeyReleased = true;

        if (glfwGetKey(game.getGraphicsContext().getWindow(), GLFW_KEY_ENTER) == GLFW_PRESS && enterKeyReleased) {
            game.getChat().addMessage(game.getChat().getCurrentMessage());
            game.getChat().setCurrentMessage("");
            game.getChat().setChatModeActive(false);
            enterKeyReleased = false;
        }
        if (glfwGetKey(game.getGraphicsContext().getWindow(), GLFW_KEY_ENTER) == GLFW_RELEASE) {
            enterKeyReleased = true;
        }

        if (glfwGetKey(game.getGraphicsContext().getWindow(), GLFW_KEY_BACKSPACE) == GLFW_PRESS && backspaceKeyReleased) {
            std::string currentMessage = game.getChat().getCurrentMessage();
            if (!currentMessage.empty()) {
                currentMessage.pop_back();
                game.getChat().setCurrentMessage(currentMessage);
            }
            backspaceKeyReleased = false;
        }
        if (glfwGetKey(game.getGraphicsContext().getWindow(), GLFW_KEY_BACKSPACE) == GLFW_RELEASE) {
            backspaceKeyReleased = true;
        }

        if (glfwGetKey(game.getGraphicsContext().getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS && escKeyReleased) {
            game.getChat().setCurrentMessage("");
            game.getChat().setChatModeActive(false);
            escKeyReleased = false;
        }
        if (glfwGetKey(game.getGraphicsContext().getWindow(), GLFW_KEY_ESCAPE) == GLFW_RELEASE) {
            escKeyReleased = true;
        }

        if (glfwGetKey(game.getGraphicsContext().getWindow(), GLFW_KEY_TAB) == GLFW_PRESS && tabKeyReleased) {
            game.getChat().selectSuggestion(); // Handle tab in chat mode
            tabKeyReleased = false;
        }
        if (glfwGetKey(game.getGraphicsContext().getWindow(), GLFW_KEY_TAB) == GLFW_RELEASE) {
            tabKeyReleased = true;
        }

        // Process letter keys for chat
        for (int key = GLFW_KEY_A; key <= GLFW_KEY_Z; ++key) {
            if (glfwGetKey(game.getGraphicsContext().getWindow(), key) == GLFW_PRESS && keyReleased[key]) {
                bool shiftPressed = glfwGetKey(game.getGraphicsContext().getWindow(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(game.getGraphicsContext().getWindow(), GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;
                char c = static_cast<char>(key);
                if (!shiftPressed) {
                    c += 32; // Convert to lowercase if shift is not pressed
                }
                std::string currentMessage = game.getChat().getCurrentMessage();
                currentMessage += c;
                game.getChat().setCurrentMessage(currentMessage);
                keyReleased[key] = false;
            }
            if (glfwGetKey(game.getGraphicsContext().getWindow(), key) == GLFW_RELEASE) {
                keyReleased[key] = true;
            }
        }

        // Process number keys for chat
        for (int key = GLFW_KEY_0; key <= GLFW_KEY_9; ++key) {
            if (glfwGetKey(game.getGraphicsContext().getWindow(), key) == GLFW_PRESS && keyReleased[key]) {
                char c = static_cast<char>(key);
                std::string currentMessage = game.getChat().getCurrentMessage();
                currentMessage += c;
                game.getChat().setCurrentMessage(currentMessage);
                keyReleased[key] = false;
            }
            if (glfwGetKey(game.getGraphicsContext().getWindow(), key) == GLFW_RELEASE) {
                keyReleased[key] = true;
            }
        }

        // Process space key for chat
        if (glfwGetKey(game.getGraphicsContext().getWindow(), GLFW_KEY_SPACE) == GLFW_PRESS && keyReleased[GLFW_KEY_SPACE]) {
            std::string currentMessage = game.getChat().getCurrentMessage();
            currentMessage += ' ';
            game.getChat().setCurrentMessage(currentMessage);
            keyReleased[GLFW_KEY_SPACE] = false;
        }
        if (glfwGetKey(game.getGraphicsContext().getWindow(), GLFW_KEY_SPACE) == GLFW_RELEASE) {
            keyReleased[GLFW_KEY_SPACE] = true;
        }
    }
    else { // Handle input when chat mode is not active
        std::shared_ptr<Player> player = game.getWorld().getPlayerById(game.getClientId());
        if (player) {
            // Code that use to just make up this method 

            static GLuint runningTextureID = game.getGraphicsContext().loadTexture("aniwooRunning.png");
            static GLuint idleTextureID = game.getGraphicsContext().loadTexture("andiwooIdle.png");

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

            bool moveUp = glfwGetKey(game.getGraphicsContext().getWindow(), GLFW_KEY_W) == GLFW_PRESS;
            bool moveDown = glfwGetKey(game.getGraphicsContext().getWindow(), GLFW_KEY_S) == GLFW_PRESS;
            bool moveLeft = glfwGetKey(game.getGraphicsContext().getWindow(), GLFW_KEY_A) == GLFW_PRESS;
            bool moveRight = glfwGetKey(game.getGraphicsContext().getWindow(), GLFW_KEY_D) == GLFW_PRESS;

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
                game.getWorld().updatePlayerPosition(game.getClientId(), position);
                game.getNetworkManager().sendPlayerMovement(game.getClientId(), position.x, position.y);
            }
            else {
                game.getNetworkManager().sendHeatBeat(game.getClientId());
            }
        }
        else {
            std::cerr << "Player with ID " << game.getClientId() << " not found." << std::endl;
        }

        static bool tKeyReleased = true;
        static bool slashKeyReleased = true;
        static bool bKeyReleased = true;

        if (glfwGetKey(game.getGraphicsContext().getWindow(), GLFW_KEY_T) == GLFW_PRESS && tKeyReleased) {
            game.getChat().setChatModeActive(true);
            tKeyReleased = false;
        }
        if (glfwGetKey(game.getGraphicsContext().getWindow(), GLFW_KEY_T) == GLFW_RELEASE) {
            tKeyReleased = true;
        }

        if (glfwGetKey(game.getGraphicsContext().getWindow(), GLFW_KEY_SLASH) == GLFW_PRESS && slashKeyReleased) {
            game.getChat().setChatModeActive(true);
            std::string currentMessage = game.getChat().getCurrentMessage();
            currentMessage += '/';
            game.getChat().setCurrentMessage(currentMessage);
            slashKeyReleased = false;
        }
        if (glfwGetKey(game.getGraphicsContext().getWindow(), GLFW_KEY_SLASH) == GLFW_RELEASE) {
            slashKeyReleased = true;
        }

        if (glfwGetKey(game.getGraphicsContext().getWindow(), GLFW_KEY_B) == GLFW_PRESS && bKeyReleased) {
            game.setDispalyInventory(false);
            game.getChat().setChatModeActive(false);
            bKeyReleased = false;
        }
        if (glfwGetKey(game.getGraphicsContext().getWindow(), GLFW_KEY_B) == GLFW_RELEASE) {
            bKeyReleased = true;
        }

        if (glfwGetKey(game.getGraphicsContext().getWindow(), GLFW_KEY_TAB) == GLFW_PRESS && tabKeyReleased) {
            game.setDispalyInventory(!game.getDispalyInventory());
            game.getChat().setChatModeActive(false);
            tabKeyReleased = false;
        }
        if (glfwGetKey(game.getGraphicsContext().getWindow(), GLFW_KEY_TAB) == GLFW_RELEASE) {
            tabKeyReleased = true;
        }

        if (glfwGetKey(game.getGraphicsContext().getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS && escKeyReleased) {
            if (game.getDispalyInventory()) {
                game.setDispalyInventory(false);
            }
            else {
                game.getChat().setChatModeActive(false);
            }
            escKeyReleased = false;
        }
        if (glfwGetKey(game.getGraphicsContext().getWindow(), GLFW_KEY_ESCAPE) == GLFW_RELEASE) {
            escKeyReleased = true;
        }
    }
}
