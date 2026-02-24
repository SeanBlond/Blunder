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

    // Resetting Interactables
    interactables.clear();
}
void ColorWindow::GenerateInteractables()
{
    // Clearing interactables
    interactables.clear();
}
void ColorWindow::DrawWindow(ui::UIRenderer* renderer)
{
    // Adding Base Quad
    renderer->addQuad(position.getCorners(), 0.0f, colors::grey.rgb());

    // Setting initial yPos to Start rendering at
    float attributeTitleHeight = position.getWidth() * 0.12f;
    float attributeYPos = position.getHeight() - (attributeTitleHeight * 0.5f + position.getBuffer());

    // Adding Label Box
    float attributeBoxWidth = position.getWidth() - 2.0f * position.getBuffer();
    renderer->addQuad(glm::vec3((position.getWidth() / 2), attributeYPos, 0.1f), glm::vec2(attributeBoxWidth, attributeTitleHeight), glm::vec3(0.51f), position.offset);

    // Adding Attribute Label
    renderer->addText(colorAttribute->getName(), glm::vec3((position.getWidth() / 2), attributeYPos, 0), largeText(), glm::vec3(1.0f), position.offset, CENTER);
    attributeYPos -= attributeTitleHeight * 0.5f;

    // Setting up useful UI sizes
    float elementHeight = position.getWidth() * 0.08f;
    float containerStartHeight = attributeYPos;

    attributeYPos -= (elementHeight * 0.5f + position.getBuffer());

    // Adding each atttribute element
    for (int i = 0; i < colorAttribute->getElementCount(); i++)
    {
        // Add Each Element
        ui::AttributeElement* element = colorAttribute->getElement(i);
        
        // Doing special sizing for color selector UI
        if (element->getType() == ui::UI_COLOR_SELECTOR)
        {
            // Rendering color selector larger than normal elements
            float attributeElementWidth = attributeBoxWidth - 2.0f * position.getBuffer();
            float colorSelectorHeight = position.getWidth() * 0.75f;
            ui::ElementPosition elementPos(glm::vec2(position.getWidth() / 2.0f, attributeYPos - (colorSelectorHeight - elementHeight) * 0.5f), glm::vec2(attributeElementWidth, colorSelectorHeight), colorSelectorHeight, &position);
            element->RenderElement(renderer, elementPos, mediumText());

            // Updating YPos
            attributeYPos -= (colorSelectorHeight + position.getBuffer());
        }
        else
        {
            // Rendering normal elements
            float attributeElementWidth = attributeBoxWidth - 2.0f * position.getBuffer();
            ui::ElementPosition elementPos(glm::vec2(position.getWidth() / 2.0f, attributeYPos), glm::vec2(attributeElementWidth, elementHeight), position.getWidth() * 0.3f, &position);
            element->RenderElement(renderer, elementPos, mediumText());

            // Updating YPos
            attributeYPos -= (elementHeight + position.getBuffer());
        }
    }

    // Rendering the element container
    glm::vec4 containerCorners = glm::vec4(
        (position.getWidth() - attributeBoxWidth) * 0.5f,
        attributeYPos + elementHeight * 0.5f,
        position.getWidth() - (position.getWidth() - attributeBoxWidth) * 0.5f,
        containerStartHeight
    );
    renderer->addQuad(containerCorners, 0.15f, glm::vec3(0.35f), position.offset);
}
void ColorWindow::ManageInteraction(GLFWwindow* window, StateMachine* state)
{

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