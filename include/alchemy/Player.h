#ifndef PLAYER_H
#define PLAYER_H

#include <GLEW/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb/stb_image.h>
#include <iostream>
#include <alchemy/gameObject.h>     

class Player : public GameObject {
public:
    Player(int clientId = 0, const glm::vec3& color = glm::vec3(1.0f), float x = 0.0f, float y = 0.0f, float width = 1.0f, float height = 1.0f, GLuint textureID = 0);
    ~Player();

    int getClientId() const;

    void handleInput();

private:
    int clientId;
    bool keyReleased[GLFW_KEY_LAST];
};

#endif // PLAYER_H
