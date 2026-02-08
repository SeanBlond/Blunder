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
void AttributeWindow::DrawWindow()
{
    // Adding Base Quad
    renderer.addQuad(position.getCorners(), 0.0f, colors::grey.rgb());

    // Setting initial yPos to Start rendering at
    float attributeTitleHeight = position.getWidth() * 0.12f;
    float attributeYPos = position.getHeight() - (attributeTitleHeight * 0.5f + position.getBuffer());

    // Adding Each Attribute
    for (int i = 0; i < attributes.size(); i++)
    {
        // Adding Label Box
        float attributeBoxWidth = position.getWidth() - 2.0f * position.getBuffer();
        renderer.addQuad(glm::vec3((position.getWidth() / 2), attributeYPos, 0.1f), glm::vec2(attributeBoxWidth, attributeTitleHeight), glm::vec3(0.51f));

        // Adding Attribute Label
        renderer.addText(attributes[i]->getName(), glm::vec3((position.getWidth() / 2), attributeYPos, 0), largText(), glm::vec3(1.0f), CENTER);


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
                element->RenderElement(&renderer, elementPos, mediumText());

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
            renderer.addQuad(containerCorners, 0.15f, glm::vec3(0.35f));

            // Adding space after containter
            attributeYPos -= position.getBuffer();
        }

        // Creating Space for Next Attribute
        attributeYPos -= position.getBuffer();
    }

    // Rendering the Quads
    renderer.renderQuads(getProjection());

    // Rendering the text
    renderer.renderText(getProjection());
}
void AttributeWindow::ManageUIInteraction(GLFWwindow* window, StateMachine* state)
{
    // Checking if StateMachine selected object differs from attribute object, and if it does, changes it
    if (attributeObject != state->getSelectedObject())
    {
        CreateUIfromObject(state->getSelectedObject());
    }

    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    xpos /= position.getWidth();
    ypos /= position.getWidth();
    
    // Checking if Mouse Position is within the UI bounds
    if (xpos > 0 && xpos < 1 && ypos > 0 && ypos < (position.getHeight() / position.getWidth()))
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
    tempCorners = glm::vec4(
        (0.08f * indent) + 0.14f,
        yPos - 0.04f,
        0.8f,
        yPos + 0.04f
    );
    ui::AttributeInteractable folderName(tempCorners, folder->getHierarchyTextUI());
    std::cout << "Interactable generated at " << smath::outputVec4(tempCorners) << std::endl;
    interactables.push_back(folderName);

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
    tempCorners = glm::vec4(
        (0.08f * indent) + 0.16f,
        yPos - 0.04f,
        0.8f,
        yPos + 0.04f
    );
    ui::AttributeInteractable elementName(tempCorners, element->getHierarchyTextUI());
    interactables.push_back(elementName);

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
    float yPos = 0.2f;
    generateFolderInteractable(objectSystem->getRootFolder(), 0, yPos);
}
void HierarchyWindow::DrawUIFolder(Folder* folder, int indent, float& yPos) 
{
    // Rendering the base folder UI
    renderer.addQuad(glm::vec3((position.getWidth() / 2), yPos, 0.1f), glm::vec2(0.96f * position.getWidth(), 0.08f * position.getWidth()), colors::lightgrey.rgb());

    // Dropdown Symbol
    if (folder->hasChildren())
        renderer.addQuad(glm::vec3((position.getWidth() * 0.06f) + (position.getWidth() * 0.08f * indent), yPos, 0.15f), glm::vec2(0.08f * position.getWidth()), colors::white.rgb(), (folder->getDropdown() ? ui::UI_DROPDOWN_T : ui::UI_DROPDOWN_F));

    // Folder Symbol
    renderer.addQuad(glm::vec3((position.getWidth() * 0.14f) + (position.getWidth() * 0.08f * indent), yPos, 0.15f), glm::vec2(0.08f * position.getWidth()), colors::white.rgb(), ui::UI_FOLDER_SYMBOL);

    // Folder Text
    ui::ElementPosition textPos(glm::vec4(
        position.getBuffer() + (0.16f + (0.08f * indent)) * position.getWidth(),
        yPos - (0.04f * position.getWidth()),
        position.getWidth() - (position.getBuffer() + (0.16f * position.getWidth())),
        yPos + (0.04f * position.getWidth())
        ), 0.0f, &position);
    folder->getHierarchyTextUI()->RenderElement(&renderer, textPos, mediumText());

    // Visibility Symbol
    renderer.addQuad(glm::vec3((position.getWidth() * 0.86f), yPos, 0.15f), glm::vec2(0.08f * position.getWidth()), colors::white.rgb(), (folder->getDisplayed() ? ui::UI_DISPLAY_T : ui::UI_DISPLAY_F));

    // Render Symbol
    renderer.addQuad(glm::vec3((position.getWidth() * 0.94f), yPos, 0.15f), glm::vec2(0.08f * position.getWidth()), colors::white.rgb(), (folder->getRendered() ? ui::UI_RENDER_T : ui::UI_RENDER_F));

    // Changing yPos
    yPos -= (position.getWidth() * 0.08f);

    // Checking if children should be displayed
    if (folder->getDropdown())
    {
        // Rendering each folder UI
        for (int i = 0; i < folder->getChildFoldersSize(); i++)
        {
            DrawUIFolder(folder->getChildFolder(i), indent + 1, yPos);
        }

        // Rendering each element UI
        for (int i = 0; i < folder->getHierarchyElementSize(); i++)
        {
            DrawUIHierarchyElement(folder->getHierarchyElement(i), indent + 1, yPos);
        }
    }
}
void HierarchyWindow::DrawUIHierarchyElement(HierarchyElement* element, int indent, float& yPos)
{
    // Rendering the base element UI
    Color baseColor = (objectSystem->getSelectedObject() == element->getObject() ? colors::grey.rgb() : colors::lightgrey.rgb());
    renderer.addQuad(glm::vec3((position.getWidth() / 2), yPos, 0.1f), glm::vec2(0.96f * position.getWidth(), 0.08f * position.getWidth()), baseColor.rgb());
    
    // Dropdown Symbol
    if (element->hasChildren())
        renderer.addQuad(glm::vec3((position.getWidth() * 0.06f) + (position.getWidth() * 0.08f * indent), yPos, 0.15f), glm::vec2(0.08f * position.getWidth()), colors::white.rgb(), (element->getDropdown() ? ui::UI_DROPDOWN_T : ui::UI_DROPDOWN_F));
    
    // Object Symbol
    renderer.addQuad(glm::vec3((position.getWidth() * 0.14f) + (position.getWidth() * 0.08f * indent), yPos, 0.15f), glm::vec2(0.08f * position.getWidth()), colors::white.rgb(), ui::UI_OBJECT_SYMBOL);
    
    // Object Text
    ui::ElementPosition textPos(glm::vec4(
        position.getBuffer() + (0.16f + (0.08f * indent)) * position.getWidth(),
        yPos - (0.04f * position.getWidth()),
        position.getWidth() - (position.getBuffer() + (0.16f * position.getWidth())),
        yPos + (0.04f * position.getWidth())
    ), 0.0f, &position);
    element->getHierarchyTextUI()->RenderElement(&renderer, textPos, mediumText());
    
    // Visibility Symbol
    renderer.addQuad(glm::vec3((position.getWidth() * 0.86f), yPos, 0.15f), glm::vec2(0.08f * position.getWidth()), colors::white.rgb(), (element->getDisplayed() ? ui::UI_DISPLAY_T : ui::UI_DISPLAY_F));
    
    // Render Symbol
    renderer.addQuad(glm::vec3((position.getWidth() * 0.94f), yPos, 0.15f), glm::vec2(0.08f * position.getWidth()), colors::white.rgb(), (element->getRendered() ? ui::UI_RENDER_T : ui::UI_RENDER_F));

    // Changing yPos
    yPos -= (position.getWidth() * 0.08f);

    // Checking if children should be displayed
    if (element->getDropdown())
    {
        // Rendering each child of the element
        for (int i = 0; i < element->getChildrenSize(); i++)
        {
            DrawUIHierarchyElement(element->getChild(i), indent + 1, yPos);
        }
    }
}
void HierarchyWindow::DrawWindow()
{
    // Adding Base Quads
    renderer.addQuad(position.getCorners(), 0.0f, colors::grey.rgb());
    renderer.addQuad(position.getBufferedCorners(), 0.01f, colors::darkerGrey.rgb());

    float hierarchyTitleHeight = 0.12f * position.getWidth();
    float yPos = position.getHeight() - (hierarchyTitleHeight * 0.5f) - position.getBuffer();

    // Adding Hierarchy Label
    renderer.addQuad(glm::vec3((position.getWidth() / 2), yPos, 0.1f), glm::vec2(position.getWidth() - 2 * position.getBuffer(), hierarchyTitleHeight), glm::vec3(0.51f));
    renderer.addText("Hierarchy", glm::vec3(0.5f * position.getWidth(), yPos, 0.15f), largText(), colors::white.rgb(), CENTER);

    // Updating yPos
    yPos -= (hierarchyTitleHeight * 0.5f) + position.getWidth() * 0.04f;

    // Starting the UI Draw from the root folder
    DrawUIFolder(objectSystem->getRootFolder(), 0, yPos);

    // Rendering the quads
    renderer.renderQuads(getProjection());

    // Rendering the text
    renderer.renderText(getProjection());
}
void HierarchyWindow::ManageUIInteraction(GLFWwindow* window, StateMachine* state)
{
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos); 

    // Converting cursor position to be relative to window size
    xpos = (xpos - position.getXOffset()) / position.getWidth();
    ypos = (ypos - position.getYOffset()) / position.getWidth();

    // Checking if Mouse Position is within the UI bounds
    if (xpos > 0 && xpos < 1 && ypos > 0 && ypos < (position.getHeight() / position.getWidth()))
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
void ViewportWindow::DrawWindow()
{
    // Setting ViewNav draw values
    glm::mat4 transform = smath::orthographic(-2, 2, -2, 2, 0.1f, 100.0f) * activeCamera->getViewMatrix() * smath::scale(glm::vec3(1.5f));

    // Drawing the ViewNav
    viewNavElement.setTransform(transform);
    glm::vec4 navCorners = glm::vec4(
        position.getWidth() - viewNavElement.getNavSize() + position.getXOffset(),
        position.getHeight() - viewNavElement.getNavSize() + position.getYOffset(),
        position.getWidth() + position.getXOffset(),
        position.getHeight() + position.getYOffset()
    );
    ui::ElementPosition elementPos(navCorners, position.getXOffset(), &position);
    viewNavElement.RenderElement(&renderer, elementPos, smallText());

    // Setting viewport size
    glm::vec4 viewportSize = glm::vec4(
        position.getXOffset(),
        position.getYOffset(),
        position.getWidth(),
        position.getHeight()
    );
    glViewport(viewportSize.x, viewportSize.y, viewportSize.z, viewportSize.w);

    // Rendering Quads
    renderer.renderQuads(getProjection());
    renderer.renderText(getProjection());
}
void ViewportWindow::ManageUIInteraction(GLFWwindow* window, StateMachine* state)
{
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    xpos = (xpos - position.getXOffset()) / position.getWidth();
    ypos = (ypos - position.getYOffset()) / position.getWidth();

    // Checking if Mouse Position is within the UI bounds
    if (xpos > 0 && xpos < 1 && ypos > 0 && ypos < (position.getHeight() / position.getWidth()))
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