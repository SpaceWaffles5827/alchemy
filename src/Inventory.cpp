#include <alchemy/Inventory.h>
#include <stdexcept>
#include <iostream>

// Constructor definition
Inventory::Inventory(const glm::vec3& pos, const glm::vec3& rot, float width, float height, GLuint textureID,
    const glm::vec2& texTopLeft, const glm::vec2& texBottomRight, int rows, int cols)
    : position(pos), rotation(rot), scale(glm::vec3(width, height, 1.0f)), textureID(textureID),
    textureTopLeft(glm::vec2(texTopLeft.x, 1.0f - texTopLeft.y)),   // Flip the y-coordinates
    textureBottomRight(glm::vec2(texBottomRight.x, 1.0f - texBottomRight.y)),   // Flip the y-coordinates
    boundingRadius(std::sqrt(width* width + height * height) / 2.0f) {

    initializeSlots(rows, cols, 48, 48, 6, 6);
}


// Destructor definition
Inventory::~Inventory() {
    // Cleanup if necessary
}

void Inventory::initializeSlots(int rows, int cols, float slotWidth, float slotHeight, float horizontalGap, float verticalGap) {
    float xOffset = -216.0f;
    float yOffset = 27.0f;
    slots.resize(rows * cols);

    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            int index = row * cols + col;
            float x = position.x + xOffset + col * (slotWidth + horizontalGap);
            float y = position.y + yOffset + row * (slotHeight + verticalGap);
            std::cout << "Placing: (" << x << ", " << y << ")\n";
            slots[index] = InventorySlot(glm::vec3(x, y, 0.0f), glm::vec3(0.0f), slotWidth, slotHeight, textureID, glm::vec2(0.0f, 1.0f), glm::vec2(1.0f, 0.0f));
        }
    }
}

void Inventory::addItemToSlot(int slotIndex, const std::string& itemName) {
    if (slotIndex < 0 || slotIndex >= slots.size()) {
        throw std::out_of_range("Invalid slot index");
    }
    slots[slotIndex].setItem(itemName);
}

void Inventory::removeItemFromSlot(int slotIndex) {
    if (slotIndex < 0 || slotIndex >= slots.size()) {
        throw std::out_of_range("Invalid slot index");
    }
    slots[slotIndex].setItem("");
}

const std::string& Inventory::getItemInSlot(int slotIndex) const {
    if (slotIndex < 0 || slotIndex >= slots.size()) {
        throw std::out_of_range("Invalid slot index");
    }
    return slots[slotIndex].getItem();
}

void Inventory::setSlotPosition(int slotIndex, float x, float y) {
    if (slotIndex < 0 || slotIndex >= slots.size()) {
        throw std::out_of_range("Invalid slot index");
    }
    slots[slotIndex].setPosition(x, y);
}

// Implement the pure virtual methods from Renderable
const glm::vec3& Inventory::getPosition() const {
    return position;
}

const glm::vec3& Inventory::getRotation() const {
    return rotation;
}

const glm::vec3& Inventory::getScale() const {
    return scale;
}

GLuint Inventory::getTextureID() const {
    return textureID;
}

void Inventory::setSlotTexture(int slotIndex, GLuint newTextureID) {
    if (slotIndex < 0 || slotIndex >= slots.size()) {
        throw std::out_of_range("Invalid slot index");
    }
    slots[slotIndex].setTexture(newTextureID);
}

const glm::vec2& Inventory::getTextureTopLeft() const {
    return textureTopLeft;
}

const glm::vec2& Inventory::getTextureBottomRight() const {
    return textureBottomRight;
}

float Inventory::getBoundingRadius() const {
    return boundingRadius;
}

// Setters for rendering properties
void Inventory::setPosition(const glm::vec3& pos) {
    position = pos;
}

void Inventory::setRotation(const glm::vec3& rot) {
    rotation = rot;
}

void Inventory::setScale(float w, float h) {
    scale = glm::vec3(w, h, 1.0f);
    updateBoundingRadius();
}

void Inventory::setTexture(GLuint newTextureID) {
    textureID = newTextureID;
}

void Inventory::setTextureCoords(const glm::vec2& topLeft, const glm::vec2& bottomRight) {
    textureTopLeft = topLeft;
    textureBottomRight = bottomRight;
}

std::vector<InventorySlot>& Inventory::getInventorySlots() {
    return slots;
}

void Inventory::setTextureTile(int tileX, int tileY, int tilesPerRow, int textureWidth, int textureHeight, int tileWidth, int tileHeight) {
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

// Private helper function to update the bounding radius
void Inventory::updateBoundingRadius() {
    boundingRadius = std::sqrt(scale.x * scale.x + scale.y * scale.y) / 2.0f;
}
