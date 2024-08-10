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

    void initTileView(int width, int height, float tileSize, GLuint textureID1, GLuint textureID2) {
        srand(time(NULL));  // Seed the random number generator

        for (int x = 0; x < width; x++) {
            for (int y = 0; y < height; y++) {
                // Generate random tile coordinates
                int randomTileX = rand() % 8; // Random x between 0 and 7
                int randomTileY = rand() % 8; // Random y between 0 and 7

                // std::cout << "test: " << randomTileX;

                // Select texture based on position for variation
                GLuint selectedTexture = (x + y) % 2 == 0 ? textureID1 : textureID2;

                // Create the tile object
                auto tile = std::make_shared<GameObject>(
                    glm::vec3(x * tileSize, y * tileSize, 0.0f), // Position
                    glm::vec3(0.0f), // Rotation
                    tileSize, tileSize, // Width and Height
                    selectedTexture
                    );

                // Set texture tile with random coordinates
                tile->setTextureTile(randomTileX, 0, 8, 256, 256, 32, 32); // Assuming each tile is 128x128 in a 1024x1024 texture

                addObject(tile);
            }
        }
    }

    const std::vector<std::shared_ptr<GameObject>>& getObjects() const {
        return objects;
    }

private:
    std::vector<std::shared_ptr<GameObject>> objects;
};

#endif // WORLD_H
