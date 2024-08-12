#ifndef GAME_H
#define GAME_H

#include <GLEW/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <alchemy/networkManager.h>
#include <alchemy/Player.h>
#include <unordered_map>
#include <alchemy/world.h>
#include <alchemy/render.h>
#include <alchemy/textRenderer.h>
#include <alchemy/chat.h> 
#include <memory>

enum class Mode {
    Game,
    LevelEdit
};

class Game {
public:
    Game(Mode mode);
    ~Game();

    void run();
    void init();

    GLuint loadTexture(const char* path);
    NetworkManager& getNetworkManager();
    World& getWorld();
    GLFWwindow& getWindow();
    TextRenderer& getTextRender();

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
    static void scroll_callback(GLFWwindow* window, double xOffset, double yOffset);
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

    GLFWwindow* window;
    GLuint VAO, VBO;
    GLuint shaderProgram, redShaderProgram;

    NetworkManager networkManager;

    int clientId;
    double tickRate;
    World world;

    glm::mat4 projection;
    std::unordered_map<int, Player> players;

    static const char* vertexShaderSource;
    static const char* fragmentShaderSource;
    static const char* redFragmentShaderSource;
    GLuint textureID1;

    float cameraZoom;
    Mode currentMode;
    Render renderer;

    std::unique_ptr<TextRenderer> textRenderer;  // TextRenderer should be initialized first
    Chat chat;  // Chat should be initialized after TextRenderer
};

#endif // GAME_H
