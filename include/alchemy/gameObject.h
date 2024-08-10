#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <GLEW/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

class GameObject {
public:
    // Constructor with default texture coordinates
    GameObject(const glm::vec3& pos, const glm::vec3& rot, float width, float height, GLuint textureID,
        const glm::vec2& texTopLeft = glm::vec2(0.0f, 1.0f), const glm::vec2& texBottomRight = glm::vec2(1.0f, 0.0f))
        : position(pos), rotation(rot), width(width), height(height), textureID(textureID),
        textureTopLeft(texTopLeft), textureBottomRight(texBottomRight) {
        scale = glm::vec3(width, height, 1.0f);  // Set scale based on width and height
    }

    virtual ~GameObject() = default;

    virtual void update(float deltaTime) {
        // Update logic here (e.g., physics, game logic)
    }

    virtual void render(GLuint shaderProgram, GLuint VAO, const glm::mat4& projection) const {
        glUseProgram(shaderProgram);

        glBindTexture(GL_TEXTURE_2D, textureID);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, scale);

        glm::mat4 combined = projection * model;

        GLuint transformLoc = glGetUniformLocation(shaderProgram, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(combined));

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }

    // Getters
    const glm::vec3& getPosition() const { return position; }
    const glm::vec3& getRotation() const { return rotation; }
    const glm::vec3& getScale() const { return scale; }
    float getWidth() const { return width; }
    float getHeight() const { return height; }
    GLuint getTextureID() const { return textureID; }
    const glm::vec2& getTextureTopLeft() const { return textureTopLeft; }
    const glm::vec2& getTextureBottomRight() const { return textureBottomRight; }

    // Setters
    void setPosition(const glm::vec3& pos) { position = pos; }
    void setRotation(const glm::vec3& rot) { rotation = rot; }
    void setScale(float w, float h) {
        width = w;
        height = h;
        scale = glm::vec3(width, height, 1.0f);
    }

    void setTextureCoords(const glm::vec2& topLeft, const glm::vec2& bottomRight) {
        textureTopLeft = topLeft;
        textureBottomRight = bottomRight;
    }

    void setTextureTile(int tileX, int tileY, int tilesPerRow, int textureWidth, int textureHeight, int tileWidth, int tileHeight) {
        // Calculate normalized tile dimensions
        float normTileWidth = static_cast<float>(tileWidth) / textureWidth;
        float normTileHeight = static_cast<float>(tileHeight) / textureHeight;

        // Calculate texture coordinates
        float left = static_cast<float>(tileX) * normTileWidth;
        float right = left + normTileWidth;
        float top = static_cast<float>(tileY) * normTileHeight;
        float bottom = top + normTileHeight;

        // Invert y-coordinates because OpenGL texture origin is at the bottom left
        float invertedTop = 1.0f - top;
        float invertedBottom = 1.0f - bottom;

        // Set the texture coordinates
        setTextureCoords(glm::vec2(left, invertedTop), glm::vec2(right, invertedBottom));
    }


private:
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    float width;
    float height;
    GLuint textureID;

    glm::vec2 textureTopLeft;   
    glm::vec2 textureBottomRight;
};

#endif // GAMEOBJECT_H
