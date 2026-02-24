#include "../UIElements.h"
using namespace ui;

// Functions
void ColorSelector::CreateMesh()
{
    // Mesh vertices & indices
    std::vector<Vertex> vertices =
    {
                 // Position            Normal         TexCoord
        Vertex({ glm::vec3(0, 0, 0.9),  glm::vec3(0),  glm::vec2(0, 0) }), // Bottom Left
        Vertex({ glm::vec3(1, 0, 0.9),  glm::vec3(0),  glm::vec2(1, 0) }), // Bottom Right
        Vertex({ glm::vec3(1, 1, 0.9),  glm::vec3(0),  glm::vec2(1, 1) }), // Top Right
        Vertex({ glm::vec3(0, 1, 0.9),  glm::vec3(0),  glm::vec2(0, 1) }), // Top Left
    };
    std::vector<unsigned int> indices =
    {
        0, 1, 2,
        2, 3, 0
    };

    // Creating mesh object
    csMesh = new Mesh(vertices, indices);

    // Creating Shaders
    const char* csVertexShader = R"(
    #version 330 core
    layout (location = 0) in vec3 vertex;
    layout (location = 1) in vec3 Normal;
    layout (location = 2) in vec2 TexCoord;
    out vec2 uv;
        
    uniform mat4 transform;

    void main()
    {
        gl_Position = transform * vec4(vertex, 1.0);
        uv = TexCoord;
    }  
    )";

    const char* csFragmentShader = R"(
    #version 330 core
    in vec2 uv;
    out vec4 fragColor;

    uniform vec2 mousePos;

    const float M_PI = 3.14159265359;
    const float maskSize = 0.075;

    vec3 hsv2rgb(vec3 c)
    {
        vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
        vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
        return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
    }

    float getHue(vec2 pos)
    {
        vec2 hueUV = (pos * 2.0) - 1.0;
        return atan(hueUV.x, hueUV.y) / (2.0 * M_PI) + 0.5;
    }
    float getSaturation(vec2 pos)
    {
        return length((pos * 2.0) - 1.0);
    }

    void main()
    {
        // Color Circle
        float circleMask = step(getSaturation(uv), 1.0);
        vec3 colorCircle = hsv2rgb(vec3(getHue(uv), getSaturation(uv), circleMask));
    
        // Mouse Pos & Mask
        vec2 mouseUV = uv - mousePos;
        float mouseMask = step(length(mouseUV), maskSize);
        float mouseMaskOutline = (length(mouseUV) > 0.75 * maskSize ? 0.75 : 1.0);
    
        // Getting Selected Color
        vec3 currentColor = hsv2rgb(vec3(getHue(mousePos), getSaturation(mousePos), mouseMaskOutline));
    
        // Output to screen
        fragColor = vec4(mix(colorCircle, currentColor, mouseMask), circleMask);
    }
    )";

    // Creating shader object
    csShader = new shdr::Shader(csVertexShader, csFragmentShader, 1);

}
ColorSelector::~ColorSelector()
{
    delete csMesh;
    delete csShader;
    csMesh = nullptr;
    csShader = nullptr;
}

// Mouse Functions
void ColorSelector::OnClick(StateMachine* state)
{

}
void ColorSelector::OnHold(StateMachine* state)
{

}
void ColorSelector::OnRelease(StateMachine* state)
{

}

// Render Function
void ColorSelector::RenderElement(UIRenderer* renderer, const ElementPosition& position, float textSize)
{
    float circleSize = position.getWidthBeforeSplit();

    // Setting up viewport to be drawn to
    glViewport(position.left_x + position.parentWindow->getXOffset(), position.bottom_y, circleSize, circleSize);

    // Shader settings
    csShader->useShader();
    glm::mat4 transform = smath::orthographic(0, 1, 0, 1);
    csShader->setMat4("transform", transform);
    csShader->setVec2("mousePos", glm::vec2(0.5));

    // Drawing Circle
    csMesh->DrawMesh();
}