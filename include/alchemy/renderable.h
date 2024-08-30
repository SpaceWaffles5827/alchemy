#ifndef RENDERABLE_H
#define RENDERABLE_H

#include "../GLEW/glew.h"
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtc/type_ptr.hpp"
#include <iostream>

class Renderable {
public:
  Renderable();
  Renderable(float width, float height, GLuint textureID,
             const glm::vec2 &texTopLeft = glm::vec2(0.0f, 1.0f),
             const glm::vec2 &texBottomRight = glm::vec2(1.0f, 0.0f),
             const glm::vec3 &initialPosition = glm::vec3(0.0f),
             const glm::vec3 &initialRotation = glm::vec3(0.0f));
  virtual ~Renderable();

  // Getters
  const glm::vec3 &getPosition() const;

  const glm::vec3 &getRotation() const;
  const glm::vec3 &getScale() const;
  GLuint getTextureID() const;
  const glm::vec2 &getTextureTopLeft() const;
  const glm::vec2 &getTextureBottomRight() const;
  float getWidth() const;
  float getHeight() const;
  bool getIsVisable() const;

  // Setters
  void setIsVisable(bool visable);

  void setPosition(const glm::vec3 &newPosition);
  void setRotation(const glm::vec3 &newRotation);
  void setScale(const glm::vec3 &newScale);

  void setTexture(GLuint newTextureID);

  void setTextureTile(int tileX, int tileY, int tilesPerRow, int textureWidth,
                      int textureHeight, int tileWidth, int tileHeight);
  void setTextureCoords(const glm::vec2 &topLeft, const glm::vec2 &bottomRight);

  void updateScale();

  void updateBoundingRadius();

  float getBoundingRadius() const;

protected:
  float width;
  float height;
  GLuint textureID;
  glm::vec3 scale;
  glm::vec2 textureTopLeft;
  glm::vec2 textureBottomRight;
  glm::vec3 position;
  glm::vec3 rotation;
  float boundingRadius;
  bool isVisable;
};

#endif // RENDERABLE_H
