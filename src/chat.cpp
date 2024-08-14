#include <alchemy/chat.h>
#include <alchemy/global.h>
#include <iostream>
#include <sstream>

Chat::Chat(GLuint screenWidth, GLuint screenHeight)
    : screenWidth(screenWidth), screenHeight(screenHeight), lineHeight(30.0f), isChatMode(false), currentMessage("") {
}

Chat::~Chat() {}

void Chat::addMessage(const std::string& message) {
    currentMessage = message;
    if (!processCommand()) {
        messages.push_back(message);
        trimMessages();
    }
    currentMessage.clear();  // Clear the current message after processing
}

void Chat::render() {
    GLfloat y = screenHeight - lineHeight;  // Start rendering from the bottom
    for (const auto& message : messages) {
        game.getTextRender().renderText(message, 10.0f, y, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));  // White text
        y -= lineHeight;
    }

    // Render the current message being typed if chat mode is active
    if (isChatMode) {
        game.getTextRender().renderText("> " + currentMessage, 10.0f, 10.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));  // White text
    }
}

std::string Chat::getCurrentMessage() {
    return currentMessage;
}

void Chat::setCurrentMessage(const std::string& message) {
    currentMessage = message;
}

bool Chat::isChatModeActive() const {
    return isChatMode;
}

void Chat::setChatModeActive(bool active) {
    isChatMode = active;
}

void Chat::trimMessages() {
    GLuint maxMessages = static_cast<GLuint>(screenHeight / lineHeight);  // Maximum messages that fit on screen
    if (messages.size() > maxMessages) {
        messages.erase(messages.begin(), messages.begin() + (messages.size() - maxMessages));
    }
}

bool Chat::processCommand() {
    if (!currentMessage.empty() && currentMessage[0] == '/') {
        std::istringstream iss(currentMessage.substr(1));  // Skip the '/'
        std::string command;
        iss >> command;

        if (command == "test") {
            std::string arg;
            if (iss >> arg) {
                std::cout << "Test command executed with argument: " << arg << std::endl;
            }
            else {
                std::cout << "Test command executed without arguments." << std::endl;
            }
            return true;
        }

        // Add more commands with arguments here as needed

        // If the command is not recognized, it can be treated as a normal message or ignored
        std::cout << "Unknown command: " << command << std::endl;
        return true;
    }

    return false;  // Not a command
}