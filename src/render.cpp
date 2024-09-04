#include "../include/alchemy/render.h"
#include <iostream>
#include <algorithm>
#include <tuple>
#include "../include/alchemy/frustum.h"
#include "../include/alchemy/hotbar.h"
#include "../include/alchemy/inventory.h"
#include "../include/alchemy/inputManager.h"
#include "../include/alchemy/graphicsContext.h"

// Default shaders
const char *Render::defaultVertexShaderSource = R"(
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in mat4 instanceTransform;

out vec2 TexCoord;

void main()
{
    gl_Position = instanceTransform * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}
)";

const char *Render::defaultFragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D ourTexture;

void main()
{
    FragColor = texture(ourTexture, TexCoord);
}
)";

const char *Render::blueVertexShaderSource = R"(
#version 330 core
layout(location = 0) in vec3 aPos;

uniform mat4 transform;

void main()
{
    gl_Position = transform * vec4(aPos, 1.0);
}
)";

// Fragment shader for coloring the hexagon blue
const char *Render::blueFragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

void main()
{
    FragColor = vec4(0.0, 0.0, 1.0, 1.0); // Blue color
}
)";

Render::Render()
    : shaderProgram(0), blueShaderProgram(0), VAO(0), VBO(0), instanceVBO(0),
      EBO(0), maxVerticesPerBatch(10000) {}

Render::~Render() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &instanceVBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);
    glDeleteProgram(blueShaderProgram);
}

void Render::initialize() {
    shaderProgram =
        loadShader(defaultVertexShaderSource, defaultFragmentShaderSource);

    blueShaderProgram =
        loadShader(blueVertexShaderSource, blueFragmentShaderSource);

    // Disable VSync (no frame rate cap)
    glfwSwapInterval(0);
    setupBuffers();
}

void Render::setShaderProgram(GLuint shaderProgram) {
    this->shaderProgram = shaderProgram;
}

void Render::setupBuffers() {
    GLfloat vertices[] = {
        // Positions        // Texture Coords
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // Bottom-left
        0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, // Bottom-right
        0.5f,  0.5f,  0.0f, 1.0f, 1.0f, // Top-right
        -0.5f, 0.5f,  0.0f, 0.0f, 1.0f  // Top-left
    };

    GLuint indices[] = {0, 1, 2, 2, 3, 0};

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenBuffers(1, &instanceVBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Instance VBO setup for transformation matrices
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, maxVerticesPerBatch * sizeof(glm::mat4),
                 nullptr, GL_DYNAMIC_DRAW); // Initial size, adjust as needed
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
                          (void *)0);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
                          (void *)(sizeof(glm::vec4)));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
                          (void *)(2 * sizeof(glm::vec4)));
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
                          (void *)(3 * sizeof(glm::vec4)));
    glEnableVertexAttribArray(5);

    glVertexAttribDivisor(
        2, 1); // Tell OpenGL this is an instanced vertex attribute
    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Render::renderGameObject(const GameObject &gameObject,
                              const glm::mat4 &projection) {
    glUseProgram(shaderProgram);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, gameObject.getPosition());
    model = glm::rotate(model, glm::radians(gameObject.getRotation().x),
                        glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(gameObject.getRotation().y),
                        glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(gameObject.getRotation().z),
                        glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, gameObject.getScale());

    glm::mat4 combined = projection * model;

    GLuint transformLoc = glGetUniformLocation(shaderProgram, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(combined));

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Render::renderUI(int width, int height) {
    glm::mat4 projectionUI =
        glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height),
                   0.0f, -1.0f, 1.0f);

    std::vector<std::shared_ptr<Renderable>> renderables;

    HotBar &playerHotbar = HotBar::getInstance();

    // Add HotBar to renderables
    {
        std::shared_ptr<Renderable> hotbarPtr =
            std::make_shared<Renderable>(playerHotbar);
        renderables.push_back(hotbarPtr);
        auto selectedSlotObject =
            std::make_shared<Renderable>(playerHotbar.getSelectedSlotObject());
        renderables.push_back(selectedSlotObject);

        for (auto &slot : playerHotbar.getHotBarSlots()) {
            renderables.push_back(std::make_shared<Renderable>(slot));
        }
    }

    // Add Inventory to renderables if it's visible
    if (Inventory::getInstance().getIsVisable()) {
        Inventory &playerInventory = Inventory::getInstance();

        std::shared_ptr<Renderable> inventoryPtr =
            std::make_shared<Renderable>(playerInventory);
        renderables.push_back(inventoryPtr);

        for (auto &slot : playerInventory.getInventorySlots()) {
            renderables.push_back(std::make_shared<Renderable>(slot));
        }
    }

    // Batch render all UI elements
    batchRenderGameObjects(renderables, projectionUI);

    // Render dragged slot if the inventory is visible and the user is dragging
    // an item
    if (Inventory::getInstance().getIsVisable() &&
        InputManager::getInstance().getIsDragging()) {
        auto &draggedSlot =
            Inventory::getInstance().getInventorySlots()
                [InputManager::getInstance().getSelectedSlotIndex()];

        if (draggedSlot.getIsVisable()) {
            double xpos, ypos;
            glfwGetCursorPos(GraphicsContext::getInstance().getWindow(), &xpos,
                             &ypos);

            glm::vec2 correctedTopLeft =
                glm::vec2(draggedSlot.getTextureTopLeft().x,
                          1.0f - draggedSlot.getTextureTopLeft().y);
            glm::vec2 correctedBottomRight =
                glm::vec2(draggedSlot.getTextureBottomRight().x,
                          1.0f - draggedSlot.getTextureBottomRight().y);

            auto draggedItemRenderable = std::make_shared<InventorySlot>(
                glm::vec3(static_cast<float>(xpos), static_cast<float>(ypos),
                          0.0f),
                glm::vec3(0.0f), draggedSlot.getScale().x,
                draggedSlot.getScale().y,
                InputManager::getInstance().getDragTextureId(),
                correctedTopLeft, correctedBottomRight);

            batchRenderGameObjects({draggedItemRenderable}, projectionUI);
        }
    }
}

GLuint Render::loadShader(const char *vertexShaderSource,
                          const char *fragmentShaderSource) {
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

void Render::batchRenderGameObjects(
    const std::vector<std::shared_ptr<Renderable>> &renderables,
    const glm::mat4 &projection) {
    if (renderables.empty())
        return;

    Frustum frustum;
    frustum.update(projection);

    // Frustum culling: Only render visible objects
    std::vector<std::shared_ptr<Renderable>> visibleRenderables;
    for (const auto &renderable : renderables) {
        if (!renderable->getIsVisable()) {
            continue;
        }

        // Perform frustum culling
        visibleRenderables.push_back(renderable);
    }

    // Sort renderables by Z position, and for renderables with the same Z, sort
    // by their index
    std::sort(visibleRenderables.begin(), visibleRenderables.end(),
              [](const std::shared_ptr<Renderable> &a,
                 const std::shared_ptr<Renderable> &b) {
                  if (a->getPosition().z == b->getPosition().z) {
                      // If Z positions are equal, sort by index or an
                      // appropriate secondary criterion
                      return a->getYSortPosition().y >
                             b->getYSortPosition().y; // Sorting by index if they
                                                    // share
                                            // the same Z
                  }
                  return a->getPosition().z < b->getPosition().z;
              });

    // Ensure shader and VAO are only bound once
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);

    for (const auto &renderable : visibleRenderables) {
        // Bind the texture for this renderable
        GLuint textureID = renderable->getTextureID();
        glBindTexture(GL_TEXTURE_2D, textureID);

        // Set texture filtering parameters (if needed)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                        GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        // Update vertices with specific texture coordinates
        GLfloat vertices[] = {
            // Positions        // Texture Coords
            -0.5f,
            -0.5f,
            0.0f,
            renderable->getTextureTopLeft().x,
            renderable->getTextureBottomRight().y, // Bottom-left
            0.5f,
            -0.5f,
            0.0f,
            renderable->getTextureBottomRight().x,
            renderable->getTextureBottomRight().y, // Bottom-right
            0.5f,
            0.5f,
            0.0f,
            renderable->getTextureBottomRight().x,
            renderable->getTextureTopLeft().y, // Top-right
            -0.5f,
            0.5f,
            0.0f,
            renderable->getTextureTopLeft().x,
            renderable->getTextureTopLeft().y // Top-left
        };

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

        // Compute model matrix (translation, rotation, scale) for this
        // renderable
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, renderable->getPosition());
        model = glm::rotate(model, glm::radians(renderable->getRotation().x),
                            glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(renderable->getRotation().y),
                            glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(renderable->getRotation().z),
                            glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, renderable->getScale());

        glm::mat4 combined = projection * model;

        // Upload the combined matrix to the GPU for this instance
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        void *bufferData =
            glMapBufferRange(GL_ARRAY_BUFFER, 0, sizeof(glm::mat4),
                             GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
        if (bufferData) {
            std::memcpy(bufferData, &combined, sizeof(glm::mat4));
            glUnmapBuffer(GL_ARRAY_BUFFER);
        }

        // Render the object
        glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, 1);
    }

    // Unbind the VAO and shader program after rendering
    glBindVertexArray(0);
    glUseProgram(0);
}

void Render::checkCompileErrors(GLuint shader, const std::string &type) {
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type
                      << "\n"
                      << infoLog << "\n";
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "ERROR::PROGRAM_LINKING_ERROR of type: " << type
                      << "\n"
                      << infoLog << "\n";
        }
    }
}

void Render::renderHexagon(const glm::vec3 &position, float radius,
                           const glm::mat4 &projection) {
    const int numSegments = 6; // Hexagon has 6 sides
    std::vector<GLfloat> vertices;

    // Generate vertices for the hexagon
    for (int i = 0; i <= numSegments; ++i) {
        float theta = 2.0f * 3.1415926f * float(i) / float(numSegments);
        float dx = radius * cosf(theta);
        float dy = radius * sinf(theta);

        vertices.push_back(dx);
        vertices.push_back(dy);
        vertices.push_back(0.0f); // Z coordinate
    }

    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat),
                 vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);

    // Use the blue shader program
    glUseProgram(blueShaderProgram);

    // Debug: Check if the shader program is bound correctly
    GLint currentProgram = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    glm::mat4 combined = projection * model;

    // Set the transform uniform in the vertex shader
    GLuint transformLoc = glGetUniformLocation(blueShaderProgram, "transform");
    if (transformLoc == -1) {
        std::cerr << "Failed to find the 'transform' uniform location!"
                  << std::endl;
        return;
    }

    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(combined));

    // Disable depth testing for 2D elements
    glDisable(GL_DEPTH_TEST);

    // Draw the hexagon
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, numSegments + 1);

    // Debug: Check for OpenGL errors
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL error: " << err << std::endl;
    }

    // Re-enable depth testing
    glEnable(GL_DEPTH_TEST);

    glBindVertexArray(0);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}
