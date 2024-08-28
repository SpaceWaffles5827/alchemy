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
    // Singleton instance getter with no parameters
    static Inventory& getInstance() {
        static Inventory instance;
        return instance;
    }

    // Delete copy constructor and assignment operator to prevent copying
    Inventory(const Inventory&) = delete;
    void operator=(const Inventory&) = delete;

    // Setter methods to update properties after creation
    void setPosition(const glm::vec3& pos);
    void setRotation(const glm::vec3& rot);
    void setDimensions(float width, float height);
    void setTexture(GLuint textureID, const glm::vec2& texTopLeft = glm::vec2(0.0f, 1.0f), const glm::vec2& texBottomRight = glm::vec2(1.0f, 0.0f));
    void setGridSize(int rows, int cols);

    void addItemToSlot(int slotIndex, const std::string& itemName);
    void removeItemFromSlot(int slotIndex);
    const std::string& getItemInSlot(int slotIndex) const;

    void setSlotPosition(int slotIndex, float x, float y);
    std::vector<InventorySlot>& getInventorySlots();

    int getSlotIndexAt(float x, float y) const;

    void loadDefaults();

    void setSlotTexture(int slotIndex, GLuint newTextureID);

    void setIsDraggingItemVisable(bool isVisable) {
        draggingItemVisable = isVisable;
    }

    bool getIsDraggingItemVisable() {
        return draggingItemVisable;
    }

    std::string getDraggingItemName() {
        return draggedItemName;
    }

    GLuint getDragTextureId() {
        return draggedTextureID;
    }

    void setSelectedSlotIndex(int index) {
        selectedSlotIndex = index;
    }

    void setDraggingTextureId(GLuint textureId) {
        draggedTextureID = textureId;
    }

    void setDraggingItemName(std::string name) {
        draggedItemName = name;
    }

    void setDraggingStartPos(glm::vec2 position) {
        dragStartPosition = position;
    }

    int getSelectedSlotIndex() {
        return selectedSlotIndex;
    }

private:
    // Private constructor with default initialization
    Inventory();

    ~Inventory();

    std::vector<InventorySlot> slots;  // Vector of inventory slots

    // Method to initialize slots in the inventory grid
    void initializeSlots();

    // Internal properties
    glm::vec3 position;
    glm::vec3 rotation;
    float width;
    float height;
    GLuint textureID;
    glm::vec2 texTopLeft;
    glm::vec2 texBottomRight;
    int rows;
    int cols;

    int selectedSlotIndex = -1;
    GLuint draggedTextureID = 0;        // The texture ID of the dragged item
    std::string draggedItemName;        // The name of the dragged item
    glm::vec2 dragStartPosition;        // The starting position of the drag
    bool draggingItemVisable;           // The visibility of the dragged item
};

#endif // INVENTORY_H
