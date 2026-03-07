#include "../UIWindows.h"
using namespace ui;

// Attribute Window Functions
void AttributeWindow::UpdateWindow()
{
    // Setting initial yPos to start position at
    float attributeTitleHeight = position.unitScale * 1.5f;
    float attributeYPos = position.getHeight() - (attributeTitleHeight * 0.5f + position.getBuffer());

    // Adding Each Attribute
    for (int i = 0; i < attributes.size(); i++)
    {
        // Adding Attribute Header
        float elementHeight = position.unitScale;
        float attributeHeaderWidth = position.getWidth() - 2.0f * position.getBuffer();
        ui::ElementPosition elementPos(glm::vec2(position.getWidth() / 2.0f, attributeYPos), glm::vec2(attributeHeaderWidth, attributeTitleHeight), 0.0f, &position);
        attributes[i]->getHeader()->UpdateElement(elementPos);

        // Checking if elements should be rendered
        if (attributes[i]->getCollapsed())
        {
            attributeYPos -= (attributeTitleHeight + elementHeight) * 0.5f + position.getBuffer();
        }
        else
        {
            attributeYPos -= attributeTitleHeight * 0.5f;

            // Setting up useful UI sizes
            attributeYPos -= (elementHeight * 0.5f + position.getBuffer());

            for (int j = 0; j < attributes[i]->getElementCount(); j++)
            {
                // Add Each Element
                ui::AttributeElement* element = attributes[i]->getElement(j);
                float attributeElementWidth = attributeHeaderWidth - 2.0f * position.getBuffer();
                ui::ElementPosition elementPos(glm::vec2(position.getWidth() / 2.0f, attributeYPos), glm::vec2(attributeElementWidth, elementHeight), position.getWidth() * 0.44f, &position);
                element->UpdateElement(elementPos);

                // Updating YPos
                attributeYPos -= (elementHeight + position.getBuffer());
            }

            // Adding space after containter
            attributeYPos -= position.getBuffer();
        }

        // Creating Space for Next Attribute
        attributeYPos -= position.getBuffer();
    }
}
void AttributeWindow::DrawWindow(ui::UIRenderer* renderer)
{
    // Adding Base Quad
    renderer->addQuad(position.getCorners(), 0.0f, colors::grey.rgb());

    // Setting initial yPos to start rendering at
    float attributeTitleHeight = position.unitScale * 1.5f;
    float attributeYPos = position.getHeight() - (attributeTitleHeight * 0.5f + position.getBuffer());

    // Adding Each Attribute
    for (int i = 0; i < attributes.size(); i++)
    {
        // Adding Label Box
        float elementHeight = position.unitScale;
        float attributeBoxWidth = position.getWidth() - 2.0f * position.getBuffer();
        renderer->addQuad(glm::vec3((position.getWidth() / 2), attributeYPos, 0.1f), glm::vec2(attributeBoxWidth, attributeTitleHeight), glm::vec3(0.51f), position.offset);

        // Adding Attribute Label
        renderer->addText(attributes[i]->getName(), glm::vec3((position.getWidth() / 2), attributeYPos, 0), largeText(), glm::vec3(1.0f), position.offset, CENTER);


        // Checking if elements should be rendered
        if (attributes[i]->getCollapsed())
        {
            attributeYPos -= (attributeTitleHeight + elementHeight) * 0.5f + position.getBuffer();
        }
        else
        {
            attributeYPos -= attributeTitleHeight * 0.5f;

            // Setting up useful UI sizes
            float containerStartHeight = attributeYPos;

            attributeYPos -= (elementHeight * 0.5f + position.getBuffer());

            for (int j = 0; j < attributes[i]->getElementCount(); j++)
            {
                // Add Each Element
                ui::AttributeElement* element = attributes[i]->getElement(j);
                element->RenderElement(renderer, mediumText());

                // Updating YPos
                attributeYPos -= (elementHeight + position.getBuffer());
            }

            // Rendering the element container
            glm::vec4 containerCorners = glm::vec4(
                (position.getWidth() - attributeBoxWidth) * 0.5f,
                attributeYPos + elementHeight * 0.5f,
                position.getWidth() - (position.getWidth() - attributeBoxWidth) * 0.5f,
                containerStartHeight
            );
            renderer->addQuad(containerCorners, 0.15f, glm::vec3(0.35f), position.offset);

            // Adding space after containter
            attributeYPos -= position.getBuffer();
        }

        // Creating Space for Next Attribute
        attributeYPos -= position.getBuffer();
    }
}
void AttributeWindow::ManageInteraction(GLFWwindow* window, StateMachine* state)
{
    // Getting the mouse Position (flipping the y because glfw is stupid)
    glm::vec2 mousePos = glm::vec2(
        state->getMouse()->mousePos.x,
        (float)(state->getWindowDimensions().y) - state->getMouse()->mousePos.y
    );

    // Checking if StateMachine selected object differs from attribute object, and if it does, changes it
    if (attributeObject != state->getSelectedObject())
    {
        CreateUIfromObject(state->getSelectedObject());
    }

    // Finding clicked element by looping through each element within each attribute
    bool elementInteracted = false;
    for (int i = 0; i < attributes.size(); i++)
    {
        // Checking attribute header
        if (checkElementInteraction(window, mousePos, attributes[i]->getHeader(), state))
            break;

        // Making sure attribute isn't collapsed before checking elements
        if (!attributes[i]->getCollapsed())
        {
            for (int j = 0; j < attributes[i]->getElementCount(); j++)
            {
                // Checking each element in the attribute
                if (checkElementInteraction(window, mousePos, attributes[i]->getElement(j), state))
                {
                    elementInteracted = true;
                    break;
                }
            }
        }

        // Breaking out of main loop
        if (elementInteracted)
            break;
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

            // Checking if Collapse Button was clicked
            if (clickedElement->getType() == ui::UI_ATTRIBUTE_HEADER)
            {
                UpdateWindow();
            }

            clickedElement = nullptr;
        }
    }
}
void AttributeWindow::ClearAttributes()
{
    // Clearing attribute array
    for (int i = 0; i < attributes.size(); i++)
    {
        delete attributes[i];
        attributes[i] = nullptr;
    }

    // Resetting Vectors
    attributes.clear();
}
void AttributeWindow::CreateUIfromObject(obj::Object* object)
{
    // Checking if object exists
    if (object == nullptr)
        return;

    // Updating Attribute Object
    attributeObject = object;

    // Clearing attributes
    ClearAttributes();

    // Adding standard transform attributes
    ui::Attribute* positionAttribute = new ui::Attribute("Position");
    positionAttribute->addElement(new FloatEntry("X", &(object->transform.position.x)));
    positionAttribute->addElement(new FloatEntry("Y", &(object->transform.position.y)));
    positionAttribute->addElement(new FloatEntry("Z", &(object->transform.position.z)));
    addAttribute(positionAttribute);

    ui::Attribute* rotationAtrribute = new ui::Attribute("Rotation");
    rotationAtrribute->addElement(new FloatEntry("X", &(object->transform.rotation.x)));
    rotationAtrribute->addElement(new FloatEntry("Y", &(object->transform.rotation.y)));
    rotationAtrribute->addElement(new FloatEntry("Z", &(object->transform.rotation.z)));
    addAttribute(rotationAtrribute);

    ui::Attribute* scaleAtrribute = new ui::Attribute("Scale");
    scaleAtrribute->addElement(new FloatEntry("X", &(object->transform.scale.x)));
    scaleAtrribute->addElement(new FloatEntry("Y", &(object->transform.scale.y)));
    scaleAtrribute->addElement(new FloatEntry("Z", &(object->transform.scale.z)));
    addAttribute(scaleAtrribute);



    // Adding Custom Mesh Attributes
    // Checking if the attributes have values before adding them
    if (object->getMesh()->hasAttributes())
    {
        // Creating the attribute
        ui::Attribute* meshAttribute = new ui::Attribute("Mesh");

        // Adding the float attributes
        for (int i = 0; i < object->getMesh()->getNumFloatAttributes(); i++)
        {
            FloatAttribute* attribute = object->getMesh()->getFloatAttribute(i);
            // Checking if the attribute should be a slider (has limits?)
            if (attribute->hasLimits)
                meshAttribute->addElement(new FloatSlider(attribute->attributeLabel, &(attribute->attributeValue), 1.0f, attribute->attributeLimits[0], attribute->attributeLimits[1]));
            else
                meshAttribute->addElement(new FloatEntry(attribute->attributeLabel, &(attribute->attributeValue)));
        }

        // Adding the int attributes
        for (int i = 0; i < object->getMesh()->getNumIntAttributes(); i++)
        {
            IntAttribute* attribute = object->getMesh()->getIntAttribute(i);
            // Checking if the attribute should be a slider (has limits?)
            if (attribute->hasLimits)
                meshAttribute->addElement(new IntSlider(attribute->attributeLabel, &(attribute->attributeValue), 1.0f, attribute->attributeLimits[0], attribute->attributeLimits[1]));
            else
                meshAttribute->addElement(new IntEntry(attribute->attributeLabel, &(attribute->attributeValue)));
        }

        // Adding the attributes
        addAttribute(meshAttribute);
    }

    // TODO: REMOVE THIS AT SOME POINT
    ui::Attribute* colorAttribute = new ui::Attribute("Color");
    colorAttribute->addElement(new ColorEntry("Test Color", &(object->testColor)));
    addAttribute(colorAttribute);
}
void AttributeWindow::UnselectWindow()
{
    // Unclicking element
    if (clickedElement != nullptr)
    {
        clickedElement->clicked = false;
        clickedElement->highlighted = false;
        clickedElement = nullptr;
    }

    // Unhighlighting an Element
    if (highlightedElement)
    {
        highlightedElement->highlighted = false;
        highlightedElement = nullptr;
    }
}
bool AttributeWindow::checkElementInteraction(GLFWwindow* window, glm::vec2 mousePos, ui::AttributeElement* element, StateMachine* state)
{
    bool elementCollided = false;

    // Collision detection
    if (element->checkCollision(mousePos) && !state->getTransforming())
    {
        elementCollided = true;

        // Clicking an Element
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) && !clickedElement)
        {
            clickedElement = element;
            clickedElement->clicked = true;
            clickedElement->OnClick(state);
        }

        // Highlighting an Element
        else
        {
            if (highlightedElement && highlightedElement != element)
            {
                highlightedElement->highlighted = false;
            }

            highlightedElement = element;
            highlightedElement->highlighted = true;

        }
    }

    // Unhighlighting an Element
    else if (highlightedElement && element == highlightedElement && highlightedElement->highlighted)
    {
        highlightedElement->highlighted = false;
        highlightedElement = nullptr;
    }

    return elementCollided;
}