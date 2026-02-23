#include "UIWindows.h"

using namespace ui;

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
    float attributeTitleHeight = position.getWidth() * 0.12f;
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
            float elementHeight = position.getWidth() * 0.08f;
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
        if (checkUICollision(xpos, ypos, interactables[i]) && !state->getTransforming())
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
    positionAttribute->addFloatEntry("X", &(object->transform.position.x));
    positionAttribute->addFloatEntry("Y", &(object->transform.position.y));
    positionAttribute->addFloatEntry("Z", &(object->transform.position.z));
    addAttribute(positionAttribute);

    ui::Attribute* rotationAtrribute = new ui::Attribute("Rotation");
    rotationAtrribute->addFloatEntry("X", &(object->transform.rotation.x));
    rotationAtrribute->addFloatEntry("Y", &(object->transform.rotation.y));
    rotationAtrribute->addFloatEntry("Z", &(object->transform.rotation.z));
    addAttribute(rotationAtrribute);

    ui::Attribute* scaleAtrribute = new ui::Attribute("Scale");
    scaleAtrribute->addFloatEntry("X", &(object->transform.scale.x));
    scaleAtrribute->addFloatEntry("Y", &(object->transform.scale.y));
    scaleAtrribute->addFloatEntry("Z", &(object->transform.scale.z));
    // Adding Test Dropdown REMOVE AT SOME POINT
    std::vector<std::string> options = { "Option 1", "Option 2", "Option 3", "Option 4" };
    testDropdownValue = 0;
    scaleAtrribute->addDropdown("Test", &testDropdownValue, options);
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
                meshAttribute->addFloatSlider(attribute->attributeLabel, &(attribute->attributeValue), 1.0f, attribute->attributeLimits[0], attribute->attributeLimits[1]);
            else
                meshAttribute->addFloatEntry(attribute->attributeLabel, &(attribute->attributeValue));
        }

        // Adding the int attributes
        for (int i = 0; i < object->getMesh()->getNumIntAttributes(); i++)
        {
            IntAttribute* attribute = object->getMesh()->getIntAttribute(i);
            // Checking if the attribute should be a slider (has limits?)
            if (attribute->hasLimits)
                meshAttribute->addIntSlider(attribute->attributeLabel, &(attribute->attributeValue), 1.0f, attribute->attributeLimits[0], attribute->attributeLimits[1]);
            else
                meshAttribute->addIntEntry(attribute->attributeLabel, &(attribute->attributeValue));
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

// Hierarchy Window Functions
void HierarchyWindow::generateFolderInteractable(Folder* folder, int indent, float& yPos)
{
    // Dropdown Button
    glm::vec4 tempCorners = glm::vec4(
        (0.08f * indent) + 0.02f,
        yPos - 0.04f,
        (0.08f * indent) + 0.06f,
        yPos + 0.04f
    );
    ui::AttributeInteractable folderDropdown(tempCorners, new ui::Toggle("folder-dropdown", folder->getDropdownAddress(), ui::UI_DROPDOWN));
    std::cout << "Interactable generated at " << smath::outputVec4(tempCorners) << std::endl;
    interactables.push_back(folderDropdown);

    // Hierarchy Text Entry
    //tempCorners = glm::vec4(
    //    (0.08f * indent) + 0.14f,
    //    yPos - 0.04f,
    //    0.8f,
    //    yPos + 0.04f
    //);
    //ui::AttributeInteractable folderName(tempCorners, folder->getHierarchyTextUI());
    //std::cout << "Interactable generated at " << smath::outputVec4(tempCorners) << std::endl;
    //interactables.push_back(folderName);

    // Visibility Button
    tempCorners = glm::vec4(
        0.82f,
        yPos - 0.04f,
        0.9f,
        yPos + 0.04f
    );
    ui::AttributeInteractable folderVisibility(tempCorners, new ui::Toggle("folder-dropdown", folder->getDisplayedAddress()));
    std::cout << "Interactable generated at " << smath::outputVec4(tempCorners) << std::endl;
    interactables.push_back(folderVisibility);

    // Render Button
    tempCorners = glm::vec4(
        0.9f,
        yPos - 0.04f,
        0.98f,
        yPos + 0.04f
    );
    ui::AttributeInteractable folderRender(tempCorners, new ui::Toggle("folder-dropdown", folder->getRenderedAddress()));
    std::cout << "Interactable generated at " << smath::outputVec4(tempCorners) << std::endl;
    interactables.push_back(folderRender);

    // Changing yPos
    yPos += 0.08f;

    // Generating interactables for each folder UI, if the folder is dropped down
    if (folder->getDropdown())
    {
        for (int i = 0; i < folder->getChildFoldersSize(); i++)
        {
            generateFolderInteractable(folder->getChildFolder(i), indent + 1, yPos);
        }

        // Generating interactables for each element UI, if the folder is dropped down
        for (int i = 0; i < folder->getHierarchyElementSize(); i++)
        {
            generateElementInteractable(folder->getHierarchyElement(i), indent + 1, yPos);
        }
    }
}
void HierarchyWindow::generateElementInteractable(HierarchyElement* element, int indent, float& yPos)
{
    // Dropdown Button
    glm::vec4 tempCorners = glm::vec4(
        (0.08f * indent) + 0.04f,
        yPos - 0.04f,
        (0.08f * indent) + 0.08f,
        yPos + 0.04f
    );
    ui::AttributeInteractable elementDropdown(tempCorners, new ui::Toggle("element-dropdown", element->getDropdownAddress(), ui::UI_DROPDOWN));
    interactables.push_back(elementDropdown);

    // Hierarchy Text Entry
    //tempCorners = glm::vec4(
    //    (0.08f * indent) + 0.16f,
    //    yPos - 0.04f,
    //    0.8f,
    //    yPos + 0.04f
    //);
    //ui::AttributeInteractable elementName(tempCorners, element->getHierarchyTextUI());
    //interactables.push_back(elementName);

    // Visibility Button
    tempCorners = glm::vec4(
        0.8f,
        yPos - 0.04f,
        0.88f,
        yPos + 0.04f
    );
    ui::AttributeInteractable elementVisibility(tempCorners, new ui::Toggle("element-dropdown", element->getDisplayedAddress()));
    interactables.push_back(elementVisibility);

    // Render Button
    tempCorners = glm::vec4(
        0.88f,
        yPos - 0.04f,
        0.96f,
        yPos + 0.04f
    );
    ui::AttributeInteractable elementRender(tempCorners, new ui::Toggle("element-dropdown", element->getRenderedAddress()));
    interactables.push_back(elementRender);

    // Changing yPos
    yPos += 0.08f;

    // Generating interactables for each child if dropdown
    if (element->getDropdown())
    {
        for (int i = 0; i < element->getChildrenSize(); i++)
        {
            generateElementInteractable(element->getChild(i), indent + 1, yPos);
        }
    }
}
void HierarchyWindow::GenerateInteractables()
{
    interactables.clear();

    float yPos = 0.2f;
    generateFolderInteractable(state->getScene()->getRootFolder(), 0, yPos);
}
void HierarchyWindow::DrawUIFolder(ui::UIRenderer* renderer, Folder* folder, int indent, float& yPos)
{
    // Rendering the base folder UI
    renderer->addQuad(glm::vec3((position.getWidth() / 2), yPos, 0.1f), glm::vec2(0.96f * position.getWidth(), 0.08f * position.getWidth()), colors::lightgrey.rgb(), position.offset);

    // Dropdown Symbol
    if (folder->hasChildren())
        renderer->addQuad(glm::vec3((position.getWidth() * 0.06f) + (position.getWidth() * 0.08f * indent), yPos, 0.15f), glm::vec2(0.08f * position.getWidth()), colors::white.rgb(), position.offset, (folder->getDropdown() ? ui::UI_DROPDOWN_T : ui::UI_DROPDOWN_F));

    // Folder Symbol
    renderer->addQuad(glm::vec3((position.getWidth() * 0.14f) + (position.getWidth() * 0.08f * indent), yPos, 0.15f), glm::vec2(0.08f * position.getWidth()), colors::white.rgb(), position.offset, ui::UI_FOLDER_SYMBOL);

    // Folder Text
    //ui::ElementPosition textPos(glm::vec4(
    //    position.getBuffer() + (0.16f + (0.08f * indent)) * position.getWidth(),
    //    yPos - (0.04f * position.getWidth()),
    //    position.getWidth() - (position.getBuffer() + (0.16f * position.getWidth())),
    //    yPos + (0.04f * position.getWidth())
    //), 0.0f, &position);
    //folder->getHierarchyTextUI()->RenderElement(renderer, textPos, mediumText());

    // Visibility Symbol
    renderer->addQuad(glm::vec3((position.getWidth() * 0.86f), yPos, 0.15f), glm::vec2(0.08f * position.getWidth()), colors::white.rgb(), position.offset, (folder->getDisplayed() ? ui::UI_DISPLAY_T : ui::UI_DISPLAY_F));

    // Render Symbol
    renderer->addQuad(glm::vec3((position.getWidth() * 0.94f), yPos, 0.15f), glm::vec2(0.08f * position.getWidth()), colors::white.rgb(), position.offset, (folder->getRendered() ? ui::UI_RENDER_T : ui::UI_RENDER_F));

    // Changing yPos
    yPos -= (position.getWidth() * 0.08f);

    // Checking if children should be displayed
    if (folder->getDropdown())
    {
        // Rendering each folder UI
        for (int i = 0; i < folder->getChildFoldersSize(); i++)
        {
            DrawUIFolder(renderer, folder->getChildFolder(i), indent + 1, yPos);
        }

        // Rendering each element UI
        for (int i = 0; i < folder->getHierarchyElementSize(); i++)
        {
            DrawUIHierarchyElement(renderer, folder->getHierarchyElement(i), indent + 1, yPos);
        }
    }
}
void HierarchyWindow::DrawUIHierarchyElement(ui::UIRenderer* renderer, HierarchyElement* element, int indent, float& yPos)
{
    // Rendering the base element UI
    Color baseColor = (state->getSelectedObject() == element->getObject() ? colors::grey.rgb() : colors::lightgrey.rgb());
    renderer->addQuad(glm::vec3((position.getWidth() / 2), yPos, 0.1f), glm::vec2(0.96f * position.getWidth(), 0.08f * position.getWidth()), baseColor.rgb(), position.offset);

    // Dropdown Symbol
    if (element->hasChildren())
        renderer->addQuad(glm::vec3((position.getWidth() * 0.06f) + (position.getWidth() * 0.08f * indent), yPos, 0.15f), glm::vec2(0.08f * position.getWidth()), colors::white.rgb(), position.offset, (element->getDropdown() ? ui::UI_DROPDOWN_T : ui::UI_DROPDOWN_F));

    // Object Symbol
    renderer->addQuad(glm::vec3((position.getWidth() * 0.14f) + (position.getWidth() * 0.08f * indent), yPos, 0.15f), glm::vec2(0.08f * position.getWidth()), colors::white.rgb(), position.offset, ui::UI_OBJECT_SYMBOL);

    // Object Text
    //ui::ElementPosition textPos(glm::vec4(
    //    position.getBuffer() + (0.16f + (0.08f * indent)) * position.getWidth(),
    //    yPos - (0.04f * position.getWidth()),
    //    position.getWidth() - (position.getBuffer() + (0.16f * position.getWidth())),
    //    yPos + (0.04f * position.getWidth())
    //), 0.0f, &position);
    //element->getHierarchyTextUI()->RenderElement(renderer, textPos, mediumText());

    // Visibility Symbol
    renderer->addQuad(glm::vec3((position.getWidth() * 0.86f), yPos, 0.15f), glm::vec2(0.08f * position.getWidth()), colors::white.rgb(), position.offset, (element->getDisplayed() ? ui::UI_DISPLAY_T : ui::UI_DISPLAY_F));

    // Render Symbol
    renderer->addQuad(glm::vec3((position.getWidth() * 0.94f), yPos, 0.15f), glm::vec2(0.08f * position.getWidth()), colors::white.rgb(), position.offset, (element->getRendered() ? ui::UI_RENDER_T : ui::UI_RENDER_F));

    // Changing yPos
    yPos -= (position.getWidth() * 0.08f);

    // Checking if children should be displayed
    if (element->getDropdown())
    {
        // Rendering each child of the element
        for (int i = 0; i < element->getChildrenSize(); i++)
        {
            DrawUIHierarchyElement(renderer, element->getChild(i), indent + 1, yPos);
        }
    }
}
void HierarchyWindow::DrawWindow(ui::UIRenderer* renderer)
{
    // Adding Base Quads
    renderer->addQuad(position.getCorners(), 0.0f, colors::grey.rgb());
    renderer->addQuad(position.getBufferedCorners(), 0.01f, colors::darkerGrey.rgb());

    float hierarchyTitleHeight = 0.12f * position.getWidth();
    float yPos = position.getHeight() - (hierarchyTitleHeight * 0.5f) - position.getBuffer();

    // Adding Hierarchy Label
    renderer->addQuad(glm::vec3((position.getWidth() / 2), yPos, 0.1f), glm::vec2(position.getWidth() - 2 * position.getBuffer(), hierarchyTitleHeight), glm::vec3(0.51f), position.offset);
    renderer->addText("Hierarchy", glm::vec3(0.5f * position.getWidth(), yPos, 0.15f), largeText(), colors::white.rgb(), position.offset, CENTER);

    // Updating yPos
    yPos -= (hierarchyTitleHeight * 0.5f) + position.getWidth() * 0.04f;

    // Starting the UI Draw from the root folder
    DrawUIFolder(renderer, state->getScene()->getRootFolder(), 0, yPos);
}
void HierarchyWindow::ManageInteraction(GLFWwindow* window, StateMachine* state)
{
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
        if (checkUICollision(xpos, ypos, interactables[i]) && !state->getTransforming())
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
                GenerateInteractables();
            }

            clickedElement = nullptr;
        }
    }
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

// Viewport Window Functions
void ViewportWindow::GenerateInteractables()
{
    // ViewNav interactable 
    float viewNavBuffer = 0.005;
    glm::vec4 tempCorners = glm::vec4(
        1 - (viewNavElement.getNavSize() / position.getWidth()) - viewNavBuffer,
        viewNavBuffer,
        1 - viewNavBuffer,
        viewNavElement.getNavSize() / position.getHeight() - viewNavBuffer
    );
    ui::AttributeInteractable viewNavInteract(tempCorners, &viewNavElement);
    interactables.push_back(viewNavInteract);
}
void ViewportWindow::DrawWindow(ui::UIRenderer* renderer)
{
    // Rendering the Scene to the Viewport
    RenderScene();

    // Setting ViewNav draw values
    glm::mat4 transform = smath::orthographic(-2, 2, -2, 2, 0.1f, 100.0f) * state->getCamera()->getViewMatrix() * smath::scale(glm::vec3(1.5f));

    // Drawing the ViewNav
    viewNavElement.setTransform(transform);
    glm::vec4 navCorners = glm::vec4(
        position.getWidth() - viewNavElement.getNavSize() + position.getXOffset(),
        position.getHeight() - viewNavElement.getNavSize() + position.getYOffset(),
        position.getWidth() + position.getXOffset(),
        position.getHeight() + position.getYOffset()
    );
    ui::ElementPosition elementPos(navCorners, position.getXOffset(), &position);
    viewNavElement.RenderElement(renderer, elementPos, smallText());

    // Setting viewport size
    glm::vec4 viewportSize = glm::vec4(
        position.getXOffset(),
        position.getYOffset(),
        position.getWidth(),
        position.getHeight()
    );
    glViewport(viewportSize.x, viewportSize.y, viewportSize.z, viewportSize.w);
}
void ViewportWindow::ManageInteraction(GLFWwindow* window, StateMachine* state)
{
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
        if (checkUICollision(xpos, ypos, interactables[i]) && !state->getTransforming())
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
                GenerateInteractables();
            }

            clickedElement = nullptr;
        }
    }
}
void ViewportWindow::UnselectWindow()
{
    // Unclicking element
    if (clickedElement != nullptr)
    {
        clickedElement->clicked = false;
        clickedElement->highlighted = false;
        clickedElement = nullptr;

    }
}
void ViewportWindow::RenderScene()
{
    // Getting the Matrices
    glm::mat4 projection = state->getCamera()->getProjectionMatrix(position.getAspectRatio(), 0.01f, 1000.0f);
    glm::mat4 view = state->getCamera()->getViewMatrix();

    // Rendering the scene
    glViewport(position.getXOffset(), position.getYOffset(), position.getWidth(), position.getHeight());
    state->getScene()->Render(projection, view);

    // Drawing Axis Lines
    viewportShader->useShader();
    viewportShader->setMat4("transform", projection * view);
    viewportMesh->DrawMesh(true, false);
}
void ViewportWindow::CreateMesh()
{
    // Create Mesh
    // Mesh vertices & indices
    std::vector<Vertex> vertices =
    {
        // Position                            Color                     TexCoord (not used)
        Vertex({glm::vec3(0, 0, 0)  * 1000.0f,  colors::red.rgb(),        glm::vec2(0) }),
        Vertex({glm::vec3(1, 0, 0)  * 1000.0f,  colors::red.rgb(),        glm::vec2(0) }),  //  X : 1
        Vertex({glm::vec3(0, 0, 0)  * 1000.0f,  colors::darkRed.rgb(),    glm::vec2(0) }),
        Vertex({glm::vec3(-1, 0, 0) * 1000.0f,  colors::darkRed.rgb(),    glm::vec2(0) }),  // -X : 3
        Vertex({glm::vec3(0, 0, 0)  * 1000.0f,  colors::green.rgb(),      glm::vec2(0) }),
        Vertex({glm::vec3(0, 1, 0)  * 1000.0f,  colors::green.rgb(),      glm::vec2(0) }),  //  Y : 5
        Vertex({glm::vec3(0, 0, 0)  * 1000.0f,  colors::darkGreen.rgb(),  glm::vec2(0) }),
        Vertex({glm::vec3(0, -1, 0) * 1000.0f,  colors::darkGreen.rgb(),  glm::vec2(0) }),  // -Y : 7
        Vertex({glm::vec3(0, 0, 0)  * 1000.0f,  colors::blue.rgb(),       glm::vec2(0) }),
        Vertex({glm::vec3(0, 0, 1)  * 1000.0f,  colors::blue.rgb(),       glm::vec2(0) }),  //  Z : 9
        Vertex({glm::vec3(0, 0, 0)  * 1000.0f,  colors::darkBlue.rgb(),   glm::vec2(0) }),
        Vertex({glm::vec3(0, 0, -1) * 1000.0f,  colors::darkBlue.rgb(),   glm::vec2(0) }),  // -Z : 11
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
    viewportMesh = new Mesh(vertices, indices);

    // Creating Shaders
    const char* viewportVertexShader = R"(
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

    const char* viewportFragmentShader = R"(
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
    viewportShader = new shdr::Shader(viewportVertexShader, viewportFragmentShader, 1);
}

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

    // Color Wheel (IMPLEMENT AT SOME POINT)

    // Brightness Slider (IMPLEMENT AT SOME POINT)

    // Adding Mode Dropdown
    colorAttribute->addDropdown("Mode", &currentColorMode, colorMode);

    // Adding Color Values (R, G, B, A)
    colorAttribute->addFloatSlider("R", &(colorData.x), 1.0f, 0.0f, 1.0f);
    colorAttribute->addFloatSlider("G", &(colorData.y), 1.0f, 0.0f, 1.0f);
    colorAttribute->addFloatSlider("B", &(colorData.z), 1.0f, 0.0f, 1.0f);
    colorAttribute->addFloatSlider("A", &(colorData.w), 1.0f, 0.0f, 1.0f);

    // Adding Hex Entry
    colorAttribute->addTextEntry("Hex", &hexCode);
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