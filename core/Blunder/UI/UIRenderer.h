#ifndef UI_RENDERER
#pragma once

#include <sstream>
#include <iostream>
#include <iomanip>
#include <vector>

#include "../../ew/external/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "../../shader/shader.h"
#include "../../math/smath.h"
#include "../../Font/font.h"

namespace ui
{
    struct UIQuad
    {
        UIQuad(glm::vec3 position, glm::vec3 color, glm::vec2 size) : position(position), color(color), size(size) {}
        glm::vec3 position;
        glm::vec3 color;
        glm::vec2 size;
    };

    class UIRenderer
    {
    public:
        // Constructors and Deconstructor
        UIRenderer(std::string fontName, int fontSize, glm::mat4 projection);
        ~UIRenderer();

        // Getters
        Font* getTextRenderer() { return textRenderer; }

        // Setters
        void setProjection(glm::mat4 projection) { this->projection = projection; }

        // Functions
        //void addQuad(glm::vec3 position, glm::vec2 size, glm::vec3 color);
        void renderText(std::string text, float x, float y, float scale, glm::vec3 color = glm::vec3(1), TextAlign alignment = LEFT);
        void renderQuad(glm::vec3 position, glm::vec2 size, glm::vec3 color);

    private:
        Font* textRenderer;
        shdr::Shader* quadShader;
        glm::mat4 projection;
        unsigned int VAO, VBO;
        //std::vector<UIQuad> quads;
    };
}

#endif // !UI_RENDERER
