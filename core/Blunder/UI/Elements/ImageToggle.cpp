#include "../UIElements.h"
using namespace ui;

// Toggle Mouse Functions
void ImageToggle::OnClick(StateMachine* state) { state->changeState(SM_UI_INTERACT); }
void ImageToggle::OnHold(StateMachine* state) {}
void ImageToggle::OnRelease(StateMachine* state)
{
    // Toggling the value
    state->exitState();
    *value = !(*value);
}

// Update Function
void ImageToggle::UpdateElement(const ElementPosition& newPosition)
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
void ImageToggle::RenderElement(UIRenderer* renderer, float textSize)
{
    // Color Modifier
    glm::vec3 colorMod(1);
    if (clicked)
        colorMod = glm::vec3(0.75f);
    else if (highlighted)
        colorMod = glm::vec3(1.25f);

    // Drawing the texture
    if (*value)
        renderer->addQuad(position.getCorners(), 0.2f, imageColor.rgb() * colorMod, position.getOffset(), trueTex);
    else
        renderer->addQuad(position.getCorners(), 0.2f, imageColor.rgb() * colorMod, position.getOffset(), falseTex);
}
