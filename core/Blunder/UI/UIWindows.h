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
        UIWindow(float width, float height, float xoffset, float yoffset, std::string name, float bufferSize = 0.25f, float unitScale = 20.0f) : position(width, height, xoffset, yoffset, bufferSize, unitScale), windowName(name) {}
        UIWindow(ui::WindowPosition position) : position(position) {}

        // Getters
        float getWidth() const { return position.getWidth(); }
        float getHeight() const { return position.getHeight(); }
        ui::WindowPosition getPosition() const { return position; }
        ui::WindowPosition* getPositionAddress() { return &position; }
        glm::mat4 getProjection() { return smath::orthographic(0.0f, position.getWidth(), 0.0f, position.getHeight()); }
        std::string getWindowName() const { return windowName; }

        // Setters
        void setWidth(float width) { this->position.dimensions.x = width; }
        void setHeight(float height) { this->position.dimensions.y = height; }
        void setDimensions(float width, float height) { position.setDimensions(glm::vec2(width, height)); }
        void setDimensions(glm::vec2 dimensions) { position.setDimensions(dimensions); }
        void setOffset(float xOffset, float yOffset) { position.setOffset(glm::vec2(xOffset, yOffset)); }
        void setOffset(glm::vec2 offset) { position.setOffset(offset); }
        void setPosition(float width, float height, float xoffset, float yoffset, float bufferSize) { position.setPosition(width, height, xoffset, yoffset, bufferSize); }
        void setPosition(glm::vec2 dimensions, glm::vec2 offset, float bufferSize) { position.setPosition(dimensions, offset, bufferSize); }
        void setPosition(float width, float height, float xoffset, float yoffset) { position.setPosition(width, height, xoffset, yoffset); }
        void setPosition(glm::vec2 dimensions, glm::vec2 offset) { position.setPosition(dimensions, offset); }

        // Functions
        //virtual void UpdateWindow() = 0;
        virtual void ResizeWindow() = 0;
        virtual void DrawWindow(ui::UIRenderer* renderer) = 0;
        virtual void ManageInteraction(GLFWwindow* window, StateMachine* state) = 0;
        virtual void UnselectWindow() = 0;
        virtual void OpenWindow() = 0;


        // Text Sizes
        float smallText() const { return (6.0f / 64.0f) * 2.5f; }
        float mediumText() const { return (7.0f / 64.0f) * 2.5f; }
        float largeText() const { return (8.0f / 64.0f) * 2.5f; }

    protected:
        ui::WindowPosition position;
        std::string windowName;
    };

    // Attribute Window
    class AttributeWindow : public UIWindow
    {
    public:
        // Constructor & Desconstructor
        AttributeWindow(float width, float height, float xoffset, float yoffset, obj::Object* attributeObject = nullptr) : UIWindow(width, height, xoffset, yoffset, "Attribute"), attributeObject(attributeObject), clickedElement(nullptr), highlightedElement(nullptr) {}
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
        void ResizeWindow() override;
        void DrawWindow(ui::UIRenderer* renderer) override;
        void ManageInteraction(GLFWwindow* window, StateMachine* state) override;
        void UnselectWindow() override;
        void OpenWindow() override { CreateUIfromObject(attributeObject); }
        void ClearAttributes();
        void CreateUIfromObject(obj::Object* object);
        bool checkElementInteraction(GLFWwindow* window, glm::vec2 mousePos, ui::AttributeElement* element, StateMachine* state);

    private:
        obj::Object* attributeObject;
        std::vector<ui::Attribute*> attributes;
        ui::AttributeElement* clickedElement;
        ui::AttributeElement* highlightedElement;
    };

    // Hierarchy Window
    class HierarchyWindow : public UIWindow
    {
    public:
        // Constructor & Deconstructor
        HierarchyWindow(float width, float height, float xoffset, float yoffset, StateMachine* state) : UIWindow(width, height, xoffset, yoffset, "Hierarchy"), state(state), clickedElement(nullptr), rootAttribute(nullptr) {}
        ~HierarchyWindow()
        {
        }

        // Functions
        void CreateHierarchyElementsFromRoot(Folder* root);
        void CreateHierarchyElementsFromFolder(Folder* folder, ui::HierarchyAttribute* folderAttribute);
        void ResizeWindow() override {}
        void DrawUIFolder(ui::UIRenderer* renderer, Folder* folder, int indent, float& yPos);
        void DrawUIHierarchyElement(ui::UIRenderer* renderer, HierarchyElement* element, int indent, float& yPos);
        void DrawWindow(ui::UIRenderer* renderer) override;
        void ManageInteraction(GLFWwindow* window, StateMachine* state) override;
        void UnselectWindow() override;
        void OpenWindow() override {}

    private:
        StateMachine* state;
        ui::HierarchyAttribute* rootAttribute;
        ui::AttributeElement* clickedElement;
    };

    // Viewport Window
    class ViewportWindow : public UIWindow
    {
    public:
        // Constructor & Deconstructor
        ViewportWindow(float width, float height, float xoffset, float yoffset, StateMachine* state)
            : UIWindow(width, height, xoffset, yoffset, "Viewport"), state(state), viewNavElement("ViewNav", 150, 0.01f), clickedElement(nullptr), highlightedElement(nullptr) { CreateMesh(); }
        ~ViewportWindow()
        {
            delete viewportMesh;
            delete viewportShader;
            viewportMesh = nullptr;
            viewportShader = nullptr;
        }

        // Functions
        void ResizeWindow() override;
        void DrawWindow(ui::UIRenderer* renderer) override;
        void ManageInteraction(GLFWwindow* window, StateMachine* state) override;
        void UnselectWindow() override;
        void OpenWindow() override { ResizeWindow(); }
        void RenderScene();
        void CreateMesh();

    private:
        StateMachine* state;
        ui::ViewNav viewNavElement;
        std::vector<ui::Attribute*> attributes;
        ui::AttributeElement* clickedElement;
        ui::AttributeElement* highlightedElement;

        // Mesh Rendering stuff
        Mesh* viewportMesh;
        shdr::Shader* viewportShader;
    };

    // Color Attribute Window
    class ColorWindow : public UIWindow
    {
    public:
        // Constructor & Deconstructor
        ColorWindow(float width, float height, float xoffset, float yoffset, Color* selectedColor) : UIWindow(width, height, xoffset, yoffset, "Color", 0.125), colorAttribute(nullptr), interactingColor(selectedColor), colorData(selectedColor->hsva()), clickedElement(nullptr), highlightedElement(nullptr)
        { hexCode = storedHexCode = Color::RGBAtoHEX(selectedColor->rgba()); CreateUIFromSelected(); }
        ~ColorWindow()
        {
            ClearAttributes();
        }

        // Functions
        void CreateUIFromSelected();
        void ClearAttributes();
        void ResizeWindow() override;
        void DrawWindow(ui::UIRenderer* renderer) override;
        void ManageInteraction(GLFWwindow* window, StateMachine* state) override;
        void UnselectWindow() override;
        void OpenWindow() override { CreateUIFromSelected(); ResizeWindow(); }

    private:
        InteractingColor interactingColor;
        std::vector<std::string> colorMode = { "RGBA", "HSVA" };
        int currentColorMode = 1;
        glm::vec4 colorData;
        std::string hexCode;
        std::string storedHexCode;
        ui::Attribute* colorAttribute;
        ui::AttributeElement* clickedElement;
        ui::AttributeElement* highlightedElement;
    };
}
#endif // !UIWINDOWS