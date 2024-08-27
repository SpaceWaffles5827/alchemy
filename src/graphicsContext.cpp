#include <alchemy/graphicsContext.h>
#include <stb/stb_image.h>
#include <alchemy/global.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

GraphicsContext& GraphicsContext::getInstance() {
    static GraphicsContext instance;
    return instance;
}

GraphicsContext::GraphicsContext()
    : window(nullptr), title("Default Window"), cameraZoom(1.0f) {}

GraphicsContext::~GraphicsContext() {
    if (window) {
        glfwDestroyWindow(window);
    }
    glfwTerminate();
}

GLFWwindow*& GraphicsContext::getWindow() {
    return window;
}

void GraphicsContext::initialize() {
    initGLFW();
    initGLEW();
}

void GraphicsContext::initGLFW() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW!" << std::endl;
        std::exit(-1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    window = glfwCreateWindow(800, 800, title.c_str(), NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        std::exit(-1);
    }

    glfwMakeContextCurrent(window);
    glfwSetWindowUserPointer(window, this);
}

void GraphicsContext::initGLEW() {
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW!" << std::endl;
        std::exit(-1);
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

GLuint GraphicsContext::loadTexture(const char* path) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cerr << "Failed to load texture: " << path << std::endl;
    }
    stbi_image_free(data);

    return textureID;
}

void GraphicsContext::updateProjectionMatrix(int width, int height) {
    float aspectRatio = static_cast<float>(width) / height;
    float viewWidth = 20.0f * cameraZoom;
    float viewHeight = viewWidth / aspectRatio;

    auto player = game.getWorld().getPlayerById(game.getClientId());
    if (!player) return;

    glm::vec2 playerPos = player->getPosition();

    glm::mat4 projection = glm::ortho(
        playerPos.x - viewWidth / 2.0f, playerPos.x + viewWidth / 2.0f,
        playerPos.y - viewHeight / 2.0f, playerPos.y + viewHeight / 2.0f,
        -1.0f, 1.0f
    );

    game.setProjectionMatrix(projection);

    GLuint transformLoc = glGetUniformLocation(game.getShaderProgram(), "transform");
    glUseProgram(game.getShaderProgram());
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

float GraphicsContext::getCameraZoom() {
    return cameraZoom;
}

void GraphicsContext::setCameraZoom(float zoom) {
    cameraZoom = zoom;
}

void GraphicsContext::updateUiProjectionMatrix(int width, int height) {
    float aspectRatio = static_cast<float>(width) / height;
    float viewWidth = 20.0f * cameraZoom;
    float viewHeight = viewWidth / aspectRatio;

    glm::mat4 projection = glm::ortho(
        viewWidth / 2.0f, viewWidth / 2.0f,
        viewHeight / 2.0f, viewHeight / 2.0f,
        -1.0f, 1.0f
    );

    GLuint transformLoc = glGetUniformLocation(game.getShaderProgram(), "transform");
    glUseProgram(game.getShaderProgram());
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(projection));
}
