#ifndef GAME_H
#define GAME_H

#include <GLEW/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "networkManager.h"
#include "player.h"
#include <unordered_map>

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
    GLuint loadTexture(const char* filename);
    void checkCompileErrors(GLuint shader, std::string type);

    GLFWwindow* window;
    GLuint VAO, VBO;
    GLuint shaderProgram, redShaderProgram;
    GLuint playerTexture; // Texture for the player

    NetworkManager networkManager;
    Player player1;

    int clientId;
    double tickRate;

    std::unordered_map<int, Player> players;
};

#endif
