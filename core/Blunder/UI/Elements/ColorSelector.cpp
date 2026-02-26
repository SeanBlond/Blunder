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

        const char* svFragmentShader = R"(
    #version 330 core
    in vec2 uv;
    out vec4 fragColor;

    uniform vec2 mousePos;
    uniform float currentHue;

    const float selectSize = 0.05;

    vec3 hsv2rgb(vec3 c)
    {
        vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
        vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
        return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
    }

    vec2 getValueAtPos(vec2 pos)
    {
        float x = clamp(pos.x, selectSize, 1.0 - selectSize);
        float y = clamp(pos.y, selectSize, 1.0 - selectSize);
        return vec2(x, y);

    }
    vec3 getColorAtPos(vec2 pos)
    {
        return hsv2rgb(vec3(currentHue, getValueAtPos(pos)));
    }

    void main()
    {
        // Getting mouse data
        float mouseMask = step(length(uv - getValueAtPos(mousePos)), selectSize);
        float mouseMaskOutline = step(length(uv - getValueAtPos(mousePos)), selectSize * 0.8);
    
        // Getting Selected Color
        vec3 mouseCol = (mouseMaskOutline >= 0.75 ? getColorAtPos(mousePos) : vec3(1.0));
    
        // Getting background mask
        vec2 background = uv * vec2(1.0 + selectSize * 2.0) - vec2(selectSize);
        float alphaMask = (background.x < 0.0 || background.x > 1.0 ? 0.0 : 1.0);
        alphaMask *= (background.y < 0.0 || background.y > 1.0 ? 0.0 : 1.0);
    
        // Output to screen
        vec3 backgroundColor = mix(vec3(0.5), getColorAtPos(uv), alphaMask);
        fragColor = vec4(mix(backgroundColor, mouseCol, mouseMask), max(alphaMask, mouseMask));
    }
    )";

        const char* hFragmentShader = R"(
    #version 330 core
    in vec2 uv;
    out vec4 fragColor;

    uniform vec2 mousePos;
    const float selectHeight = 0.05f;

    vec3 hsv2rgb(vec3 c)
    {
        vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
        vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
        return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
    }
    float sdBox( in vec2 p, in vec2 b )
    {
        vec2 d = abs(p)-b;
        return length(max(d,0.0)) + min(max(d.x,d.y),0.0);
    }
    float getValueAtY(float y)
    {
        return clamp(y, (selectHeight * 0.5), 1.0 - (selectHeight * 0.5));
    }
    vec3 getHueAtY(float y)
    {
        return hsv2rgb(vec3(getValueAtY(y), 1.0, 1.0));
    }

    void main()
    {
        // Getting selected value
        float selectedValue = mousePos.y;
    
        // Value Slide Background
        float background = uv.y * (1.0 + selectHeight) - (selectHeight * 0.5);
        float alphaMask = (background < 0.0 || background > 1.0 ? 0.0 : 1.0);
    
        // Box for current selected value
        vec2 boxUV = (1.0 - uv * 2.0);
        float box = sdBox(vec2(boxUV) + vec2(0, 2.0 * selectedValue - 1.0), vec2(1.0, selectHeight));
        float boxMask = step(box, 0.0);
        float boxOutline = step(box, -0.025);
    
        // Output to screen
        vec3 boxValue = mix(vec3(1.0), getHueAtY(selectedValue), boxOutline);
        vec3 value = mix(getHueAtY(background), boxValue, boxMask);
        fragColor = vec4(vec3(value), max(alphaMask, boxMask));
    }
    )";

        // Creating shader object
        svShader = new shdr::Shader(csVertexShader, svFragmentShader, 1);
        hShader = new shdr::Shader(csVertexShader, hFragmentShader, 1);
    }

}
ColorSelector::~ColorSelector()
{
    delete csMesh;
    delete svShader;
    delete hShader;
    csMesh = nullptr;
    svShader = nullptr;
    hShader = nullptr;
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
    float svSize = position.getWidth() - position.getBuffer() - position.getFixedUnit();

    // Setting up color circle viewport
    glViewport(position.left_x + position.parentWindow->getXOffset(), position.bottom_y, svSize, svSize);

    // Shader settings
    svShader->useShader();
    glm::mat4 transform = smath::orthographic(0, 1, 0, 1);
    svShader->setMat4("transform", transform);
    svShader->setVec2("mousePos", glm::vec2(0.0f, 1.0f));
    svShader->setFloat("currentHue", 0.0f);

    // Draw SV Selector
    csMesh->DrawMesh();

    // Rendering the Value Slider
    glm::vec2 sliderSize = glm::vec2(position.getWidthAfterSplit(), position.getWidthBeforeSplit());

    // Setting up color circle viewport
    glViewport(position.right_x - position.getWidthAfterSplit() + position.parentWindow->getXOffset(), position.bottom_y, sliderSize.x, sliderSize.y);

    // Shader settings
    hShader->useShader();
    transform = smath::orthographic(0, 1, 0, 1);
    hShader->setMat4("transform", transform);
    hShader->setVec2("mousePos", glm::vec2(0.0f));

    // Drawing Hue Selector
    csMesh->DrawMesh();
}