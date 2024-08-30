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

private:
  MobState state;
  Direction direction;
  const float speed = 2.0f; // Speed at which the mob moves, in units per second
  glm::vec3 velocity;       // Velocity vector for the mob's movement
};

#endif // MOB_H
