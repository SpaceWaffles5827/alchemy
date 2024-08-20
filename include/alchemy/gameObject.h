#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <GLEW/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "Renderable.h"

class GameObject : public Renderable {
public:
    // Constructor with default texture coordinates
    GameObject(const glm::vec3& pos, const glm::vec3& rot, float width, float height, GLuint textureID,
        const glm::vec2& texTopLeft = glm::vec2(0.0f, 1.0f), const glm::vec2& texBottomRight = glm::vec2(1.0f, 0.0f))
        : position(pos), rotation(rot), width(width), height(height), textureID(textureID),
        textureTopLeft(texTopLeft), textureBottomRight(texBottomRight), boundingRadius(std::sqrt(width* width + height * height) / 2.0f) {
        scale = glm::vec3(width, height, 1.0f);  // Set scale based on width and height
    }

    virtual ~GameObject() = default;

    void update(float deltaTime) {
        // Implementation of the update method
    }

    // Getters (Override from Renderable base class)
    virtual const glm::vec3& getPosition() const override { return position; }
    virtual const glm::vec3& getRotation() const override { return rotation; }
    virtual const glm::vec3& getScale() const override { return scale; }
    virtual GLuint getTextureID() const override { return textureID; }
    virtual const glm::vec2& getTextureTopLeft() const override { return textureTopLeft; }
    virtual const glm::vec2& getTextureBottomRight() const override { return textureBottomRight; }
    virtual float getBoundingRadius() const override { return boundingRadius; }

    // Reintroducing the width and height getters
    float getWidth() const { return width; }
    float getHeight() const { return height; }

    // Setters
    void setPosition(const glm::vec3& pos) { position = pos; }
    void setRotation(const glm::vec3& rot) { rotation = rot; }
    void setScale(float w, float h) {
        width = w;
        height = h;
        scale = glm::vec3(width, height, 1.0f);
        updateBoundingRadius();
    }

    void setTextureCoords(const glm::vec2& topLeft, const glm::vec2& bottomRight) {
        textureTopLeft = topLeft;
        textureBottomRight = bottomRight;
    }

    void setTextureTile(int tileX, int tileY, int tilesPerRow, int textureWidth, int textureHeight, int tileWidth, int tileHeight) {
        // Calculate normalized tile dimensions
        float normTileWidth = static_cast<float>(tileWidth) / textureWidth;
        float normTileHeight = static_cast<float>(tileHeight) / textureHeight;

        // Calculate texture coordinates
        float left = static_cast<float>(tileX) * normTileWidth;
        float right = left + normTileWidth;
        float top = static_cast<float>(tileY) * normTileHeight;
        float bottom = top + normTileHeight;

        // Invert y-coordinates because OpenGL texture origin is at the bottom left
        float invertedTop = 1.0f - top;
        float invertedBottom = 1.0f - bottom;

        // Set the texture coordinates
        setTextureCoords(glm::vec2(left, invertedTop), glm::vec2(right, invertedBottom));
    }

    void setTexture(GLuint newTextureID) {
        textureID = newTextureID;
    }

private:
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    float width;
    float height;
    GLuint textureID;

    glm::vec2 textureTopLeft;
    glm::vec2 textureBottomRight;

    float boundingRadius;

    void updateBoundingRadius() {
        boundingRadius = std::sqrt(width * width + height * height) / 2.0f;
    }
};

#endif // GAMEOBJECT_H
