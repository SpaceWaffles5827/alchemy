#ifndef RENDERABLE_H
#define RENDERABLE_H

#include <glm/glm.hpp>
#include <GLEW/glew.h>

class Renderable {
public:
    virtual ~Renderable() = default;

    virtual const glm::vec3& getPosition() const = 0;
    virtual const glm::vec3& getRotation() const = 0;
    virtual const glm::vec3& getScale() const = 0;
    virtual GLuint getTextureID() const = 0;
    virtual const glm::vec2& getTextureTopLeft() const = 0;
    virtual const glm::vec2& getTextureBottomRight() const = 0;
    virtual float getBoundingRadius() const = 0;
};

#endif // RENDERABLE_H
