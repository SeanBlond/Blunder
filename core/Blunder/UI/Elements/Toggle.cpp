#include "../UIElements.h"
using namespace ui;

// Toggle Mouse Functions
void Toggle::OnClick(StateMachine* state) {}
void Toggle::OnHold(StateMachine* state) {}
void Toggle::OnRelease(StateMachine* state)
{
    // Toggling the value
    state->exitState();
    *value = !(*value);
}

// Update Function
void Toggle::UpdateElement(const ElementPosition& newPosition)
{

}

// Render Function
void Toggle::RenderElement(UIRenderer* renderer, float textSize)
{
    float width = (position.right_x - position.left_x);
    float ySize = (position.top_y - position.bottom_y);
    float yPos = position.top_y - (ySize / 2);

    // Drawing Label Text
    renderer->addText(label, glm::vec3(position.split - position.getBuffer(), yPos, 0), textSize, glm::vec3(1.0f), position.parentWindow->offset, RIGHT);

    // Calculating Color
    glm::vec3 color;
    if (*value)
        color = colors::darkerGrey.rgb();
    else
        color = colors::blunderGreen.rgb();

    // Color Modifier
    glm::vec3 colorMod(1);
    if (clicked)
        colorMod = glm::vec3(0.75f);
    else if (highlighted)
        colorMod = glm::vec3(1.25f);

    // Drawing Toggle Box
    renderer->addQuad(position.getRightCorners(ySize), 0.2f, color * colorMod, position.parentWindow->offset);

    // Draw Check (eventually)

}
