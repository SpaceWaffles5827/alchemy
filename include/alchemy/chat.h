#ifndef CHAT_H
#define CHAT_H

#include <GLEW/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>

class Chat {
public:
    Chat(GLuint screenWidth, GLuint screenHeight);
    ~Chat();

    void addMessage(const std::string& message);
    void render();
    std::string getCurrentMessage();
    void setCurrentMessage(const std::string& message);
    bool isChatModeActive() const;
    void setChatModeActive(bool active);
    void selectSuggestion();

private:
    std::vector<std::string> messages;
    GLuint screenWidth, screenHeight;
    GLfloat lineHeight;
    std::string currentMessage;
    bool isChatMode;
    std::vector<std::string> suggestions;
    std::unordered_map<std::string, std::vector<std::string>> commandMap;

    void trimMessages();
    bool processCommand();
    void updateSuggestions();

    void saveLevel(const std::string& filename);
    void loadLevel(const std::string& filename);
    void saveWorld();  // No parameters needed now
    void loadWorld(const std::string& id);  // Takes an id as a parameter
};

#endif // CHAT_H
