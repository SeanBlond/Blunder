#ifndef FONT
#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <string>

#include "../shader/shader.h"
#include "../math/smath.h"
#include "../ew/external/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

enum TextAlign { LEFT, CENTER, RIGHT };

struct Character {
    glm::vec4    Positions; // Corner Positon of the glyph on the bitmap UV  y---w
    glm::ivec2   Size;      // Size of the glyph                             |   |
    glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph     |   |
    unsigned int Advance;   // Horizontal offset to advance to next glyph    x---z
};

class Font
{
public:
    // Constructors
    Font(std::string fntFile, std::string fontImage);

    // Getters
    Character getCharacter(std::string::const_iterator character) { return Characters[*character]; }

    // Setters
    void setProjection(glm::mat4 projection) { this->projection = projection; }

    // Functions
    void ReadFNTFile(std::string filePath);
    void RenderText(std::string text, float x, float y, float scale, glm::vec3 color = glm::vec3(1), TextAlign alignment = LEFT);

private:
    // Font Info
    int fontSize;
    int lineHeight;
    uint8_t firstChar;
    uint8_t lastChar;
    glm::ivec2 bitmapSize;

    // OpenGL Rendering Info
    shdr::Shader* textShader;
    glm::mat4 projection;
    std::vector<Character> Characters;
    unsigned int textVAO, textVBO;
    unsigned int bitmapTexture;
};

#endif // !FONT
