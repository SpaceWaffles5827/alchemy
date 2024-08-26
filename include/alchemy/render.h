#ifndef RENDER_H
#define RENDER_H

#include <GLEW/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "GameObject.h"
#include <vector>
#include <memory>
#include <map>

class Render {
public:
    static Render& getInstance() {
        static Render instance;
        return instance;
    }

    Render(Render const&) = delete;
    void operator=(Render const&) = delete;

    void initialize();
    void renderGameObject(const GameObject& gameObject, const glm::mat4& projection);
    void batchRenderGameObjects(const std::vector<std::shared_ptr<Renderable>>& renderables, const glm::mat4& projection);
    void setShaderProgram(GLuint shaderProgram);
    void renderUI(int width, int height);

private:
    Render();
    ~Render();

    void setupBuffers();
    GLuint loadShader(const char* vertexShaderSource, const char* fragmentShaderSource);
    void checkCompileErrors(GLuint shader, const std::string& type);

    GLuint shaderProgram;
    GLuint VAO, VBO, EBO;
    GLuint instanceVBO;

    static const char* defaultVertexShaderSource;
    static const char* defaultFragmentShaderSource;

    size_t maxVerticesPerBatch; // Maximum vertices per batch for rendering
};

#endif // RENDER_H
