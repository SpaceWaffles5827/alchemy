#ifndef CHAT_H
#define CHAT_H

#include <GLEW/glew.h>
#include <string>
#include <vector>
#include <map>

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

    void saveWorld(const std::string& worldName); // Updated method declaration
    void loadWorld(const std::string& id);
    void selectSuggestion(); // Make this method public

private:
    GLuint screenWidth;
    GLuint screenHeight;
    GLfloat lineHeight;
    bool isChatMode;
    std::string currentMessage;
    std::vector<std::string> messages;
    std::vector<std::string> suggestions;
    std::map<std::string, std::vector<std::string>> commandMap;

    bool processCommand();
    void updateSuggestions();
    void trimMessages();

    void saveLevel(const std::string& filename);
    void loadLevel(const std::string& filename);
};

#endif // CHAT_H
