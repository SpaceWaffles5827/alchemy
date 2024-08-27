#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <alchemy/renderable.h>

class GameObject : public Renderable {
public:
    GameObject(const glm::vec3& pos, const glm::vec3& rot, float width, float height, GLuint textureID,
        const glm::vec2& texTopLeft = glm::vec2(0.0f, 1.0f),
        const glm::vec2& texBottomRight = glm::vec2(1.0f, 0.0f))
        : Renderable(width, height, textureID, texTopLeft, texBottomRight, pos, rot) {}

    virtual ~GameObject() = default;

    void update(float deltaTime) {
        // Update logic specific to GameObject
    }
};

#endif // GAMEOBJECT_H
