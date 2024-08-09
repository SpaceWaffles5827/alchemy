#ifndef RENDER_H
#define RENDER_H

#include <GLEW/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "GameObject.h"
#include <vector>

class Render {
public:
    Render();
    ~Render();

    void initialize();
    void renderGameObject(const GameObject& gameObject, const glm::mat4& projection);
    void batchRenderGameObjects(const std::vector<std::shared_ptr<GameObject>>& gameObjects, const glm::mat4& projection);

    GLuint loadShader(const char* vertexShaderSource, const char* fragmentShaderSource);
    void setShaderProgram(GLuint shaderProgram);

private:
    GLuint shaderProgram;
    GLuint VAO, VBO;
    GLuint instanceVBO;  // Added instance VBO for batch rendering

    void setupBuffers();
    void checkCompileErrors(GLuint shader, std::string type);

    static const char* defaultVertexShaderSource;
    static const char* defaultFragmentShaderSource;
};

#endif // RENDER_H
