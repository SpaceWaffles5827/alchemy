#include <alchemy/Inventory.h>
#include <alchemy/InventorySlot.h>
#include <stdexcept>
#include <iostream>
#include <alchemy/global.h>

// Constructor definition
Inventory::Inventory(const glm::vec3& pos, const glm::vec3& rot, float width, float height, GLuint textureID,
    const glm::vec2& texTopLeft, const glm::vec2& texBottomRight, int rows, int cols)
    : Renderable(width, height, textureID, glm::vec2(texTopLeft.x, 1.0f - texTopLeft.y), glm::vec2(texBottomRight.x, 1.0f - texBottomRight.y), pos, rot) {

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
            float x = getPosition().x + xOffset + col * (slotWidth + horizontalGap);
            float y = getPosition().y + yOffset + row * (slotHeight + verticalGap);
            slots[index] = InventorySlot(glm::vec3(x, y, 0.0f), glm::vec3(0.0f), slotWidth, slotHeight, getTextureID(), glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f));
        }
    }
}

void Inventory::loadDefaults() {
    GLuint specialTextureID = game.getGraphicsContext().loadTexture("stone_bricks.png");
    slots[0].setTexture(specialTextureID);
    slots[0].setItem("Stone");
    slots[1].setTexture(specialTextureID);
    slots[1].setItem("Stone");
    slots[2].setTexture(specialTextureID);
    slots[2].setItem("Stone");
    slots[3].setTexture(specialTextureID);
    slots[3].setItem("Stone");
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
    slots[slotIndex].setPosition(glm::vec3(x, y, 0));
}

void Inventory::setSlotTexture(int slotIndex, GLuint newTextureID) {
    if (slotIndex < 0 || slotIndex >= slots.size()) {
        throw std::out_of_range("Invalid slot index");
    }
    slots[slotIndex].setTexture(newTextureID);
}

std::vector<InventorySlot>& Inventory::getInventorySlots() {
    return slots;
}