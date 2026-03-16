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
    enum QuadStyle { QUAD_RECT, QUAD_CIRCLE };
    const int NUM_UI_ICONS = 11;
    enum UITexture { 
        UI_NO_TEXTURE = -1, 
        UI_DROPDOWN_T, 
        UI_DROPDOWN_F, 
        UI_RENDER_T, 
        UI_RENDER_F, 
        UI_DISPLAY_T, 
        UI_DISPLAY_F, 
        UI_DRAG_POSITION, 
        UI_FOLDER_SYMBOL, 
        UI_OBJECT_SYMBOL, 
        UI_LIGHT_SYMBOL, 
        UI_CAMERA_SYMBOL,
    };
    struct UIVertex
    {
        glm::vec3 Position;
        glm::vec2 TexCoord;
        glm::vec3 TextColor;
        glm::vec2 UVCoord;
        int Style;
    };

    class UIRenderer
    {
    public:
        // Constructors and Deconstructor
        UIRenderer(std::string fntFilePath, std::string fontBitmapFilePath, std::string uiBitmapFilePath, float textHeight = 0.8f);
        ~UIRenderer();

        // Getters
        Font* getTextRenderer() { return textRenderer; }

        // Functions
        void createBitmapUVData();
        void addText(std::string text, glm::vec3 position, float scale, glm::vec3 color, TextAlign alignment = LEFT);
        void addText(std::string text, glm::vec3 position, float scale, glm::vec3 color, glm::vec2 offset, TextAlign alignment = LEFT);
        void addQuad(glm::vec3 position, glm::vec2 size, glm::vec3 color, UITexture texture = UI_NO_TEXTURE, QuadStyle style = QUAD_RECT);
        void addQuad(glm::vec4 corners, float depth, glm::vec3 color, UITexture texture = UI_NO_TEXTURE, QuadStyle style = QUAD_RECT);
        void addQuad(glm::vec3 position, glm::vec2 size, glm::vec3 color, glm::vec2 offset, UITexture texture = UI_NO_TEXTURE, QuadStyle style = QUAD_RECT);
        void addQuad(glm::vec4 corners, float depth, glm::vec3 color, glm::vec2 offset, UITexture texture = UI_NO_TEXTURE, QuadStyle style = QUAD_RECT);
        void UpdateMesh();
        void renderText(glm::vec2 screenSize);
        void renderQuads(glm::vec2 screenSize);

    private:
        float textHeight;
        Font* textRenderer;
        shdr::Shader* quadShader;
        unsigned int VAO, VBO, EBO;
        int currentQuadCount;
        std::vector<UIVertex> vertices;
        std::vector<unsigned int> indices;

        shdr::Texture2D* uiBitmap;
        std::vector<glm::vec4> uiTextureCorners;
    };
}

#endif // !UI_RENDERER
