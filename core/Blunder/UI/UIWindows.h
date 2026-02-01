#ifndef UIWINDOWS
#pragma once

#include <iostream>
#include <vector>

#include "../../ew/external/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "../StateMachine/StateMachine.h"
#include "../ObjectSystem/ObjectSystem.h"
#include "UIElements.h"

// Window Parent Class
class UIWindow
{
public:
    // Constructor
    UIWindow(float width, float height, float xoffset, float yoffset, std::string fntFilePath, std::string fontBitmapFilePath, std::string uiBitmapFilePath) : width(width), height(height), renderer(fntFilePath, fontBitmapFilePath, uiBitmapFilePath) {}

    // Getters
    float getWidth() { return width; }
    float getHeight() { return height; }
    glm::mat4 getProjection() { return smath::orthographic(0.0f, width, 0.0f, height); }

    // Setters
    void setWidth(float width) { this->width = width; }
    void setHeight(float height) { this->height = height; }
    void setDimensions(float width, float height, float xoffset, float yoffset) { this->width = width; this->height = height; this->xoffset = xoffset; this->yoffset = yoffset; }
    void setDimensions(glm::vec2 dimensions, glm::vec2 offset) { this->width = dimensions.x; this->height = dimensions.y; this->xoffset = offset.x; this->yoffset = offset.y; }

    // Functions
    virtual void GenerateInteractables() = 0;
    virtual void DrawWindow() = 0;
    virtual void ManageUIInteraction(GLFWwindow* window, StateMachine* state) = 0;


    // Text Sizes
    float smallText() const { return (height * 2.2e-4); }
    float mediumText() const { return (height * 2.75e-4); }
    float largText() const { return (height * 4e-4); }

protected:
    float width;
    float height;
    float xoffset;
    float yoffset;
    ui::UIRenderer renderer;
};


// Attribute Window
class AttributeWindow : public UIWindow
{
public:
    // Constructor & Desconstructor
    AttributeWindow(float width, float height, float xoffset, float yoffset, std::string fntFilePath, std::string fontBitmapFilePath, std::string uiBitmapFilePath, obj::Object* attributeObject = nullptr) : UIWindow(width, height, xoffset, yoffset, fntFilePath, fontBitmapFilePath, uiBitmapFilePath), attributeObject(attributeObject), clickedElement(nullptr) { CreateUIfromObject(attributeObject); }
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
    void DrawWindow() override;
    void ManageUIInteraction(GLFWwindow* window, StateMachine* state) override;
    void ClearAttributes();
    void CreateUIfromObject(obj::Object* object);

private:
    obj::Object* attributeObject;
    std::vector<ui::Attribute*> attributes;
    std::vector<ui::AttributeInteractable> interactables;
    ui::AttributeElement* clickedElement;
};

// Hierarchy Window
class HierarchyWindow : public UIWindow
{
public:
    // Constructor & Deconstructor
    HierarchyWindow(float width, float height, float xoffset, float yoffset, std::string fntFilePath, std::string fontBitmapFilePath, std::string uiBitmapFilePath, obs::ObjectSystem* objectSystem) : UIWindow(width, height, xoffset, yoffset, fntFilePath, fontBitmapFilePath, uiBitmapFilePath), objectSystem(objectSystem), clickedElement(nullptr) {}
    ~HierarchyWindow()
    {
        interactables.clear();
    }

    // Getters
    obs::ObjectSystem* getObjectSystem() { return objectSystem; }

    // Setters
    void setObjectSystem(obs::ObjectSystem* objectSystem) { this->objectSystem = objectSystem; }

    // Functions
    void generateFolderInteractable(Folder* folder, int indent, float& yPos);
    void generateElementInteractable(HierarchyElement* element, int indent, float& yPos);
    void GenerateInteractables() override;
    void DrawUIFolder(Folder* folder, int indent, float& yPos);
    void DrawUIHierarchyElement(HierarchyElement* element, int indent, float& yPos);
    void DrawWindow() override;
    void ManageUIInteraction(GLFWwindow* window, StateMachine* state) override;

private:
    std::vector<ui::AttributeInteractable> interactables;
    obs::ObjectSystem* objectSystem;
    ui::AttributeElement* clickedElement;
};

// Viewport Window
class ViewportWindow : public UIWindow
{
public:
    // Constructor & Deconstructor
    ViewportWindow(float width, float height, float xoffset, float yoffset, std::string fntFilePath, std::string fontBitmapFilePath, std::string uiBitmapFilePath, obs::ObjectSystem* objectSystem, OrbitCamera* camera) : UIWindow(width, height, xoffset, yoffset, fntFilePath, fontBitmapFilePath, uiBitmapFilePath), objectSystem(objectSystem), activeCamera(camera), viewNavElement("ViewNav", 0.01f), clickedElement(nullptr) {}
    ~ViewportWindow() 
    {
        interactables.clear();
    }

    // Getters


    // Setters


    // Functions
    void GenerateInteractables() override;
    void DrawWindow() override;
    void ManageUIInteraction(GLFWwindow* window, StateMachine* state) override;

private:
    OrbitCamera* activeCamera;
    obs::ObjectSystem* objectSystem;
    ui::ViewNav viewNavElement;
    std::vector<ui::Attribute*> attributes;
    std::vector<ui::AttributeInteractable> interactables;
    ui::AttributeElement* clickedElement;
};

#endif // !UIWINDOWS
