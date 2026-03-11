#include "../UIWindows.h"
using namespace ui;

// Hierarchy Window Functions
void HierarchyWindow::CreateHierarchyElementsFromRoot(Folder* root)
{
    // Deleting old root (if applicable)
    if (rootAttribute)
    {
        delete rootAttribute;
    }

    // Creating root attribute from passed in element
    rootAttribute = new ui::HierarchyAttribute(root, ui::UI_FOLDER_SYMBOL);
    CreateHierarchyElementsFromFolder(root, rootAttribute);
}
void HierarchyWindow::CreateHierarchyElementsFromFolder(Folder* folder, ui::HierarchyAttribute* folderAttribute)
{
    // Adding each hierarchy element at an attribute child
    for (int i = 0; i < folder->getHierarchyElementSize(); i++)
    {
        // TODO: Make it so different element symbols are passed in to the child constructor
        folderAttribute->children.push_back(new ui::HierarchyAttribute(folder->getHierarchyElement(i), ui::UI_OBJECT_SYMBOL));
    }

    // Going through eaach child folder of the main folder and creating attributes for them
    for (int i = 0; i < folder->getChildFoldersSize(); i++)
    {
        // Creating a temporry pointer to the new attribute
        ui::HierarchyAttribute* tempAttribute = new ui::HierarchyAttribute(folder->getChildFolder(i), ui::UI_FOLDER_SYMBOL);

        // Properly creating the folder attribute
        CreateHierarchyElementsFromFolder(folder->getChildFolder(i), tempAttribute);

        // Adding the fully created attribute to the child vector
        folderAttribute->children.push_back(tempAttribute);
    }
}
void HierarchyWindow::ResizeAttribute(ui::HierarchyAttribute* attribute, int indent, float& yPos)
{
    // Creating the base position that all elements of the attribute will be based on
    ElementPosition tempPos = ElementPosition(
        glm::vec2(position.getWidth() * 0.5f + (indent * position.unitScale * 0.5f) + position.getXOffset(), yPos),    // Center
        glm::vec2(position.getWidth() - 2.0f * position.getBuffer() - (indent * position.unitScale), position.unitScale),   // Size
        0.0f,   // Split
        &(this->position)   // Parent Position
    );

    // Resizing the dropdown toggle (if applicable)
    if (attribute->dropdownToggle)
        attribute->dropdownToggle->UpdateElement(ElementPosition(tempPos.getStartCorners(position.unitScale), 0.0f, &(this->position)));

    // Resizing the hierarchy text element
    attribute->nameEntry->UpdateElement(ElementPosition(glm::vec4(
        position.unitScale * (2.0f + indent),
        tempPos.bottom_y,
        tempPos.getWidth() - (position.unitScale * 2.0f),
        tempPos.top_y),
    0.0f, &(this->position)));

    // Resizing the display toggle
    attribute->displayToggle->UpdateElement(ElementPosition(tempPos.getEndCorners(position.unitScale) - ui::ElementPosition::getXPosCorners(position.unitScale), 0.0f, &(this->position)));

    // Resizing the render toggle
    attribute->renderToggle->UpdateElement(ElementPosition(tempPos.getEndCorners(position.unitScale), 0.0f, &(this->position)));
    std::cout << "Resized Render Toggle to " << smath::outputVec4(tempPos.getEndCorners(position.unitScale)) << std::endl;

    // Updating yPos for next attribute
    yPos -= position.unitScale;

    // Resizing attribute children
    for (int i = 0; i < attribute->children.size(); i++)
    {
        ResizeAttribute(attribute->children[i], indent + 1, yPos);
    }
}
void HierarchyWindow::ResizeWindow()
{
    // Setting up initial positioning values
    float hierarchyTitleHeight = position.unitScale * 1.5f;
    float yPos = position.getHeight() - hierarchyTitleHeight - position.getBuffer() - (position.unitScale * 0.5f);

    // Starting the resizing from the root folder
    ResizeAttribute(rootAttribute, 0, yPos);
}
void HierarchyWindow::DrawUIHierarchyAttribute(ui::UIRenderer* renderer, ui::HierarchyAttribute* attribute, int indent, float& yPos)
{
    // Drawing the dropdown symbol (if applicable)
    if (attribute->children.size() > 0)
        attribute->dropdownToggle->RenderElement(renderer, mediumText());

    // Drawing the element symbol
    glm::vec4 symbolCorners = glm::vec4(
        position.unitScale * (1.0f + indent),
        yPos - position.unitScale * 0.5f,
        position.unitScale * (2.0f + indent),
        yPos + position.unitScale * 0.5f
    );
    renderer->addQuad(symbolCorners, 0.21f, colors::lightestgrey.rgb(), position.offset, attribute->elementSymbol);

    // TODO: Drawing the Element Name
    attribute->nameEntry->RenderElement(renderer, mediumText());

    // Drawing the display symbol
    attribute->displayToggle->RenderElement(renderer, mediumText());
    
    // Drawing the render symbol
    attribute->renderToggle->RenderElement(renderer, mediumText());

    // Changing the yPos
    yPos -= position.unitScale;

    // Rendering the attributes children
    for (int i = 0; i < attribute->children.size(); i++)
    {
        DrawUIHierarchyAttribute(renderer, attribute->children[i], indent + 1, yPos);
    }
}
void HierarchyWindow::DrawWindow(ui::UIRenderer* renderer)
{
    // Adding Base Quads
    renderer->addQuad(position.getCorners(), 0.0f, colors::grey.rgb());
    renderer->addQuad(position.getBufferedCorners(), 0.01f, colors::darkerGrey.rgb());

    float hierarchyTitleHeight = position.unitScale * 1.5f;
    float yPos = position.getHeight() - (hierarchyTitleHeight * 0.5f) - position.getBuffer();

    // Adding Hierarchy Label
    renderer->addQuad(glm::vec3((position.getWidth() / 2), yPos, 0.1f), glm::vec2(position.getWidth() - 2 * position.getBuffer(), hierarchyTitleHeight), glm::vec3(0.51f), position.offset);
    renderer->addText("Hierarchy", glm::vec3(0.5f * position.getWidth(), yPos, 0.15f), largeText(), colors::white.rgb(), position.offset, CENTER);

    // Updating yPos
    yPos -= (hierarchyTitleHeight * 0.5f) + (position.unitScale * 0.5f);

    // Starting the UI Draw from the root folder
    DrawUIHierarchyAttribute(renderer, rootAttribute, 0, yPos);
}
void HierarchyWindow::ManageInteraction(GLFWwindow* window, StateMachine* state)
{
    /*
    // Converting mouse position to relative coordinates
    glm::vec2 relMousePos = state->getMouse()->mousePos / position.dimensions;

    // Finding CLicked Element
    //std::cout << "Mouse Pos: (" << xpos << ", " << ypos << ")" << std::endl;
    for (int i = 0; i < interactables.size(); i++)
    {
        // Highlighting an Element
        if (smath::checkUICollision(relMousePos, interactables[i].corners) && !state->getTransforming())
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
            if (clickedElement->getType() == ui::UI_DROPDOWN)
            {
                //GenerateInteractables();
            }

            clickedElement = nullptr;
        }
    }
    */
}
void HierarchyWindow::UnselectWindow()
{
    // Unclicking element
    if (clickedElement != nullptr)
    {
        clickedElement->clicked = false;
        clickedElement->highlighted = false;
        clickedElement = nullptr;

    }
}