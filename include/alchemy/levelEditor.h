#ifndef LEVELEDITOR_H
#define LEVELEDITOR_H

#include <GLEW/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "networkManager.h"
#include "player.h"
#include <unordered_map>
#include <alchemy/world.h>

class LevelEditor {
public:
    LevelEditor();
    ~LevelEditor();

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
    void checkCompileErrors(GLuint shader, std::string type);
    void updateProjectionMatrix(int width, int height);

    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);

    GLFWwindow* window;
    GLuint VAO, VBO;
    GLuint shaderProgram, redShaderProgram;

    NetworkManager networkManager;
    Player clientPlayer;

    int clientId;
    double tickRate;
    World world;

    glm::mat4 projection;
    std::unordered_map<int, Player> players;

    static const char* vertexShaderSource;
    static const char* fragmentShaderSource;
    static const char* redFragmentShaderSource;
};

#endif // LEVELEDITOR_H
