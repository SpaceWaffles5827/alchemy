#ifndef GRAPHICS_CONTEXT_H
#define GRAPHICS_CONTEXT_H

#include <string>
#include <iostream>
#include <GLEW/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

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

    GLuint getTextureID1() {
        return textureID1;
    }

    GLuint getTextureID2() {
        return textureID2;
    }

    GLuint getInventoryTextureID() {
        return inventoryTextureID;
    }

    GLuint getHotbarTextureId() {
        return hotbarTextureId;
    }

    void setTextureID1(GLuint texture) {
         textureID1 = texture;
    }

    void setTextureID2(GLuint texture) {
         textureID2 = texture;
    }

    void setInventoryTextureID(GLuint texture) {
         inventoryTextureID = texture;
    }

    void setHotbarTextureId(GLuint texture) {
         hotbarTextureId = texture;
    }

    void setProjectionMatrix(glm::mat4 projectionMatrix) {
        projection = projectionMatrix;
    }

    glm::mat4 getProjection() {
        return projection;
    }

    GLuint getDefaultShader() {
        return 0;
    }

private:
    GraphicsContext();  // Private constructor
    ~GraphicsContext(); // Private destructor

    void initGLFW();
    void initGLEW();

    float cameraZoom;

    GLFWwindow* window;
    std::string title;

    GLuint textureID1;
    GLuint textureID2;
    GLuint inventoryTextureID;
    GLuint hotbarTextureId;

    glm::mat4 projection;
    glm::mat4 projectionUi;
};

#endif // GRAPHICS_CONTEXT_H
