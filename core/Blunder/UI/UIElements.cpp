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
void Attribute::addTextEntry(std::string label, std::string* value)
{
    elements.push_back(new TextEntry(label, value));
}
void Attribute::addDropdown(std::string label, int* value, std::vector<std::string> options)
{
    elements.push_back(new Dropdown(label, value, options));
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
void Toggle::OnClick(StateMachine* state) {}
void Toggle::OnHold(StateMachine* state) {}
void Toggle::OnRelease(StateMachine* state)
{
    // Toggling the value
    state->exitState();
    *value = !(*value);
}

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

// Dropdown Mouse Functions
void Dropdown::OnClick(StateMachine* state)
{
    // Activating dropdown
    droppedDown = true;

    // Clamping value to bhe within the options range
    *value = smath::clamp(*value, 0, (int)options.size());
}
void Dropdown::OnHold(StateMachine* state)
{
    // Checking if option ui values have been set before checking mouse 
    if (optionSize.x != 0)
    {
        // Checking each options for a mouse y-collision
        for (int i = 0; i < options.size(); i++)
        {
            // Setting corners
            glm::vec2 optionCorners = glm::vec2(
                firstOptionYPos - (optionSize.y / 2) + (optionSize.y * (i + 1)),
                firstOptionYPos + (optionSize.y / 2) + (optionSize.y * (i + 1))
            );

            // Checking collision
            if (smath::checkUICollision_Y(state->getMouse()->mousePos, optionCorners))
            {
                *value = i;
            }
        }
    }
}
void Dropdown::OnRelease(StateMachine* state)
{
    // Resetting values
    droppedDown = false;
    optionSize = glm::vec2(0);
    firstOptionYPos = 0;
}

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


// Element Render Functions
void FloatEntry::RenderElement(UIRenderer* renderer, const ElementPosition& position, float textSize)
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
    renderer->addText(label, glm::vec3(position.split - position.getBuffer(), yPos, 0), textSize, glm::vec3(1.0f), RIGHT);

    // Color Modifier
    glm::vec3 colorMod(1);
    if (clicked || text.getTyping())
        colorMod = glm::vec3(0.75f);
    else if (highlighted)
        colorMod = glm::vec3(1.25f);

    // Drawing Text Box
    renderer->addQuad(position.getRightCorners(), 0.2f, colors::darkerGrey.rgb() * colorMod);

    // Rounding value to 3 decimals places and converting to string
    //std::stringstream stream;
    //stream << std::fixed << std::setprecision(3) << (*value);

    // Drawing Value
    text.addText(renderer, glm::vec3(position.getMiddleAfterSplit(), yPos, 0), textSize, glm::vec3(1.0f), CENTER);
}

void FloatSlider::RenderElement(UIRenderer* renderer, const ElementPosition& position, float textSize)
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

        *value = tempValue;
        text.setStored(false);
    }
    if (!text.getTyping())
    {
        text.setText(std::to_string(*value));
    }

    // Drawing Label Text
    renderer->addText(label, glm::vec3(position.split - position.getBuffer(), yPos, 0), textSize, glm::vec3(1.0f), RIGHT);

    // Color Modifier
    glm::vec3 colorMod(1);
    if (clicked || text.getTyping())
        colorMod = glm::vec3(0.75f);
    else if (highlighted)
        colorMod = glm::vec3(1.25f);

    // Drawing Base Box
    renderer->addQuad(position.getRightCorners(), 0.2f, colors::darkerGrey.rgb() * colorMod);

    // Draw Slider Box
    float sliderWidth = position.getWidthAfterSplit() * getPercentage();
    renderer->addQuad(position.getRightCorners(sliderWidth), 0.225f, colors::blunderGreen.rgb() * colorMod);

    // Drawing Value
    text.addText(renderer, glm::vec3(position.getMiddleAfterSplit(), yPos, 0), textSize, glm::vec3(1.0f), CENTER);
}

void IntEntry::RenderElement(UIRenderer* renderer, const ElementPosition& position, float textSize)
{
    float width = (position.right_x - position.left_x);
    float ySize = (position.top_y - position.bottom_y);
    float yPos = position.top_y - (ySize / 2);

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
    renderer->addText(label, glm::vec3(position.split - position.getBuffer(), yPos, 0), textSize, glm::vec3(1.0f), RIGHT);

    // Color Modifier
    glm::vec3 colorMod(1);
    if (clicked || text.getTyping())
        colorMod = glm::vec3(0.75f);
    else if (highlighted)
        colorMod = glm::vec3(1.25f);

    // Drawing Text Box
    renderer->addQuad(position.getRightCorners(), 0.2f, colors::darkerGrey.rgb() * colorMod);

    // Drawing Value
    text.addText(renderer, glm::vec3(position.getMiddleAfterSplit(), yPos, 0), textSize, glm::vec3(1.0f), CENTER);
}

void IntSlider::RenderElement(UIRenderer* renderer, const ElementPosition& position, float textSize)
{
    float width = (position.right_x - position.left_x);
    float ySize = (position.top_y - position.bottom_y);
    float yPos = position.top_y - (ySize / 2);

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
    renderer->addText(label, glm::vec3(position.split - position.getBuffer(), yPos, 0), textSize, glm::vec3(1.0f), RIGHT);

    // Color Modifier
    glm::vec3 colorMod(1);
    if (clicked || text.getTyping())
        colorMod = glm::vec3(0.75f);
    else if (highlighted)
        colorMod = glm::vec3(1.25f);

    // Drawing Base Box
    renderer->addQuad(position.getRightCorners(), 0.2f, colors::darkerGrey.rgb() * colorMod);

    // Draw Slider Box
    float sliderWidth = position.getWidthAfterSplit() * getPercentage();
    renderer->addQuad(position.getRightCorners(sliderWidth), 0.225f, colors::blunderGreen.rgb() * colorMod);

    // Drawing Value
    text.addText(renderer, glm::vec3(position.getMiddleAfterSplit(), yPos, 0), textSize, glm::vec3(1.0f), CENTER);
}

void Toggle::RenderElement(UIRenderer* renderer, const ElementPosition& position, float textSize)
{
    float width = (position.right_x - position.left_x);
    float ySize = (position.top_y - position.bottom_y);
    float yPos = position.top_y - (ySize / 2);

    // Drawing Label Text
    renderer->addText(label, glm::vec3(position.split - position.getBuffer(), yPos, 0), textSize, glm::vec3(1.0f), RIGHT);

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
    renderer->addQuad(position.getRightCorners(ySize), 0.2f, color * colorMod);

    // Draw Check (eventually)

}

void TextEntry::RenderElement(UIRenderer* renderer, const ElementPosition& position, float textSize)
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
    renderer->addText(label, glm::vec3(position.split - position.getBuffer(), yPos, 0), textSize, glm::vec3(1.0f), RIGHT);

    // Color Modifier
    glm::vec3 colorMod(1);
    if (clicked || text.getTyping())
        colorMod = glm::vec3(0.75f);
    else if (highlighted)
        colorMod = glm::vec3(1.25f);

    // Drawing Text Box
    renderer->addQuad(glm::vec3(width * 0.69f, yPos, 0.2f), glm::vec2(width * 0.5f, ySize), colors::darkerGrey.rgb() * colorMod);

    // Drawing Value
    text.addText(renderer, glm::vec3(position.getMiddleAfterSplit(), yPos, 0), textSize, glm::vec3(1.0f), CENTER);
}

void Dropdown::RenderElement(UIRenderer* renderer, const ElementPosition& position, float textSize)
{
    float width = (position.right_x - position.left_x);
    float ySize = (position.top_y - position.bottom_y);
    float yPos = position.top_y - (ySize / 2);

    // Drawing Label Text
    renderer->addText(label, glm::vec3(position.split - position.getBuffer(), yPos, 0), textSize, glm::vec3(1.0f), RIGHT);

    // Color Modifier
    glm::vec3 colorMod(1);
    if (clicked)
        colorMod = glm::vec3(0.75f);
    else if (highlighted)
        colorMod = glm::vec3(1.25f);

    // Drawing Dropdown Box
    renderer->addQuad(position.getRightCorners(), 0.2f, colors::darkerGrey.rgb() * colorMod);

    // Drawing Dropdown Icon
    ui::UITexture dropdownIcon = (droppedDown ? UI_DROPDOWN_T : UI_DROPDOWN_F);
    renderer->addQuad(position.getRightCorners(ySize), 0.21f, colors::lightestgrey.rgb(), dropdownIcon);

    // Drawing each option if dropped down
    if (droppedDown)
    {

        // Setting optionYSize for UI interaction
        float optionWidth = position.getRightCorners().z - position.getRightCorners().x;
        optionSize = glm::vec2(optionWidth, ySize);

        // Checking if there is enough space for the dropdown to drop downwards
        bool directionDown = true;
        float lowestPoint = position.bottom_y - ySize * options.size();
        if (lowestPoint < ySize)
        {
            // Dropdown needs to drop upwards
            directionDown = false;
            optionSize *= glm::vec2(1, -1);
        }

        firstOptionYPos = position.parentWindow->getHeight() - yPos - (ySize / 2);

        // Drawing a quad that outlines the options
        glm::vec4 outlineCorner = glm::vec4(
            position.split - position.getBuffer(),
            lowestPoint - position.getBuffer(),
            position.right_x + position.getBuffer(),
            position.bottom_y
        );
        renderer->addQuad(outlineCorner, 0.89f, glm::vec3(0.35f));

        // Drawing each option
        for (int i = 0; i < options.size(); i++)
        {
            float optionYOffset = -(ySize * (i + 1));

            // Highlighting the option if it is currently selected
            glm::vec3 optionColor = (i == *value ? colors::blunderGreen.rgb() : colors::darkerGrey.rgb());

            // Drawing Dropdown Box
            renderer->addQuad(position.getRightCorners(glm::vec2(0, optionYOffset)), 0.9f, optionColor);

            // Drawing Option Circle
            renderer->addQuad(glm::vec3(position.split + ySize * 0.5f, yPos + optionYOffset, 0.91f), glm::vec2(width * 0.02f), colors::lightestgrey.rgb(), UI_NO_TEXTURE, QUAD_CIRCLE);

            // Drawing Value
            renderer->addText(options[i], glm::vec3(position.split + ySize, yPos + optionYOffset, 1), textSize, glm::vec3(1.0f), LEFT);
        }
    }

    // Drawing Value
    renderer->addText(options[*value], glm::vec3(position.split + ySize, yPos, 0), textSize, glm::vec3(1.0f), LEFT);
}

void HierarchyTextEntry::RenderElement(UIRenderer* renderer, const ElementPosition& position, float textSize)
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
        renderer->addQuad(glm::vec3(width * 0.5f + position.right_x, yPos, 0.2f), glm::vec2(width, ySize), colors::darkerGrey.rgb() * colorMod);

    // Drawing Value
    text.addText(renderer, glm::vec3(position.right_x, yPos, 0), textSize, glm::vec3(1.0f));
}

// ViewNav Element
void ViewNav::CreateMesh()
{
    // Create Mesh
    // Mesh vertices & indices
    std::vector<Vertex> vertices =
    {
        // Position                     Color                     TexCoord (not used)
        Vertex({glm::vec3(0, 0, 0),    colors::red.rgb(),       glm::vec2(0) }),
        Vertex({glm::vec3(1, 0, 0),    colors::red.rgb(),       glm::vec2(0) }),  //  X : 1
        Vertex({glm::vec3(0, 0, 0),    colors::darkRed.rgb(),   glm::vec2(0) }),
        Vertex({glm::vec3(-1, 0, 0),   colors::darkRed.rgb(),   glm::vec2(0) }),  // -X : 3
        Vertex({glm::vec3(0, 0, 0),    colors::green.rgb(),     glm::vec2(0) }),
        Vertex({glm::vec3(0, 1, 0),    colors::green.rgb(),     glm::vec2(0) }),  //  Y : 5
        Vertex({glm::vec3(0, 0, 0),    colors::darkGreen.rgb(), glm::vec2(0) }),
        Vertex({glm::vec3(0, -1, 0),   colors::darkGreen.rgb(), glm::vec2(0) }),  // -Y : 7
        Vertex({glm::vec3(0, 0, 0),    colors::blue.rgb(),      glm::vec2(0) }),
        Vertex({glm::vec3(0, 0, 1),    colors::blue.rgb(),      glm::vec2(0) }),  //  Z : 9
        Vertex({glm::vec3(0, 0, 0),    colors::darkBlue.rgb(),  glm::vec2(0) }),
        Vertex({glm::vec3(0, 0, -1),   colors::darkBlue.rgb(),  glm::vec2(0) }),  // -Z : 11
    };
    std::vector<unsigned int> indices =
    {
        0, 1, 0,
        2, 3, 2,
        4, 5, 4,
        6, 7, 6,
        8, 9, 8,
        10, 11, 10,
    };

    // Creating mesh object
    navMesh = new Mesh(vertices, indices);

    // Creating Shaders
    const char* navVertexShader = R"(
        #version 330 core
        layout (location = 0) in vec3 vertex;
        layout (location = 1) in vec3 LineColor;
        layout (location = 2) in vec2 TexCoord;
        out vec2 TexCoords;
        out vec3 Color;
        
        uniform mat4 transform;

        void main()
        {
            gl_Position = transform * vec4(vertex, 1.0);
            TexCoords = TexCoord;
            Color = LineColor;
        }  
        )";

    const char* navFragmentShader = R"(
        #version 330 core
        in vec2 TexCoords;
        in vec3 Color;
        out vec4 fragColor;

        void main()
        {    
            fragColor = vec4(Color, 1.0);
        }  
        )";

    // Creating shader object
    navShader = new shdr::Shader(navVertexShader, navFragmentShader, 1);

}
ViewNav::~ViewNav()
{
    delete navMesh;
    delete navShader;
    navMesh = nullptr;
    navShader = nullptr;
}

// Functions
void ViewNav::RenderElement(UIRenderer* renderer, const ElementPosition& position, float textSize)
{
    float width = (position.right_x - position.left_x);
    float ySize = (position.top_y - position.bottom_y);
    float yPos = position.top_y - (ySize / 2);

    // Updating screen position
    screenPos = glm::vec3(position.left_x - ySize * 0.5f, ySize * 0.5f, ySize);

    // Adding background when highlighted
    if (highlighted || clicked)
        renderer->addQuad(position.getCorners(), -0.99999f, colors::lightgrey.rgb(), UI_NO_TEXTURE, QUAD_CIRCLE);

    // Unique data for each axis
    glm::vec3 axisColors[] = { colors::white.rgb(), colors::red.rgb(), colors::darkRed.rgb(), colors::green.rgb(), colors::darkGreen.rgb(), colors::blue.rgb(), colors::darkBlue.rgb() };
    std::string axisTitles[] = { "", "+X", "-X", "+Y", "-Y", "+Z", "-Z" };

    // Adding quads for each point
    glm::vec4 axises[7] =
    {
        glm::vec4(0, 0, 0, 1),   // POSITIVE_X
        glm::vec4(1, 0, 0, 1),   // POSITIVE_X
        glm::vec4(-1, 0, 0, 1),  // NEGATIVE_X
        glm::vec4(0, 1, 0, 1),   // POSITIVE_Y
        glm::vec4(0, -1, 0, 1),  // NEGATIVE_Y
        glm::vec4(0, 0, 1, 1),   // POSITIVE_Z
        glm::vec4(0, 0, -1, 1),  // NEGATIVE_Z
    };
    for (int i = 0; i < 7; i++)
    {
        // Getting the coordinate of the axis point
        glm::vec3 axisPos = glm::vec3(position.right_x - (0.5f * ySize), yPos - (0.5f * ySize), 0) + glm::vec3(ySize * 0.5f, ySize * 0.5f, -1) * glm::vec3(transform * axises[i]);

        // Adding the quad for each axis
        renderer->addQuad(axisPos, glm::vec2(ySize * 0.13f), axisColors[i], UI_NO_TEXTURE, QUAD_CIRCLE);

        // (Conditionally) Adding the text 
        if (highlighted || clicked)
        {
            glm::vec3 textColor = (i % 2 == 0 ? glm::vec3(1) : glm::vec3(0));
            renderer->addText(axisTitles[i], axisPos + glm::vec3(0, 0, 0.01f), ySize * 0.00125f, textColor, CENTER);
        }
    }

    // Setting up viewport to be drawn to
    glViewport(position.left_x - ySize, yPos - ySize, ySize, ySize);

    // Shader settings
    navShader->useShader();
    navShader->setMat4("transform", transform);

    // Drawing lines
    navMesh->DrawMesh(true, false, false);
}
void ViewNav::OnClick(StateMachine* state)
{
    // Storing Initial Mouse Pos and camera settings
    slideStarted = false;
    initialMousePos = state->getMouse()->mousePos;
    storedCameraOrbit = state->getCamera()->getAngles();
}
void ViewNav::OnHold(StateMachine* state)
{
    // Checking if value should slide with mouse
    if (glm::length(state->getMouse()->mousePos - initialMousePos) > 1.0f || slideStarted)
    {
        slideStarted = true;
        glm::vec2 mouseDelta = (state->getMouse()->mousePos - initialMousePos) * glm::vec2(1, -1);
        glm::vec2 calcAngle = storedCameraOrbit + mouseDelta * speed;
        state->getCamera()->setAngles(calcAngle);
    }
}
void ViewNav::OnRelease(StateMachine* state)
{
    if (slideStarted)
    {
        state->exitState();
        slideStarted = false;
    }
    else
    {
        // Getting the mouse position relative to the center of the ViewNav
        glm::vec2 relativeMousePos = (state->getMouse()->mousePos - glm::vec2(screenPos)) / (0.5f * screenPos.z) * glm::vec2(1, -1);

        // Checking which point the mouse is closest to (if at all)
        ViewAxis closestAxis = getClosestAxis(relativeMousePos, 0.1f);
        
        // Setting the View Axis (if a axis button was clicked)
        if (closestAxis != VIEW_NONE)
        {
            if (closestAxis == VIEW_POSITIVE_X)
            {
                state->getCamera()->setAngles(0, smath::PI / 2);
            }
            else if (closestAxis == VIEW_NEGATIVE_X)
            {
                state->getCamera()->setAngles(smath::PI, smath::PI / 2);
            }
            else if (closestAxis == VIEW_POSITIVE_Y)
            {
                state->getCamera()->setAngles(smath::PI / 2, 0);
            }
            else if (closestAxis == VIEW_NEGATIVE_Y)
            {
                state->getCamera()->setAngles(smath::PI / 2, smath::PI);
            }
            else if (closestAxis == VIEW_POSITIVE_Z)
            {
                state->getCamera()->setAngles(smath::PI / 2, smath::PI / 2);
            }
            else if (closestAxis == VIEW_NEGATIVE_Z)
            {
                state->getCamera()->setAngles(-smath::PI / 2, smath::PI / 2);
            }
        }
    }
}
ViewNav::ViewAxis ViewNav::getClosestAxis(glm::vec2 position, float distanceClamp) const
{
    // Checking which point the position is closest to (if it's close at all)
    ViewAxis closestAxis = VIEW_NONE;
    float depth = 0; // For checking if an axis is "above" another

    // Temporary array of each possible axis
    glm::vec4 axises[6] =
    {
        glm::vec4(1, 0, 0, 1),   // POSITIVE_X
        glm::vec4(-1, 0, 0, 1),  // NEGATIVE_X
        glm::vec4(0, 1, 0, 1),   // POSITIVE_Y
        glm::vec4(0, -1, 0, 1),  // NEGATIVE_Y
        glm::vec4(0, 0, 1, 1),   // POSITIVE_Z
        glm::vec4(0, 0, -1, 1),  // NEGATIVE_Z
    };

    // Checking each axis
    for (int i = 0; i < 6; i++)
    {
        glm::vec3 testPos = glm::vec3(transform * axises[i]);
        if (glm::distance(position, glm::vec2(testPos)) <= distanceClamp && testPos.z < depth)
        {
            closestAxis = (ViewAxis)i;
            depth - testPos.z;
        }
    }

    // Returning closest axis
    return closestAxis;

}
