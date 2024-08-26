#include <alchemy/Player.h>
#include <alchemy/global.h>

Player::Player(int clientId, const glm::vec3& color, float x, float y, float width, float height, GLuint textureID)
    : GameObject(glm::vec3(x, y, 0.0f), glm::vec3(0.0f), width, height, textureID), clientId(clientId) {

    for (int i = 0; i < GLFW_KEY_LAST; ++i) {
        keyReleased[i] = true;
    }
}

Player::~Player() {
}

void Player::handleInput() {
    static GLuint runningTextureID = game.getGraphicsContext().loadTexture("aniwooRunning.png");
    static GLuint idleTextureID = game.getGraphicsContext().loadTexture("andiwooIdle.png");

    bool positionUpdated = false;
    bool isMoving = false;

    auto player = game.getWorld().getPlayerById(clientId);
    if (!player) return;

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
        game.getWorld().updatePlayerPosition(clientId, position);
        game.getNetworkManager().sendPlayerMovement(clientId, position.x, position.y);
    }
    else {
        game.getNetworkManager().sendHeatBeat(clientId);
    }
}

int Player::getClientId() const {
    return clientId;
}
