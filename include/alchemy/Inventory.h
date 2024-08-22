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
    // Constructor with default texture coordinates and slot grid dimensions
    Inventory(const glm::vec3& pos = glm::vec3(0.0f),
        const glm::vec3& rot = glm::vec3(0.0f),
        float width = 1.0f,
        float height = 1.0f,
        GLuint textureID = 0,
        const glm::vec2& texTopLeft = glm::vec2(0.0f, 1.0f),
        const glm::vec2& texBottomRight = glm::vec2(1.0f, 0.0f),
        int rows = 9,
        int cols = 3);

    ~Inventory();

    void addItemToSlot(int slotIndex, const std::string& itemName);
    void removeItemFromSlot(int slotIndex);
    const std::string& getItemInSlot(int slotIndex) const;

    void setSlotPosition(int slotIndex, float x, float y);
    std::vector<InventorySlot> getInventorySlots();

    // Implement pure virtual methods from Renderable
    const glm::vec3& getPosition() const override;
    const glm::vec3& getRotation() const override;
    const glm::vec3& getScale() const override;
    GLuint getTextureID() const override;
    const glm::vec2& getTextureTopLeft() const override;
    const glm::vec2& getTextureBottomRight() const override;
    float getBoundingRadius() const override;

    // Setters for rendering properties
    void setPosition(const glm::vec3& pos);
    void setRotation(const glm::vec3& rot);
    void setScale(float w, float h);
    void setTexture(GLuint newTextureID);
    void setTextureCoords(const glm::vec2& topLeft, const glm::vec2& bottomRight);
    void setTextureTile(int tileX, int tileY, int tilesPerRow, int textureWidth, int textureHeight, int tileWidth, int tileHeight);

private:
    std::vector<InventorySlot> slots;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    GLuint textureID;
    glm::vec2 textureTopLeft;
    glm::vec2 textureBottomRight;
    float boundingRadius;

    void updateBoundingRadius();
    void initializeSlots(int rows, int cols, float slotWidth, float slotHeight);
};

#endif // INVENTORY_H
