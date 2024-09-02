#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include <memory>
#include <algorithm>
#include "gameObject.h"
#include "../alchemy/player.h"
#include "../alchemy/soward.h"
#include "../glm/glm.hpp"
#include "../alchemy/mob.h"

class World {
public:
  static World &getInstance() {
    static World instance;
    return instance;
  }

  World(const World &) = delete;
  void operator=(const World &) = delete;

  void addObject(std::shared_ptr<GameObject> object);
  void addPlayer(std::shared_ptr<Player> object);
  void addWeapon(std::shared_ptr<Soward> object);
  void addMob(std::shared_ptr<Mob> object);

  void updatePlayerPosition(int clientId, const glm::vec3 &newPosition);
  void update(float deltaTime);
  void initTileView(int width, int height, float tileSize, GLuint textureID1);
  void clearObjects();
  void clearPlayers();

  const std::vector<std::shared_ptr<GameObject>> &getObjects() const;
  const std::vector<std::shared_ptr<Player>> &getPlayers() const;
  const std::vector<std::shared_ptr<Soward>> &getWeapons() const;
  const std::vector<std::shared_ptr<Mob>> &getMobs() const;
  std::shared_ptr<Player> getPlayerById(int clientId) const;
  void eraseObject(const glm::vec3 &position);

private:
  World() = default;
  ~World() = default;

  std::vector<std::shared_ptr<Mob>> mobs;
  std::vector<std::shared_ptr<GameObject>> objects;
  std::vector<std::shared_ptr<Player>> players;
  std::vector<std::shared_ptr<Soward>> weapons;
};

#endif // WORLD_H
