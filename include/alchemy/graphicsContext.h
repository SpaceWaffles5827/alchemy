#ifndef GRAPHICS_CONTEXT_H
#define GRAPHICS_CONTEXT_H

#include <string>
#include <iostream>

class GraphicsContext {
public:
    GraphicsContext()
        : window(nullptr), title("Default Window") {}

    GraphicsContext(const std::string& title);
    ~GraphicsContext();

    void initialize();
    GLFWwindow*& getWindow();

    GLuint loadTexture(const char* path);

    void updateProjectionMatrix(int width, int height);

    float getCameraZoom();
    void setCameraZoom(float zoom);

private:
    void initGLFW();
    void initGLEW();

    float cameraZoom;

    GLFWwindow* window;
    std::string title;
};

#endif // GRAPHICS_CONTEXT_H
