#ifndef UIWINDOWS
#pragma once

#include <iostream>
#include <vector>

#include "../../ew/external/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "../../line/line.h"
#include "../Runtime/StateMachine.h"
#include "../Scene/Scene.h"
#include "UIElements.h"

namespace ui
{
    // Window Parent Class
    class UIWindow
    {
    public:
        // Constructor
        UIWindow(float width, float height, float xoffset, float yoffset) : position(width, height, xoffset, yoffset, 0.02f) {}
        UIWindow(ui::WindowPosition position) : position(position) {}

        // Getters
        float getWidth() const { return position.getWidth(); }
        float getHeight() const { return position.getHeight(); }
        ui::WindowPosition getPosition() const { return position; }
        ui::WindowPosition* getPositionAddress() { return &position; }
        glm::mat4 getProjection() { return smath::orthographic(0.0f, position.getWidth(), 0.0f, position.getHeight()); }

        // Setters
        void setWidth(float width) { this->position.dimensions.x = width; }
        void setHeight(float height) { this->position.dimensions.y = height; }
        void setDimensions(float width, float height, float xoffset, float yoffset, float bufferSize) { position.setPosition(width, height, xoffset, yoffset, bufferSize); }
        void setDimensions(glm::vec2 dimensions, glm::vec2 offset, float bufferSize) { position.setPosition(dimensions, offset, bufferSize); }
        void setDimensions(float width, float height, float xoffset, float yoffset) { position.setPosition(width, height, xoffset, yoffset); }
        void setDimensions(glm::vec2 dimensions, glm::vec2 offset) { position.setPosition(dimensions, offset); }

        // Functions
        virtual void GenerateInteractables() = 0;
        virtual void DrawWindow(ui::UIRenderer* renderer) = 0;
        virtual void ManageInteraction(GLFWwindow* window, StateMachine* state) = 0;
        virtual void UnselectWindow() = 0;


        // Text Sizes
        float smallText() const { return (position.dimensions.y * 2.2e-4); }
        float mediumText() const { return (position.dimensions.y * 2.75e-4); }
        float largeText() const { return (position.dimensions.y * 4e-4); }

    protected:
        ui::WindowPosition position;
    };

    // Attribute Window
    class AttributeWindow : public UIWindow
    {
    public:
        // Constructor & Desconstructor
        AttributeWindow(float width, float height, float xoffset, float yoffset, obj::Object* attributeObject = nullptr) : UIWindow(width, height, xoffset, yoffset), attributeObject(attributeObject), clickedElement(nullptr) { CreateUIfromObject(attributeObject); }
        ~AttributeWindow()
        {
            ClearAttributes();
        }

        // Getters
        std::vector<ui::Attribute*> getAttribute() { return attributes; }
        ui::Attribute* getAttribute(int index) { return attributes[index]; }

        // Setters
        void addAttribute(ui::Attribute* attribute) { attributes.push_back(attribute); }

        // Functions
        // Once UI Quads are better optimized, GenerateInteractables should be reworked
        void GenerateInteractables() override;
        void DrawWindow(ui::UIRenderer* renderer) override;
        void ManageInteraction(GLFWwindow* window, StateMachine* state) override;
        void UnselectWindow() override;
        void ClearAttributes();
        void CreateUIfromObject(obj::Object* object);

    private:
        obj::Object* attributeObject;
        std::vector<ui::Attribute*> attributes;
        std::vector<ui::AttributeInteractable> interactables;
        ui::AttributeElement* clickedElement;


        // REMOVE AT SOME POINT
        int testDropdownValue;
    };

    // Hierarchy Window
    class HierarchyWindow : public UIWindow
    {
    public:
        // Constructor & Deconstructor
        HierarchyWindow(float width, float height, float xoffset, float yoffset, StateMachine* state) : UIWindow(width, height, xoffset, yoffset), state(state), clickedElement(nullptr) {}
        ~HierarchyWindow()
        {
            interactables.clear();
        }

        // Functions
        void generateFolderInteractable(Folder* folder, int indent, float& yPos);
        void generateElementInteractable(HierarchyElement* element, int indent, float& yPos);
        void GenerateInteractables() override;
        void DrawUIFolder(ui::UIRenderer* renderer, Folder* folder, int indent, float& yPos);
        void DrawUIHierarchyElement(ui::UIRenderer* renderer, HierarchyElement* element, int indent, float& yPos);
        void DrawWindow(ui::UIRenderer* renderer) override;
        void ManageInteraction(GLFWwindow* window, StateMachine* state) override;
        void UnselectWindow() override;

    private:
        std::vector<ui::AttributeInteractable> interactables;
        StateMachine* state;
        ui::AttributeElement* clickedElement;
    };

    // Viewport Window
    class ViewportWindow : public UIWindow
    {
    public:
        // Constructor & Deconstructor
        ViewportWindow(float width, float height, float xoffset, float yoffset, StateMachine* state)
            : UIWindow(width, height, xoffset, yoffset), state(state), viewNavElement("ViewNav", 150, 0.01f), clickedElement(nullptr) { CreateMesh(); }
        ~ViewportWindow()
        {
            interactables.clear();

            delete viewportMesh;
            delete viewportShader;
            viewportMesh = nullptr;
            viewportShader = nullptr;
        }

        // Functions
        void GenerateInteractables() override;
        void DrawWindow(ui::UIRenderer* renderer) override;
        void ManageInteraction(GLFWwindow* window, StateMachine* state) override;
        void UnselectWindow() override;
        void RenderScene();
        void CreateMesh();

    private:
        StateMachine* state;
        ui::ViewNav viewNavElement;
        std::vector<ui::Attribute*> attributes;
        std::vector<ui::AttributeInteractable> interactables;
        ui::AttributeElement* clickedElement;

        // Mesh Rendering stuff
        Mesh* viewportMesh;
        shdr::Shader* viewportShader;
    };

    // Color Attribute Window
    class ColorWindow : public UIWindow
    {
    public:
        // Constructor & Deconstructor
        ColorWindow(float width, float height, float xoffset, float yoffset, Color* selectedColor) : UIWindow(width, height, xoffset, yoffset), colorAttribute(nullptr), selectedColor(selectedColor), colorData(selectedColor->rgba())
        { hexCode = Color::RGBAtoHEX(colorData); CreateUIFromSelected(); }
        ~ColorWindow()
        {
            ClearAttributes();
        }

        // Functions
        void CreateUIFromSelected();
        void ClearAttributes();
        void GenerateInteractables() override;
        void DrawWindow(ui::UIRenderer* renderer) override;
        void ManageInteraction(GLFWwindow* window, StateMachine* state) override;
        void UnselectWindow() override;

    private:
        Color* selectedColor;
        std::vector<std::string> colorMode = { "RGBA", "HSVA" };
        std::string hexCode;
        int currentColorMode = 0;
        glm::vec4 colorData;
        ui::Attribute* colorAttribute;
        ui::AttributeElement* clickedElement;
        std::vector<ui::AttributeInteractable> interactables;
    };
}
#endif // !UIWINDOWS