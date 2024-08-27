#ifndef GRAPHICS_CONTEXT_H
#define GRAPHICS_CONTEXT_H

#include <string>
#include <iostream>
#include <GLEW/glew.h>
#include <GLFW/glfw3.h>

class GraphicsContext {
public:
    static GraphicsContext& getInstance();

    // Deleted functions to ensure only one instance
    GraphicsContext(const GraphicsContext&) = delete;
    GraphicsContext& operator=(const GraphicsContext&) = delete;

    void initialize();
    GLFWwindow*& getWindow();

    GLuint loadTexture(const char* path);

    void updateProjectionMatrix(int width, int height);

    float getCameraZoom();
    void setCameraZoom(float zoom);

    void updateUiProjectionMatrix(int width, int height);

private:
    GraphicsContext();  // Private constructor
    ~GraphicsContext(); // Private destructor

    void initGLFW();
    void initGLEW();

    float cameraZoom;

    GLFWwindow* window;
    std::string title;
};

#endif // GRAPHICS_CONTEXT_H
