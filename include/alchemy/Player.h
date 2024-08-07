#ifndef PLAYER_H
#define PLAYER_H

#include <GLEW/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb/stb_image.h>
#include <iostream>

class Player {
public:
    Player(int clientId = 0, const glm::vec3& color = glm::vec3(1.0f), float x = 0.0f, float y = 0.0f);
    ~Player();

    bool loadTexture(const char* filename);
    void render(GLuint shaderProgram, GLuint VAO, const glm::mat4& projection) const;
    void updatePosition(float x, float y);
    glm::vec2 getPosition() const;
    int getClientId() const;
    bool isTextureLoaded() const;

private:
    int clientId;
    glm::vec3 color;
    glm::vec2 position;
    GLuint texture;
    bool textureLoaded;
};

#endif // PLAYER_H
