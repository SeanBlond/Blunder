#include "../UIElements.h"
using namespace ui;

// Float Entry Mouse Functions
void FloatEntry::OnClick(StateMachine* state)
{
    // Storing Initial Mouse Pos
    slideStarted = false;
    saveValue = *value;
    initialMousePos = state->getMouse()->mousePos;
}
void FloatEntry::OnHold(StateMachine* state)
{
    // Checking if value should slide with mouse
    if (abs(state->getMouse()->mousePos.x - initialMousePos.x) > 10.0f || slideStarted)
    {
        slideStarted = true;
        saveValue += state->getMouse()->mouseDelta.x * speed;
        setValue(saveValue);
    }
}
void FloatEntry::OnRelease(StateMachine* state)
{
    // Checking if Text should be typed
    if (!slideStarted)
    {
        text.setTyping(true);
        text.selectAll();
        state->setTextInput(&text);
        state->changeState(SM_UI_TYPING);
    }
    else
    {
        state->exitState();
        slideStarted = false;
    }
}

// Update Function
void FloatEntry::UpdateElement(const ElementPosition& newPosition)
{
    this->position = newPosition;

    // Updating interactable
    if (interactable)
    {
        delete interactable;
    }
    glm::vec4 corners = glm::vec4(
        position.getRightCorners().x,
        position.parentWindow->getHeight() - position.getRightCorners().w,
        position.getRightCorners().z,
        position.parentWindow->getHeight() - position.getRightCorners().y
    );
    interactable = new ui::QuadInteractable(corners);
}

// Render Function
void FloatEntry::RenderElement(UIRenderer* renderer, float textSize)
{
    float width = (position.right_x - position.left_x);
    float ySize = (position.top_y - position.bottom_y);
    float yPos = position.top_y - (ySize / 2);

    // Updating Text
    if (text.getStored())
    {
        float tempValue = *value;
        if ((int)(text.getText()[0]) == 0)
        {
            tempValue = 0;
        }
        else
        {
            try
            {
                tempValue = std::stof(text.getText());
            }
            catch (std::invalid_argument)
            {
                std::cout << "ERROR: Float Value could not be assigned from \"" << text.getText() << "\"" << std::endl;
            }
        }

        setValue(tempValue);
        text.setStored(false);
    }
    if (!text.getTyping())
    {
        text.setText(std::to_string(*value));
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

    // Rounding value to 3 decimals places and converting to string
    //std::stringstream stream;
    //stream << std::fixed << std::setprecision(3) << (*value);

    // Drawing Value
    text.addText(renderer, glm::vec3(position.getMiddleAfterSplit(), yPos, 0), textSize, glm::vec3(1.0f), position.parentWindow->offset, CENTER);
}