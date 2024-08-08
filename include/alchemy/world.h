#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include <memory>
#include "GameObject.h"
#include <glm/glm.hpp>

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

    void initTileView(int tileCountX, int tileCountY, float tileSize) {
        float startX = 0;
        float startY = 0;

        for (int y = 0; y < tileCountY; ++y) {
            for (int x = 0; x < tileCountX; ++x) {
                int worldX = startX + x * (tileSize / 5);
                int worldY = startY + y * (tileSize / 5);

                std::shared_ptr<GameObject> tile = std::make_shared<GameObject>(
                    glm::vec3(worldX, worldY, 0.0f),  // position
                    glm::vec3(0.0f),                  // rotation
                    tileSize,                         // width
                    tileSize                          // height
                    );

                // Add the tile if the sum of x and y is even to create a checkerboard pattern
                if ((x + y) % 2 == 0) {
                    addObject(tile);
                }
            }
        }
    }

private:
    std::vector<std::shared_ptr<GameObject>> objects;
};

#endif // WORLD_H
