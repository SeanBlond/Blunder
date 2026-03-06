#include "../UIElements.h"
using namespace ui;

// Color Entry Mouse Functions
void ColorEntry::OnClick(StateMachine* state) {}
void ColorEntry::OnHold(StateMachine* state) {}
void ColorEntry::OnRelease(StateMachine* state)
{
    // TODO: Create Color Pop-Up window
    std::cout << "Color Entry clicked" << std::endl;
    state->exitState();
}

// Update Function
void ColorEntry::UpdateElement(const ElementPosition& newPosition)
{
    this->position = newPosition;

    // Updating interactable
    if (interactable)
    {
        delete interactable;
    }
    glm::vec4 corners = position.getRightCorners() + glm::vec4(
        position.parentWindow->getXOffset(),
        position.parentWindow->getYOffset(),
        position.parentWindow->getXOffset(),
        position.parentWindow->getYOffset()
    );
    interactable = new ui::QuadInteractable(corners);
}

// Render Function
void ColorEntry::RenderElement(UIRenderer* renderer, float textSize)
{
    float width = (position.right_x - position.left_x);
    float height = (position.top_y - position.bottom_y);
    float yPos = position.top_y - (height / 2);

    // Drawing Label Text
    renderer->addText(label, glm::vec3(position.split - position.getBuffer(), yPos, 0), textSize, glm::vec3(1.0f), position.parentWindow->offset, RIGHT);

    // Color Modifier
    glm::vec3 colorMod(1);
    if (clicked)
        colorMod = glm::vec3(0.75f);
    else if (highlighted)
        colorMod = glm::vec3(1.25f);

    // Drawing Color Box
    renderer->addQuad(position.getRightCorners(), 0.2f, value->rgb() * colorMod, position.parentWindow->offset);

    // Drawing Alpha/Color Box
    renderer->addQuad(position.getEndCorners(height), 0.21f, glm::vec3(value->alpha()), position.parentWindow->offset);
}
