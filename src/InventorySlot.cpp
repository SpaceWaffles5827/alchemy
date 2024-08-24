#define GLM_ENABLE_EXPERIMENTAL
#include <alchemy/InventorySlot.h>
#include <glm/gtx/quaternion.hpp> // Ensure this comes after the experimental define

InventorySlot::InventorySlot()
    : position(0.0f), rotation(0.0f), scale(1.0f), textureID(0), boundingRadius(1.0f), item("") {
    updateBoundingRadius();
}

InventorySlot::InventorySlot(const glm::vec3& pos, const glm::vec3& rot, float width, float height, GLuint textureID, const glm::vec2& texTopLeft, const glm::vec2& texBottomRight)
    : position(pos), rotation(rot), scale(glm::vec3(width, height, 1.0f)), textureID(textureID),
    textureTopLeft(texTopLeft),
    textureBottomRight(texBottomRight) {
    updateBoundingRadius();
}   

InventorySlot::~InventorySlot() {
    // Cleanup if necessary
}

void InventorySlot::setPosition(float x, float y) {
    position = glm::vec3(x, y, 0.0f);
}

void InventorySlot::setItem(const std::string& itemName) {
    item = itemName;
}

const std::string& InventorySlot::getItem() const {
    return item;
}

bool InventorySlot::isEmpty() const {
    return item.empty();
}

const glm::vec3& InventorySlot::getPosition() const {
    return position;
}

const glm::vec3& InventorySlot::getRotation() const {
    return rotation;
}

const glm::vec3& InventorySlot::getScale() const {
    return scale;
}

GLuint InventorySlot::getTextureID() const {
    return textureID;
}

const glm::vec2& InventorySlot::getTextureTopLeft() const {
    return textureTopLeft;
}

const glm::vec2& InventorySlot::getTextureBottomRight() const {
    return textureBottomRight;
}

float InventorySlot::getBoundingRadius() const {
    return boundingRadius;
}

void InventorySlot::setTexture(GLuint newTextureID) {
    textureID = newTextureID;
}

bool InventorySlot::containsPoint(float x, float y) const {
    float slotLeft = position.x - scale.x / 2.0f;
    float slotRight = position.x + scale.x / 2.0f;
    float slotBottom = position.y - scale.y / 2.0f;
    float slotTop = position.y + scale.y / 2.0f;

    return x >= slotLeft && x <= slotRight && y >= slotBottom && y <= slotTop;
}

void InventorySlot::updateBoundingRadius() {
    boundingRadius = glm::length(glm::vec2(scale.x, scale.y)) / 2.0f;
}
