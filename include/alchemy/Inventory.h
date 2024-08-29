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
};

#endif // INVENTORY_H
