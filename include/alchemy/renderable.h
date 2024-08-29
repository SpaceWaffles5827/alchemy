#ifndef RENDERABLE_H
#define RENDERABLE_H

#include <GLEW/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

class Renderable {
public:
    Renderable()
        : width(1.0f), height(1.0f), textureID(0),
        textureTopLeft(glm::vec2(0.0f, 1.0f)), textureBottomRight(glm::vec2(1.0f, 0.0f)),
        isVisable(true),
        scale(glm::vec3(1.0f, 1.0f, 1.0f)), position(glm::vec3(0.0f)), rotation(glm::vec3(0.0f)) {
    }

    Renderable(float width, float height, GLuint textureID,
        const glm::vec2& texTopLeft = glm::vec2(0.0f, 1.0f),
        const glm::vec2& texBottomRight = glm::vec2(1.0f, 0.0f),
        const glm::vec3& initialPosition = glm::vec3(0.0f),
        const glm::vec3& initialRotation = glm::vec3(0.0f))
        : width(width), height(height), textureID(textureID),
        textureTopLeft(texTopLeft), textureBottomRight(texBottomRight),
        isVisable(true),
        scale(glm::vec3(width, height, 1.0f)), position(initialPosition), rotation(initialRotation) {
    }

    virtual ~Renderable() = default;

    // Getters
    const glm::vec3& getPosition() const { 
        return position;
    }
    const glm::vec3& getRotation() const { return rotation; }
    const glm::vec3& getScale() const { return scale; }
    GLuint getTextureID() const { return textureID; }
    const glm::vec2& getTextureTopLeft() const { return textureTopLeft; }
    const glm::vec2& getTextureBottomRight() const { return textureBottomRight; }
    float getWidth() const { return width; }
    float getHeight() const { return height; }
    bool getIsVisable() const { return isVisable; }

    // Setters
    void setIsVisable(bool visable) {
        isVisable = visable;
    }

    void setPosition(const glm::vec3& newPosition) {
        position = newPosition; 
    }
    void setRotation(const glm::vec3& newRotation) { rotation = newRotation; }
    void setScale(const glm::vec3& newScale) {
        scale = newScale;
        width = scale.x;
        height = scale.y;
        updateScale();
    }

    void setTexture(GLuint newTextureID) {
        textureID = newTextureID;
    }

    void setTextureTile(int tileX, int tileY, int tilesPerRow, int textureWidth, int textureHeight, int tileWidth, int tileHeight) {
        float normTileWidth = static_cast<float>(tileWidth) / textureWidth;
        float normTileHeight = static_cast<float>(tileHeight) / textureHeight;

        float left = static_cast<float>(tileX) * normTileWidth;
        float right = left + normTileWidth;
        float top = static_cast<float>(tileY) * normTileHeight;
        float bottom = top + normTileHeight;

        float invertedTop = 1.0f - top;
        float invertedBottom = 1.0f - bottom;

        setTextureCoords(glm::vec2(left, invertedTop), glm::vec2(right, invertedBottom));
    }

    void setTextureCoords(const glm::vec2& topLeft, const glm::vec2& bottomRight) {
        textureTopLeft = topLeft;
        textureBottomRight = bottomRight;
    }

    void updateScale() {
        scale = glm::vec3(width, height, 1.0f);
    }

    void updateBoundingRadius() {
        boundingRadius = std::sqrt(width * width + height * height) / 2.0f;
    }

    float getBoundingRadius() const { return boundingRadius; }

protected:
    float width;
    float height;
    GLuint textureID;
    glm::vec3 scale;
    glm::vec2 textureTopLeft;
    glm::vec2 textureBottomRight;
    glm::vec3 position;
    glm::vec3 rotation;
    float boundingRadius;
    bool isVisable;
};

#endif // RENDERABLE_H
