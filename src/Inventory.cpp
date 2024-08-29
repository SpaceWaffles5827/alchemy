#include <alchemy/inventory.h>
#include <alchemy/inventorySlot.h>
#include <stdexcept>
#include <iostream>
#include <alchemy/global.h>

// Constructor definition with no parameters
Inventory::Inventory()
    : Renderable(176.0f * 3, 166.0f * 3, textureID, glm::vec2(0.0f, 1.0f), glm::vec2(1.0f, 0.0f), glm::vec3(400.0f, 400.0f, 0.0f), glm::vec3(0.0f)),
    rotation(glm::vec3(0.0f)),
    width(1.0f),
    height(1.0f),
    textureID(0),
    texTopLeft(glm::vec2(0.0f, 0.0f)),
    texBottomRight(glm::vec2(1.0f, 0.0f)),
    rows(2),
    cols(2)
{
    isVisable = false;
    initializeSlots();
}

// Destructor definition
Inventory::~Inventory() {
    // Cleanup if necessary
}

void Inventory::initializeSlots() {
    float slotWidth = 48.0f;
    float slotHeight = 48.0f;
    float horizontalGap = 6.0f;
    float verticalGap = 6.0f;
    float xOffset = -216.0f;
    float yOffset = 27.0f;
    slots.resize(rows * cols);

    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            int index = row * cols + col;
            float x = position.x + xOffset + col * (slotWidth + horizontalGap);
            float y = position.y + yOffset + row * (slotHeight + verticalGap);
            slots[index] = InventorySlot(glm::vec3(x, y, 0.0f), glm::vec3(0.0f), slotWidth, slotHeight, 0, glm::vec2(0.0f, 1.0f), glm::vec2(1.0f, 0.0f));
        }
    }
}

void Inventory::loadDefaults() {
    GLuint specialTextureID = GraphicsContext::getInstance().loadTexture("textures/world/stone_bricks.png");
    slots[0].setTexture(specialTextureID);
    slots[0].setItem("Stone");
    slots[1].setTexture(specialTextureID);
    slots[1].setItem("Stone");
    slots[2].setTexture(specialTextureID);
    slots[2].setItem("Stone");
    slots[3].setTexture(specialTextureID);
    slots[3].setItem("Stone");
    slots[15].setIsVisable(false);
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

int Inventory::getSlotIndexAt(float x, float y) const {
    for (int i = 0; i < slots.size(); ++i) {
        if (slots[i].containsPoint(x, y)) {
            return i;
        }
    }
    return -1;
}

// Setter methods
void Inventory::setPosition(const glm::vec3& pos) {
    position = pos;

    float slotWidth = 48.0f;
    float slotHeight = 48.0f;
    float horizontalGap = 6.0f;
    float verticalGap = 6.0f;
    float xOffset = -216.0f;
    float yOffset = 27.0f;

    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            int index = row * cols + col;
            float x = position.x + xOffset + col * (slotWidth + horizontalGap);
            float y = position.y + yOffset + row * (slotHeight + verticalGap);
            slots[index].setPosition(glm::vec3(x, y, 0.0f));
        }
    }
}

void Inventory::setRotation(const glm::vec3& rot) {
    rotation = rot;
}

void Inventory::setDimensions(float newWidth, float newHeight) {
    width = newWidth;
    height = newHeight;

    setPosition(position);
}

void Inventory::setTexture(GLuint newTextureID, const glm::vec2& newTexTopLeft, const glm::vec2& newTexBottomRight) {
    // Update the Inventory class's texture properties
    textureID = newTextureID;
    texTopLeft = newTexTopLeft;
    texBottomRight = newTexBottomRight;

    // Update the Renderable base class's texture properties
    Renderable::setTexture(newTextureID);
    Renderable::setTextureCoords(glm::vec2(texTopLeft.x, 1.0f - texTopLeft.y), glm::vec2(texBottomRight.x, 1.0f - texBottomRight.y));

    // Recalculate the position of the slots to adjust to the new texture coordinates
    setPosition(position);
}

void Inventory::setGridSize(int newRows, int newCols) {
    rows = newRows;
    cols = newCols;
    initializeSlots();  // Reinitialize slots with the new grid size
}
