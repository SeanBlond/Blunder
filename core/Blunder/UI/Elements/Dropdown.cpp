#include "../UIElements.h"
using namespace ui;

// Dropdown Mouse Functions
void Dropdown::OnClick(StateMachine* state)
{
    state->changeState(SM_UI_INTERACT);

    // Activating dropdown
    droppedDown = true;

    // Clamping value to bhe within the options range
    *value = smath::clamp(*value, 0, (int)options.size());
}
void Dropdown::OnHold(StateMachine* state)
{
    // Getting mouse Position (flipping the y because glfw is stupid)
    glm::vec2 mousePos = glm::vec2(
        state->getMouse()->mousePos.x,
        (float)(state->getWindowDimensions().y) - state->getMouse()->mousePos.y
    );

    // Checking each options for a mouse y-collision
    for (int i = 0; i < options.size(); i++)
    {
        // Setting corners
        float optionHeight = position.getHeight();
        float firstOptionYPos = (position.top_y - (optionHeight / 2)) + position.getYOffset();
        glm::vec2 optionCorners = glm::vec2(
            firstOptionYPos - (optionHeight / 2) - (optionHeight * (i + 1)),
            firstOptionYPos + (optionHeight / 2) - (optionHeight * (i + 1))
        );

        // Checking collision
        if (smath::checkUICollision_Y(mousePos, optionCorners))
        {
            *value = i;
        }
    }
}
void Dropdown::OnRelease(StateMachine* state)
{
    // Resetting values
    droppedDown = false;
    state->exitState();
}

// Update Function
void Dropdown::UpdateElement(const ElementPosition& newPosition)
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
void Dropdown::RenderElement(UIRenderer* renderer, float textSize)
{
    float width = position.getWidth();
    float ySize = position.getHeight();
    float yPos = position.top_y - (ySize / 2);

    // Drawing Label Text
    renderer->addText(label, glm::vec3(position.split - position.getBuffer(), yPos, 0), textSize, glm::vec3(1.0f), position.parentWindow->offset, RIGHT);

    // Color Modifier
    glm::vec3 colorMod(1);
    if (clicked)
        colorMod = glm::vec3(0.75f);
    else if (highlighted)
        colorMod = glm::vec3(1.25f);

    // Drawing Dropdown Box
    renderer->addQuad(position.getRightCorners(), 0.2f, colors::darkerGrey.rgb() * colorMod, position.parentWindow->offset);

    // Drawing Dropdown Icon
    ui::UITexture dropdownIcon = (droppedDown ? UI_DROPDOWN_T : UI_DROPDOWN_F);
    renderer->addQuad(position.getRightCorners(ySize), 0.21f, colors::lightestgrey.rgb(), position.parentWindow->offset, dropdownIcon);

    // Drawing each option if dropped down
    if (droppedDown)
    {

        // Setting optionYSize for UI interaction
        float optionWidth = position.getWidthAfterSplit();

        // Checking if there is enough space for the dropdown to drop downwards
        bool directionDown = true;
        float lowestPoint = position.bottom_y - ySize * options.size();
        if (lowestPoint < ySize)
        {
            // Dropdown needs to drop upwards
            directionDown = false;
            //optionSize *= glm::vec2(1, -1);
        }

        // Drawing a quad that outlines the options
        glm::vec4 outlineCorner = glm::vec4(
            position.split - position.getBuffer(),
            lowestPoint - position.getBuffer(),
            position.right_x + position.getBuffer(),
            position.bottom_y
        );
        renderer->addQuad(outlineCorner, 0.89f, glm::vec3(0.35f), position.parentWindow->offset);

        // Drawing each option
        for (int i = 0; i < options.size(); i++)
        {
            float optionYOffset = -(ySize * (i + 1));

            // Highlighting the option if it is currently selected
            glm::vec3 optionColor = (i == *value ? colors::blunderGreen.rgb() : colors::darkerGrey.rgb());

            // Drawing Dropdown Box
            renderer->addQuad(position.getRightCorners(glm::vec2(0, optionYOffset)), 0.9f, optionColor, position.parentWindow->offset);

            // Drawing Option Circle
            renderer->addQuad(glm::vec3(position.split + ySize * 0.5f, yPos + optionYOffset, 0.91f), glm::vec2(width * 0.02f), colors::lightestgrey.rgb(), position.parentWindow->offset, UI_NO_TEXTURE, QUAD_CIRCLE);

            // Drawing Value
            renderer->addText(options[i], glm::vec3(position.split + ySize, yPos + optionYOffset, 1), textSize, glm::vec3(1.0f), position.parentWindow->offset, LEFT);
        }
    }

    // Drawing Value
    renderer->addText(options[*value], glm::vec3(position.split + ySize, yPos, 0), textSize, glm::vec3(1.0f), position.parentWindow->offset, LEFT);
}
