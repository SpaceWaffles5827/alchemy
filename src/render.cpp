#include <alchemy/render.h>
#include <iostream>
#include <tuple>
#include <alchemy/frustum.h>
#include <alchemy/global.h>
#include <alchemy/hotbar.h>

// Default shaders
const char* Render::defaultVertexShaderSource = R"(
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord; // Add texture coordinates as an input
layout(location = 2) in mat4 instanceTransform;

out vec2 TexCoord; // Pass texture coordinates to the fragment shader

void main()
{
    gl_Position = instanceTransform * vec4(aPos, 1.0);
    TexCoord = aTexCoord; // Pass texture coordinates to fragment shader
}
)";


const char* Render::defaultFragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D ourTexture;

void main()
{
    FragColor = texture(ourTexture, TexCoord);
}
)";

Render::Render() : shaderProgram(0), VAO(0), VBO(0), instanceVBO(0), EBO(0), maxVerticesPerBatch(10000) {}

Render::~Render() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &instanceVBO);
    glDeleteBuffers(1, &EBO);
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
        -0.5f,  0.5f, 0.0f,  0.0f, 1.0f   // Top-left
    };

    GLuint indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenBuffers(1, &instanceVBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Instance VBO setup for transformation matrices
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, maxVerticesPerBatch * sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW); // Initial size, adjust as needed
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
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Render::renderUI(int width, int height) {
    glm::mat4 projectionUI = glm::ortho(
        0.0f, static_cast<float>(width),
        static_cast<float>(height), 0.0f,
        -1.0f, 1.0f
    );

    std::vector<std::shared_ptr<Renderable>> renderables;

    // Always render the hot bar
    HotBar& playerHotbar = HotBar::getInstance();
    renderables.push_back(std::shared_ptr<Renderable>(&playerHotbar, [](Renderable*) {}));
    Renderable selectedSlotObject = playerHotbar.getSelectedSlotObject();
    renderables.push_back(std::shared_ptr<Renderable>(&selectedSlotObject, [](Renderable*) {}));
    for (auto& slot : playerHotbar.getHotBarSlots()) {
        renderables.push_back(std::shared_ptr<Renderable>(&slot, [](Renderable*) {}));
    }

    // Render the inventory only if it's being displayed
    if (game.getDispalyInventory()) {
        Inventory& playerInventory = Inventory::getInstance();

        renderables.push_back(std::shared_ptr<Renderable>(&playerInventory, [](Renderable*) {}));
        for (auto& slot : playerInventory.getInventorySlots()) {
            renderables.push_back(std::shared_ptr<Renderable>(&slot, [](Renderable*) {}));
        }
    }

    batchRenderGameObjects(renderables, projectionUI);

    if (game.getDispalyInventory() && InputManager::getInstance().getIsDragging()) {
        auto& draggedSlot = Inventory::getInstance().getInventorySlots()[game.getSelectedSlotIndex()];

        if (draggedSlot.getIsVisable()) {
            double xpos, ypos;
            glfwGetCursorPos(GraphicsContext::getInstance().getWindow(), &xpos, &ypos);

            glm::vec2 correctedTopLeft = glm::vec2(draggedSlot.getTextureTopLeft().x, 1.0f - draggedSlot.getTextureTopLeft().y);
            glm::vec2 correctedBottomRight = glm::vec2(draggedSlot.getTextureBottomRight().x, 1.0f - draggedSlot.getTextureBottomRight().y);

            auto draggedItemRenderable = std::make_shared<InventorySlot>(
                glm::vec3(static_cast<float>(xpos), static_cast<float>(ypos), 0.0f),
                glm::vec3(0.0f),
                draggedSlot.getScale().x,
                draggedSlot.getScale().y,
                game.getDragTextureId(),
                correctedTopLeft,
                correctedBottomRight
            );

            batchRenderGameObjects({ draggedItemRenderable }, projectionUI);
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

struct TextureGroupComparator {
    bool operator()(const std::tuple<GLuint, glm::vec2, glm::vec2>& lhs, const std::tuple<GLuint, glm::vec2, glm::vec2>& rhs) const {
        if (std::get<0>(lhs) != std::get<0>(rhs))
            return std::get<0>(lhs) < std::get<0>(rhs);

        if (std::get<1>(lhs).x != std::get<1>(rhs).x)
            return std::get<1>(lhs).x < std::get<1>(rhs).x;
        if (std::get<1>(lhs).y != std::get<1>(rhs).y)
            return std::get<1>(lhs).y < std::get<1>(rhs).y;

        if (std::get<2>(lhs).x != std::get<2>(rhs).x)
            return std::get<2>(lhs).x < std::get<2>(rhs).x;
        return std::get<2>(lhs).y < std::get<2>(rhs).y;
    }
};

void Render::batchRenderGameObjects(const std::vector<std::shared_ptr<Renderable>>& renderables, const glm::mat4& projection) {
    if (renderables.empty()) return;

    Frustum frustum;
    frustum.update(projection);

    // Group renderable objects by texture ID and texture coordinates using std::tuple with a custom comparator
    std::map<std::tuple<GLuint, glm::vec2, glm::vec2>, std::vector<std::shared_ptr<Renderable>>, TextureGroupComparator> textureGroups;

    for (const auto& renderable : renderables) {
       // Check if the renderable object is within the camera's view
       // commenting this out for now because there
       // is something wrong with implementation
       // if (frustum.isInFrustum(renderable->getPosition(), renderable->getBoundingRadius())) {} 
        if (!renderable->getIsVisable()) {
            continue;
        }
        auto groupKey = std::make_tuple(renderable->getTextureID(), renderable->getTextureTopLeft(), renderable->getTextureBottomRight());
        textureGroups[groupKey].push_back(renderable);
    }

    // Render each group of objects with the same texture and texture coordinates
    for (const auto& group : textureGroups) {
        GLuint textureID = std::get<0>(group.first);
        const glm::vec2& texTopLeft = std::get<1>(group.first);
        const glm::vec2& texBottomRight = std::get<2>(group.first);
        const auto& objects = group.second;

        if (objects.empty()) continue;

        size_t totalObjects = objects.size();
        size_t maxInstances = maxVerticesPerBatch / 6;
        size_t numBatches = (totalObjects + maxInstances - 1) / maxInstances;

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);

        glBindTexture(GL_TEXTURE_2D, textureID);

        // Set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glGenerateMipmap(GL_TEXTURE_2D);

        std::vector<glm::mat4> instanceTransforms;
        instanceTransforms.reserve(maxInstances);

        // Update vertices with specific texture coordinates for this batch
        GLfloat vertices[] = {
            // Positions        // Texture Coords (dynamic)
            -0.5f, -0.5f, 0.0f,  texTopLeft.x, texBottomRight.y,  // Bottom-left
             0.5f, -0.5f, 0.0f,  texBottomRight.x, texBottomRight.y,  // Bottom-right
             0.5f,  0.5f, 0.0f,  texBottomRight.x, texTopLeft.y,  // Top-right
            -0.5f,  0.5f, 0.0f,  texTopLeft.x, texTopLeft.y   // Top-left
        };

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

        for (size_t batchIndex = 0; batchIndex < numBatches; ++batchIndex) {
            size_t startIdx = batchIndex * maxInstances;
            size_t endIdx = (std::min)(startIdx + maxInstances, totalObjects);

            instanceTransforms.clear();
            for (size_t i = startIdx; i < endIdx; ++i) {
                const auto& renderable = objects[i];
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, renderable->getPosition());
                model = glm::rotate(model, glm::radians(renderable->getRotation().x), glm::vec3(1.0f, 0.0f, 0.0f));
                model = glm::rotate(model, glm::radians(renderable->getRotation().y), glm::vec3(0.0f, 1.0f, 0.0f));
                model = glm::rotate(model, glm::radians(renderable->getRotation().z), glm::vec3(0.0f, 0.0f, 1.0f));
                model = glm::scale(model, renderable->getScale());

                glm::mat4 combined = projection * model;
                instanceTransforms.push_back(combined);
            }

            glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);

            void* bufferData = glMapBufferRange(GL_ARRAY_BUFFER, 0, instanceTransforms.size() * sizeof(glm::mat4), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
            if (bufferData) {
                std::memcpy(bufferData, instanceTransforms.data(), instanceTransforms.size() * sizeof(glm::mat4));
                glUnmapBuffer(GL_ARRAY_BUFFER);
            }

            glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, static_cast<GLsizei>(endIdx - startIdx));
        }

        glBindVertexArray(0);
    }
}

void Render::checkCompileErrors(GLuint shader, const std::string& type) {
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n";
        }
    }
    else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n";
        }
    }
}
