#include "../UIWindows.h"
using namespace ui;

// Color Window Functions
void ColorWindow::CreateUIFromSelected()
{
    // Checking if the selected color exists
    if (!selectedColor)
        return;

    // Clearing attributes at the beginning
    ClearAttributes();

    // Creating the attribute
    colorAttribute = new ui::Attribute("Color");

    // Color Selector
    colorAttribute->addElement(new ColorSelector(selectedColor));

    // Adding Mode Dropdown
    colorAttribute->addElement(new Dropdown("Mode", &currentColorMode, colorMode));

    // Adding Color Values (R, G, B, A)
    colorAttribute->addElement(new FloatSlider("R", &(colorData.x), 1.0f, 0.0f, 1.0f));
    colorAttribute->addElement(new FloatSlider("G", &(colorData.y), 1.0f, 0.0f, 1.0f));
    colorAttribute->addElement(new FloatSlider("B", &(colorData.z), 1.0f, 0.0f, 1.0f));
    colorAttribute->addElement(new FloatSlider("A", &(colorData.w), 1.0f, 0.0f, 1.0f));

    // Adding Hex Entry
    colorAttribute->addElement(new TextEntry("Hex", &hexCode));
}
void ColorWindow::ClearAttributes()
{
    // Deleting colorattribute
    if (colorAttribute)
    {
        delete colorAttribute;
        colorAttribute = nullptr;
    }
}
void ColorWindow::UpdateWindow()
{
    // Setting initial yPos to Start rendering at
    float elementHeight = position.unitScale;
    float attributeYPos = position.getHeight() - elementHeight * 0.5f;

    // Setting up useful UI sizes
    attributeYPos -= elementHeight * 0.5f;

    // Adding each atttribute element
    for (int i = 0; i < colorAttribute->getElementCount(); i++)
    {
        // Add Each Element
        ui::AttributeElement* element = colorAttribute->getElement(i);
        float attributeElementWidth = position.getWidth() - 2.0f * position.getBuffer();

        // Doing special sizing for color selector UI
        if (element->getType() == ui::UI_COLOR_SELECTOR)
        {
            // Rendering color selector larger than normal elements
            float colorSelectorHeight = position.getWidth() * (2.0f / 3.0f);
            attributeYPos -= (colorSelectorHeight * 0.5f) + position.getBuffer();
            ui::ElementPosition elementPos(glm::vec2(position.getWidth() / 2.0f, attributeYPos), glm::vec2(attributeElementWidth, colorSelectorHeight), 0.0f, &position);
            element->UpdateElement(elementPos);

            // Updating YPos
            attributeYPos -= (colorSelectorHeight * 0.5f) + elementHeight * 0.5f + position.getBuffer();
        }
        else
        {
            // Rendering normal elements
            ui::ElementPosition elementPos;

            // Different spacing for dropdown and text entry
            if (element->getType() == ui::UI_DROPDOWN)
                elementPos = ui::ElementPosition(glm::vec2(position.getWidth() / 2.0f, attributeYPos), glm::vec2(attributeElementWidth, elementHeight), position.getWidth() * (21.0f / 58.0f), &position);
            else
                elementPos = ui::ElementPosition(glm::vec2(position.getWidth() / 2.0f, attributeYPos), glm::vec2(attributeElementWidth, elementHeight), position.getWidth() * (15.0f / 58.0f), &position);


            element->UpdateElement(elementPos);

            // Updating YPos
            attributeYPos -= (elementHeight + position.getBuffer());
        }
    }
}
void ColorWindow::DrawWindow(ui::UIRenderer* renderer)
{
    // Adding Base Quad
    renderer->addQuad(position.getCorners(), 0.0f, glm::vec3(0.35f));

    // Setting initial yPos to Start rendering at
    float elementHeight = position.unitScale;
    float attributeYPos = position.getHeight() - elementHeight * 0.5f;

    // Adding Label Box
    renderer->addQuad(glm::vec3((position.getWidth() / 2), attributeYPos, 0.1f), glm::vec2(position.getWidth(), elementHeight), glm::vec3(0.51f), position.offset);

    // Adding Attribute Label
    renderer->addText(colorAttribute->getName(), glm::vec3((position.getWidth() / 2), attributeYPos, 0), mediumText(), glm::vec3(1.0f), position.offset, CENTER);

    // Setting up useful UI sizes
    attributeYPos -= elementHeight * 0.5f;

    // Adding each atttribute element
    for (int i = 0; i < colorAttribute->getElementCount(); i++)
    {
        // Add Each Element
        ui::AttributeElement* element = colorAttribute->getElement(i);
        float attributeElementWidth = position.getWidth() - 2.0f * position.getBuffer();
        
        // Doing special sizing for color selector UI
        if (element->getType() == ui::UI_COLOR_SELECTOR)
        {
            // Rendering color selector larger than normal elements
            float colorSelectorHeight = position.getWidth() * (2.0f / 3.0f);
            attributeYPos -= (colorSelectorHeight * 0.5f) + position.getBuffer();
            element->RenderElement(renderer, 
                mediumText());

            // Updating YPos
            attributeYPos -= (colorSelectorHeight * 0.5f) + elementHeight * 0.5f + position.getBuffer();
        }
        else
        {
            // Rendering normal elements
            element->RenderElement(renderer, mediumText());

            // Updating YPos
            attributeYPos -= (elementHeight + position.getBuffer());
        }
    }
}
void ColorWindow::ManageInteraction(GLFWwindow* window, StateMachine* state)
{
    // Getting (and temporarily storing) the mouse Position (flipping the y because glfw is stupid)
    glm::vec2 mousePos = glm::vec2(
        state->getMouse()->mousePos.x,
        (float)(state->getWindowDimensions().y) - state->getMouse()->mousePos.y
    );

    // Finding clicked element by looping through each element within each attribute
    //std::cout << "Mouse Pos: " << smath::outputVec2(mousePos) << std::endl;
    for (int i = 0; i < colorAttribute->getElementCount(); i++)
    {
        ui::AttributeElement* currentElement = colorAttribute->getElement(i);

        if (currentElement->getInteractable())

            // Collision detection
            if (currentElement->checkCollision(mousePos) && !state->getTransforming())
            {
                // Clicking an Element
                if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) && clickedElement == nullptr)
                {
                    state->changeState(SM_UI_INTERACT);
                    clickedElement = currentElement;
                    clickedElement->clicked = true;
                    clickedElement->OnClick(state);
                }

                // Highilighting an Element
                else
                    currentElement->highlighted = true;
            }

        // Unhighlighting an Element
        else if (currentElement->highlighted)
            currentElement->highlighted = false;
    }

    // Managing Clicked Element
    if (clickedElement != nullptr)
    {
        clickedElement->OnHold(state);

        // Unclicking an Element
        if (clickedElement->clicked && !glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1))
        {
            clickedElement->OnRelease(state);
            clickedElement->clicked = false;
            clickedElement = nullptr;
        }
    }
}
void ColorWindow::UnselectWindow()
{
    // Unclicking element
    if (clickedElement != nullptr)
    {
        clickedElement->clicked = false;
        clickedElement->highlighted = false;
        clickedElement = nullptr;
    }
}