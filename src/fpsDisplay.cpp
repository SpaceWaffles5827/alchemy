#include <alchemy/fpsDisplay.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <glm/glm.hpp>
#include <alchemy/textRenderer.h>

FPSDisplay& FPSDisplay::getInstance() {
    static FPSDisplay instance;
    return instance;
}

FPSDisplay::FPSDisplay() : lastTime(0.0), nbFrames(0), fps(0.0f), visible(false) {
    lastTime = glfwGetTime();
}

void FPSDisplay::update() {
    double currentTime = glfwGetTime();
    nbFrames++;

    if (currentTime - lastTime >= 1.0) {
        fps = nbFrames / (currentTime - lastTime);
        nbFrames = 0;
        lastTime += 1.0;

        updateFPSText();
    }
}

void FPSDisplay::render() {
    if (visible) {
        TextRenderer::getInstance().renderText(fpsText, 10.0f, 770.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
    }
}

void FPSDisplay::setIsVisable(bool visable) {
    visible = visable;
}

bool FPSDisplay::isVisable() const {
    return visible;
}

void FPSDisplay::updateFPSText() {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << "FPS: " << fps;
    fpsText = oss.str();
}
