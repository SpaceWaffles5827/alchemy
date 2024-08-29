#ifndef HOTBAR_H
#define HOTBAR_H

#include <GLEW/glew.h>
#include <string>
#include <vector>
#include "renderable.h"
#include <alchemy/inventorySlot.h>
#include <glm/glm.hpp>

class HotBar : public Renderable {
public:
    // Singleton instance getter with no parameters
    static HotBar& getInstance() {
        static HotBar instance;
        return instance;
    }

    // Delete copy constructor and assignment operator to prevent copying
    HotBar(const HotBar&) = delete;
    void operator=(const HotBar&) = delete;

    // Setter methods to update properties after creation
    void setPosition(const glm::vec3& pos);
    void setDimensions(float width, float height);
    void setTexture(GLuint textureID, const glm::vec2& texTopLeft = glm::vec2(0.0f, 1.0f), const glm::vec2& texBottomRight = glm::vec2(1.0f, 0.0f));
    void setSlotTexture(int slotIndex, GLuint newTextureID);

    void addItemToSlot(int slotIndex, const std::string& itemName);
    void removeItemFromSlot(int slotIndex);
    const std::string& getItemInSlot(int slotIndex) const;

    void setSlotPosition(int slotIndex, float x, float y);
    std::vector<InventorySlot>& getHotBarSlots();

    int getSlotIndexAt(float x, float y) const;

    void loadDefaults();

    void setSelectedSlotIndex(int index);
    int getSelectedSlotIndex() const;

    Renderable getSelectedSlotObject();

private:
    HotBar();
    ~HotBar();

    std::vector<InventorySlot> slots;

    void initializeSlots();

    int selectedSlotIndex;

    float width;
    float height;
    GLuint textureID;

    Renderable selectedSlotObject;

    glm::vec2 texTopLeft;
    glm::vec2 texBottomRight;
};

#endif // HOTBAR_H
