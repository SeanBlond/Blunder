#include "../UIElements.h"
using namespace ui;

// Toggle Mouse Functions
void AttributeHeader::OnClick(StateMachine* state) { state->changeState(SM_UI_INTERACT); }
void AttributeHeader::OnHold(StateMachine* state) {}
void AttributeHeader::OnRelease(StateMachine* state)
{
    // Toggling the value
    state->exitState();
    *value = !(*value);
}

// Update Function
void AttributeHeader::UpdateElement(const ElementPosition& newPosition)
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
void AttributeHeader::RenderElement(UIRenderer* renderer, float textSize)
{
    // Drawing Base Quad
    renderer->addQuad(position.getCorners(), 0.2, colors::lightergrey.rgb());

    // Drawing Label Text
    renderer->addText(label, glm::vec3(position.getXCenter(), position.getYCenter(), 0), textSize, glm::vec3(1.0f), position.parentWindow->offset, CENTER);

    // Color Modifier
    glm::vec3 colorMod(1);
    if (clicked)
        colorMod = glm::vec3(0.75f);
    else if (highlighted)
        colorMod = glm::vec3(1.25f);

    // Drawing Dropdown symbol
    if (*value)
        renderer->addQuad(position.getStartCorners(position.getHeight()), 0.21f, colors::lightestgrey.rgb() * colorMod, position.parentWindow->offset, UI_DROPDOWN_F);
    else
        renderer->addQuad(position.getStartCorners(position.getHeight()), 0.21f, colors::lightestgrey.rgb() * colorMod, position.parentWindow->offset, UI_DROPDOWN_T);

}