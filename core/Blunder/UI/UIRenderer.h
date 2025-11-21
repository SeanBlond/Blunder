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

    constexpr int NUM_TEXTURES = 11;
    enum UITexture { UI_DISPLAY_T, UI_DISPLAY_F, UI_DROPDOWN_T, UI_DROPDOWN_F, UI_RENDER_T, UI_RENDER_F, UI_FOLDER_SYMBOL, UI_OBJECT_SYMBOL, UI_LIGHT_SYMBOL, UI_CAMERA_SYMBOL };

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
        void renderTextureQuad(glm::vec3 position, glm::vec2 size, UITexture texture);

    private:
        Font* textRenderer;
        shdr::Shader* quadShader;
        shdr::Shader* quadTextureShader;
        glm::mat4 projection;
        unsigned int VAO, VBO;
        std::vector<shdr::Texture2D*> ui_textures;
        //std::vector<UIQuad> quads;
    };
}

#endif // !UI_RENDERER
