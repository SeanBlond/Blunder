#include "../UIElements.h"
using namespace ui;

// Functions
void ColorSelector::CreateShaders()
{
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
vec2 getClampedPos(vec2 pos) { return clamp(pos, vec2(selectSize), vec2(1.0 - selectSize)); }
vec3 getColorAtClampedPos(vec2 pos) { return hsv2rgb(vec3(currentHue, (getClampedPos(pos) - vec2(selectSize))/vec2(1.0 - 2.0 * selectSize))); }

void main()
{
    // Getting selected value
    vec2 clampedMousePos = getClampedPos(mousePos);
    float mouseMask = step(length(uv - clampedMousePos), selectSize);
    float mouseMaskOutline = step(length(uv - clampedMousePos), selectSize * 0.8);
    
    // Getting Selected Color
    vec3 mouseCol = (mouseMaskOutline >= 0.75 ? getColorAtClampedPos(mousePos) : vec3(1.0));
    
    // Getting background mask
    vec2 background = uv * vec2(1.0 + selectSize * 2.0) - vec2(selectSize);
    float alphaMask = (background.x < 0.0 || background.x > 1.0 ? 0.0 : 1.0);
    alphaMask *= (background.y < 0.0 || background.y > 1.0 ? 0.0 : 1.0);
    
    // Output to screen
    vec3 backgroundColor = mix(vec3(0.5), getColorAtClampedPos(uv), alphaMask);
    fragColor = vec4(mix(backgroundColor, mouseCol, mouseMask), max(alphaMask, mouseMask));
}
    )";

    const char* hFragmentShader = R"(
#version 330 core
in vec2 uv;
out vec4 fragColor;

uniform vec2 mousePos;
const float selectHeight = 0.1f;

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
float getClampedY(float y) { return clamp(y, (selectHeight * 0.5), 1.0 - (selectHeight * 0.5)); }
vec3 getColorAtClampedY(float y) { return hsv2rgb(vec3((getClampedY(y) - 0.5 * selectHeight)/(1.0 - selectHeight), 1.0, 1.0)); }

void main()
{
    // Getting mouse value
    float clampedMouse = getClampedY(mousePos.y);
    
    // Value Slide Background
    float background = uv.y * (1.0 + selectHeight) - (selectHeight * 0.5);
    float alphaMask = (background < 0.0 || background > 1.0 ? 0.0 : 1.0);
    
    // Box for current selected value
    float box = sdBox(vec2(1.0 - uv * 2.0) + vec2(0, 2.0 * clampedMouse - 1.0), vec2(1.5, selectHeight));
    float boxMask = step(box, 0.0);
    float boxOutline = step(box, -0.025);
    
    // Output to screen
    vec3 boxValue = mix(vec3(1.0), getColorAtClampedY(clampedMouse), boxOutline);
    vec3 value = mix(getColorAtClampedY(background), boxValue, boxMask);
    fragColor = vec4(mix(vec3(0.0), value, max(alphaMask, boxMask)), max(alphaMask, boxMask));
}
    )";

    const char* aFragmentShader = R"(
#version 330 core
in vec2 uv;
out vec4 fragColor;

uniform vec2 mousePos;
const float selectHeight = 0.1f;

float sdBox( in vec2 p, in vec2 b )
{
    vec2 d = abs(p)-b;
    return length(max(d,0.0)) + min(max(d.x,d.y),0.0);
}
float getClampedY(float y) { return clamp(y, (selectHeight * 0.5), 1.0 - (selectHeight * 0.5)); }
float getValueAtClampedY(float y) { return (getClampedY(y) - 0.5 * selectHeight)/(1.0 - selectHeight); }

void main()
{
    // Getting mouse value
    float clampedMouse = getClampedY(mousePos.y);
    
    // Background UV and Mask
    float background = uv.y * (1.0 + selectHeight) - (selectHeight * 0.5);
    float alphaMask = (background < 0.0 || background > 1.0 ? 0.0 : 1.0);
    
    // Background grid
    float gridValue = 1.0 - 0.75 * mod((floor(uv.x * 4.0) + floor(uv.y * 19.0)), 2.0);
    gridValue = mix(gridValue, 1.0, getValueAtClampedY(uv.y));
    
    // Box for current selected value
    float box = sdBox(vec2(1.0 - uv * 2.0) + vec2(0, 2.0 * clampedMouse - 1.0), vec2(1.5, selectHeight));
    float boxMask = step(box, 0.0);
    float boxOutline = step(box, -0.025);
    
    // Output to screen
    float boxValue = mix(1.0, getValueAtClampedY(clampedMouse), boxOutline);
    float value = mix(gridValue, boxValue, boxMask);
    fragColor = vec4(vec3(mix(0.0, value, max(alphaMask, boxMask))), max(alphaMask, boxMask));
}
    )";

    // Creating shader objectS
    svShader = new shdr::Shader(csVertexShader, svFragmentShader, 1);
    hShader = new shdr::Shader(csVertexShader, hFragmentShader, 1);
    aShader = new shdr::Shader(csVertexShader, aFragmentShader, 1);
}
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

}
ColorSelector::~ColorSelector()
{
    delete csMesh;
    delete svShader;
    delete hShader;
    delete aShader;
    csMesh = nullptr;
    svShader = nullptr;
    hShader = nullptr;
    aShader = nullptr;
}

// Mouse Functions
void ColorSelector::OnClick(StateMachine* state)
{
    // Getting mouse Position (flipping the y because glfw is stupid)
    glm::vec2 mousePos = glm::vec2(
        state->getMouse()->mousePos.x,
        (float)(state->getWindowDimensions().y) - state->getMouse()->mousePos.y
    );

    setSelectedInteraction(mousePos);
}
void ColorSelector::OnHold(StateMachine* state)
{
    // Getting mouse Position (flipping the y because glfw is stupid)
    glm::vec2 mousePos = glm::vec2(
        state->getMouse()->mousePos.x,
        (float)(state->getWindowDimensions().y) - state->getMouse()->mousePos.y
    );

    // Managing interaction
    interactingColor->interacting = true;
    ManageMouseInteraction(mousePos);
}
void ColorSelector::OnRelease(StateMachine* state)
{
    // Resetting interaction
    interactingColor->interacting = false;
    selectedInteraction = -1;
    state->exitState();
}

// Update Function
void ColorSelector::UpdateElement(const ElementPosition& newPosition)
{
    this->position = newPosition;

    // Updating interactable
    if (interactable)
    {
        delete interactable;
    }
    glm::vec4 corners = position.getCorners() + position.getOffsetCorners();
    interactable = new ui::QuadInteractable(corners);
}

// Render Function
void ColorSelector::RenderElement(UIRenderer* renderer, float textSize)
{
    // Updating values while not interacting
    if (!interactingColor->interacting)
    {
        svMousePos = glm::vec2(interactingColor->selectedColor->s(), interactingColor->selectedColor->v());
        hMousePos = glm::vec2(0, interactingColor->selectedColor->h());
        aMousePos = glm::vec2(0, interactingColor->selectedColor->a());
    }

    // Getting widths of the different selectors
    float svWidth = position.getWidth() * (40.0f / 58.0f);
    float haWidth = position.getWidth() * (8.0f / 58.0f);

    // Setting up Viewport for mesh rendering
    glViewport(position.left_x + position.parentWindow->getXOffset(), position.bottom_y + position.parentWindow->getYOffset(), position.getWidth(), position.getHeight());

    // Drawing S&V Selector
    svShader->useShader();
    glm::mat4 transform = smath::orthographic(0, position.getWidth(), 0, position.getHeight()) * smath::scale(glm::vec3(svWidth, position.getHeight(), 1.0f));
    svShader->setMat4("transform", transform);
    svShader->setVec2("mousePos", svMousePos);
    svShader->setFloat("currentHue", interactingColor->selectedColor->h());
    csMesh->DrawMesh();

    // Drawing Hue Selector
    hShader->useShader();
    float hOffset = svWidth + position.getBuffer();
    transform = smath::orthographic(0, position.getWidth(), 0, position.getHeight()) * smath::translate(glm::vec3(hOffset, 0, 0)) * smath::scale(glm::vec3(haWidth, position.getHeight(), 1.0f));
    hShader->setMat4("transform", transform);
    hShader->setVec2("mousePos", hMousePos);
    csMesh->DrawMesh();

    // Drawing Alpha Selector
    aShader->useShader();
    float aOffset = hOffset + haWidth + position.getBuffer();
    transform = smath::orthographic(0, position.getWidth(), 0, position.getHeight()) * smath::translate(glm::vec3(aOffset, 0, 0)) * smath::scale(glm::vec3(haWidth, position.getHeight(), 1.0f));
    aShader->setMat4("transform", transform);
    aShader->setVec2("mousePos", aMousePos);
    csMesh->DrawMesh();
}
void ColorSelector::ManageMouseInteraction(glm::vec2 mousePos)
{
    // Checking what selector is being interacted with
    if (selectedInteraction == 0)
    {
        // Interacting with SV selector
        glm::vec2 selectorDimensions = glm::vec2(position.getWidthBeforeSplit(), position.getHeight());
        svMousePos = smath::clamp01((mousePos - (glm::vec2(position.left_x, position.bottom_y) + position.getOffset())) / selectorDimensions);
    }
    else if (selectedInteraction == 1)
    {
        // Interacting with H selector
        glm::vec2 selectorDimensions = glm::vec2(position.getWidthAfterSplit() * 0.5f, position.getHeight());
        hMousePos = smath::clamp01((mousePos - (glm::vec2(position.split, position.bottom_y) + position.getOffset())) / selectorDimensions);
    }
    else if (selectedInteraction == 2)
    {
        // Interacting with A selector
        glm::vec2 selectorDimensions = glm::vec2(position.getWidthAfterSplit() * 0.5f, position.getHeight());
        aMousePos = smath::clamp01((mousePos - (glm::vec2(position.getMiddleAfterSplit(), position.bottom_y) + position.getOffset())) / selectorDimensions);
    }

    // Setting the color
    interactingColor->selectedColor->setHSVA(glm::vec4(hMousePos.y, svMousePos, aMousePos.y));
}
void ColorSelector::setSelectedInteraction(glm::vec2 mousePos)
{
    // Getting the sv interaction corners
    glm::vec4 svCorners = position.getLeftCorners() + position.getOffsetCorners();

    // Checking what selector is being collided with
    if (smath::checkUICollision(mousePos, svCorners))
    {
        // Interacting with SV selector
        selectedInteraction = 0;
    }
    else
    {
        // Checking whether the H or A selector is collided with
        if (mousePos.x < position.getMiddleAfterSplit() + position.getXOffset())
        {
            // Interacting with H selector
            selectedInteraction = 1;
        }
        else
        {
            // Interacting with A selector
            selectedInteraction = 2;
        }
    }
}