#include <alchemy/player.h>
#include <alchemy/global.h>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

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

void Player::attack(glm::vec2 mousePos) {
    int width, height;
    GraphicsContext::getInstance().getWindowSize(width, height);

    // Retrieve the first weapon using a reference
    const std::vector<std::shared_ptr<Soward>> &weapons =
        World::getInstance().getWeapons();
    if (weapons.empty()) {
        std::cerr << "No weapons available for attack." << std::endl;
        return;
    }
    auto &playerWeapon = weapons[0];

    // Calculate the center of the window (player's position)
    glm::vec2 playerPos(width / 2.0f, height / 2.0f);

    // Calculate direction from player position to mouse position, in screen
    // coordinates
    glm::vec2 direction = mousePos - playerPos;

    // Invert the y-component to switch from clockwise to counterclockwise
    direction.y = -direction.y;

    // Normalize direction if it's not a zero vector
    if (glm::length(direction) > 0) {
        direction = glm::normalize(direction);
    }

    // Calculate the angle in degrees (counterclockwise)
    float angle = atan2(direction.y, direction.x) * 180.0f / M_PI;
    if (angle < 0) {
        angle += 360.0f; // Convert negative angles to positive
    }

    // Determine the start and end rotation based on the angle
    glm::vec3 startRotation = glm::vec3(0.0f, 0.0f, 0.0f - 45.0f + angle - 90);
    glm::vec3 endRotation =
        glm::vec3(0.0f, 0.0f, 0.0f - 45.0f + angle + 180 - 90);

    // Flip start and stop angles if the angle is between 270 and 90 degrees
    if (angle >= 270.0f || angle < 90.0f) {
        std::swap(startRotation.z, endRotation.z);
    }

    // Start the animation with a duration of 0.5 seconds
    playerWeapon->startAnimation(glm::vec3(0.0f, 0.0f, 0.0f), // Start position
                                 glm::vec3(0.0f, 0.0f, 0.0f), // End position
                                 startRotation,               // Start rotation
                                 endRotation,                 // End rotation
                                 0.2f,                        // Duration
                                 true // Hide after animation
    );

    // Output direction in human-readable format
    std::cout << "Attack direction: "
              << (angle >= 337.5f || angle < 22.5f ? "Right"
                  : angle < 67.5f                  ? "Bottom Right"
                  : angle < 112.5f                 ? "Bottom"
                  : angle < 157.5f                 ? "Bottom Left"
                  : angle < 202.5f                 ? "Left"
                  : angle < 247.5f                 ? "Top Left"
                  : angle < 292.5f                 ? "Top"
                                                   : "Top Right")
              << std::endl;
}
