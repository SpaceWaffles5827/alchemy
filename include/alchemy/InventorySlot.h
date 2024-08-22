#ifndef INVENTORYSLOT_H
#define INVENTORYSLOT_H

#include <GLEW/glew.h>
#include <string>
#include "Renderable.h"
#include <glm/glm.hpp>

class InventorySlot : public Renderable {
public:
    InventorySlot();
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

private:
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    GLuint textureID;
    glm::vec2 textureTopLeft;
    glm::vec2 textureBottomRight;
    float boundingRadius;
    std::string item;
};

#endif // INVENTORYSLOT_H
