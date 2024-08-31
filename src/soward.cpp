#include "../include/alchemy/soward.h"
#include "../include/alchemy/graphicsContext.h"
#include "../include/alchemy/game.h"
#include "../include/alchemy/world.h"
#include <alchemy/global.h>
#include <iostream>

// Default Constructor
Soward::Soward() : Renderable() {
    textureID = GraphicsContext::getInstance().loadTexture(
        "textures/weapon/Cobalt_Sword.png");
    health = 100;
    mana = 50;
    animationProgress = 0.0f;
    totalAnimationDuration = 0.0f;
}

// Parameterized Constructor
Soward::Soward(float width, float height, GLuint textureID,
               const glm::vec2 &texTopLeft, const glm::vec2 &texBottomRight,
               const glm::vec3 &initialPosition,
               const glm::vec3 &initialRotation)
    : Renderable(width, height, textureID, texTopLeft, texBottomRight,
                 initialPosition, initialRotation) {
    health = 100;
    mana = 50;
    animationProgress = 0.0f;
    totalAnimationDuration = 0.0f;
}

// Destructor
Soward::~Soward() {
    // Cleanup code if required
}

// Attack method
void Soward::attack() {
    std::cout << "Soward attacks!" << std::endl;
    // Attack logic here
}

// Defend method
void Soward::defend() {
    std::cout << "Soward defends!" << std::endl;
    // Defense logic here
}

// Update method with deltaTime to handle animation
void Soward::updateAnimation(float deltaTime) {
    const std::shared_ptr<Player> clientPlayer =
        World::getInstance().getPlayerById(game.getClientId());

    if (animationProgress < totalAnimationDuration) {
        this->setIsVisable(true);
        float progress = animationProgress / totalAnimationDuration;

        // Calculate the current position and rotation
        glm::vec3 currentPosition =
            glm::mix(startPosition, endPosition, progress) +
            clientPlayer->getPosition();
        glm::vec3 currentRotation =
            glm::mix(startRotation, endRotation, progress);

        // Calculate the offset to move the sprite's bottom-left corner to the
        // origin
        glm::vec3 bottomLeftOffset(-width / 2.0f, -height / 2.0f, 0.0f);

        // Apply the translation to the origin, rotation, and translation back
        glm::mat4 transform = glm::mat4(1.0f);
        transform = glm::translate(
            transform, currentPosition); // Translate to the current position
        transform = glm::rotate(
            transform, glm::radians(currentRotation.z),
            glm::vec3(0.0f, 0.0f, 1.0f)); // Apply rotation around the origin
        transform = glm::translate(
            transform, -bottomLeftOffset); // Move the bottom-left corner back
                                           // to its original position

        // Extract the new position from the transformation matrix
        position = glm::vec3(transform[3]);

        rotation =
            currentRotation; // Assuming you only care about Z rotation here

        animationProgress += deltaTime;
    } else {
        this->setIsVisable(false);
        position = endPosition + clientPlayer->getPosition();
        rotation = endRotation;
    }
}

// Special ability method
void Soward::specialAbility() {
    std::cout << "Soward uses a special ability!" << std::endl;
    // Special ability logic here
}

// Start animation with specified start and end positions and rotations
void Soward::startAnimation(const glm::vec3 &startPosition,
                            const glm::vec3 &endPosition,
                            const glm::vec3 &startRotation,
                            const glm::vec3 &endRotation, float duration, 
                            bool hideAfter) {

    this->startPosition = startPosition;
    this->endPosition = endPosition;
    this->startRotation = startRotation;
    this->endRotation = endRotation;
    this->hideAfterAnimation = hideAfter;
    this->animationProgress = 0.0f;
    this->totalAnimationDuration = duration;
    std::cout << "Animation started with duration: " << duration << " seconds"
              << std::endl;
}
