#include "../include/alchemy/world.h"
#include <cstdlib>
#include <ctime>

void World::addObject(std::shared_ptr<GameObject> object) {
    objects.push_back(object);
}

void World::addPlayer(std::shared_ptr<Player> object) {
    players.push_back(object);
}

void World::addWeapon(std::shared_ptr<Soward> object) {
    weapons.push_back(object);
}

void World::addMob(std::shared_ptr<Mob> object) { mobs.push_back(object); }

void World::updatePlayerPosition(int clientId, const glm::vec3 &newPosition) {
    auto player = getPlayerById(clientId);
    if (player) {
        player->setPosition(newPosition);
    }
}

void World::update(float deltaTime) {
    for (auto &obj : objects) {
        obj->update(deltaTime);
    }

    for (auto &player : players) {
        player->update(deltaTime);
    }
}

void World::initTileView(int width, int height, float tileSize, GLuint textureID1) {
    srand(time(NULL));

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            int randomTileX = rand() % 8; // Random x between 0 and 7
            int randomTileY = rand() % 3; // Random y between 0 and 7

            auto tile = std::make_shared<GameObject>(
                glm::vec3(x * tileSize, y * tileSize, 0.0f), glm::vec3(0.0f),
                tileSize, tileSize, textureID1);

            tile->setTextureTile(randomTileX, randomTileY, 8, 256, 256, 32, 32);

            addObject(tile);
        }
    }
}

void World::clearObjects() { objects.clear(); }

void World::clearPlayers() { players.clear(); }

const std::vector<std::shared_ptr<GameObject>> &World::getObjects() const {
    return objects;
}

const std::vector<std::shared_ptr<Player>> &World::getPlayers() const {
    return players;
}

const std::vector<std::shared_ptr<Soward>> &World::getWeapons() const {
    return weapons;
}

const std::vector<std::shared_ptr<Mob>> &World::getMobs() const { return mobs; }

std::shared_ptr<Player> World::getPlayerById(int clientId) const {
    auto it = std::find_if(players.begin(), players.end(),
                           [clientId](const std::shared_ptr<Player> &player) {
                               return player->getClientId() == clientId;
                           });

    if (it != players.end()) {
        return *it;
    }

    return nullptr; // Return nullptr if the player with the given clientId is
                    // not found
}

void World::eraseObject(const glm::vec3 &position) {
    auto it =
        std::remove_if(objects.begin(), objects.end(),
                       [&position](const std::shared_ptr<GameObject> &obj) {
                           return obj->getPosition() == position;
                       });

    if (it != objects.end()) {
        objects.erase(it, objects.end());
        std::cout << "Object at position (" << position.x << ", " << position.y
                  << ", " << position.z << ") deleted." << std::endl;
    } else {
        std::cout << "No object found at position (" << position.x << ", "
                  << position.y << ", " << position.z << ")." << std::endl;
    }
}
