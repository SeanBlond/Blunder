#include "../UIWindows.h"
using namespace ui;

// Hierarchy Window Functions
void HierarchyWindow::DrawUIFolder(ui::UIRenderer* renderer, Folder* folder, int indent, float& yPos)
{
    // Rendering the base folder UI
    renderer->addQuad(glm::vec3((position.getWidth() / 2), yPos, 0.1f), glm::vec2(position.getWidth() - 2.0f * position.getBuffer(), position.unitScale), colors::lightgrey.rgb(), position.offset);

    // Dropdown Symbol
    if (folder->hasChildren())
        renderer->addQuad(glm::vec3((position.unitScale * 0.5f + position.getBuffer()) + (position.unitScale * indent), yPos, 0.15f), glm::vec2(position.unitScale), colors::white.rgb(), position.offset, (folder->getDropdown() ? ui::UI_DROPDOWN_T : ui::UI_DROPDOWN_F));

    // Folder Symbol
    renderer->addQuad(glm::vec3((position.unitScale * 1.5f + position.getBuffer()) + (position.unitScale * indent), yPos, 0.15f), glm::vec2(position.unitScale), colors::white.rgb(), position.offset, ui::UI_FOLDER_SYMBOL);

    // Folder Text
    //ui::ElementPosition textPos(glm::vec4(
    //    position.getBuffer() + (0.16f + (0.08f * indent)) * position.getWidth(),
    //    yPos - (0.04f * position.getWidth()),
    //    position.getWidth() - (position.getBuffer() + (0.16f * position.getWidth())),
    //    yPos + (0.04f * position.getWidth())
    //), 0.0f, &position);
    //folder->getHierarchyTextUI()->RenderElement(renderer, textPos, mediumText());

    // Visibility Symbol
    renderer->addQuad(glm::vec3(position.getWidth() - (position.unitScale * 1.5f + position.getBuffer()), yPos, 0.15f), glm::vec2(position.unitScale), colors::white.rgb(), position.offset, (folder->getDisplayed() ? ui::UI_DISPLAY_T : ui::UI_DISPLAY_F));

    // Render Symbol
    renderer->addQuad(glm::vec3(position.getWidth() - (position.unitScale * 0.5f + position.getBuffer()), yPos, 0.15f), glm::vec2(position.unitScale), colors::white.rgb(), position.offset, (folder->getRendered() ? ui::UI_RENDER_T : ui::UI_RENDER_F));

    // Changing yPos
    yPos -= (position.unitScale);

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
    renderer->addQuad(glm::vec3((position.getWidth() / 2), yPos, 0.1f), glm::vec2(position.getWidth() - 2.0f * position.getBuffer(), position.unitScale), baseColor.rgb(), position.offset);

    // Dropdown Symbol
    if (element->hasChildren())
        renderer->addQuad(glm::vec3((position.unitScale * 0.5f + position.getBuffer()) + (position.unitScale * indent), yPos, 0.15f), glm::vec2(position.unitScale), colors::white.rgb(), position.offset, (element->getDropdown() ? ui::UI_DROPDOWN_T : ui::UI_DROPDOWN_F));

    // Object Symbol
    renderer->addQuad(glm::vec3((position.unitScale * 1.5f + position.getBuffer()) + (position.unitScale * indent), yPos, 0.15f), glm::vec2(position.unitScale), colors::white.rgb(), position.offset, ui::UI_OBJECT_SYMBOL);

    // Object Text
    //ui::ElementPosition textPos(glm::vec4(
    //    position.getBuffer() + (0.16f + (0.08f * indent)) * position.getWidth(),
    //    yPos - (0.04f * position.getWidth()),
    //    position.getWidth() - (position.getBuffer() + (0.16f * position.getWidth())),
    //    yPos + (0.04f * position.getWidth())
    //), 0.0f, &position);
    //element->getHierarchyTextUI()->RenderElement(renderer, textPos, mediumText());

    // Visibility Symbol
    renderer->addQuad(glm::vec3(position.getWidth() - (position.unitScale * 1.5f + position.getBuffer()), yPos, 0.15f), glm::vec2(position.unitScale), colors::white.rgb(), position.offset, (element->getDisplayed() ? ui::UI_DISPLAY_T : ui::UI_DISPLAY_F));

    // Render Symbol
    renderer->addQuad(glm::vec3(position.getWidth() - (position.unitScale * 0.5f + position.getBuffer()), yPos, 0.15f), glm::vec2(position.unitScale), colors::white.rgb(), position.offset, (element->getRendered() ? ui::UI_RENDER_T : ui::UI_RENDER_F));

    // Changing yPos
    yPos -= (position.unitScale);

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

    float hierarchyTitleHeight = position.unitScale * 1.5f;
    float yPos = position.getHeight() - (hierarchyTitleHeight * 0.5f) - position.getBuffer();

    // Adding Hierarchy Label
    renderer->addQuad(glm::vec3((position.getWidth() / 2), yPos, 0.1f), glm::vec2(position.getWidth() - 2 * position.getBuffer(), hierarchyTitleHeight), glm::vec3(0.51f), position.offset);
    renderer->addText("Hierarchy", glm::vec3(0.5f * position.getWidth(), yPos, 0.15f), largeText(), colors::white.rgb(), position.offset, CENTER);

    // Updating yPos
    yPos -= (hierarchyTitleHeight * 0.5f) + (position.unitScale * 0.5f);

    // Starting the UI Draw from the root folder
    DrawUIFolder(renderer, state->getScene()->getRootFolder(), 0, yPos);
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