#ifndef CHAT_H
#define CHAT_H

#include <GLEW/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <memory>

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

private:
    std::vector<std::string> messages;
    GLuint screenWidth, screenHeight;
    GLfloat lineHeight;
    std::string currentMessage;
    bool isChatMode;

    void trimMessages();
    bool processCommand();  // Updated to handle commands with arguments
};

#endif // CHAT_H
