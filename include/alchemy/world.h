#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include <memory>
#include <algorithm>
#include "GameObject.h"
#include <alchemy/Player.h>
#include <glm/glm.hpp>

class World {
public:
    static World& getInstance() {
        static World instance;
        return instance;
    }

    // Delete copy constructor and assignment operator to prevent copying
    World(const World&) = delete;
    void operator=(const World&) = delete;

    void addObject(std::shared_ptr<GameObject> object);
    void addPlayer(std::shared_ptr<Player> object);
    void updatePlayerPosition(int clientId, const glm::vec3& newPosition);
    void update(float deltaTime);
    void initTileView(int width, int height, float tileSize, GLuint textureID1, GLuint textureID2);
    void clearObjects();
    void clearPlayers();

    const std::vector<std::shared_ptr<GameObject>>& getObjects() const;
    const std::vector<std::shared_ptr<Player>>& getPlayers() const;
    std::shared_ptr<Player> getPlayerById(int clientId) const;
    void eraseObject(const glm::vec3& position);

private:
    World() = default;
    ~World() = default;

    std::vector<std::shared_ptr<GameObject>> objects;
    std::vector<std::shared_ptr<Player>> players;
};

#endif // WORLD_H
