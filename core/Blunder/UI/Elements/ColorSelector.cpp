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
    {
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

        const char* hsFragmentShader = R"(
    #version 330 core
    in vec2 uv;
    out vec4 fragColor;

    uniform vec2 mousePos;

    const float M_PI = 3.14159265359;
    const float maskSize = 0.05;

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
        return length((pos * (2.0 + 4.0 * maskSize)) - (1.0 + 2.0 * maskSize));
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

        const char* vFragmentShader = R"(
    #version 330 core
    in vec2 uv;
    out vec4 fragColor;

    uniform float mouseY;
    const float selectHeight = 0.05f;

    float sdBox( in vec2 p, in vec2 b )
    {
        vec2 d = abs(p)-b;
        return length(max(d,0.0)) + min(max(d.x,d.y),0.0);
    }

    float getValueAtY(float y)
    {
        return clamp(y, (selectHeight * 0.5), 1.0 - (selectHeight * 0.5));
    }

    void main()
    {
        // Getting selected value
        float selectedValue = getValueAtY(mouseY);
    
        // Value Slide Background
        float background = uv.y * (1.0 + selectHeight) - (selectHeight * 0.5);
        float alphaMask = (background < 0.0 || background > 1.0 ? 0.0 : 1.0);
    
        // Box for current selected value
        vec2 boxUV = (1.0 - uv * 2.0);
        float box = sdBox(vec2(boxUV) + vec2(0, 2.0 * selectedValue - 1.0), vec2(1.0, selectHeight));
        float boxMask = step(box, 0.0);
        float boxOutline = step(box, -0.025);
    
        // Output to screen
        float boxValue = mix(1.0 - selectedValue, selectedValue, boxOutline);
        float value = mix(background, boxValue, boxMask);
        fragColor = vec4(vec3(value), max(alphaMask, boxMask));
    }
    )";

        // Creating shader object
        hsShader = new shdr::Shader(csVertexShader, hsFragmentShader, 1);
        vShader = new shdr::Shader(csVertexShader, vFragmentShader, 1);
    }

}
ColorSelector::~ColorSelector()
{
    delete csMesh;
    delete hsShader;
    delete vShader;
    csMesh = nullptr;
    hsShader = nullptr;
    vShader = nullptr;
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
    // Rendering the Color Circle
    float circleSize = position.getWidthBeforeSplit();

    // Setting up color circle viewport
    glViewport(position.left_x + position.parentWindow->getXOffset(), position.bottom_y, circleSize, circleSize);

    // Shader settings
    hsShader->useShader();
    glm::mat4 transform = smath::orthographic(0, 1, 0, 1);
    hsShader->setMat4("transform", transform);
    hsShader->setVec2("mousePos", glm::vec2(0.5));

    // Drawing Circle
    csMesh->DrawMesh();

    // Rendering the Value Slider
    glm::vec2 sliderSize = glm::vec2(position.getWidthAfterSplit(), position.getWidthBeforeSplit());

    // Setting up color circle viewport
    glViewport(position.right_x - position.getWidthAfterSplit() + position.parentWindow->getXOffset(), position.bottom_y, sliderSize.x, sliderSize.y);

    // Shader settings
    vShader->useShader();
    transform = smath::orthographic(0, 1, 0, 1);
    vShader->setMat4("transform", transform);
    vShader->setFloat("mousePos", 0.75f);

    // Drawing Circle
    csMesh->DrawMesh();
}