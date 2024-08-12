#ifndef TEXTRENDERER_H
#define TEXTRENDERER_H

#include <map>
#include <string>
#include <GLEW/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

struct Character {
    GLuint textureID;  // ID handle of the glyph texture
    glm::ivec2 size;   // Size of the glyph
    glm::ivec2 bearing; // Offset from baseline to left/top of glyph
    GLuint advance;    // Offset to advance to next glyph
};

class TextRenderer {
public:
    TextRenderer(GLuint width, GLuint height);
    ~TextRenderer();

    // Load a font file and generate character textures
    void loadFont(const std::string& font, GLuint fontSize);

    // Render a string of text using the loaded font
    void renderText(const std::string& text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);

    void updateScreenSize(GLuint width, GLuint height);

private:
    std::map<GLchar, Character> characters; // Map storing character data
    GLuint VAO, VBO; // Vertex Array Object and Vertex Buffer Object
    GLuint shaderProgram; // Shader program for rendering text
    GLuint screenWidth, screenHeight; // Screen dimensions
    FT_Library ft; // FreeType library object

    void initRenderData(); // Initialize VAO, VBO, and shader
    GLuint loadShader(const char* vertexShaderSource, const char* fragmentShaderSource); // Helper function to load shaders
    void checkCompileErrors(GLuint shader, const std::string& type); // Check shader compilation/linking errors
};

#endif // TEXTRENDERER_H
