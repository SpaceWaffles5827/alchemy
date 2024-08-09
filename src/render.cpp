#include <alchemy/render.h>
#include <iostream>

// Default shaders
const char* Render::defaultVertexShaderSource = R"(
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 2) in mat4 instanceTransform;

void main()
{
    gl_Position = instanceTransform * vec4(aPos, 1.0);
}
)";


const char* Render::defaultFragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

void main()
{
    FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);  // Solid red color
}
)";

Render::Render() : shaderProgram(0), VAO(0), VBO(0), instanceVBO(0) {}

Render::~Render() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &instanceVBO);
    glDeleteProgram(shaderProgram);
}

void Render::initialize() {
    shaderProgram = loadShader(defaultVertexShaderSource, defaultFragmentShaderSource);
    setupBuffers();
}

void Render::setShaderProgram(GLuint shaderProgram) {
    this->shaderProgram = shaderProgram;
}

void Render::setupBuffers() {
    // Define vertices for a quad (two triangles forming a square)
    GLfloat vertices[] = {
        // Positions        // Texture Coords
        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f,  // Bottom-left
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f,  // Bottom-right
         0.5f,  0.5f, 0.0f,  1.0f, 1.0f,  // Top-right
         0.5f,  0.5f, 0.0f,  1.0f, 1.0f,  // Top-right
        -0.5f,  0.5f, 0.0f,  0.0f, 1.0f,  // Top-left
        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f   // Bottom-left
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &instanceVBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Instance VBO setup for transformation matrices
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));
    glEnableVertexAttribArray(5);

    glVertexAttribDivisor(2, 1); // Tell OpenGL this is an instanced vertex attribute
    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Render::renderGameObject(const GameObject& gameObject, const glm::mat4& projection) {
    glUseProgram(shaderProgram);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, gameObject.getPosition());
    model = glm::rotate(model, glm::radians(gameObject.getRotation().x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(gameObject.getRotation().y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(gameObject.getRotation().z), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, gameObject.getScale());

    glm::mat4 combined = projection * model;

    GLuint transformLoc = glGetUniformLocation(shaderProgram, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(combined));

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void Render::checkCompileErrors(GLuint shader, std::string type) {
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "| ERROR::SHADER: Compile-time error: Type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "| ERROR::Program: Link-time error: Type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}

GLuint Render::loadShader(const char* vertexShaderSource, const char* fragmentShaderSource) {
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    checkCompileErrors(vertexShader, "VERTEX");

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    checkCompileErrors(fragmentShader, "FRAGMENT");

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    checkCompileErrors(program, "PROGRAM");

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

void Render::batchRenderGameObjects(const std::vector<std::shared_ptr<GameObject>>& gameObjects, const glm::mat4& projection) {
    std::cout << "hello" << std::endl;
}
