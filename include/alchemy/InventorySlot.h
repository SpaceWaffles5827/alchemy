#ifndef INVENTORYSLOT_H
#define INVENTORYSLOT_H

#include <GLEW/glew.h>
#include <string>
#include "Renderable.h"
#include <glm/glm.hpp>

class InventorySlot : public Renderable {
public:
    // Default constructor
    InventorySlot();

    // Constructor with parameters to match usage in renderUI
    InventorySlot(const glm::vec3& pos,
        const glm::vec3& rot,
        float width,
        float height,
        GLuint textureID,
        const glm::vec2& texTopLeft,
        const glm::vec2& texBottomRight
    );

    ~InventorySlot();

    void setPosition(float x, float y);
    void setItem(const std::string& itemName);
    const std::string& getItem() const;

    bool isEmpty() const;

    // Implement pure virtual methods from Renderable
    const glm::vec3& getPosition() const override;
    const glm::vec3& getRotation() const override;
    const glm::vec3& getScale() const override;
    GLuint getTextureID() const override;
    const glm::vec2& getTextureTopLeft() const override;
    const glm::vec2& getTextureBottomRight() const override;
    float getBoundingRadius() const override;

    void setTexture(GLuint newTextureID) {
        textureID = newTextureID;
    }

    bool containsPoint(float x, float y) const {
        float slotLeft = position.x - scale.x / 2.0f;
        float slotRight = position.x + scale.x / 2.0f;
        float slotBottom = position.y - scale.y / 2.0f;
        float slotTop = position.y + scale.y / 2.0f;

        return x >= slotLeft && x <= slotRight && y >= slotBottom && y <= slotTop;
    }

private:
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    GLuint textureID;
    glm::vec2 textureTopLeft;
    glm::vec2 textureBottomRight;
    float boundingRadius;
    std::string item;

    // Method to update the bounding radius based on the scale
    void updateBoundingRadius();
};

#endif // INVENTORYSLOT_H
