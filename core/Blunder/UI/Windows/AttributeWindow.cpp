#include "../UIWindows.h"
using namespace ui;

// Attribute Window Functions
void AttributeWindow::GenerateInteractables()
{
    // Clearing previously generated interactables
    interactables.clear();

    // Setting up initial positioning values
    float attributeTitleHeight = 0.12f;
    float attributeYPos = attributeTitleHeight * 0.5f + position.bufferSize;

    // Going through UI Positions and creating interactables out of the elements
    for (int i = 0; i < attributes.size(); i++)
    {
        // Attribute Dropdown Button
        glm::vec4 dropdownCorners = glm::vec4(
            position.bufferSize,
            attributeYPos - attributeTitleHeight * 0.5f,
            1.0f - position.bufferSize,
            attributeYPos + attributeTitleHeight * 0.5f
        );
        ui::AttributeInteractable dropDownInteractable(dropdownCorners, attributes[i]->getDropDownButton());
        interactables.push_back(dropDownInteractable);

        // Checking if elements should be added
        if (attributes[i]->getCollapsed())
        {
            attributeYPos += 0.1f + position.bufferSize;
        }
        else
        {
            attributeYPos += attributeTitleHeight * 0.5f;

            // Setting up useful UI sizes
            float elementHeight = 0.08f;

            attributeYPos += elementHeight * 0.5f + position.bufferSize;

            // Adding Each Element to interactables
            for (int j = 0; j < attributes[i]->getElementCount(); j++)
            {
                // Positioning for the element
                float attributeElementWidth = position.getWidth() - 4.0f * position.getBuffer();
                float attributeElementYPos = attributeYPos * position.getWidth();
                ui::ElementPosition elementPos(glm::vec2(position.getWidth() / 2.0f, attributeElementYPos), glm::vec2(attributeElementWidth, position.getWidth() * elementHeight), position.getWidth() * 0.44f, &position);

                // Corners for input
                glm::vec4 corners = glm::vec4(0);
                if (attributes[i]->getElement(j)->getType() == ui::UI_TOGGLE) // Toggle has smaller corners
                {
                    corners = elementPos.getRightCorners(elementHeight * position.getWidth()) / position.getWidth();
                }
                else // Everything else is the same
                {
                    corners = elementPos.getRightCorners() / position.getWidth();
                }

                // Adding the interactable
                ui::AttributeInteractable tempInteractable(corners, attributes[i]->getElement(j));
                interactables.push_back(tempInteractable);

                //std::cout << "Interactable Generated at (" << corners.x << ", " << corners.y << ", " << corners.z << ", " << corners.w << ")\n";

                // Updating YPos
                attributeYPos += elementHeight + position.bufferSize;
            }

            // Adding space after containter
            attributeYPos += position.bufferSize;
        }

        // Creating Space for Next Attribute
        attributeYPos += position.bufferSize;
    }
}
void AttributeWindow::DrawWindow(ui::UIRenderer* renderer)
{
    // Adding Base Quad
    renderer->addQuad(position.getCorners(), 0.0f, colors::grey.rgb());

    // Setting initial yPos to Start rendering at
    float attributeTitleHeight = position.unitScale * 1.5f;
    float attributeYPos = position.getHeight() - (attributeTitleHeight * 0.5f + position.getBuffer());

    // Adding Each Attribute
    for (int i = 0; i < attributes.size(); i++)
    {
        // Adding Label Box
        float attributeBoxWidth = position.getWidth() - 2.0f * position.getBuffer();
        renderer->addQuad(glm::vec3((position.getWidth() / 2), attributeYPos, 0.1f), glm::vec2(attributeBoxWidth, attributeTitleHeight), glm::vec3(0.51f), position.offset);

        // Adding Attribute Label
        renderer->addText(attributes[i]->getName(), glm::vec3((position.getWidth() / 2), attributeYPos, 0), largeText(), glm::vec3(1.0f), position.offset, CENTER);


        // Checking if elements should be rendered
        if (attributes[i]->getCollapsed())
        {
            attributeYPos -= (position.getWidth() * 0.1f + position.getBuffer());
        }
        else
        {
            attributeYPos -= attributeTitleHeight * 0.5f;

            // Setting up useful UI sizes
            float elementHeight = position.unitScale;
            float containerStartHeight = attributeYPos;

            attributeYPos -= (elementHeight * 0.5f + position.getBuffer());

            for (int j = 0; j < attributes[i]->getElementCount(); j++)
            {
                // Add Each Element
                ui::AttributeElement* element = attributes[i]->getElement(j);
                float attributeElementWidth = attributeBoxWidth - 2.0f * position.getBuffer();
                ui::ElementPosition elementPos(glm::vec2(position.getWidth() / 2.0f, attributeYPos), glm::vec2(attributeElementWidth, elementHeight), position.getWidth() * 0.44f, &position);
                element->RenderElement(renderer, elementPos, mediumText());

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
    // Checking if StateMachine selected object differs from attribute object, and if it does, changes it
    if (attributeObject != state->getSelectedObject())
    {
        CreateUIfromObject(state->getSelectedObject());
    }

    // Converting Mouse Pos to Local Coordinates
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    xpos = (xpos - position.getXOffset()) / position.getWidth();
    ypos = (ypos - position.getYOffset()) / position.getWidth();

    // Finding CLicked Element
    //std::cout << "Mouse Pos: (" << xpos << ", " << ypos << ")" << std::endl;
    for (int i = 0; i < interactables.size(); i++)
    {
        // Highlighting an Element
        if (smath::checkUICollision(glm::vec2(xpos, ypos), interactables[i].corners) && !state->getTransforming())
        {
            // Clicking an Element
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) && clickedElement == nullptr)
            {
                state->changeState(SM_UI_INTERACT);
                interactables[i].element->clicked = true;
                clickedElement = (interactables[i].element);
                clickedElement->OnClick(state);
            }

            // Highilighting an Element
            else
                interactables[i].element->highlighted = true;
        }

        // Unhighlighting an Element
        else if (interactables[i].element->highlighted)
            interactables[i].element->highlighted = false;


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

            // Checking if Dropdown Button was clicked
            if (clickedElement->getType() == ui::UI_ATTRIBUTE_COLLAPSE)
            {
                GenerateInteractables();
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
    interactables.clear();
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
    // Adding Test Dropdown REMOVE AT SOME POINT
    std::vector<std::string> options = { "Option 1", "Option 2", "Option 3", "Option 4" };
    testDropdownValue = 0;
    scaleAtrribute->addElement(new Dropdown("Test", &testDropdownValue, options));
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

    // Generating the Interactables
    GenerateInteractables();
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
}