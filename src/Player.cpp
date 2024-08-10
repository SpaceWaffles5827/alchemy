#include <alchemy/Player.h>

Player::Player(int clientId, const glm::vec3& color, float x, float y, float width, float height, GLuint textureID)
    : GameObject(glm::vec3(x, y, 0.0f), glm::vec3(0.0f), width, height, textureID), clientId(clientId) {
    // Additional initialization if needed
}

Player::~Player() {
}

int Player::getClientId() const {
    return clientId;
}
