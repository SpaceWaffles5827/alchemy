#include "../include/alchemy/renderable.h"
#include "../include/alchemy/graphicsContext.h"
#include <cmath>
#include <iostream>

// Default constructor
Renderable::Renderable()
    : width(1.0f), height(1.0f), textureID(0),
      textureTopLeft(glm::vec2(0.0f, 1.0f)),
      textureBottomRight(glm::vec2(1.0f, 0.0f)), isVisable(true),
      scale(glm::vec3(1.0f, 1.0f, 1.0f)), position(glm::vec3(0.0f)),
      ySort(false), rotation(glm::vec3(0.0f)), ySortOffset(0.0f) {
    updateBoundingRadius();
}

// Parameterized constructor
Renderable::Renderable(float width, float height, GLuint textureID,
                       const glm::vec2 &texTopLeft,
                       const glm::vec2 &texBottomRight,
                       const glm::vec3 &initialPosition,
                       const glm::vec3 &initialRotation,
                       float initialYSortOffset)
    : width(width), height(height), textureID(textureID), ySort(false),
      textureTopLeft(texTopLeft), textureBottomRight(texBottomRight),
      isVisable(true), scale(glm::vec3(width, height, 1.0f)),
      position(initialPosition), rotation(initialRotation),
      ySortOffset(initialYSortOffset) {
    updateBoundingRadius();
}

// Destructor
Renderable::~Renderable() = default;

const glm::vec3 &Renderable::getPosition() const { return position; }

const glm::vec3 &Renderable::getRotation() const { return rotation; }

const glm::vec3 &Renderable::getScale() const { return scale; }

GLuint Renderable::getTextureID() const { return textureID; }

const glm::vec2 &Renderable::getTextureTopLeft() const {
    return textureTopLeft;
}

const glm::vec2 &Renderable::getTextureBottomRight() const {
    return textureBottomRight;
}

float Renderable::getWidth() const { return width; }

float Renderable::getHeight() const { return height; }

bool Renderable::getIsVisable() const { return isVisable; }

float Renderable::getYSortOffset() const { return ySortOffset; }

// Setters
void Renderable::setIsVisable(bool visable) { isVisable = visable; }

void Renderable::setPosition(const glm::vec3 &newPosition) {
    position = newPosition;
}

void Renderable::setRotation(const glm::vec3 &newRotation) {
    rotation = newRotation;
}

void Renderable::setScale(const glm::vec3 &newScale) {
    scale = newScale;
    width = scale.x;
    height = scale.y;
    updateScale();
}

void Renderable::setTexture(GLuint newTextureID) { textureID = newTextureID; }

void Renderable::setTextureTile(int tileX, int tileY, int tilesPerRow,
                                int textureWidth, int textureHeight,
                                int tileWidth, int tileHeight) {
    float normTileWidth = static_cast<float>(tileWidth) / textureWidth;
    float normTileHeight = static_cast<float>(tileHeight) / textureHeight;

    float left = static_cast<float>(tileX) * normTileWidth;
    float right = left + normTileWidth;
    float top = static_cast<float>(tileY) * normTileHeight;
    float bottom = top + normTileHeight;

    float invertedTop = 1.0f - top;
    float invertedBottom = 1.0f - bottom;

    setTextureCoords(glm::vec2(left, invertedTop),
                     glm::vec2(right, invertedBottom));
}

void Renderable::setTextureCoords(const glm::vec2 &topLeft,
                                  const glm::vec2 &bottomRight) {
    textureTopLeft = topLeft;
    textureBottomRight = bottomRight;
}

void Renderable::setYSortOffset(float offset) { ySortOffset = offset; }

void Renderable::updateScale() { scale = glm::vec3(width, height, 1.0f); }

void Renderable::updateBoundingRadius() {
    boundingRadius = std::sqrt(width * width + height * height) / 2.0f;
}

float Renderable::getBoundingRadius() const { return boundingRadius; }

bool Renderable::getIsYSorted() const { return ySort; }

void Renderable::setYSorting(bool status) { ySort = status; }

glm::vec3 Renderable::getYSortPosition() const {
    return glm::vec3(position.x, position.y - (height * 0.5f) + ySortOffset,
                     position.z);
}

// New method to calculate base Y position
float Renderable::calculateBaseYPosition() const {
    return position.y - (height * 0.5f) + ySortOffset;
}
