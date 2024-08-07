#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include <memory>
#include "GameObject.h"

class World {
public:
    World() = default;
    ~World() = default;

    void addObject(std::shared_ptr<GameObject> object) {
        objects.push_back(object);
    }

    void update(float deltaTime) {
        for (auto& obj : objects) {
            obj->update(deltaTime);
        }
    }

    // Renders all game objects within the world
    void render(GLuint shaderProgram, GLuint VAO, const glm::mat4& projection) const {
        for (const auto& obj : objects) {
            obj->render(shaderProgram, VAO, projection);
        }
    }

private:
    std::vector<std::shared_ptr<GameObject>> objects; 
};

#endif // WORLD_H
