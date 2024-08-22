#include <alchemy/InventorySlot.h>

InventorySlot::InventorySlot()
    : position(0.0f), rotation(0.0f), scale(1.0f), textureID(0), boundingRadius(1.0f), item("") {
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

// Implement the pure virtual methods from Renderable
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
