#include "../UIElements.h"
using namespace ui;

// Text Entry Mouse Functions
void TextEntry::OnClick(StateMachine* state)
{
    saveValue = *value;
}
void TextEntry::OnHold(StateMachine* state) {}
void TextEntry::OnRelease(StateMachine* state)
{
    text.setTyping(true);
    text.selectAll();
    state->setTextInput(&text);
    state->changeState(SM_UI_TYPING);
}

// Update Function
void TextEntry::UpdateElement(const ElementPosition& newPosition)
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
void TextEntry::RenderElement(UIRenderer* renderer, float textSize)
{
    float width = (position.right_x - position.left_x);
    float ySize = (position.top_y - position.bottom_y);
    float yPos = position.top_y - (ySize / 2);

    // Updating Text
    if (text.getStored())
    {
        *value = text.getText();
        text.setStored(false);
    }
    if (!text.getTyping())
    {
        text.setText(*value);
    }

    // Drawing Label Text
    renderer->addText(label, glm::vec3(position.split - position.getBuffer(), yPos, 0), textSize, glm::vec3(1.0f), position.parentWindow->offset, RIGHT);

    // Color Modifier
    glm::vec3 colorMod(1);
    if (clicked || text.getTyping())
        colorMod = glm::vec3(0.75f);
    else if (highlighted)
        colorMod = glm::vec3(1.25f);

    // Drawing Text Box
    renderer->addQuad(position.getRightCorners(), 0.2f, colors::darkerGrey.rgb() * colorMod, position.parentWindow->offset);

    // Drawing Value
    text.addText(renderer, glm::vec3(position.getMiddleAfterSplit(), yPos, 0), textSize, glm::vec3(1.0f), position.parentWindow->offset, CENTER);
}
