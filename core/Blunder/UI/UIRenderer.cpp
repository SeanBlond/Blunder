#include "UIRenderer.h"
using namespace ui;

// UI Quads
// Constructors and Deconstructor
UIRenderer::UIRenderer(std::string fntFilePath, std::string fontBitmapFilePath, std::string uiBitmapFilePath, float textHeight)
{ 
    // Setting Up Text Renderer
    currentQuadCount = 0;
    textRenderer = new Font(fntFilePath, fontBitmapFilePath);
    this->textHeight = textHeight;

    // Shaders
    {
        const char* quadVertexShader = R"(
        #version 330 core
        layout (location = 0) in vec3 vertex;
        layout (location = 1) in vec2 TexCoord;
        layout (location = 2) in vec3 UIColor;
        layout (location = 3) in vec2 UVCoord;
        layout (location = 4) in int Style;
        out vec2 TexCoords;
        out vec2 UVCoords;
        out vec3 Color;
        flat out int vertStyle;
        
        uniform mat4 projection;

        void main()
        {
            gl_Position = projection * vec4(vertex, 1.0);
            TexCoords = TexCoord;
            UVCoords = UVCoord;
            Color = UIColor;
            vertStyle = Style;
        }  
        )";

        const char* quadFragmentShader = R"(
        #version 330 core
        in vec2 TexCoords;
        in vec3 Color;
        in vec2 UVCoords;
        flat in int vertStyle;

        out vec4 fragColor;

        uniform sampler2D text;
        uniform bool drawCircle;

        void main()
        {    
            float alphaValue = 1;
            if (vertStyle == 1) // DRAW CIRCLE
            {
                alphaValue = step(length(UVCoords - 0.5), 0.5);
            }
            
            vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).a * alphaValue);
            fragColor = vec4(Color, 1.0) * sampled;
            //fragColor = vec4(TexCoords, 0.0, 1.0);
        }  
        )";

        // Setting Up Shader
        quadShader = new shdr::Shader(quadVertexShader, quadFragmentShader, 1);
    }

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
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(UIVertex), &vertices[0], GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_DYNAMIC_DRAW);

    // Position Attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(UIVertex), (void*)0);

    // TexCoord Attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(UIVertex), (void*)offsetof(UIVertex, TexCoord));

    // TextColor Attribute
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(UIVertex), (void*)offsetof(UIVertex, TextColor));

    // UV Attribute
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(UIVertex), (void*)offsetof(UIVertex, UVCoord));

    // UV Attribute
    glEnableVertexAttribArray(4);
    glVertexAttribIPointer(4, 1, GL_INT, sizeof(UIVertex), (void*)offsetof(UIVertex, Style));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
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

    // Getting each icon from each row from top to bottom (pngs are flipped!)
    int iconsRead = 0;
    const int NUM_COLUMNS = 8;
    const int NUM_ROWS = 2;
    float xUV = 1.0f / (float)NUM_COLUMNS;
    float yUV = 1.0f / (float)NUM_ROWS;

    while (iconsRead < NUM_UI_ICONS)
    {
        float xOffset = ((float)(iconsRead % NUM_COLUMNS) / (float)NUM_COLUMNS);
        int yOffset = iconsRead / NUM_COLUMNS;

        // Adding an icon corners
        glm::vec4 tempCorner = glm::vec4(
            xOffset,
            yOffset,
            xOffset + xUV,
            yOffset + yUV
        );
        uiTextureCorners.push_back(tempCorner);

        iconsRead++;
    }
}
void UIRenderer::addText(std::string text, glm::vec3 position, float scale, glm::vec3 color, TextAlign alignment)
{
    // Raising text if the value is not set
    if (position.z <= 0.0f)
        position.z = textHeight;

    textRenderer->AddText(text, position, scale, color, alignment);
}
void UIRenderer::addText(std::string text, glm::vec3 position, float scale, glm::vec3 color, glm::vec2 offset, TextAlign alignment)
{
    addText(text, position + glm::vec3(offset, 0), scale, color, alignment);
}
void UIRenderer::addQuad(glm::vec3 position, glm::vec2 size, glm::vec3 color, UITexture texture, QuadStyle style)
{
    // Converting position & size to corners and depth so the other addQuad() function can be called
    glm::vec4 corners = glm::vec4(
        position.x - (size.x / 2.0f),
        position.y - (size.y / 2.0f),
        position.x + (size.x / 2.0f),
        position.y + (size.y / 2.0f)
    );
    float depth = position.z;

    addQuad(corners, depth, color, texture, style);
}
void UIRenderer::addQuad(glm::vec4 corners, float depth, glm::vec3 color, UITexture texture, QuadStyle style)
{
    // Getting the TexCoord location of the texture
    glm::vec4 uvCoords;
    if (texture == UI_NO_TEXTURE)
        uvCoords = glm::vec4(0.075f, 0.25f, 0.075f, 0.25f);
    else
        uvCoords = uiTextureCorners[texture];

    // Adding Vertex Information
    /*
        (x,w)------(z,w)
         |            |
         |            |
         |            |
        (x,y)------(z,y)
    */
    UIVertex tempVertices[4] = {
        //         Positions                                TexCoord                            Color   UV Coords         Style
        UIVertex({ glm::vec3(corners.x, corners.y, depth),  glm::vec2(uvCoords.x, uvCoords.w),  color,  glm::vec2(0, 0),  style }),  // Bottom Left
        UIVertex({ glm::vec3(corners.z, corners.y, depth),  glm::vec2(uvCoords.z, uvCoords.w),  color,  glm::vec2(1, 0),  style }),  // Bottom Right
        UIVertex({ glm::vec3(corners.z, corners.w, depth),  glm::vec2(uvCoords.z, uvCoords.y),  color,  glm::vec2(1, 1),  style }),  // Top Right
        UIVertex({ glm::vec3(corners.x, corners.w, depth),  glm::vec2(uvCoords.x, uvCoords.y),  color,  glm::vec2(0, 1),  style }),  // Top Left
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
void UIRenderer::addQuad(glm::vec3 position, glm::vec2 size, glm::vec3 color, glm::vec2 offset, UITexture texture, QuadStyle style)
{
    addQuad(position + glm::vec3(offset, 0.0f), size, color, texture, style);
}
void UIRenderer::addQuad(glm::vec4 corners, float depth, glm::vec3 color, glm::vec2 offset, UITexture texture, QuadStyle style)
{
    glm::vec4 newCorners = glm::vec4(
        corners.x + offset.x,
        corners.y + offset.y,
        corners.z + offset.x,
        corners.w + offset.y
    );
    addQuad(newCorners, depth, color, texture, style);
}

void UIRenderer::UpdateMesh()
{
    glBindVertexArray(VAO);

    // Updating VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(UIVertex), &vertices[0], GL_DYNAMIC_DRAW);

    // Updating EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_DYNAMIC_DRAW);

    glBindVertexArray(0);
}
void UIRenderer::renderQuads(glm::vec2 screenSize)
{
    // Checking if there are quads to draw before attempting to render it
    if (indices.size() != 0)
    {
        // Updating Mesh Data
        UpdateMesh();

        // Creating projection matrix
        glm::mat4 projection = smath::orthographic(0, screenSize.x, 0, screenSize.y);

        // Setting up shader and texture
        quadShader->useShader();
        quadShader->setMat4("projection", projection);
        uiBitmap->Bind(0);

        // Rendeing the meshes
        glViewport(0, 0, screenSize.x, screenSize.y);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // Clearing the mesh data
        vertices.clear();
        indices.clear();
    }
}
void UIRenderer::renderText(glm::vec2 screenSize)
{
    textRenderer->RenderText(screenSize);
}