#include "UIWindows.h"

// Extra Functions
bool checkUICollision(float xpos, float ypos, ui::AttributeInteractable interactable)
{
    bool xCollision = (xpos > interactable.corners.x && xpos < interactable.corners.z);
    bool yCollision = (ypos > interactable.corners.y && ypos < interactable.corners.w);
    return (xCollision && yCollision);
}


// Attribute Window Functions
void AttributeWindow::GenerateInteractables()
{
    interactables.clear();

    // Going through UI Positions and creating interactables out of the elements
    float attributeYPos = 0.1f;

    for (int i = 0; i < attributes.size(); i++)
    {
        // Attribute Dropdown Button
        glm::vec4 dropdownCorners = glm::vec4(
            0.04f,
            attributeYPos - 0.06f,
            0.96f,
            attributeYPos + 0.06f
        );
        ui::AttributeInteractable dropDownInteractable(dropdownCorners, attributes[i]->getDropDownButton());
        interactables.push_back(dropDownInteractable);
        std::cout << "Interactable Generated at (" << dropdownCorners.x << ", " << dropdownCorners.y << ", " << dropdownCorners.z << ", " << dropdownCorners.w << ")\n";

        // Other Elements
        if (attributes[i]->getCollapsed())
        {
            attributeYPos += 0.1f;
        }
        else
        {
            attributeYPos += 0.13f;

            for (int j = 0; j < attributes[i]->getElementCount(); j++)
            {
                // Adding Each Element to interactables

                // Current Corners only works for Float Entries & Sliders
                glm::vec4 corners = glm::vec4(
                    0.69f - 0.25,
                    attributeYPos - 0.05f,
                    0.69f + 0.25,
                    attributeYPos + 0.05f
                );

                ui::AttributeInteractable tempInteractable(corners, attributes[i]->getElement(j));
                interactables.push_back(tempInteractable);

                //std::cout << "Interactable Generated at (" << corners.x << ", " << corners.y << ", " << corners.z << ", " << corners.w << ")\n";

                // Updating YPos
                attributeYPos += 0.12f;
            }
        }

        // Making space for other attributes
        attributeYPos += 0.05f;
    }
}
void AttributeWindow::DrawAttributeWindow()
{
    // Rendering Base Quad
    renderer.renderQuad(glm::vec3(width / 2.0f, height / 2.0f, 0.0f), glm::vec2(width, height), colors::grey.rgb());

    // Drawing Each Attribute
    float attributeYPos = height - (width * 0.1f);
    for (int i = 0; i < attributes.size(); i++)
    {
        // Rendering Label Box
        renderer.renderQuad(glm::vec3((width / 2), attributeYPos, 0.1f), glm::vec2(0.92f * width, 0.12f * width), glm::vec3(0.51f));

        // Rendering Attribute Label
        renderer.renderText(attributes[i]->getName(), (width / 2), attributeYPos - (width * 0.035f), largText(), glm::vec3(1.0f), CENTER);

        // Render Dropdown Button

        // Checking if elements should be rendered
        if (attributes[i]->getCollapsed())
        {
            attributeYPos -= (width * 0.1f);
        }
        else
        {
            attributeYPos -= (width * 0.06f);

            // Render Elements Container Quad
            float containerHeight = width * ((attributes[i]->getElementCount() * 0.1f) + ((attributes[i]->getElementCount() + 1) * 0.02f));
            float containerPosY = attributeYPos - (containerHeight * 0.5f);
            renderer.renderQuad(glm::vec3((width / 2), containerPosY, 0.15f), glm::vec2(0.92f * width, containerHeight), glm::vec3(0.35f));

            attributeYPos -= (width * 0.07f);

            for (int j = 0; j < attributes[i]->getElementCount(); j++)
            {
                // Render Each Element
                ui::AttributeElement* element = attributes[i]->getElement(j);
                element->RenderElement(&renderer, attributeYPos, width, mediumText());

                // Updating YPos
                attributeYPos -= (width * 0.12f);
            }
        }

        // Creating Space for Next Attribute
        attributeYPos -= (width * 0.05f);
    }
}
void AttributeWindow::ManageUIInteraction(GLFWwindow* window, StateMachine* state)
{
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    xpos /= width;
    ypos /= width;
    
    // Checking if Mouse Position is within the UI bounds
    if (xpos > 0 && xpos < 1 && ypos > 0 && ypos < (height / width))
    {
        //std::cout << "Mouse Pos: (" << xpos << ", " << ypos << ")" << std::endl;
        for (int i = 0; i < interactables.size(); i++)
        {
            // Highlighting an Element
            if (checkUICollision(xpos, ypos, interactables[i]) && !state->getTransforming())
            {
                // Clicking an Element
                if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) && clickedElement == nullptr)
                {
                    state->changeState(SM_UI_INTERACT);
                    interactables[i].element->clicked = true;
                    clickedElement = (interactables[i].element);
                    clickedElement->OnClick(glm::vec2(xpos, ypos) * width);
                }

                // Highilighting an Element
                else
                    interactables[i].element->highlighted = true;
            }

            // Unhighlighting an Element
            else if (interactables[i].element->highlighted)
                interactables[i].element->highlighted = false;


        }
    }

    // Managing Clicked Element
    if (clickedElement != nullptr)
    {
        clickedElement->OnHold(glm::vec2(xpos, ypos) * width);

        // Unclicking an Element
        if (clickedElement->clicked && !glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1))
        {
            clickedElement->OnRelease(state);
            clickedElement->clicked = false;

            // Checking if Dropdown Button was clicked
            if (clickedElement->getType() == ui::UI_DROPDOWN)
            {
                GenerateInteractables();
            }

            clickedElement = nullptr;
        }
    }
}

// Hierarchy Window Functions
void HierarchyWindow::GenerateInteractables()
{
}
void HierarchyWindow::DrawAttributeWindow()
{
    /*
    // Rendering Base Quads
    renderer.renderQuad(glm::vec3(width / 2.0f, height / 2.0f, 0.0f), glm::vec2(width, height), colors::grey.rgb());
    renderer.renderQuad(glm::vec3(width / 2.0f, height / 2.0f, 0.01f), glm::vec2(width*0.92f, height - (width * 0.08f)), colors::darkerGrey.rgb());

    float yPos = height - (width * 0.1f);

    // Rendering Hierarchy Label
    renderer.renderQuad(glm::vec3((width / 2), yPos, 0.1f), glm::vec2(0.92f * width, 0.12f * width), glm::vec3(0.51f));
    renderer.renderText("Hierarchy", (width / 2), yPos - (width * 0.035f), largText(), glm::vec3(1.0f), CENTER);

    yPos -= (width * 0.1f);
    std::vector<int> savedIDs;
    std::vector<obj::Object*> objects = objectSystem->getObjects();
    for (int i = 0; i < objects.size(); i++)
    {
        // Checking if object has already been listed
        int index = smath::vectorFind(savedIDs, objects[i]->getID());

        if (index != -1)
            continue;
        
        // Drawing object
        renderer.renderQuad(glm::vec3((width / 2), yPos, 0.2f), glm::vec2(0.92f * width, 0.08f * width), colors::lightgrey.rgb());
        if (objects[i]->hasChildren()) // Checking if dropdown arrow should be rendered
            renderer.renderQuad(glm::vec3((width * 0.08f), yPos, 0.21f), glm::vec2(0.04f * width), colors::white.rgb());
        renderer.renderQuad(glm::vec3((width * 0.16f), yPos, 0.21f), glm::vec2(0.04f * width), colors::white.rgb()); // Object Symbol
        renderer.renderText(objects[i]->getName(), (width * 0.2f), yPos - (width * 0.03f), mediumText(), colors::white.rgb(), LEFT);
        renderer.renderQuad(glm::vec3((width * 0.84f), yPos, 0.21f), glm::vec2(0.06f * width, 0.04f * width), colors::white.rgb()); // Visibility Symbol
        renderer.renderQuad(glm::vec3((width * 0.92f), yPos, 0.21f), glm::vec2(0.05f * width, 0.04f * width), colors::white.rgb()); // Render Symbol
        
        savedIDs.push_back(objects[i]->getID());

        // Drawing Object Children
        for (int j = 0; j < objects[i]->getChildren().size(); j++)
        {
            yPos -= (width * 0.08f);
            //std::cout << "  o " << objects[i]->getChildren()[j]->getName() << std::endl;
            savedIDs.push_back(objects[i]->getChildren()[j]->getID());
        }

        yPos -= (width * 0.08f);
    }
    */
}
void HierarchyWindow::ManageUIInteraction(GLFWwindow* window, StateMachine* state)
{

}