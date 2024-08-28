#include <alchemy/mob.h>
#include <alchemy/graphicsContext.h>
#include <iostream>

Mob::Mob() {
    GLuint defaultTextureID = GraphicsContext::getInstance().loadTexture("textures/mobs/CATSPRITESHEET_Orange.png");
    setTexture(defaultTextureID);
    setTextureTile(0, 0, 4, 128, 416, 32, 32);

    state = MobState::WALKING;  // Set initial state to walking
    direction = Direction::EAST; // Set initial direction to east
    velocity = glm::vec3(0.0f);  // Initial velocity is zero
}

Mob::~Mob() {
    // Cleanup code here (if needed)
}

void Mob::update(float deltaTime) {
    static float animationTime = 0.0f;
    static int currentFrame = 0;

    if (state == MobState::WALKING || state == MobState::IDLE) {
        // Increment the animation time
        animationTime += deltaTime;

        // Update the current frame every 0.1 seconds (adjust as needed)
        if (animationTime >= 0.1f) {
            currentFrame = (currentFrame + 1) % 8; // There are 8 frames in total for walking animation
            animationTime = 0.0f;
        }

        int spriteRow = 0;

        // Determine the correct sprite row based on the direction and state
        if (state == MobState::IDLE) {
            switch (direction) {
            case Direction::SOUTH: spriteRow = 0; break; // Idle facing down
            case Direction::EAST: spriteRow = 1; break;  // Idle facing right
            case Direction::WEST: spriteRow = 2; break;  // Idle facing left
            case Direction::NORTH: spriteRow = 3; break; // Idle facing up
            default: break;
            }
        }
        else if (state == MobState::WALKING) {
            // Set the correct row for walking
            switch (direction) {
            case Direction::SOUTH: spriteRow = currentFrame < 4 ? 5 : 6; break; // Walking down
            case Direction::EAST: spriteRow = currentFrame < 4 ? 9 : 10; break; // Walking right
            case Direction::WEST: spriteRow = currentFrame < 4 ? 7 : 8; break; // Walking left
            case Direction::NORTH: spriteRow = currentFrame < 4 ? 11 : 12; break; // Walking up
            default: break;
            }
        }

        // Set the texture tile to the correct frame in the row
        int frameInRow = currentFrame % 4;
        setTextureTile(frameInRow, spriteRow, 4, 128, 416, 32, 32);

        // Update the mob's position if it's walking
        if (state == MobState::WALKING) {
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
    }
    else if (state == MobState::IDLE) {
        // If idle, reset the animation frame to the first frame of the idle row
        currentFrame = 0;
    }
}
