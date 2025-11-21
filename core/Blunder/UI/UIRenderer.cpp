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
        layout(location = 1) in vec2 atexCoord;
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
    const char* quadTextureFragmentShader = R"(
        #version 330 core
        out vec4 FragColor;
        in vec2 texCoord;

        uniform sampler2D texture1;

        void main()
        {
            FragColor = texture(texture1, texCoord);
        }
        )";

    // Setting Up Shaders
    quadShader = new shdr::Shader(quadVertexShader, quadFragmentShader, 1);
    quadTextureShader = new shdr::Shader(quadVertexShader, quadTextureFragmentShader, 1);

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

    // Loading Textures for each texture enum value
    ui_textures.resize(11);
    ui_textures[UI_DISPLAY_T] = new shdr::Texture2D("assets/UI-Textures/Display_T.png", GL_LINEAR, GL_REPEAT);
    ui_textures[UI_DISPLAY_F] = new shdr::Texture2D("assets/UI-Textures/Display_F.png", GL_LINEAR, GL_REPEAT);
    ui_textures[UI_DROPDOWN_T] = new shdr::Texture2D("assets/UI-Textures/Dropdown_T.png", GL_LINEAR, GL_REPEAT);
    ui_textures[UI_DROPDOWN_F] = new shdr::Texture2D("assets/UI-Textures/Dropdown_F.png", GL_LINEAR, GL_REPEAT);
    ui_textures[UI_FOLDER_T] = new shdr::Texture2D("assets/UI-Textures/Folder_T.png", GL_LINEAR, GL_REPEAT);
    ui_textures[UI_FOLDER_F] = new shdr::Texture2D("assets/UI-Textures/Folder_F.png", GL_LINEAR, GL_REPEAT);
    ui_textures[UI_RENDER_T] = new shdr::Texture2D("assets/UI-Textures/Render_T.png", GL_LINEAR, GL_REPEAT);
    ui_textures[UI_RENDER_F] = new shdr::Texture2D("assets/UI-Textures/Render_F.png", GL_LINEAR, GL_REPEAT);
    ui_textures[UI_OBJECT_SYMBOL] = new shdr::Texture2D("assets/UI-Textures/Object Symbol.png", GL_LINEAR, GL_REPEAT);
    ui_textures[UI_LIGHT_SYMBOL] = new shdr::Texture2D("assets/UI-Textures/Light Symbol.png", GL_LINEAR, GL_REPEAT);
    ui_textures[UI_CAMERA_SYMBOL] = new shdr::Texture2D("assets/UI-Textures/Camera Symbol.png", GL_LINEAR, GL_REPEAT);
}
UIRenderer::~UIRenderer()
{
    quadShader->deallocateShader();
    quadTextureShader->deallocateShader();

    delete textRenderer;
    delete quadShader;

    textRenderer = nullptr;
    quadShader = nullptr;
    quadTextureShader = nullptr;

    for (int i = 0; i < ui_textures.size(); i++)
    {
        delete ui_textures[i];
    }
    ui_textures.clear();

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
void UIRenderer::renderTextureQuad(glm::vec3 position, glm::vec2 size, UITexture texture)
{
    // activate corresponding render state	
    quadTextureShader->useShader();
    quadTextureShader->setMat4("projection", projection);
    glBindVertexArray(VAO);

    // Bind and setting the texture
    ui_textures[texture]->Bind(0);
    quadTextureShader->setInt("texture1", 0);

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
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}