#include <alchemy/player.h>
#include <alchemy/global.h>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Player::Player(int clientId, const glm::vec3& color, float x, float y, float width, float height, GLuint textureID, int health)
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
    glm::vec2 playerScreenPos(width / 2.0f, height / 2.0f);

    // Calculate direction from player position to mouse position, in screen
    // coordinates
    glm::vec2 direction = mousePos - playerScreenPos;

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

    // **New: Check if attack hit any mobs within a 180-degree cone based on
    // mouse direction**

    // Define attack hit radius or range
    float attackRange = 2.0f; // Example range for detecting nearby mobs

    // Get all mobs in the world
    const std::vector<std::shared_ptr<Mob>> &mobs =
        World::getInstance().getMobs();

    // Use the click direction as the attack direction (normalized)
    glm::vec2 attackDirection = direction;

    // Loop through all mobs to check for collision with the attack
    for (const auto &mob : mobs) {
        // Calculate mob position relative to player
        glm::vec3 mobPos = mob->getPosition();
        glm::vec2 mobDirection =
            glm::vec2(mobPos.x - position.x, mobPos.y - position.y);

        // Check if the mob is within the attack range
        float distance = glm::length(mobDirection);
        if (distance > attackRange) {
            continue; // Mob is too far away, skip
        }

        // Normalize mob direction vector
        if (glm::length(mobDirection) > 0) {
            mobDirection = glm::normalize(mobDirection);
        }

        // Calculate the angle between the attack direction (from click) and the
        // mob's direction
        float dotProduct = glm::dot(attackDirection, mobDirection);
        float angleBetween =
            acos(dotProduct) * 180.0f / M_PI; // Convert from radians to degrees

        std::cout << "Angle between attack direction and mob: " << angleBetween
                  << " degrees\n";

        // Check if the mob is within the 180-degree cone (angle ≤ 90 degrees)
        if (angleBetween <= 90.0f) {
            // Mob is hit by the attack, deduct 10 HP
            int mobHealth = mob->getHealth();
            mobHealth -= 10; // Deduct health
            mob->setHealth(mobHealth);

            // Print out for debug purposes
            std::cout << "Mob hit! New health: " << mobHealth << std::endl;

            // Optionally, handle mob death if health is <= 0
            if (mobHealth <= 0) {
                std::cout << "Mob has been defeated!" << std::endl;
                // need to make it remove from the mob list eventaully
                mob->setIsVisable(false);
            }
        }
    }
}

int Player::getHealth() { return health; }

void Player::setHealth(int healthValue) { health = healthValue; }