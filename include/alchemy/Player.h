#ifndef PLAYER_H
#define PLAYER_H

#include <GLEW/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Player {
public:
    Player() : clientId(0), color(1.0f, 1.0f, 1.0f), position(0.0f, 0.0f) {}
    Player(int clientId, const glm::vec3& color, float x = 0.0f, float y = 0.0f);
    ~Player();

    void render(GLuint shaderProgram, GLuint VAO) const;
    void updatePosition(float x, float y);
    glm::vec2 getPosition() const;
    int getClientId() const;

private:
    int clientId;
    glm::vec3 color;
    glm::vec2 position;
};

#endif
