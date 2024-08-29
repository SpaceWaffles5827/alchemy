#include <alchemy/hotbar.h>
#include <alchemy/InventorySlot.h>
#include <stdexcept>
#include <iostream>
#include <alchemy/global.h>
#include <alchemy/graphicsContext.h>
#include <alchemy/renderable.h>

// Constructor definition with no parameters
HotBar::HotBar()
    : Renderable(183.0f * 3, 23.0f * 3, textureID, glm::vec2(0.0f, 1.0f), glm::vec2(1.0f, 0.0f), glm::vec3(400.0f, 800.0f - 35.0f, 0.0f)),
    width(1.0f),
    height(1.0f),
    textureID(0),
    texTopLeft(glm::vec2(0.0f, 0.0f)),
    texBottomRight(glm::vec2(1.0f, 0.0f)),
    selectedSlotIndex(1)
{
    initializeSlots();
    GLuint selectedSlotTexture = GraphicsContext::getInstance().loadTexture("textures/ui/selected.png");
    selectedSlotObject = Renderable(23.0f * 3, 23.0f * 3, selectedSlotTexture, glm::vec2(0.0f, 1.0f), glm::vec2(1.0f, 0.0f), glm::vec3(400.0f - 239.0f, 765.0f , 0.0f));
}

// Destructor definition
HotBar::~HotBar() {
    // Cleanup if necessary
}

Renderable HotBar::getSelectedSlotObject() {
    return selectedSlotObject;
}

void HotBar::initializeSlots() {
    float slotWidth = 45.0f;
    float slotHeight = 45.0f;
    float horizontalGap = 15.0f;
    float xOffset = -239.0f;
    slots.resize(9);

    for (int i = 0; i < slots.size(); ++i) {
        float x = position.x + xOffset + i * (slotWidth + horizontalGap);
        float y = position.y;
        slots[i] = InventorySlot(glm::vec3(x, y, 0.0f), glm::vec3(0.0f), slotWidth, slotHeight, textureID, glm::vec2(0.0f, 1.0f), glm::vec2(1.0f, 0.0f));
    }
}

void HotBar::loadDefaults() {
    GLuint specialTextureID = GraphicsContext::getInstance().loadTexture("textures/world/stone_bricks.png");
    for (int i = 0; i < slots.size(); ++i) {
        slots[i].setTexture(specialTextureID);
        slots[i].setItem("Stone");
    }
}

void HotBar::addItemToSlot(int slotIndex, const std::string& itemName) {
    if (slotIndex < 0 || slotIndex >= slots.size()) {
        throw std::out_of_range("Invalid slot index");
    }
    slots[slotIndex].setItem(itemName);
}

void HotBar::removeItemFromSlot(int slotIndex) {
    if (slotIndex < 0 || slotIndex >= slots.size()) {
        throw std::out_of_range("Invalid slot index");
    }
    slots[slotIndex].setItem("");
}

const std::string& HotBar::getItemInSlot(int slotIndex) const {
    if (slotIndex < 0 || slotIndex >= slots.size()) {
        throw std::out_of_range("Invalid slot index");
    }
    return slots[slotIndex].getItem();
}

void HotBar::setSlotPosition(int slotIndex, float x, float y) {
    if (slotIndex < 0 || slotIndex >= slots.size()) {
        throw std::out_of_range("Invalid slot index");
    }
    slots[slotIndex].setPosition(glm::vec3(x, y, 0));
}

void HotBar::setSlotTexture(int slotIndex, GLuint newTextureID) {
    if (slotIndex < 0 || slotIndex >= slots.size()) {
        throw std::out_of_range("Invalid slot index");
    }
    slots[slotIndex].setTexture(newTextureID);
}

std::vector<InventorySlot>& HotBar::getHotBarSlots() {
    return slots;
}

int HotBar::getSlotIndexAt(float x, float y) const {
    for (int i = 0; i < slots.size(); ++i) {
        if (slots[i].containsPoint(x, y)) {
            return i;
        }
    }
    return -1;
}

// Setter methods
void HotBar::setPosition(const glm::vec3& pos) {
    position = pos;
    float slotWidth = 45.0f;
    float slotHeight = 45.0f;
    float horizontalGap = 15.0f;
    float xOffset = -239.0f;

    for (int i = 0; i < slots.size(); ++i) {
        float x = position.x + xOffset + i * (slotWidth + horizontalGap);
        float y = position.y;
        slots[i].setPosition(glm::vec3(x, y, 0.0f));
    }

    // Update the position of the selected slot object
    float offset = slotWidth + horizontalGap; // slotWidth + horizontalGap
    selectedSlotObject.setPosition(glm::vec3(position.x + xOffset + (selectedSlotIndex * offset), position.y, 0.0f));
}

void HotBar::setDimensions(float newWidth, float newHeight) {
    width = newWidth;
    height = newHeight;
}

void HotBar::setTexture(GLuint newTextureID, const glm::vec2& newTexTopLeft, const glm::vec2& newTexBottomRight) {
    textureID = newTextureID;
    texTopLeft = newTexTopLeft;
    texBottomRight = newTexBottomRight;

    Renderable::setTexture(newTextureID);
    Renderable::setTextureCoords(glm::vec2(texTopLeft.x, 1.0f - texTopLeft.y), glm::vec2(texBottomRight.x, 1.0f - texBottomRight.y));
}

void HotBar::setSelectedSlotIndex(int index) {
    if (index < 0 || index >= slots.size()) {
        throw std::out_of_range("Invalid slot index");
    }
    selectedSlotIndex = index;

    float slotWidth = 45.0f;
    float horizontalGap = 15.0f;
    float xOffset = -239.0f;
    float offset = slotWidth + horizontalGap; // slotWidth + horizontalGap

    // Update the position of the selected slot object based on the selected slot index
    selectedSlotObject.setPosition(glm::vec3(position.x + xOffset + (index * offset), position.y, 0));
}

int HotBar::getSelectedSlotIndex() const {
    return selectedSlotIndex;
}