#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <GLEW/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

class GameObject {
public:
    GameObject(const glm::vec3& pos, const glm::vec3& rot, float width, float height)
        : position(pos), rotation(rot), width(width), height(height) {
        scale = glm::vec3(width, height, 1.0f);  // Set scale based on width and height
    }

    virtual ~GameObject() = default;

    virtual void update(float deltaTime) {
        // Update logic here (e.g., physics, game logic)
    }

    virtual void render(GLuint shaderProgram, GLuint VAO, const glm::mat4& projection) const {
        glUseProgram(shaderProgram);

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

    // Setters
    void setPosition(const glm::vec3& pos) { position = pos; }
    void setRotation(const glm::vec3& rot) { rotation = rot; }
    void setScale(float w, float h) {
        width = w;
        height = h;
        scale = glm::vec3(width, height, 1.0f);
    }

private:
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    float width;
    float height;
};

#endif // GAMEOBJECT_H
