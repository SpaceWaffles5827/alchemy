#include <alchemy/chat.h>
#include <alchemy/global.h>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <fstream>

std::string toLowerCase(const std::string& str) {
    std::string lowerStr = str;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
    return lowerStr;
}

Chat::Chat(GLuint screenWidth, GLuint screenHeight)
    : screenWidth(screenWidth), screenHeight(screenHeight), lineHeight(30.0f), isChatMode(false), currentMessage("") {

    commandMap = {
        {"setmode", {"leveledit", "play", "pause"}},
        {"saveworld", {}},  
        {"loadworld", {}}
    };
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

    // Render suggestions if available
    if (!suggestions.empty()) {
        GLfloat suggestionY = 50.0f;
        for (const auto& suggestion : suggestions) {
            game.getTextRender().renderText(suggestion, 10.0f, suggestionY, 0.8f, glm::vec3(0.7f, 0.7f, 0.7f));  // Grey text
            suggestionY += lineHeight;
        }
    }
}

std::string Chat::getCurrentMessage() {
    return currentMessage;
}

void Chat::setCurrentMessage(const std::string& message) {
    currentMessage = message;
    updateSuggestions();
}

bool Chat::isChatModeActive() const {
    return isChatMode;
}

void Chat::setChatModeActive(bool active) {
    isChatMode = active;
}

void Chat::trimMessages() {
    GLuint maxMessages = static_cast<GLuint>(screenHeight / lineHeight);
    if (messages.size() > maxMessages) {
        messages.erase(messages.begin(), messages.begin() + (messages.size() - maxMessages));
    }
}

bool Chat::processCommand() {
    if (!currentMessage.empty() && currentMessage[0] == '/') {
        std::istringstream iss(currentMessage.substr(1));  // Skip the '/'
        std::string command;
        iss >> command;

        command = toLowerCase(command);  // Convert command to lowercase

        if (command == "setmode") {
            std::string mode;
            iss >> mode;
            mode = toLowerCase(mode);  // Convert mode to lowercase
            if (mode == "leveledit") {
                game.setCurrentMode(Mode::LevelEdit);
                std::cout << "Switched to Level Edit mode." << std::endl;
            }
            else if (mode == "play") {
                game.setCurrentMode(Mode::Play);
                std::cout << "Switched to Play mode." << std::endl;
            }
            else if (mode == "pause") {
                game.setCurrentMode(Mode::Pause);
                std::cout << "Game Paused." << std::endl;
            }
            else {
                std::cout << "Unknown mode: " << mode << std::endl;
            }
            return true;
        }

        if (command == "saveworld") {
            saveWorld();  // No arguments needed
            std::cout << "World saved." << std::endl;
            return true;
        }

        if (command == "loadworld") {
            std::string id;
            iss >> id;
            if (!id.empty()) {
                loadWorld(id);
                std::cout << "World loaded with ID: " << id << std::endl;
            }
            else {
                std::cout << "No ID specified for loading world." << std::endl;
            }
            return true;
        }

        std::cout << "Unknown command: " << command << std::endl;
        return true;
    }

    return false;  // Not a command
}

void Chat::updateSuggestions() {
    suggestions.clear();
    if (currentMessage.empty() || currentMessage[0] != '/') return;

    std::istringstream iss(currentMessage.substr(1));  // Skip the '/'
    std::string command;
    iss >> command;

    command = toLowerCase(command);  // Convert command to lowercase

    // Auto-complete command names
    if (commandMap.find(command) == commandMap.end()) {
        int count = 0;
        for (const auto& cmd : commandMap) {
            if (toLowerCase(cmd.first).find(command) == 0 && count < 3) {
                suggestions.push_back("/" + cmd.first);
                count++;
            }
        }
    }

    // Auto-complete command arguments if a command is fully typed
    if (commandMap.find(command) != commandMap.end()) {
        std::string partialArg;
        iss >> partialArg;
        partialArg = toLowerCase(partialArg);  // Convert partialArg to lowercase

        int count = 0;
        for (const auto& arg : commandMap[command]) {
            if (toLowerCase(arg).find(partialArg) == 0 && count < 3) {
                suggestions.push_back("/" + command + " " + arg);
                count++;
            }
        }
    }
}

void Chat::selectSuggestion() {
    if (!suggestions.empty()) {
        setCurrentMessage(suggestions.front());
    }
}

void Chat::saveLevel(const std::string& filename) {
    std::ofstream outFile(filename);
    if (outFile.is_open()) {
        // Serialize your level data here, e.g.:
        // outFile << levelData;
        outFile.close();
    }
    else {
        std::cerr << "Failed to open file for saving: " << filename << std::endl;
    }
}

void Chat::loadLevel(const std::string& filename) {
    std::ifstream inFile(filename);
    if (inFile.is_open()) {
        // Deserialize your level data here, e.g.:
        // inFile >> levelData;
        inFile.close();
    }
    else {
        std::cerr << "Failed to open file for loading: " << filename << std::endl;
    }
}

void Chat::saveWorld() {
    std::ofstream outFile("worldData.txt");
    if (outFile.is_open()) {
        // Serialize your world data here, e.g.:
        // outFile << worldData;
        outFile.close();
    }
    else {
        std::cerr << "Failed to open file for saving: worldData.txt" << std::endl;
    }
}

void Chat::loadWorld(const std::string& id) {
    std::string filename = "worldData_" + id + ".txt";
    std::ifstream inFile(filename);
    if (inFile.is_open()) {
        // Deserialize your world data here, e.g.:
        // inFile >> worldData;
        inFile.close();
    }
    else {
        std::cerr << "Failed to open file for loading: " << filename << std::endl;
    }
}
