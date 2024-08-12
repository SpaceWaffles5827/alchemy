#include <alchemy/chat.h>
#include <alchemy/global.h>

Chat::Chat(GLuint screenWidth, GLuint screenHeight)
    : screenWidth(screenWidth), screenHeight(screenHeight), lineHeight(30.0f), isChatMode(false), currentMessage("") {
}

Chat::~Chat() {}

void Chat::addMessage(const std::string& message) {
    messages.push_back(message);
    trimMessages();
    currentMessage.clear();  // Clear the current message after sending
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
