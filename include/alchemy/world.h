#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include <memory>
#include <algorithm>
#include "GameObject.h"
#include "Player.h"
#include <glm/glm.hpp>

class World {
public:
    World() = default;
    ~World() = default;

    void addObject(std::shared_ptr<GameObject> object) {
        objects.push_back(object);
    }

    void addPlayer(std::shared_ptr<Player> object) {
        players.push_back(object);
    }

    void updatePlayerPosition(int clientId, const glm::vec3& newPosition) {
        auto player = getPlayerById(clientId);
        if (player) {
            player->setPosition(newPosition);
        }
    }

    void update(float deltaTime) {
        for (auto& obj : objects) {
            obj->update(deltaTime);
        }

        for (auto& player : players) {
            player->update(deltaTime);
        }
    }

    void initTileView(int width, int height, float tileSize, GLuint textureID1, GLuint textureID2) {
        srand(time(NULL));  // Seed the random number generator

        for (int x = 0; x < width; x++) {
            for (int y = 0; y < height; y++) {
                int randomTileX = rand() % 8; // Random x between 0 and 7
                int randomTileY = rand() % 3; // Random y between 0 and 7

                GLuint selectedTexture = (x + y) % 2 == 0 ? textureID1 : textureID2;

                auto tile = std::make_shared<GameObject>(
                    glm::vec3(x * tileSize, y * tileSize, 0.0f), // Position
                    glm::vec3(0.0f), // Rotation
                    tileSize, tileSize, // Width and Height
                    selectedTexture
                );

                tile->setTextureTile(randomTileX, randomTileY, 8, 256, 256, 32, 32);

                addObject(tile);
            }
        }
    }

    void clearObjects() {
        objects.clear();
    }

    void clearPlayers() {
        players.clear();
    }

    const std::vector<std::shared_ptr<GameObject>>& getObjects() const {
        return objects;
    }

    const std::vector<std::shared_ptr<Player>>& getPlayers() const {
        return players;
    }

    std::shared_ptr<Player> getPlayerById(int clientId) const {
        auto it = std::find_if(players.begin(), players.end(), [clientId](const std::shared_ptr<Player>& player) {
            return player->getClientId() == clientId;
            });

        if (it != players.end()) {
            return *it;
        }

        return nullptr; // Return nullptr if the player with the given clientId is not found
    }

    // Erase an object by its position
    void eraseObject(const glm::vec3& position) {
        auto it = std::remove_if(objects.begin(), objects.end(),
            [&position](const std::shared_ptr<GameObject>& obj) {
                return obj->getPosition() == position;
            });

        if (it != objects.end()) {
            objects.erase(it, objects.end());
            std::cout << "Object at position (" << position.x << ", " << position.y << ", " << position.z << ") deleted." << std::endl;
        }
        else {
            std::cout << "No object found at position (" << position.x << ", " << position.y << ", " << position.z << ")." << std::endl;
        }
    }

private:
    std::vector<std::shared_ptr<GameObject>> objects;
    std::vector<std::shared_ptr<Player>> players;
};

#endif // WORLD_H
