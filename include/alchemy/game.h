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
#include <alchemy/UIObject.h>
#include <memory>
#include <vector>
#include <alchemy/Inventory.h>

enum class Mode {
    Game,
    LevelEdit,
    Play,
    Pause,
    Resume
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

    void saveLevel(const std::string& filename);
    void loadLevel(const std::string& filename);
    void saveWorld(const std::string& filename);
    void loadWorld(const std::string& filename);

    void setCurrentMode(Mode mode) { currentMode = mode; }

    GLuint getTheTexture() {
        return textureID1;
    }

private:
    void initGLFW();
    void initGLEW();
    void processInput();
    void update(double deltaTime);
    void render();
    void renderUI(int width, int height);
    void cleanup();
    void checkCompileErrors(GLuint shader, std::string type);
    void updateProjectionMatrix(int width, int height);
    void updateUiProjectionMatrix(int width, int height);
    void renderTileSelectionUI();
    static void scroll_callback(GLFWwindow* window, double xOffset, double yOffset);
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

    void handleInventorySlotClick(int slotIndex);
    void handleWorldInteraction(double xpos, double ypos, int width, int height);
    void handleRightClickInteraction();

    bool keyReleased[GLFW_KEY_LAST];
    int selectedTileX;
    int selectedTileY;
    bool tileSelectionVisible;
    bool displayInventory;
    bool showFps;

    GLFWwindow* window;
    GLuint VAO, VBO;
    GLuint shaderProgram, redShaderProgram;

    NetworkManager networkManager;

    int clientId;
    double tickRate;
    World world;

    glm::mat4 projection;
    glm::mat4 projectionUi;
    std::unordered_map<int, Player> players;

    GLuint textureID1;
    GLuint textureID2;
    GLuint inventoryTextureID;

    float cameraZoom;
    Mode currentMode;
    Render renderer;

    std::unique_ptr<TextRenderer> textRenderer;
    Chat chat;

    std::shared_ptr<UIObject> inventoryUIObject;

    Inventory playerInventory;
};

#endif // GAME_H
