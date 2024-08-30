#ifndef GAME_H
#define GAME_H

#include <GLEW/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <alchemy/networkManager.h>
#include <alchemy/player.h>
#include <unordered_map>
#include <alchemy/world.h>
#include <alchemy/render.h>
#include <alchemy/textRenderer.h>
#include <alchemy/chat.h>
#include <alchemy/uiObject.h>
#include <memory>
#include <vector>
#include <alchemy/inventory.h>
#include <alchemy/graphicsContext.h>
#include <alchemy/inputManager.h>
#include <alchemy/audioManager.h>

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

    Mode getGameMode();
    void setCurrentMode(Mode mode) { currentMode = mode; }

    int getClientId();

private:
    void update(double deltaTime);
    void render();
    void cleanup();
    void checkCompileErrors(GLuint shader, std::string type);

    int clientId;
    double tickRate;

    std::unordered_map<int, Player> players;

    Mode currentMode;

    std::shared_ptr<UIObject> inventoryUIObject;
};

#endif // GAME_H
