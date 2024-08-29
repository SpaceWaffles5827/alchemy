#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include <GLEW/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb/stb_image.h>
#include <iostream>
#include <alchemy/gameObject.h>

class InputManager {
public:
    static InputManager& getInstance();

    InputManager(const InputManager&) = delete;
    InputManager& operator=(const InputManager&) = delete;

    void handleInput();
    void registerCallbacks();
    bool getIsDragging();

    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void scroll_callback(GLFWwindow* window, double xOffset, double yOffset);
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

    void setIsDraggingItemVisable(bool isVisable) {
        draggingItemVisable = isVisable;
    }

    bool getIsDraggingItemVisable() {
        return draggingItemVisable;
    }

    void setDraggingTextureId(GLuint textureId) {
        draggedTextureID = textureId;
    }

    void setDraggingItemName(std::string name) {
        draggedItemName = name;
    }

    void setDraggingStartPos(glm::vec2 position) {
        dragStartPosition = position;
    }

    std::string getDraggingItemName() {
        return draggedItemName;
    }

    GLuint getDragTextureId() {
        return draggedTextureID;
    }

    void setSelectedSlotIndex(int index) {
        selectedSlotIndex = index;
    }

    int getSelectedSlotIndex() {
        return selectedSlotIndex;
    }

    bool isDraggingFromHotbar() const {
        return draggingFromHotbar;
    }

    void setDraggingFromHotbar(bool fromHotbar) {
        draggingFromHotbar = fromHotbar;
    }

private:
    InputManager();  // Private constructor
    ~InputManager(); // Private destructor

    bool keyReleased[GLFW_KEY_LAST];
    bool isDragging;

    void handleWorldInteraction(double xpos, double ypos, int width, int height);

    bool draggingItemVisable;
    bool draggingFromHotbar = false; // Tracks whether the item is being dragged from the hotbar

    GLuint draggedTextureID = 0;        // The texture ID of the dragged item
    std::string draggedItemName;        // The name of the dragged item
    glm::vec2 dragStartPosition;        // The starting position of the drag
    int selectedSlotIndex = -1;
};

#endif // INPUTMANAGER_H
