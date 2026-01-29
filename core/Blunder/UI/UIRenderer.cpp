#include "UIRenderer.h"
using namespace ui;

// UI Quads
// Constructors and Deconstructor
UIRenderer::UIRenderer(std::string fntFilePath, std::string fontBitmapFilePath, std::string uiBitmapFilePath, float textHeight)
{ 
    // Setting Up Text Renderer
    textRenderer = new Font(fntFilePath, fontBitmapFilePath);
    this->textHeight = textHeight;

    // Shaders
    const char* quadVertexShader = R"(
        #version 330 core
        layout (location = 0) in vec3 vertex;
        layout (location = 1) in vec2 TexCoord;
        layout (location = 2) in vec3 UIColor;
        out vec2 TexCoords;
        out vec3 Color;
        
        uniform mat4 projection;

        void main()
        {
            gl_Position = projection * vec4(vertex, 1.0);
            TexCoords = TexCoord;
            Color = UIColor;
        }  
        )";

    const char* quadFragmentShader = R"(
        #version 330 core
        in vec2 TexCoords;
        in vec3 Color;
        out vec4 fragColor;

        uniform sampler2D text;

        void main()
        {    
            vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
            fragColor = vec4(Color, 1.0) * sampled;
            //fragColor = vec4(TexCoords, 0.0, 1.0);
        }  
        )";

    // Setting Up Shader
    quadShader = new shdr::Shader(quadVertexShader, quadFragmentShader, 1);

    // Creating the UI Bitmap
    uiBitmap = new shdr::Texture2D(uiBitmapFilePath.c_str(), GL_LINEAR_MIPMAP_LINEAR, GL_TEXTURE_WRAP_S);

    // Setting up UI Bitmap UV data
    createBitmapUVData();

    // Temp Quad Data
    addQuad(glm::vec3(0), glm::vec2(1), glm::vec3(1));

    // Configuring VAO, VBO, and EBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(TextVertex), &vertices[0], GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_DYNAMIC_DRAW);

    // Position Attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TextVertex), (void*)0);

    // TexCoord Attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(TextVertex), (void*)offsetof(TextVertex, TexCoord));

    // TextColor Attribute
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(TextVertex), (void*)offsetof(TextVertex, TextColor));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Loading UI Texture bitmap

    // Loading Textures for each texture enum value
    //ui_textures.resize(11);
    //ui_textures[UI_DISPLAY_T] = new shdr::Texture2D("assets/UI-Textures/Display_T.png", GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT);
    //ui_textures[UI_DISPLAY_F] = new shdr::Texture2D("assets/UI-Textures/Display_F.png", GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT);
    //ui_textures[UI_DROPDOWN_T] = new shdr::Texture2D("assets/UI-Textures/Dropdown_T.png", GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT);
    //ui_textures[UI_DROPDOWN_F] = new shdr::Texture2D("assets/UI-Textures/Dropdown_F.png", GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT);
    //ui_textures[UI_RENDER_T] = new shdr::Texture2D("assets/UI-Textures/Render_T.png", GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT);
    //ui_textures[UI_RENDER_F] = new shdr::Texture2D("assets/UI-Textures/Render_F.png", GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT);
    //ui_textures[UI_FOLDER_SYMBOL] = new shdr::Texture2D("assets/UI-Textures/Folder_Symbol.png", GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT);
    //ui_textures[UI_OBJECT_SYMBOL] = new shdr::Texture2D("assets/UI-Textures/Object_Symbol.png", GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT);
    //ui_textures[UI_LIGHT_SYMBOL] = new shdr::Texture2D("assets/UI-Textures/Light_Symbol.png", GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT);
    //ui_textures[UI_CAMERA_SYMBOL] = new shdr::Texture2D("assets/UI-Textures/Camera_Symbol.png", GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT);
}
UIRenderer::~UIRenderer()
{
    quadShader->deallocateShader();

    delete textRenderer;
    delete quadShader;
    delete uiBitmap;
    textRenderer = nullptr;
    quadShader = nullptr;
    uiBitmap = nullptr;

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

// Functions
void UIRenderer::createBitmapUVData()
{
    // corner vec4 format
    // y---w
    // |   |
    // |   |
    // x---z

    // Getting each icon from each row from top to bottom
    for (int i = 6; i > 0; i--)
    {
        // Left Icon
        glm::vec4 tempCorner = glm::vec4(
            1.0f,
            (float)i / 6.0f,
            0.5f,
            (float)(i - 1) / 6.0f
            );
        uiTextureCorners.push_back(tempCorner);
        
        // Right Icon
        tempCorner = glm::vec4(
            0.5f,
            (float)i / 6.0f,
            0.0f,
            (float)(i - 1) / 6.0f
            );
        uiTextureCorners.push_back(tempCorner);
    }

}
void UIRenderer::addText(std::string text, glm::vec3 position, float scale, glm::vec3 color, TextAlign alignment)
{
    // Raising text if the value is not set
    if (position.z <= 0.0f)
        position.z = textHeight;

    textRenderer->AddText(text, position, scale, color, alignment);
}
void UIRenderer::renderText(glm::mat4 projection)
{
    textRenderer->RenderText(projection);
}
void UIRenderer::addQuad(glm::vec3 position, glm::vec2 size, glm::vec3 color, UITexture texture)
{
    // Getting the TexCoord location of the texture
    glm::vec4 uvCoords;
    if (texture == UI_NO_TEXTURE)
        uvCoords = glm::vec4(0);
    else
        uvCoords = uiTextureCorners[texture];

    // Adding Vertex Information
    TextVertex tempVertices[4] = {
                        // Positions                                                         // TexCoord                         // TextColor
        TextVertex({ glm::vec3(position.x - (size.x / 2.0f), position.y - (size.y / 2.0f), position.z),  glm::vec2(uvCoords.x, uvCoords.w),  color }),  // Bottom Left
        TextVertex({ glm::vec3(position.x + (size.x / 2.0f), position.y - (size.y / 2.0f), position.z),  glm::vec2(uvCoords.z, uvCoords.w),  color }),  // Bottom Right
        TextVertex({ glm::vec3(position.x + (size.x / 2.0f), position.y + (size.y / 2.0f), position.z),  glm::vec2(uvCoords.z, uvCoords.y),  color }),  // Top Right
        TextVertex({ glm::vec3(position.x - (size.x / 2.0f), position.y + (size.y / 2.0f), position.z),  glm::vec2(uvCoords.x, uvCoords.y),  color }),  // Top Left
    };
    vertices.insert(vertices.end(), std::begin(tempVertices), std::end(tempVertices));


    // Adding Index Information
    unsigned int offset = vertices.size() - 4;
    unsigned int tempIndices[6] = {
        offset,     offset + 1,  offset + 2,
        offset + 2, offset + 3,  offset
    };
    indices.insert(indices.end(), std::begin(tempIndices), std::end(tempIndices));
}
void UIRenderer::UpdateMesh()
{
    glBindVertexArray(VAO);

    // Updating VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(TextVertex), &vertices[0], GL_DYNAMIC_DRAW);

    // Updating EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_DYNAMIC_DRAW);

    glBindVertexArray(0);
}
void UIRenderer::renderQuads(glm::mat4 projection)
{
    // Updating Mesh Data
    UpdateMesh();

    // Setting up shader and texture
    quadShader->setMat4("projection", projection);
    quadShader->useShader();
    uiBitmap->Bind(0);

    // Rendeing the meshes
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Clearing Vertices & Indices after rendering
    vertices.clear();
    indices.clear();
}