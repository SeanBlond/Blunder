#include "../UIWindows.h"
using namespace ui;

// Hierarchy Window Functions
// UI Creation
void HierarchyWindow::CreateHierarchyElementsFromRoot(HierarchyFolder* root)
{
    // Deleting old root (if applicable)
    if (rootFolderAttribute)
    {
        delete rootFolderAttribute;
    }

    // Creating root attribute from passed in element
    rootFolderAttribute = new ui::HierarchyFolderAttribute(root, nullptr);
    CreateFolderAttribute(root, rootFolderAttribute);
}
void HierarchyWindow::CreateFolderAttribute(HierarchyFolder* folder, ui::HierarchyFolderAttribute* folderAttribute)
{
    // Going through each child folder of the main folder and creating attributes for them
    for (int i = 0; i < folder->getChildFoldersSize(); i++)
    {
        // Creating a temporary pointer to the new attribute
        ui::HierarchyFolderAttribute* tempAttribute = new ui::HierarchyFolderAttribute(folder->getChildFolder(i), folderAttribute);

        // Properly creating the folder attribute
        CreateFolderAttribute(folder->getChildFolder(i), tempAttribute);

        // Adding the fully created attribute to the child vector
        folderAttribute->folderChildren.push_back(tempAttribute);
    }

    // Adding each child element at an attribute child
    for (int i = 0; i < folder->getHierarchyElementSize(); i++)
    {
        // Creating a temporary pointer to the new attribute
        ui::HierarchyElementAttribute* tempAttribute = new ui::HierarchyElementAttribute(folder->getHierarchyElement(i), folderAttribute, nullptr, ui::UI_OBJECT_SYMBOL);

        // Properly creating the element attribute
        CreateElementAttribute(folder->getHierarchyElement(i), tempAttribute);

        // TODO: Make it so different element symbols are passed in to the child constructor
        folderAttribute->elementChildren.push_back(tempAttribute);
    }
}
void HierarchyWindow::CreateElementAttribute(HierarchyElement* element, ui::HierarchyElementAttribute* elementAttribute)
{
    // Looping through element child and adding it to the attribute
    for (int i = 0; i < element->getChildrenSize(); i++)
    {
        // Creating a temporary pointer to the new attribute
        ui::HierarchyElementAttribute* tempAttribute = new ui::HierarchyElementAttribute(element->getChild(i), elementAttribute->folderParent, elementAttribute, ui::UI_OBJECT_SYMBOL);

        // Properly creating the element attribute
        CreateElementAttribute(element->getChild(i), tempAttribute);

        // TODO: Make it so different element symbols are passed in to the child constructor
        elementAttribute->elementChildren.push_back(tempAttribute);
    }
}

// Resizing
void HierarchyWindow::ResizeWindow()
{
    // Setting up initial positioning values
    float hierarchyTitleHeight = position.unitScale * 1.5f;
    float yPos = position.getHeight() - hierarchyTitleHeight - position.getBuffer() - (position.unitScale * 0.5f);

    // Starting the resizing from the root folder
    ResizeFolderAttribute(rootFolderAttribute, 0, yPos);
}
void HierarchyWindow::ResizeFolderAttribute(ui::HierarchyFolderAttribute* attribute, int indent, float& yPos)
{
    // Creating the base position that all elements of the attribute will be based on
    ElementPosition tempPos = ElementPosition(
        glm::vec2(position.getWidth() * 0.5f + (indent * position.unitScale * 0.5f), yPos),    // Center
        glm::vec2(position.getWidth() - (2.0f * position.getBuffer()) - (indent * position.unitScale), position.unitScale),   // Size
        0.0f,   // Split
        &(this->position)   // Parent Position
    );

    // Resizing the dropdown toggle (if applicable)
    if (attribute->dropdownToggle)
        attribute->dropdownToggle->UpdateElement(ElementPosition(tempPos.getStartCorners(position.unitScale), 0.0f, &(this->position)));

    // Resizing the hierarchy text element
    attribute->nameEntry->UpdateElement(ElementPosition(glm::vec4(
        position.unitScale * (2.0f + indent) + position.getBuffer(),
        tempPos.bottom_y,
        tempPos.getWidth() - (position.unitScale * 2.0f),
        tempPos.top_y),
        0.0f, &(this->position)));

    // Resizing the display toggle
    attribute->displayToggle->UpdateElement(ElementPosition(tempPos.getEndCorners(position.unitScale) - ui::ElementPosition::getXPosCorners(position.unitScale), 0.0f, &(this->position)));

    // Resizing the render toggle
    attribute->renderToggle->UpdateElement(ElementPosition(tempPos.getEndCorners(position.unitScale), 0.0f, &(this->position)));

    // Updating yPos for next attribute
    yPos -= position.unitScale;

    // Resizing attribute children (if not dropped down)
    if (attribute->folder->getDropdown())
    {
        for (int i = 0; i < attribute->folderChildren.size(); i++)
        {
            ResizeFolderAttribute(attribute->folderChildren[i], indent + 1, yPos);
        }
        for (int i = 0; i < attribute->elementChildren.size(); i++)
        {
            ResizeElementAttribute(attribute->elementChildren[i], indent + 1, yPos);
        }
    }
}
void HierarchyWindow::ResizeElementAttribute(ui::HierarchyElementAttribute* attribute, int indent, float& yPos)
{
    // Creating the base position that all elements of the attribute will be based on
    ElementPosition tempPos = ElementPosition(
        glm::vec2(position.getWidth() * 0.5f + (indent * position.unitScale * 0.5f), yPos),    // Center
        glm::vec2(position.getWidth() - (2.0f * position.getBuffer()) - (indent * position.unitScale), position.unitScale),   // Size
        0.0f,   // Split
        &(this->position)   // Parent Position
    );

    // Resizing the dropdown toggle (if applicable)
    if (attribute->dropdownToggle)
        attribute->dropdownToggle->UpdateElement(ElementPosition(tempPos.getStartCorners(position.unitScale), 0.0f, &(this->position)));

    // Resizing the hierarchy text element
    attribute->nameEntry->UpdateElement(ElementPosition(glm::vec4(
        position.unitScale * (2.0f + indent) + position.getBuffer(),
        tempPos.bottom_y,
        tempPos.getWidth() - (position.unitScale * 2.0f),
        tempPos.top_y),
        0.0f, &(this->position)));

    // Resizing the display toggle
    attribute->displayToggle->UpdateElement(ElementPosition(tempPos.getEndCorners(position.unitScale) - ui::ElementPosition::getXPosCorners(position.unitScale), 0.0f, &(this->position)));

    // Resizing the render toggle
    attribute->renderToggle->UpdateElement(ElementPosition(tempPos.getEndCorners(position.unitScale), 0.0f, &(this->position)));

    // Updating yPos for next attribute
    yPos -= position.unitScale;

    // Resizing attribute children (if not dropped down)
    if (attribute->element->getDropdown())
    {
        for (int i = 0; i < attribute->elementChildren.size(); i++)
        {
            ResizeElementAttribute(attribute->elementChildren[i], indent + 1, yPos);
        }
    }
}

// Drawing
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
    DrawFolderAttribute(renderer, rootFolderAttribute, 0, yPos);

    // Drawing the hierarchy positioning quads (if applicable)
    if (storedHierarchyPositions.size() > 0)
    {
        for (int i = 0; i < storedHierarchyPositions.size(); i++)
        {
            renderer->addQuad(glm::vec3(
                position.getWidth() - (1.75f * position.unitScale),
                storedHierarchyPositions[i].yPos,
                0.95f
            ), glm::vec2(position.unitScale, position.unitScale * 0.15f), colors::blue.rgb(), position.offset);
        }
    }
}
void HierarchyWindow::DrawFolderAttribute(ui::UIRenderer* renderer, ui::HierarchyFolderAttribute* attribute, int indent, float& yPos)
{
    // TODO: Checking if highlighted quad needs to be drawn
    /*if (attribute->folder == state->getSelectedFolder())
    {
        renderer->addQuad(
            glm::vec3(position.getXCenter(), yPos, 0.2f),
            glm::vec2(position.getWidth() - (2.0f * position.getBuffer()), position.unitScale),
            colors::grey.rgb());
    }*/

    // Drawing the dropdown symbol (if applicable)
    if (attribute->folder->hasChildren())
        attribute->dropdownToggle->RenderElement(renderer, mediumText());

    // Drawing the element symbol
    glm::vec4 symbolCorners = glm::vec4(
        position.unitScale * (1.0f + indent) + position.getBuffer(),
        yPos - position.unitScale * 0.5f,
        position.unitScale * (2.0f + indent) + position.getBuffer(),
        yPos + position.unitScale * 0.5f
    );
    renderer->addQuad(symbolCorners, 0.21f, colors::lightestgrey.rgb(), position.offset, ui::UI_FOLDER_SYMBOL);

    // TODO: Drawing the Element Name
    attribute->nameEntry->RenderElement(renderer, mediumText());

    // Drawing the display symbol
    attribute->displayToggle->RenderElement(renderer, mediumText());

    // Drawing the render symbol
    attribute->renderToggle->RenderElement(renderer, mediumText());

    // Changing the yPos
    yPos -= position.unitScale;

    // Rendering the attributes children (if not dropped down)
    if (attribute->folder->getDropdown())
    {
        for (int i = 0; i < attribute->folderChildren.size(); i++)
        {
            DrawFolderAttribute(renderer, attribute->folderChildren[i], indent + 1, yPos);
        }
        for (int i = 0; i < attribute->elementChildren.size(); i++)
        {
            DrawElementAttribute(renderer, attribute->elementChildren[i], indent + 1, yPos);
        }
    }
}
void HierarchyWindow::DrawElementAttribute(ui::UIRenderer* renderer, ui::HierarchyElementAttribute* attribute, int indent, float& yPos)
{
    // Checking if highlighted quad needs to be drawn
    if (attribute->element->getObject() == state->getSelectedObject())
    {
        renderer->addQuad(
            glm::vec3(position.getXCenter(), yPos, 0.2f),
            glm::vec2(position.getWidth() - (2.0f * position.getBuffer()), position.unitScale),
            colors::grey.rgb());
    }

    // Drawing the dropdown symbol (if applicable)
    if (attribute->element->hasChildren())
        attribute->dropdownToggle->RenderElement(renderer, mediumText());

    // Drawing the element symbol
    glm::vec4 symbolCorners = glm::vec4(
        position.unitScale * (1.0f + indent) + position.getBuffer(),
        yPos - position.unitScale * 0.5f,
        position.unitScale * (2.0f + indent) + position.getBuffer(),
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

    // Rendering the attributes children (if not dropped down)
    if (attribute->element->getDropdown())
    {
        for (int i = 0; i < attribute->elementChildren.size(); i++)
        {
            DrawElementAttribute(renderer, attribute->elementChildren[i], indent + 1, yPos);
        }
    }
}

// Interaction Management
void HierarchyWindow::ManageInteraction(GLFWwindow* window, StateMachine* state)
{
    // Getting mouse Position (flipping the y because glfw is stupid)
    glm::vec2 mousePos = glm::vec2(
        state->getMouse()->mousePos.x,
        (float)(state->getWindowDimensions().y) - state->getMouse()->mousePos.y
    );

    // Determinig current element
    ui::AttributeElement* currentElement = CheckFolderAttributeInteraction(rootFolderAttribute, mousePos);

    // Doing interaction stuff if collision exists
    if (currentElement && !state->getTransforming())
    {
        // Clicking an Element
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) && clickedElement == nullptr)
        {
            clickedElement = currentElement;
            clickedElement->clicked = true;
            clickedElement->OnClick(state);
        }

        // Highilighting an Element
        else
        {
            if (highlightedElement && highlightedElement != currentElement)
            {
                highlightedElement->highlighted = false;
            }

            highlightedElement = currentElement;
            highlightedElement->highlighted = true;

        }
    }

    // Unhighlighting an Element
    else if (highlightedElement && currentElement != highlightedElement && highlightedElement->highlighted)
    {
        highlightedElement->highlighted = false;
        highlightedElement = nullptr;
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

            // Checking if UI Sizing needs to be redone
            if (clickedElement->getType() == ui::UI_ATTRIBUTE_HEADER)
                ResizeWindow();

            clickedElement = nullptr;
        }
    }
}
ui::AttributeElement* HierarchyWindow::CheckFolderAttributeInteraction(ui::HierarchyFolderAttribute* attribute, glm::vec2 pos)
{
    // Checking the dropdown element (if applicable)
    if ((attribute->folder->hasChildren()) && attribute->dropdownToggle->checkCollision(pos))
        return attribute->dropdownToggle;

    // Checking the name entry
    if (attribute->nameEntry->checkCollision(pos))
        return attribute->nameEntry;

    // Checking the display toggle
    if (attribute->displayToggle->checkCollision(pos))
        return attribute->displayToggle;

    // Checking the render toggle
    if (attribute->renderToggle->checkCollision(pos))
        return attribute->renderToggle;

    // Checking the children (if not dropped down)
    if (attribute->folder->getDropdown())
    {
        for (int i = 0; i < attribute->folderChildren.size(); i++)
        {
            // Checking if a child interaction returns a non-null element, then returning it
            ui::AttributeElement* childInteraction = CheckFolderAttributeInteraction(attribute->folderChildren[i], pos);
            if (childInteraction) { return childInteraction; }
        }

        for (int i = 0; i < attribute->elementChildren.size(); i++)
        {
            // Checking if a child interaction returns a non-null element, then returning it
            ui::AttributeElement* childInteraction = CheckElementAttributeInteraction(attribute->elementChildren[i], pos);
            if (childInteraction) { return childInteraction; }
        }
    }

    // Not collision found, return null
    return nullptr;
}
ui::AttributeElement* HierarchyWindow::CheckElementAttributeInteraction(ui::HierarchyElementAttribute* attribute, glm::vec2 pos)
{
    // Checking the dropdown element (if applicable)
    if (attribute->element->hasChildren() && attribute->dropdownToggle->checkCollision(pos))
        return attribute->dropdownToggle;

    // Checking the name entry
    if (attribute->nameEntry->checkCollision(pos))
        return attribute->nameEntry;

    // Checking the display toggle
    if (attribute->displayToggle->checkCollision(pos))
        return attribute->displayToggle;

    // Checking the render toggle
    if (attribute->renderToggle->checkCollision(pos))
        return attribute->renderToggle;

    // Checking the children (if not dropped down)
    if (attribute->element->getDropdown())
    {
        for (int i = 0; i < attribute->elementChildren.size(); i++)
        {
            // Checking if a child interaction returns a non-null element, then returning it
            ui::AttributeElement* childInteraction = CheckElementAttributeInteraction(attribute->elementChildren[i], pos);
            if (childInteraction) { return childInteraction; }
        }
    }

    // Not collision found, return null
    return nullptr;
}

// Hierarchy Positioning
void HierarchyWindow::GeneratePositionList(HierarchyFolder* excludedFolder, HierarchyElement* excludedElement)
{
    // Clearing old positions
    storedHierarchyPositions.clear();

    // Setting up initial positioning values
    float hierarchyTitleHeight = position.unitScale * 1.5f;
    float yPos = position.getHeight() - hierarchyTitleHeight - position.getBuffer() - (position.unitScale * 0.5f);

    GeneratePositionFromFolder(rootFolderAttribute, yPos, excludedFolder, excludedElement);

    std::cout << "GENERATED POSITION LIST: " << std::endl;
    for (int i = 0; i < storedHierarchyPositions.size(); i++)
    {
        std::cout << i << ", yPos: " << storedHierarchyPositions[i].yPos
            << ", folder parent: " << (storedHierarchyPositions[i].folderParent ? storedHierarchyPositions[i].folderParent->getName() : "N/A")
            << ", element parent: " << (storedHierarchyPositions[i].elementParent ? storedHierarchyPositions[i].elementParent->getName() : "N/A")
            << std::endl;
    }
}
void HierarchyWindow::GeneratePositionFromFolder(HierarchyFolderAttribute* folder, float& yOffset, HierarchyFolder* excludedFolder, HierarchyElement* excludedElement)
{
    // Checking if the folder requested was the exlcuded one
    if (folder->folder == excludedFolder)
        return;

    // Generating the child position
    storedHierarchyPositions.push_back(HierarchyPositioning(yOffset, folder->folder));

    // Generating the below 
    yOffset -= position.unitScale * 0.5f;
    storedHierarchyPositions.push_back(HierarchyPositioning(yOffset, (folder->folderParent ? folder->folderParent->folder : nullptr)));

    // Changing yOffset for next attributes
    yOffset -= position.unitScale * 0.5f;

    // Calling Generate positions on all other children
    if (folder->folder->hasChildren())
    {
        for (int i = 0; i < folder->folderChildren.size(); i++)
        {
            GeneratePositionFromFolder(folder->folderChildren[i], yOffset, excludedFolder, excludedElement);
        }
        for (int i = 0; i < folder->elementChildren.size(); i++)
        {
            GeneratePositionFromElement(folder->elementChildren[i], yOffset, excludedElement);
        }
    }
}
void HierarchyWindow::GeneratePositionFromElement(HierarchyElementAttribute* element, float& yOffset, HierarchyElement* excludedElement)
{
    // Checking if the element requested was the exlcuded one
    if (element->element == excludedElement)
        return;

    // Generating the child position
    storedHierarchyPositions.push_back(HierarchyPositioning(yOffset, element->folderParent->folder, element->element));

    // Generating the below 
    yOffset -= position.unitScale * 0.5f;
    storedHierarchyPositions.push_back(HierarchyPositioning(yOffset, element->folderParent->folder, (element->elementParent ? element->elementParent->element : nullptr)));

    // Changing yOffset for next attributes
    yOffset -= position.unitScale * 0.5f;

    // Calling Generate positions on all other children
    if (element->element->hasChildren())
    {
        for (int i = 0; i < element->elementChildren.size(); i++)
        {
            GeneratePositionFromElement(element->elementChildren[i], yOffset, excludedElement);
        }
    }
}

// Unselecting
void HierarchyWindow::UnselectWindow()
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
