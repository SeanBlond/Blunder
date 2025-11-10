#include "UIRenderer.h"
using namespace ui;

// UI Quads
// Constructors and Deconstructor
UIRenderer::UIRenderer(std::string fontName, int fontSize, glm::mat4 projection)
{
    // Setting Up Projection Matrix;
    this->projection = projection;

    // Setting Up Text Renderer
    textRenderer = new Font(fontName, fontSize);

    // Shaders
    const char* quadVertexShader = R"(
        #version 330 core
        layout(location = 0) in vec3 aPos;
        layout(location = 2) in vec2 atexCoord;
        out vec2 texCoord;

        uniform mat4 projection;

        void main()
        {

            gl_Position = projection * vec4(aPos, 1);
            texCoord = atexCoord;
        }
        )";

    const char* quadFragmentShader = R"(
        #version 330 core
        out vec4 FragColor;
        in vec2 texCoord;

        uniform vec3 color;

        void main()
        {
            FragColor = vec4(vec3(color), 1);
        }
        )";

    // Setting Up Shaders
    quadShader = new shdr::Shader(quadVertexShader, quadFragmentShader, 1);

    // Configuring VAO and VBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 5, NULL, GL_DYNAMIC_DRAW);

    // Position Attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

    // Color Attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
UIRenderer::~UIRenderer()
{
    quadShader->deallocateShader();

    delete textRenderer;
    delete quadShader;

    textRenderer = nullptr;
    quadShader = nullptr;

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

// Functions
void UIRenderer::renderText(std::string text, float x, float y, float scale, glm::vec3 color, TextAlign alignment)
{
    textRenderer->setProjection(projection);
    textRenderer->RenderText(text, x, y, scale, color, alignment);
}
void UIRenderer::renderQuad(glm::vec3 position, glm::vec2 size, glm::vec3 color)
{
    // activate corresponding render state	
    quadShader->useShader();
    quadShader->setMat4("projection", projection);
    glBindVertexArray(VAO);

    // Selecting Color
    quadShader->setVec3("color", color);

    // update VBO for each character
    float vertices[6][5] = {
        // Positions                                                                                                                // Tex Coords
        {position.x - (size.x / 2.0f),   position.y + (size.y / 2.0f),   position.z,   0.0f, 0.0f},
        {position.x - (size.x / 2.0f),   position.y - (size.y / 2.0f),   position.z,   0.0f, 1.0f},
        {position.x + (size.x / 2.0f),   position.y - (size.y / 2.0f),   position.z,   1.0f, 1.0f},

        {position.x - (size.x / 2.0f),   position.y + (size.y / 2.0f),   position.z,   0.0f, 0.0f},
        {position.x + (size.x / 2.0f),   position.y - (size.y / 2.0f),   position.z,   1.0f, 1.0f},
        {position.x + (size.x / 2.0f),   position.y + (size.y / 2.0f),   position.z,   1.0f, 0.0f}
    };

    // update content of textVBO memory
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // render quad
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}