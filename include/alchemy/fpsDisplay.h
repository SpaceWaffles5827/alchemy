#ifndef FPSDISPLAY_H
#define FPSDISPLAY_H

#include <GLEW/glew.h>
#include <GLFW/glfw3.h>
#include <string>

class FPSDisplay {
public:
    // Singleton instance retrieval
    static FPSDisplay& getInstance();

    // Deleted copy constructor and assignment operator to prevent copying
    FPSDisplay(const FPSDisplay&) = delete;
    FPSDisplay& operator=(const FPSDisplay&) = delete;

    ~FPSDisplay() = default;

    void update();
    void render();

    void setIsVisable(bool visable); // Set visibility of FPS display
    bool isVisable() const; // Check if FPS display is visible

private:
    // Private constructor for singleton pattern
    FPSDisplay();

    double lastTime;
    int nbFrames;
    float fps;
    bool visible;

    std::string fpsText;

    void updateFPSText();
};

#endif // FPSDISPLAY_H
