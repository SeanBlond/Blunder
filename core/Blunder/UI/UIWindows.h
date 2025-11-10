#ifndef UIWINDOWS
#pragma once

#include <iostream>
#include <vector>

#include "../../ew/external/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "../StateMachine/StateMachine.h"
#include "../ObjectRender/ObjectRender.h"
#include "UIElements.h"

// Window Parent Class
class UIWindow
{
public:
    // Constructor
    UIWindow(float width, float height, std::string fontName, int fontSize) : width(width), height(height), renderer(fontName, fontSize, smath::orthographic(0.0f, width, 0.0f, height)) {}

    // Getters
    float getWidth() { return width; }
    float getHeight() { return height; }

    // Setters
    void setWidth(float width) { this->width = width; renderer.setProjection(smath::orthographic(0, width, 0, height)); }
    void setHeight(float height) { this->height = height; renderer.setProjection(smath::orthographic(0, width, 0, height)); }
    void setDimensions(float width, float height) { this->width = width; this->height = height; renderer.setProjection(smath::orthographic(0, width, 0, height)); }
    void setDimensions(glm::vec2 dimensions) { this->width = dimensions.x; this->height = dimensions.y; renderer.setProjection(smath::orthographic(0, width, 0, height)); }

    // Functions
    virtual void GenerateInteractables() = 0;
    virtual void DrawAttributeWindow() = 0;
    virtual void ManageUIInteraction(GLFWwindow* window, StateMachine* state) = 0;


    // Text Sizes
    float smallText() { return (height * 2.2e-4); }
    float mediumText() { return (height * 3e-4); }
    float largText() { return (height * 4e-4); }

protected:
    float width;
    float height;
    ui::UIRenderer renderer;
};


// Attribute Window
class AttributeWindow : public UIWindow
{
public:
    // Constructor & Desconstructor
    AttributeWindow(float width, float height, std::string fontName, int fontSize) : UIWindow(width, height, fontName, fontSize), clickedElement(nullptr) {}
    ~AttributeWindow()
    {
        attributes.clear();
        interactables.clear();
    }

    // Getters
    std::vector<ui::Attribute*> getAttribute() { return attributes; }
    ui::Attribute* getAttribute(int index) { return attributes[index]; }

    // Setters
    void addAttribute(ui::Attribute* attribute) { attributes.push_back(attribute); }

    // Functions
    // Once UI Quads are better optimized, GenerateInteractables will should be reworked
    void GenerateInteractables() override;
    void DrawAttributeWindow() override;
    void ManageUIInteraction(GLFWwindow* window, StateMachine* state) override;

private:
    std::vector<ui::Attribute*> attributes;
    std::vector<ui::AttributeInteractable> interactables;
    ui::AttributeElement* clickedElement;
};

// Hierarchy Window
class HierarchyWindow : public UIWindow
{
public:
    // Constructor & Deconstructor
    HierarchyWindow(float width, float height, std::string fontName, int fontSize, obr::ObjectRender* objectSystem) : UIWindow(width, height, fontName, fontSize), objectSystem(objectSystem) {}
    ~HierarchyWindow()
    {
        interactables.clear();
    }

    // Getters
    obr::ObjectRender* getObjectSystem() { return objectSystem; }

    // Setters
    void setObjectSystem(obr::ObjectRender* objectSystem) { this->objectSystem = objectSystem; }

    // Functions
    void GenerateInteractables() override;
    void DrawAttributeWindow() override;
    void ManageUIInteraction(GLFWwindow* window, StateMachine* state) override;

private:
    std::vector<ui::HierarchyInteractable> interactables;
    obr::ObjectRender* objectSystem;
};

#endif // !UIWINDOWS
