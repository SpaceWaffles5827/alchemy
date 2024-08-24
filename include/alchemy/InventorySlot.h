#ifndef INVENTORYSLOT_H
#define INVENTORYSLOT_H

#include <string>
#include <GLEW/glew.h>
#include "Renderable.h"
#include <glm/glm.hpp>

class InventorySlot : public Renderable {
public:
    // Default constructor
    InventorySlot()
        : Renderable(48.0f, 48.0f, 0),
        item("") {}

    // Parameterized constructor with seven parameters
    InventorySlot(const glm::vec3& position, const glm::vec3& rotation, float width, float height, GLuint textureID, const glm::vec2& texTopLeft, const glm::vec2& texBottomRight)
        : Renderable(width, height, textureID, glm::vec2(texTopLeft.x, 1.0f - texTopLeft.y), glm::vec2(texBottomRight.x, 1.0f - texBottomRight.y)) {
        setPosition(position);
        setRotation(rotation);
    }

    // Parameterized constructor with four parameters
    InventorySlot(float x, float y, float width, float height, GLuint textureID);

    ~InventorySlot() = default;

    void setItem(const std::string& itemName);
    const std::string& getItem() const;
    bool isEmpty() const;
    bool containsPoint(float x, float y) const;
    glm::vec2 getCenter() const; // Returns the center of the slot

private:
    std::string item; // Item held in this slot
};

#endif // INVENTORYSLOT_H
