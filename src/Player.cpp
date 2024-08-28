#include <alchemy/Player.h>
#include <alchemy/global.h>

Player::Player(int clientId, const glm::vec3& color, float x, float y, float width, float height, GLuint textureID)
    : GameObject(glm::vec3(x, y, 0.0f), glm::vec3(0.0f), width, height, textureID), clientId(clientId),
    currentState(PlayerState::Idle), currentDirection(PlayerDirection::South) {

    for (int i = 0; i < GLFW_KEY_LAST; ++i) {
        keyReleased[i] = true;
    }
}

Player::~Player() {
}

int Player::getClientId() const {
    return clientId;
}

void Player::attack() {
    std::cout << "Attack\n";
}
