#ifndef PLAYER_H
#define PLAYER_H

#include "../GLEW/glew.h"
#include "../GLFW/glfw3.h"
#include "../alchemy/gameObject.h"
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtc/type_ptr.hpp"
#include "../stb/stb_image.h"
#include <iostream>

enum class PlayerState { Idle, Walking, Attacking };

enum class PlayerDirection { North, South, East, West };

class Player : public GameObject {
public:
    Player(int clientId = 0, const glm::vec3 &color = glm::vec3(1.0f),
         float x = 0.0f, float y = 0.0f, float width = 1.0f,
         float height = 1.0f, GLuint textureID = 0, int health = 100);
  ~Player();

  int getClientId() const;

  void attack(glm::vec2 mousePos);

  int getHealth();
  void setHealth(int healthValue);

private:
  int clientId;
  bool keyReleased[GLFW_KEY_LAST];

  int health;

  PlayerState currentState;
  PlayerDirection currentDirection;
};

#endif // PLAYER_H
