#include <alchemy/Player.h>

Player::Player(int clientId, const glm::vec3& color, float x, float y, float width, float height)
    : clientId(clientId), color(color), position(x, y), width(width), height(height), texture(0), textureLoaded(false) {}

Player::~Player() {
    if (texture) {
        glDeleteTextures(1, &texture);
    }
}

bool Player::loadTexture(const char* filename) {
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);
    if (!data) {
        std::cerr << "Failed to load texture '" << filename << "'." << std::endl;
        std::cerr << "STB Reason: " << stbi_failure_reason() << std::endl;
        return false;
    }

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    if (nrChannels == 3) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    }
    else if (nrChannels == 4) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    }
    else {
        std::cerr << "Unsupported texture format for '" << filename << "'." << std::endl;
        stbi_image_free(data);
        return false;
    }

    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    stbi_image_free(data);
    textureLoaded = true;
    return true;
}

void Player::render(GLuint shaderProgram, GLuint VAO, const glm::mat4& projection) const {
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);

    // Bind the texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(glGetUniformLocation(shaderProgram, "ourTexture"), 0);

    // Set the transformation matrix
    glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(position, 0.0f));
    transform = glm::scale(transform, glm::vec3(width, height, 1.0f)); // Apply the width and height
    glm::mat4 combined = projection * transform;
    GLint transformLoc = glGetUniformLocation(shaderProgram, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(combined));

    // Draw the player
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
}

void Player::updatePosition(float x, float y) {
    position = glm::vec2(x, y);
}

glm::vec2 Player::getPosition() const {
    return position;
}

int Player::getClientId() const {
    return clientId;
}

bool Player::isTextureLoaded() const {
    return textureLoaded;
}

float Player::getWidth() const {
    return width;
}

float Player::getHeight() const {
    return height;
}
