#ifndef GAME_H
#define GAME_H

#include <GLEW/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "networkManager.h"
#include "player.h"

class Game {
public:
    Game();
    ~Game();

    void run();

private:
    void initGLFW();
    void initGLEW();
    void setupShaders();
    void setupBuffers();
    void processInput();
    void update(double deltaTime);
    void render();
    void cleanup();

    GLFWwindow* window;
    GLuint VAO, VBO;
    GLuint shaderProgram, redShaderProgram;

    NetworkManager networkManager;
    Player player1, player2;
    int clientId;
    double tickRate;
};

#endif // GAME_H
