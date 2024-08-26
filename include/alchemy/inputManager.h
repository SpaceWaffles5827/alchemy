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
    InputManager();
    ~InputManager();

    bool keyReleased[GLFW_KEY_LAST];
    void handleInput();

    // Register GLFW callbacks
    void registerCallbacks();

    bool getIsDragging();

    // GLFW Callbacks
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void scroll_callback(GLFWwindow* window, double xOffset, double yOffset);
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

private:
    bool isDragging;
    void handleWorldInteraction(double xpos, double ypos, int width, int height);
};

#endif // INPUTMANAGER_H
