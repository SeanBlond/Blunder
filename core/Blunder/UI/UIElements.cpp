#include "UIElements.h"

using namespace ui;

// Attribute Functions
void Attribute::addFloatEntry(std::string label, float* value, float speed) 
{ 
    elements.push_back(new FloatEntry(label, value, speed));
}
void Attribute::addFloatSlider(std::string label, float* value, float speed, float min, float max)
{
    elements.push_back(new FloatSlider(label, value, speed, min, max));
}
void Attribute::addIntEntry(std::string label, int* value, float speed)
{
    elements.push_back(new IntEntry(label, value, speed));
}
void Attribute::addIntSlider(std::string label, int* value, float speed, int min, int max)
{
    elements.push_back(new IntSlider(label, value, speed, min, max));
}
void Attribute::addToggle(std::string label, bool* value)
{
    elements.push_back(new Toggle(label, value));
}


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
        float newValue = saveValue + ((state->getMouse()->mousePos.x - initialMousePos.x) / 100.0f) * speed;
        setValue(newValue);
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

// Float Slider Mouse Functions
void FloatSlider::OnClick(StateMachine* state)
{
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
        float newValue = saveValue + ((state->getMouse()->mousePos.x - initialMousePos.x) / 100.0f) * speed;
        setValue(newValue);
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

// Int Entry Mouse Functions
void IntEntry::OnClick(StateMachine* state)
{
    // Storing Initial Mouse Pos
    slideStarted = false;
    saveValue = *value;
    initialMousePos = state->getMouse()->mousePos;
}
void IntEntry::OnHold(StateMachine* state)
{
    // Checking if value should slide with mouse
    if (abs(state->getMouse()->mousePos.x - initialMousePos.x) > 10.0f || slideStarted)
    {
        slideStarted = true;
        float newValue = saveValue + ((state->getMouse()->mousePos.x - initialMousePos.x) / 100.0f) * speed;
        setValue(round(newValue));
    }
}
void IntEntry::OnRelease(StateMachine* state)
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

// Int Slider Mouse Functions
void IntSlider::OnClick(StateMachine* state)
{
    // Storing Initial Mouse Pos
    slideStarted = false;
    saveValue = *value;
    initialMousePos = state->getMouse()->mousePos;
}
void IntSlider::OnHold(StateMachine* state)
{
    // Checking if value should slide with mouse
    if (abs(state->getMouse()->mousePos.x - initialMousePos.x) > 10.0f || slideStarted)
    {
        slideStarted = true;
        float newValue = saveValue + ((state->getMouse()->mousePos.x - initialMousePos.x) / 100.0f) * speed;
        setValue(round(newValue));
    }
}
void IntSlider::OnRelease(StateMachine* state)
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

// Toggle Mouse Functions
void Toggle::OnClick(StateMachine* state)
{
    // Toggle does not do anything on click
}
void Toggle::OnHold(StateMachine* state)
{
    // Toggle does nothing on hold
}
void Toggle::OnRelease(StateMachine* state)
{
    // Toggling the value
    state->exitState();
    *value = !(*value);
    std::cout << "Toggle Clicked: " << *value << std::endl;
}


// Element Render Functions
void FloatEntry::RenderElement(UIRenderer* renderer, float ypos, float width, float textSize)
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

        setValue(tempValue);
        text.setStored(false);
    }
    if (!text.getTyping())
    {
        text.setText(std::to_string(*value));
    }

    // Drawing Label Text
    renderer->renderText(label, (width * 0.42f), ypos - (width * 0.035f), textSize, glm::vec3(1.0f), RIGHT);
    
    // Color Modifier
    glm::vec3 colorMod(1);
    if (clicked || text.getTyping())
        colorMod = glm::vec3(0.75f);
    else if (highlighted)
        colorMod = glm::vec3(1.25f);

    // Drawing Text Box
    renderer->renderQuad(glm::vec3(width * 0.69f, ypos, 0.2f), glm::vec2(width * 0.5f, width * 0.1f), colors::darkerGrey.rgb() * colorMod);

    // Rounding value to 3 decimals places and converting to string
    //std::stringstream stream;
    //stream << std::fixed << std::setprecision(3) << (*value);

    // Drawing Value
    text.renderText(renderer, (width * 0.69f), ypos - (width * 0.035f), textSize, glm::vec3(1.0f), CENTER);
}

void FloatSlider::RenderElement(UIRenderer* renderer, float ypos, float width, float textSize)
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
    renderer->renderText(label, (width * 0.42f), ypos - (width * 0.035f), textSize, glm::vec3(1.0f), RIGHT);

    // Color Modifier
    glm::vec3 colorMod(1);
    if (clicked || text.getTyping())
        colorMod = glm::vec3(0.75f);
    else if (highlighted)
        colorMod = glm::vec3(1.25f);

    // Drawing Base Box
    renderer->renderQuad(glm::vec3(width * 0.69f, ypos, 0.2f), glm::vec2(width * 0.5f, width * 0.1f), colors::darkerGrey.rgb() * colorMod);
    
    // Draw Slider Box
    float sliderWidth = width * 0.5f * getPercentage();
    float sliderPosX = (width * 0.69f) - (width * (1.0f - getPercentage()) * 0.25f);
    renderer->renderQuad(glm::vec3(sliderPosX, ypos, 0.225f), glm::vec2(sliderWidth, width * 0.1f), colors::blunderGreen.rgb() * colorMod);

    // Drawing Value
    text.renderText(renderer, (width * 0.69f), ypos - (width * 0.035f), textSize, glm::vec3(1.0f), CENTER);
}

void IntEntry::RenderElement(UIRenderer* renderer, float ypos, float width, float textSize)
{
    // Updating Text
    if (text.getStored())
    {
        int tempValue = *value;
        if ((int)(text.getText()[0]) == 0)
        {
            tempValue = 0;
        }
        else
        {
            try
            {
                tempValue = std::stoi(text.getText());
            }
            catch (std::invalid_argument)
            {
                std::cout << "ERROR: Int Value could not be assigned from \"" << text.getText() << "\"" << std::endl;
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
    renderer->renderText(label, (width * 0.42f), ypos - (width * 0.035f), textSize, glm::vec3(1.0f), RIGHT);

    // Color Modifier
    glm::vec3 colorMod(1);
    if (clicked || text.getTyping())
        colorMod = glm::vec3(0.75f);
    else if (highlighted)
        colorMod = glm::vec3(1.25f);

    // Drawing Text Box
    renderer->renderQuad(glm::vec3(width * 0.69f, ypos, 0.2f), glm::vec2(width * 0.5f, width * 0.1f), colors::darkerGrey.rgb() * colorMod);

    // Drawing Value
    text.renderText(renderer, (width * 0.69f), ypos - (width * 0.035f), textSize, glm::vec3(1.0f), CENTER);
}

void IntSlider::RenderElement(UIRenderer* renderer, float ypos, float width, float textSize)
{
    // Updating Text
    if (text.getStored())
    {
        int tempValue = *value;
        if ((int)(text.getText()[0]) == 0)
        {
            tempValue = 0;
        }
        else
        {
            try
            {
                tempValue = std::stoi(text.getText());
            }
            catch (std::invalid_argument)
            {
                std::cout << "ERROR: Int Value could not be assigned from \"" << text.getText() << "\"" << std::endl;
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
    renderer->renderText(label, (width * 0.42f), ypos - (width * 0.035f), textSize, glm::vec3(1.0f), RIGHT);

    // Color Modifier
    glm::vec3 colorMod(1);
    if (clicked || text.getTyping())
        colorMod = glm::vec3(0.75f);
    else if (highlighted)
        colorMod = glm::vec3(1.25f);

    // Drawing Base Box
    renderer->renderQuad(glm::vec3(width * 0.69f, ypos, 0.2f), glm::vec2(width * 0.5f, width * 0.1f), colors::darkerGrey.rgb() * colorMod);
    
    // Draw Slider Box
    float sliderWidth = width * 0.5f * getPercentage();
    float sliderPosX = (width * 0.69f) - (width * (1.0f - getPercentage()) * 0.25f);
    renderer->renderQuad(glm::vec3(sliderPosX, ypos, 0.225f), glm::vec2(sliderWidth, width * 0.1f), colors::blunderGreen.rgb() * colorMod);

    // Drawing Value
    text.renderText(renderer, (width * 0.69f), ypos - (width * 0.035f), textSize, glm::vec3(1.0f), CENTER);
}

void Toggle::RenderElement(UIRenderer* renderer, float ypos, float width, float textSize)
{
    // Drawing Label Text
    renderer->renderText(label, (width * 0.42f), ypos - (width * 0.035f), textSize, glm::vec3(1.0f), RIGHT);
    
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
    renderer->renderQuad(glm::vec3(width * 0.49f, ypos, 0.25f), glm::vec2(width * 0.1f, width * 0.1f), color * colorMod);

    // Draw Check (eventually)

}