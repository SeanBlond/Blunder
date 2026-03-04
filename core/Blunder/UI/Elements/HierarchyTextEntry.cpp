#include "../UIElements.h"
using namespace ui;

// Hierarchy Text Entry Mouse Functions
void HierarchyTextEntry::OnClick(StateMachine* state)
{
    // If the clicks occur within 0.25 second, it will register as a double click, and do text entry
    if (TimeManager::getInstance()->getTime() - clickTime < 0.25f)
    {
        textTriggered = true;
        saveValue = *value;
    }
    // Otherwise, it will register as a single click
    else
    {
        // Checking if object reference exists before setting it to be the selected object
        if (object != nullptr)
        {
            state->selectObject(object);
        }
    }

    // Resetting click time
    clickTime = TimeManager::getInstance()->getTime();
}
void HierarchyTextEntry::OnHold(StateMachine* state) {}
void HierarchyTextEntry::OnRelease(StateMachine* state)
{
    // Checking if typing was triggered
    if (textTriggered)
    {
        text.setTyping(true);
        text.selectAll();
        state->setTextInput(&text);
        state->changeState(SM_UI_TYPING);
        textTriggered = false;
    }
}

// Update Function
void HierarchyTextEntry::UpdateElement(const ElementPosition& newPosition)
{
    this->position = newPosition;

    // TODO: Update interactable
}

// Render Function
void HierarchyTextEntry::RenderElement(UIRenderer* renderer, float textSize)
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

    // Color Modifier
    glm::vec3 colorMod(1);
    if (clicked || text.getTyping())
        colorMod = glm::vec3(0.75f);
    else if (highlighted)
        colorMod = glm::vec3(1.25f);

    // Drawing Text Box
    if (text.getTyping())
        renderer->addQuad(position.getCorners(), 0.2f, colors::darkerGrey.rgb() * colorMod, position.parentWindow->offset);

    // Drawing Value
    text.addText(renderer, glm::vec3(position.left_x, yPos, 0), textSize, glm::vec3(1.0f), position.parentWindow->offset);
}
