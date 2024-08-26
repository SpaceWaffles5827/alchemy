#ifndef INVENTORY_H
#define INVENTORY_H

#include <GLEW/glew.h>
#include <string>
#include <vector>
#include "Renderable.h"
#include <alchemy/InventorySlot.h>
#include <glm/glm.hpp>

class Inventory : public Renderable {
public:
    Inventory(const glm::vec3& pos = glm::vec3(0.0f),
        const glm::vec3& rot = glm::vec3(0.0f),
        float width = 1.0f,
        float height = 1.0f,
        GLuint textureID = 0,
        const glm::vec2& texTopLeft = glm::vec2(0.0f, 1.0f),
        const glm::vec2& texBottomRight = glm::vec2(1.0f, 0.0f),
        int rows = 2,
        int cols = 2);

    ~Inventory();

    void addItemToSlot(int slotIndex, const std::string& itemName);
    void removeItemFromSlot(int slotIndex);
    const std::string& getItemInSlot(int slotIndex) const;

    void setSlotPosition(int slotIndex, float x, float y);
    std::vector<InventorySlot>& getInventorySlots();

    int getSlotIndexAt(float x, float y) const {
        for (int i = 0; i < slots.size(); ++i) {
            if (slots[i].containsPoint(x, y)) {
                return i;
            }
        }
        return -1;
    }

    void loadDefaults();

    void setSlotTexture(int slotIndex, GLuint newTextureID);

private:
    std::vector<InventorySlot> slots;  // Vector of inventory slots

    // Method to initialize slots in the inventory grid
    void initializeSlots(int rows, int cols, float slotWidth, float slotHeight, float horizontalGap, float verticalGap);
};

#endif // INVENTORY_H
