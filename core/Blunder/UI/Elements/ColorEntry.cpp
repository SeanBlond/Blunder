#include "../UIElements.h"
#include "../../Runtime/WindowManager.h"
using namespace ui;

// Color Entry Mouse Functions
void ColorEntry::OnClick(StateMachine* state) {}
void ColorEntry::OnHold(StateMachine* state) {}
void ColorEntry::OnRelease(StateMachine* state)
{
    // TODO: Create Color Pop-Up window
    std::cout << "Color Entry clicked" << std::endl;
    state->getWindowManager()->setPopUpWindow(new ui::ColorWindow(150, 260, 
        position.right_x + (position.getBuffer() * 3.0f) + position.getXOffset(), 
        position.getYCenter() + position.getYOffset() - (260 - position.getFixedUnit()), value));
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
    glm::vec4 corners = position.getRightCorners() + position.getOffsetCorners();
    interactable = new ui::QuadInteractable(corners);
}

// Render Function
void ColorEntry::RenderElement(UIRenderer* renderer, float textSize)
{
    // Drawing Label Text
    renderer->addText(label, glm::vec3(position.split - position.getBuffer(), position.getYCenter(), 0), textSize, glm::vec3(1.0f), position.parentWindow->offset, RIGHT);

    // Color Modifier
    glm::vec3 colorMod(1);
    if (clicked)
        colorMod = glm::vec3(0.75f);
    else if (highlighted)
        colorMod = glm::vec3(1.25f);

    // Drawing Color Box
    renderer->addQuad(position.getRightCorners(), 0.2f, value->rgb() * colorMod, position.parentWindow->offset);

    // Drawing Alpha/Color Box
    renderer->addQuad(position.getEndCorners(position.getHeight()), 0.21f, glm::vec3(value->alpha()), position.parentWindow->offset);
}
