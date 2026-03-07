#include "../UIElements.h"
using namespace ui;

// Float Slider Mouse Functions
void FloatSlider::OnClick(StateMachine* state)
{
    // Setting State
    state->changeState(SM_UI_TRANSLATING);

    // Storing Initial Mouse Pos
    slideStarted = false;
    saveValue = *value;
    initialMousePos = state->getMouse()->mousePos;
}
void FloatSlider::OnHold(StateMachine* state)
{
    // Checking if value should slide with mouse
    if (abs(state->getMouse()->mousePos.x - initialMousePos.x) > 10.0f || slideStarted)
    {
        slideStarted = true;
        saveValue += state->getMouse()->mouseDelta.x * speed;
        setValue(saveValue);
    }
}
void FloatSlider::OnRelease(StateMachine* state)
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
void FloatSlider::UpdateElement(const ElementPosition& newPosition)
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
void FloatSlider::RenderElement(UIRenderer* renderer, float textSize)
{
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

        *value = tempValue;
        text.setStored(false);
    }
    if (!text.getTyping())
    {
        text.setText(std::to_string(*value));
    }

    // Drawing Label Text
    renderer->addText(label, glm::vec3(position.split - position.getBuffer(), position.getYCenter(), 0), textSize, glm::vec3(1.0f), position.parentWindow->offset, RIGHT);

    // Color Modifier
    glm::vec3 colorMod(1);
    if (clicked || text.getTyping())
        colorMod = glm::vec3(0.75f);
    else if (highlighted)
        colorMod = glm::vec3(1.25f);

    // Drawing Base Box
    renderer->addQuad(position.getRightCorners(), 0.2f, colors::darkerGrey.rgb() * colorMod, position.parentWindow->offset);

    // Draw Slider Box
    float sliderWidth = position.getWidthAfterSplit() * getPercentage();
    renderer->addQuad(position.getRightCorners(sliderWidth), 0.225f, colors::blunderGreen.rgb() * colorMod, position.parentWindow->offset);

    // Drawing Value
    text.addText(renderer, glm::vec3(position.getMiddleAfterSplit(), position.getYCenter(), 0), textSize, glm::vec3(1.0f), position.parentWindow->offset, CENTER);
}