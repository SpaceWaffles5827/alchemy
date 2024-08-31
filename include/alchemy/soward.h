#ifndef SOWARD_H
#define SOWARD_H

#include "../alchemy/renderable.h"
#include <glm/glm.hpp>

class Soward : public Renderable {
  public:
    // Constructors
    Soward();
    Soward(float width, float height, GLuint textureID,
           const glm::vec2 &texTopLeft = glm::vec2(0.0f, 1.0f),
           const glm::vec2 &texBottomRight = glm::vec2(1.0f, 0.0f),
           const glm::vec3 &initialPosition = glm::vec3(0.0f),
           const glm::vec3 &initialRotation = glm::vec3(0.0f));
    virtual ~Soward(); // Destructor

    // Overridable methods for specific Soward behavior
    void attack();
    void defend();
    void updateAnimation(float deltaTime); // Override to provide specific
                                           // update logic for Soward

    // Additional methods specific to Soward
    void specialAbility(); // Example of a special ability
    void startAnimation(const glm::vec3 &startPosition,
                        const glm::vec3 &endPosition,
                        const glm::vec3 &startRotation,
                        const glm::vec3 &endRotation, float duration, 
                        bool hideAfter);

  protected:
    int health;
    int mana;

    // Animation attributes
    glm::vec3 startPosition;
    glm::vec3 endPosition;
    glm::vec3 startRotation;
    glm::vec3 endRotation;
    float animationProgress;
    float totalAnimationDuration;
    bool hideAfterAnimation;
};

#endif // SOWARD_H
