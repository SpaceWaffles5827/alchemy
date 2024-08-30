#include <alchemy/inventorySlot.h>

// Parameterized constructor definition with four parameters
InventorySlot::InventorySlot(float x, float y, float width, float height, GLuint textureID)
    : Renderable(width, height, textureID) {
    setPosition(glm::vec3(x, y, 0.0f)); // Set the position based on x, y coordinates

    // Flip the texture vertically
    setTextureCoords(glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 0.0f));
}

void InventorySlot::setItem(const std::string& itemName) {
    item = itemName; // Set the item name in the slot
}

const std::string& InventorySlot::getItem() const {
    return item; // Return the item name in the slot
}

bool InventorySlot::isEmpty() const {
    return item.empty(); // Check if the slot is empty
}

bool InventorySlot::containsPoint(float x, float y) const {
    glm::vec2 center = getCenter();
    float halfWidth = getWidth() / 2.0f;
    float halfHeight = getHeight() / 2.0f;
    return (x >= center.x - halfWidth && x <= center.x + halfWidth &&
        y >= center.y - halfHeight && y <= center.y + halfHeight);
}

glm::vec2 InventorySlot::getCenter() const {
    glm::vec3 pos = getPosition();
    return glm::vec2(pos.x, pos.y); // Return the center of the slot based on position
}
