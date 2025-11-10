#ifndef FONT
#pragma once

#include <iostream>
#include <map>
#include <string>

#include "../shader/shader.h"
#include "../math/smath.h"
#include "../ew/external/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

enum TextAlign { LEFT, CENTER, RIGHT };

struct Character {
    unsigned int TextureID; // ID handle of the glyph texture
    glm::ivec2   Size;      // Size of glyph
    glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
    unsigned int Advance;   // Horizontal offset to advance to next glyph
};

class Font
{
public:
    // Constructors
    Font(std::string fontName, int fontSize);

    // Getters
    Character getCharacter(std::string::const_iterator character) { return Characters[*character]; }

    // Setters
    void setProjection(glm::mat4 projection) { this->projection = projection; }

    // Functions
    void RenderText(std::string text, float x, float y, float scale, glm::vec3 color = glm::vec3(1), TextAlign alignment = LEFT);

private:
    shdr::Shader* textShader;
    glm::mat4 projection;
    std::map<GLchar, Character> Characters;
    unsigned int textVAO, textVBO;
    unsigned int texture;
};

#endif // !FONT
