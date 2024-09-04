#ifndef MOB_H
#define MOB_H

#include "../alchemy/renderable.h"
#include "../glm/glm.hpp"

enum class MobState { IDLE, WALKING, ATTACKING, DEAD };

enum class Direction { NORTH, SOUTH, EAST, WEST };

class Mob : public Renderable {
  public:
    Mob();
    ~Mob();

    // Getters
    MobState getState() const { return state; }
    Direction getDirection() const { return direction; }

    // Setters
    void setState(MobState newState) { state = newState; }
    void setDirection(Direction newDirection) { direction = newDirection; }

    void update(float deltaTime);

    int getHealth();
    void setHealth(int healthValue);

    // Apply knockback effect with smooth animation
    void applyKnockback(const glm::vec2 &knockbackDirection,
                        float knockbackStrength, float duration);

  private:
    MobState state;
    Direction direction;
    const float speed = 2.0f;
    glm::vec3 velocity;
    int health;

    // Knockback animation variables
    glm::vec3 knockbackStartPos;
    glm::vec3 knockbackEndPos;
    float knockbackTime;
    float knockbackDuration;
    bool isBeingKnockedBack;

    // Easing functions
    float easeOutCubic(float t);
    float easeInCubic(float t);
};

#endif // MOB_H
