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
    enum UITexture { UI_NO_TEXTURE = -1, UI_DISPLAY_T, UI_DISPLAY_F, UI_DROPDOWN_T, UI_DROPDOWN_F, UI_RENDER_T, UI_RENDER_F, UI_FOLDER_SYMBOL, UI_OBJECT_SYMBOL, UI_LIGHT_SYMBOL, UI_CAMERA_SYMBOL };

    class UIRenderer
    {
    public:
        // Constructors and Deconstructor
        UIRenderer(std::string fntFilePath, std::string bitmapFilePath);
        ~UIRenderer();

        // Getters
        Font* getTextRenderer() { return textRenderer; }

        // Functions
        //void addQuad(glm::vec3 position, glm::vec2 size, glm::vec3 color);
        void addText(std::string text, glm::vec3(position), float scale, glm::vec3 color = glm::vec3(1), TextAlign alignment = LEFT);
        void renderText(glm::mat4 projection);
        void addQuad(glm::vec3 position, glm::vec2 size, glm::vec3 color, UITexture texture = UI_NO_TEXTURE);
        void UpdateMesh();
        void renderQuads(glm::mat4 projection);

    private:
        Font* textRenderer;
        shdr::Shader* quadShader;
        unsigned int VAO, VBO, EBO;
        std::vector<TextVertex> vertices;
        std::vector<unsigned int> indices;

        shdr::Texture2D* uiBitmap;
        std::vector<glm::vec4> uiTextureCorners;
    };
}

#endif // !UI_RENDERER
