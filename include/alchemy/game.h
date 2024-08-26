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
#include <alchemy/Inventory.h>
#include <alchemy/graphicsContext.h>
#include <alchemy/inputManager.h>

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

    NetworkManager& getNetworkManager();
    World& getWorld();
    GraphicsContext& getGraphicsContext();
    TextRenderer* getTextRender();

    void saveLevel(const std::string& filename);
    void loadLevel(const std::string& filename);
    void saveWorld(const std::string& filename);
    void loadWorld(const std::string& filename);

    Chat& getChat();
    void updateProjectionMatrix(int width, int height);

    void setCurrentMode(Mode mode) { currentMode = mode; }

    GLuint getTheTexture() {
        return textureID1;
    }

    Mode getGameMode();

    Inventory& getPlayerInventory();

    bool getDispalyInventory();
    void setDispalyInventory(bool status);
    int getClientId();

    void setCameraZoom(float zoom);

    float getCameraZoom();

    void setSelectedSlotIndex(int slotIndex);

    int getSelectedSlotIndex();

    glm::mat4 getProjection();

    void setDraggingTextureId(GLuint textureId);

    void setDraggingItemName(std::string name);

    void setDraggingStartPos(glm::vec2);

    void setProjectionMatrix(glm::mat4 projectionMatrix);

    GLuint getShaderProgram();

private:
    void update(double deltaTime);
    void render();
    void renderUI(int width, int height);
    void cleanup();
    void checkCompileErrors(GLuint shader, std::string type);
    void updateUiProjectionMatrix(int width, int height);


    int selectedSlotIndex = -1;         // The index of the slot being dragged
    GLuint draggedTextureID = 0;        // The texture ID of the dragged item
    std::string draggedItemName;        // The name of the dragged item
    glm::vec2 dragStartPosition;        // The starting position of the drag

    void handleWorldInteraction(double xpos, double ypos, int width, int height);

    int selectedTileX;
    int selectedTileY;
    bool showFps;
    bool displayInventory;

    // GLFWwindow* window;
    GraphicsContext graphicsContext;

    GLuint VAO, VBO; // move this to graphics context 
    GLuint shaderProgram, redShaderProgram;

    NetworkManager networkManager;

    int clientId;
    double tickRate;
    World world;

    glm::mat4 projection;
    glm::mat4 projectionUi;
    std::unordered_map<int, Player> players;

    InputManager inputManager;

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
