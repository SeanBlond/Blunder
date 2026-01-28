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

    // Drawing the time
    std::cout << TimeManager::getInstance()->getFPS() << std::endl;
    std::string fpsTime = "FPS: " + std::to_string(TimeManager::getInstance()->getFPS());
    renderer.renderText(fpsTime, (width * 0.1f), attributeYPos, mediumText(), colors::white.rgb());
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
    addAttribute(scaleAtrribute);

    // Generating the Interactables
    GenerateInteractables();
}

// Hierarchy Window Functions
void HierarchyWindow::generateFolderInteractable(Folder* folder, int indent, float& yPos)
{
    // Dropdown Button
    glm::vec4 dropdownCorners = glm::vec4(
        (0.08f * indent) + 0.04f,
        yPos - 0.04f,
        (0.08f * indent) + 0.08f,
        yPos + 0.04f
    );
    ui::AttributeInteractable folderDropdown(dropdownCorners, new ui::Toggle("folder-dropdown", folder->getDropdownAddress(), ui::UI_DROPDOWN));
    interactables.push_back(folderDropdown);

    // Eventually add Text Entry for the name

    // Visibility Button
    dropdownCorners = glm::vec4(
        0.8f,
        yPos - 0.04f,
        0.88f,
        yPos + 0.04f
    );
    ui::AttributeInteractable folderVisibility(dropdownCorners, new ui::Toggle("folder-dropdown", folder->getDisplayedAddress()));
    interactables.push_back(folderVisibility);

    // Render Button
    dropdownCorners = glm::vec4(
        0.88f,
        yPos - 0.04f,
        0.96f,
        yPos + 0.04f
    );
    ui::AttributeInteractable folderRender(dropdownCorners, new ui::Toggle("folder-dropdown", folder->getRenderedAddress()));
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
    glm::vec4 dropdownCorners = glm::vec4(
        (0.08f * indent) + 0.04f,
        yPos - 0.04f,
        (0.08f * indent) + 0.08f,
        yPos + 0.04f
    );
    std::cout << "Interactable Generated at (" << dropdownCorners.x << ", " << dropdownCorners.y << ", " << dropdownCorners.z << ", " << dropdownCorners.w << ")\n";
    ui::AttributeInteractable elementDropdown(dropdownCorners, new ui::Toggle("folder-dropdown", element->getDropdownAddress(), ui::UI_DROPDOWN));
    interactables.push_back(elementDropdown);

    // Eventually add Text Entry for the name

    // Visibility Button
    dropdownCorners = glm::vec4(
        0.8f,
        yPos - 0.04f,
        0.88f,
        yPos + 0.04f
    );
    std::cout << "Interactable Generated at (" << dropdownCorners.x << ", " << dropdownCorners.y << ", " << dropdownCorners.z << ", " << dropdownCorners.w << ")\n";
    ui::AttributeInteractable elementVisibility(dropdownCorners, new ui::Toggle("element-dropdown", element->getDisplayedAddress()));
    interactables.push_back(elementVisibility);

    // Render Button
    dropdownCorners = glm::vec4(
        0.88f,
        yPos - 0.04f,
        0.96f,
        yPos + 0.04f
    );
    std::cout << "Interactable Generated at (" << dropdownCorners.x << ", " << dropdownCorners.y << ", " << dropdownCorners.z << ", " << dropdownCorners.w << ")\n";
    ui::AttributeInteractable elementRender(dropdownCorners, new ui::Toggle("element-dropdown", element->getRenderedAddress()));
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
    renderer.renderQuad(glm::vec3((width / 2), yPos, 0.2f), glm::vec2(0.92f * width, 0.08f * width), colors::lightgrey.rgb());

    // Dropdown Symbol
    if (folder->hasChildren())
        renderer.renderTextureQuad(glm::vec3((width * 0.08f) + (width * 0.08f * indent), yPos, 0.25f), glm::vec2(0.08f * width), (folder->getDropdown() ? ui::UI_DROPDOWN_T : ui::UI_DROPDOWN_F));

    // Folder Symbol
    renderer.renderTextureQuad(glm::vec3((width * 0.16f) + (width * 0.08f * indent), yPos, 0.25f), glm::vec2(0.08f * width), ui::UI_FOLDER_SYMBOL);

    // Folder Text
    renderer.renderText(folder->getName(), (width * 0.2f) + (width * 0.08f * indent), yPos - (width * 0.03f), mediumText(), colors::white.rgb(), LEFT);

    // Visibility Symbol
    renderer.renderTextureQuad(glm::vec3((width * 0.84f), yPos, 0.25f), glm::vec2(0.08f * width), (folder->getDisplayed() ? ui::UI_DISPLAY_T : ui::UI_DISPLAY_F));

    // Render Symbol
    renderer.renderTextureQuad(glm::vec3((width * 0.92f), yPos, 0.25f), glm::vec2(0.08f * width), (folder->getRendered() ? ui::UI_RENDER_T : ui::UI_RENDER_F));

    // Changing yPos
    yPos -= (width * 0.08f);

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
    Color baseColor = (objectSystem->getSelectedElement() == element ? colors::grey.rgb() : colors::lightgrey.rgb());
    renderer.renderQuad(glm::vec3((width / 2), yPos, 0.2f), glm::vec2(0.92f * width, 0.08f * width), baseColor.rgb());
    
    // Dropdown Symbol
    if (element->hasChildren())
        renderer.renderTextureQuad(glm::vec3((width * 0.08f) + (width * 0.08f * indent), yPos, 0.25f), glm::vec2(0.08f * width), (element->getDropdown() ? ui::UI_DROPDOWN_T : ui::UI_DROPDOWN_F));
    
    // Object Symbol
    renderer.renderTextureQuad(glm::vec3((width * 0.16f) + (width * 0.08f * indent), yPos, 0.25f), glm::vec2(0.08f * width), ui::UI_OBJECT_SYMBOL);
    
    // Object Text
    renderer.renderText(element->getName(), (width * 0.2f) + (width * 0.08f * indent), yPos - (width * 0.03f), mediumText(), colors::white.rgb(), LEFT);
    
    // Visibility Symbol
    renderer.renderTextureQuad(glm::vec3((width * 0.84f), yPos, 0.25f), glm::vec2(0.08f * width), (element->getDisplayed() ? ui::UI_DISPLAY_T : ui::UI_DISPLAY_F));
    
    // Render Symbol
    renderer.renderTextureQuad(glm::vec3((width * 0.92f), yPos, 0.25f), glm::vec2(0.08f * width), (element->getRendered() ? ui::UI_RENDER_T : ui::UI_RENDER_F));

    // Changing yPos
    yPos -= (width * 0.08f);

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
void HierarchyWindow::DrawAttributeWindow()
{
    // Rendering Base Quads
    renderer.renderQuad(glm::vec3(width / 2.0f, height / 2.0f, 0.0f), glm::vec2(width, height), colors::grey.rgb());
    renderer.renderQuad(glm::vec3(width / 2.0f, height / 2.0f, 0.01f), glm::vec2(width*0.92f, height - (width * 0.08f)), colors::darkerGrey.rgb());

    float yPos = height - (width * 0.1f);

    // Rendering Hierarchy Label
    renderer.renderQuad(glm::vec3((width / 2), yPos, 0.1f), glm::vec2(0.92f * width, 0.12f * width), glm::vec3(0.51f));
    renderer.renderText("Hierarchy", (width / 2), yPos - (width * 0.035f), largText(), glm::vec3(1.0f), CENTER);

    // Updating yPos
    yPos -= (width * 0.1f);

    // Starting the UI Draw from the root folder
    DrawUIFolder(objectSystem->getRootFolder(), 0, yPos);
}
void HierarchyWindow::ManageUIInteraction(GLFWwindow* window, StateMachine* state)
{
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    xpos = (xpos - xoffset) / width;
    ypos = (ypos - yoffset) / width;

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