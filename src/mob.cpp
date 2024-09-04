#include <alchemy/mob.h>
#include <alchemy/graphicsContext.h>
#include <iostream>

Mob::Mob() {
    GLuint defaultTextureID = GraphicsContext::getInstance().loadTexture(
        "textures/mobs/CATSPRITESHEET_Orange.png");
    setTexture(defaultTextureID);
    setTextureTile(0, 0, 4, 128, 416, 32, 32);

    state = MobState::WALKING;   // Set initial state to walking
    direction = Direction::EAST; // Set initial direction to east
    velocity = glm::vec3(0.0f);  // Initial velocity is zero
    health = 100;

    // Initialize knockback variables
    isBeingKnockedBack = false;
    knockbackTime = 0.0f;
    knockbackDuration = 0.0f;
    setPosition(glm::vec3(0.0f, 0.0f, 1.0f));
}

Mob::~Mob() {
    // Cleanup code here (if needed)
}

float Mob::easeOutCubic(float t) { return 1 - pow(1 - t, 3); }

float Mob::easeInCubic(float t) { return t * t * t; }

void Mob::update(float deltaTime) {
    static float animationTime = 0.0f;
    static int currentFrame = 0;

    if (isBeingKnockedBack) {
        // Update the knockback animation
        knockbackTime += deltaTime;
        if (knockbackTime >= knockbackDuration) {
            // Stop knockback animation
            knockbackTime = knockbackDuration;
            isBeingKnockedBack = false;
        }

        // Apply easing for smoother knockback effect
        float t = knockbackTime / knockbackDuration;
        float easedT = easeOutCubic(t);
        glm::vec3 newPos = glm::mix(knockbackStartPos, knockbackEndPos, easedT);
        setPosition(newPos);

        // Dynamic scaling for punchier effect
        float scale =
            1.0f - (0.1f * (1.0f - easedT)); // Scale down during knockback
        setScale(glm::vec3(scale));
    }

    if (state == MobState::WALKING || state == MobState::IDLE) {
        // Increment the animation time
        animationTime += deltaTime;

        // Update the current frame every 0.1 seconds (adjust as needed)
        if (animationTime >= 0.1f) {
            currentFrame =
                (currentFrame + 1) %
                8; // There are 8 frames in total for walking animation
            animationTime = 0.0f;
        }

        int spriteRow = 0;

        // Determine the correct sprite row based on the direction and state
        if (state == MobState::IDLE) {
            switch (direction) {
            case Direction::SOUTH:
                spriteRow = 0;
                break; // Idle facing down
            case Direction::EAST:
                spriteRow = 1;
                break; // Idle facing right
            case Direction::WEST:
                spriteRow = 2;
                break; // Idle facing left
            case Direction::NORTH:
                spriteRow = 3;
                break; // Idle facing up
            default:
                break;
            }
        } else if (state == MobState::WALKING) {
            // Set the correct row for walking
            switch (direction) {
            case Direction::SOUTH:
                spriteRow = currentFrame < 4 ? 5 : 6;
                break; // Walking down
            case Direction::EAST:
                spriteRow = currentFrame < 4 ? 9 : 10;
                break; // Walking right
            case Direction::WEST:
                spriteRow = currentFrame < 4 ? 7 : 8;
                break; // Walking left
            case Direction::NORTH:
                spriteRow = currentFrame < 4 ? 11 : 12;
                break; // Walking up
            default:
                break;
            }
        }

        // Set the texture tile to the correct frame in the row
        int frameInRow = currentFrame % 4;
        setTextureTile(frameInRow, spriteRow, 4, 128, 416, 32, 32);

        // Update the mob's position if it's walking
        if (!isBeingKnockedBack && state == MobState::WALKING) {
            glm::vec3 currentPosition = getPosition();

            switch (direction) {
            case Direction::EAST:
                currentPosition.x += speed * deltaTime;
                if (currentPosition.x >= 9.0f) {
                    currentPosition.x = 9.0f;
                    direction = Direction::WEST;
                }
                break;
            case Direction::WEST:
                currentPosition.x -= speed * deltaTime;
                if (currentPosition.x <= 0.0f) {
                    currentPosition.x = 0.0f;
                    direction = Direction::EAST;
                }
                break;
            default:
                break;
            }

            setPosition(currentPosition);
        }
    } else if (state == MobState::IDLE) {
        // If idle, reset the animation frame to the first frame of the idle row
        currentFrame = 0;
    }
}

void Mob::applyKnockback(const glm::vec2 &knockbackDirection,
                         float knockbackStrength, float duration) {
    // Set the start position for the knockback animation
    knockbackStartPos = getPosition();

    // Negate the knockbackDirection to push the mob in the correct direction
    glm::vec2 flippedKnockbackDirection = -knockbackDirection;

    // Calculate the end position based on the flipped knockback direction
    knockbackEndPos =
        knockbackStartPos +
        glm::vec3(flippedKnockbackDirection.x * knockbackStrength,
                  flippedKnockbackDirection.y * knockbackStrength, 0.0f);

    // Set knockback duration and start the animation
    knockbackTime = 0.0f;
    knockbackDuration = duration;
    isBeingKnockedBack = true;
}

int Mob::getHealth() { return health; }

void Mob::setHealth(int healthValue) { health = healthValue; }
