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

    // Deleted functions to ensure only one instance
    InputManager(const InputManager&) = delete;
    InputManager& operator=(const InputManager&) = delete;

    void handleInput();
    void registerCallbacks();
    bool getIsDragging();

    // GLFW Callbacks
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void scroll_callback(GLFWwindow* window, double xOffset, double yOffset);
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

private:
    InputManager();  // Private constructor
    ~InputManager(); // Private destructor

    bool keyReleased[GLFW_KEY_LAST];
    bool isDragging;

    void handleWorldInteraction(double xpos, double ypos, int width, int height);
};

#endif // INPUTMANAGER_H
