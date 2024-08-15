#include <alchemy/chat.h>
#include <alchemy/global.h>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

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
            std::string worldName;
            iss >> worldName;
            if (!worldName.empty()) {
                saveWorld(worldName);
                std::cout << "World saved as: " << worldName << std::endl;
            }
            else {
                std::cout << "No world name provided for saving." << std::endl;
            }
            return true;
        }

        if (command == "loadworld") {
            std::string worldName;
            iss >> worldName;
            if (!worldName.empty()) {
                loadWorld(worldName);
                std::cout << "World loaded with name: " << worldName << std::endl;
            }
            else {
                std::cout << "No world name specified for loading." << std::endl;
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
    if (command == "loadworld") {
        std::string partialWorldName;
        iss >> partialWorldName;
        partialWorldName = toLowerCase(partialWorldName);  // Convert partialWorldName to lowercase

        for (const auto& entry : fs::directory_iterator(".")) {
            if (entry.path().extension() == ".txt" && entry.path().filename().string().find("worldData_") == 0) {
                std::string worldName = entry.path().stem().string().substr(10);  // Extract world name from "worldData_<name>.txt"
                if (worldName.find(partialWorldName) == 0) {
                    suggestions.push_back("/loadworld " + worldName);
                }
            }
        }
    }
    else if (commandMap.find(command) != commandMap.end()) {
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

void Chat::saveWorld(const std::string& worldName) {
    std::string filename = "worldData_" + worldName + ".txt";
    std::ofstream outFile(filename);
    if (outFile.is_open()) {
        // Save all game objects
        outFile << "Objects:\n";
        for (const auto& object : game.getWorld().getObjects()) {
            glm::vec3 pos = object->getPosition();
            glm::vec3 scale = object->getScale();
            glm::vec3 rot = object->getRotation();
            GLuint texID = object->getTextureID();

            outFile << pos.x << "," << pos.y << "," << pos.z << ","
                << scale.x << "," << scale.y << "," << scale.z << ","
                << rot.x << "," << rot.y << "," << rot.z << ","
                << texID << ","
                << object->getTextureTopLeft().x << "," << object->getTextureTopLeft().y << ","
                << object->getTextureBottomRight().x << "," << object->getTextureBottomRight().y << "\n";
        }

        // Save all players
        outFile << "Players:\n";
        for (const auto& player : game.getWorld().getPlayers()) {
            glm::vec3 pos = player->getPosition();
            float width = player->getWidth();
            float height = player->getHeight();
            GLuint texID = player->getTextureID();

            outFile << player->getClientId() << ","
                << pos.x << "," << pos.y << ","
                << width << "," << height << ","
                << texID << "\n";
        }

        std::cout << "World saved to " << filename << std::endl;
        outFile.close();
    }
    else {
        std::cerr << "Failed to open file for saving: " << filename << std::endl;
    }
}

void Chat::loadWorld(const std::string& worldName) {
    std::string filename = "worldData_" + worldName + ".txt";
    std::ifstream inFile(filename);
    if (inFile.is_open()) {
        game.getWorld().clearObjects();

        std::string line;
        bool loadingObjects = false;

        while (std::getline(inFile, line)) {
            if (line == "Objects:") {
                loadingObjects = true;
                continue;
            }
            if (line == "Players:") {
                loadingObjects = false;
                continue;
            }

            if (loadingObjects) {
                std::istringstream iss(line);
                std::string token;
                std::vector<float> values;

                while (std::getline(iss, token, ',')) {
                    values.push_back(std::stof(token));
                }

                if (values.size() == 14) {  // Ensure the correct number of values
                    glm::vec3 pos(values[0], values[1], values[2]);
                    glm::vec3 scale(values[3], values[4], values[5]);
                    glm::vec3 rot(values[6], values[7], values[8]);
                    GLuint texID = static_cast<GLuint>(values[9]);
                    glm::vec2 texTopLeft(values[10], values[11]);
                    glm::vec2 texBottomRight(values[12], values[13]);

                    auto object = std::make_shared<GameObject>(pos, rot, scale.x, scale.y, texID, texTopLeft, texBottomRight);
                    game.getWorld().addObject(object);
                }
            }
            else {
                std::istringstream iss(line);
                std::string token;
                std::vector<float> values;

                while (std::getline(iss, token, ',')) {
                    values.push_back(std::stof(token));
                }

                if (values.size() == 7) {  // Ensure the correct number of values for players
                    int clientId = static_cast<int>(values[0]);
                    glm::vec3 pos(values[1], values[2], 0.0f);  // Assuming players are 2D and Z is 0
                    float width = values[3];
                    float height = values[4];
                    GLuint texID = static_cast<GLuint>(values[5]);

                    auto player = std::make_shared<Player>(clientId, pos, 0.0f, 0.0f, width, height, texID);
                    game.getWorld().addPlayer(player);
                }
            }
        }

        std::cout << "World loaded from " << filename << std::endl;
        inFile.close();
    }
    else {
        std::cerr << "Failed to open file for loading: " << filename << std::endl;
    }
}
