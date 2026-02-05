#ifndef UI_ELEMENTS
#pragma once

#include <sstream>
#include <iostream>
#include <functional>
#include <iomanip>
#include <vector>

#include "../../ew/external/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "../StateMachine/StateMachine.h"
#include "../Color.h"
#include "UIRenderer.h"
#include "TextInput.h"
#include "../Time.h"

namespace ui
{
    enum ElementType { UI_TOGGLE, UI_FLOAT_SLIDER, UI_FLOAT_ENTRY, UI_INT_SLIDER, UI_INT_ENTRY, UI_TEXT_ENTRY, UI_DROPDOWN, UI_VIEW_NAV };

    struct WindowPosition;
    struct ElementPosition
    {
        /*
                 THE STRUCTURE
            (lx,ty)----|----(rx,ty)      (x,w)----|----(z,w)
               |       |       |           |      |      |
               |       |       |     OR    |      |      |   
               |       |       |           |      |      |
            (lx,by)----|----(rx,by)      (x,y)----|----(z,y)
                       ^
                     split
        */

        ElementPosition(glm::vec4 corners, float split, WindowPosition* parentWindow)
            : left_x(corners.x), right_x(corners.z), bottom_y(corners.y), top_y(corners.z), split(split), parentWindow(parentWindow) {
        }
        ElementPosition(glm::vec2 bottomCorner, glm::vec2 topCorner, float split, WindowPosition* parentWindow)
            : left_x(bottomCorner.x), right_x(topCorner.x), bottom_y(bottomCorner.y), top_y(topCorner.y), split(split), parentWindow(parentWindow) {
        }
        ElementPosition(float left_x, float right_x, float bottom_y, float top_y, float split, WindowPosition* parentWindow)
            : left_x(left_x), right_x(right_x), bottom_y(bottom_y), top_y(top_y), split(split), parentWindow(parentWindow) {
        }
        ElementPosition()
            : left_x(0), right_x(0), bottom_y(0), top_y(0), split(0), parentWindow(nullptr) {
        }

        // Multi-Getters
        glm::vec4 getCorners() { return glm::vec4(left_x, right_x, bottom_y, top_y); }

        // Multi-Setters
        void setCorners(glm::vec4 corners) {
            left_x = corners.x;
            right_x = corners.z;
            bottom_y = corners.y;
            top_y = corners.z;
        }
        void setCorners(float left_x, float right_x, float bottom_y, float top_y) {
            this->left_x = left_x;
            this->right_x = right_x;
            this->bottom_y = bottom_y;
            this->top_y = top_y;
        }
        void setPositions(glm::vec4 corners, float split)
        {
            setCorners(corners);
            this->split = split;
        }
        void setPositions(float left_x, float right_x, float bottom_y, float top_y, float split)
        {
            setCorners(left_x, right_x, bottom_y, top_y);
            this->split = split;
        }

        float left_x, right_x, bottom_y, top_y, split;
        WindowPosition* parentWindow;
    };

    // Attribute Element Parent Class
    class AttributeElement
    {
    public:
        AttributeElement(std::string label, ElementType type) : label(label), type(type) {}

        // Getters
        std::string getLabel() { return label; }
        ElementType getType() { return type; }

        // Setters
        //void setHighlighted(bool highlighted) { this->highlighted = highlighted; }
        //void setClicked(bool clicked) { this->clicked = clicked; }


        // Override Functions
        virtual void RenderElement(UIRenderer* renderer, ElementPosition position, float textSize) = 0;
        virtual void OnClick(StateMachine* state) = 0;
        virtual void OnHold(StateMachine* state) = 0;
        virtual void OnRelease(StateMachine* state) = 0;

        bool clicked = false;
        bool highlighted = false;
    protected:
        ElementType type;
        std::string label;
    };

    struct AttributeInteractable
    {
        AttributeInteractable(glm::vec4 corners, AttributeElement* element) : corners(corners), element(element) {}
        AttributeElement* element;
        glm::vec4 corners;
    };

    // Float Entry
    class FloatEntry : public AttributeElement
    {
    public:
        FloatEntry(std::string label, float* value, float speed = 1.0f) : value(value), speed(speed), text(std::to_string(*value)), AttributeElement(label, UI_FLOAT_ENTRY)
        {
            initialMousePos = glm::vec3(0); saveValue = 0; slideStarted = false;
        }

        // Element Functions
        void setValue(float value) { *(this->value) = value; }
        void setValue(float* value) { this->value = value; }
        void RenderElement(UIRenderer* renderer, ElementPosition position, float textSize) override;
        void OnClick(StateMachine* state) override;
        void OnHold(StateMachine* state) override;
        void OnRelease(StateMachine* state) override;

    private:
        float* value;
        float speed;
        TextInput text;

        // Interaction Elements
        glm::vec2 initialMousePos;
        bool slideStarted;
        float saveValue;
    };

    // Float Slider
    class FloatSlider : public AttributeElement
    {
    public:
        FloatSlider(std::string label, float* value, float speed = 1.0f, float min = 0.0f, float max = 1.0f) : value(value), speed(speed), min(min), max(max), text(std::to_string(*value)), AttributeElement(label, UI_FLOAT_SLIDER)
        {
            initialMousePos = glm::vec3(0); saveValue = 0; slideStarted = false;
        }

        // Getters
        float getPercentage() { return smath::clamp((*value - min) / (max - min), 0.0f, 1.0f); }

        // Element Functions
        void setValue(float value) { *(this->value) = smath::clamp(value, min, max); }
        void setValue(float* value) { this->value = value; }
        void RenderElement(UIRenderer* renderer, ElementPosition position, float textSize) override;
        void OnClick(StateMachine* state) override;
        void OnHold(StateMachine* state) override;
        void OnRelease(StateMachine* state) override;

    private:
        float* value;
        TextInput text;
        float min;
        float max;
        float speed;

        // Interaction Elements
        glm::vec2 initialMousePos;
        bool slideStarted;
        float saveValue;
    };

    // Int Entry
    class IntEntry : public AttributeElement
    {
    public:
        IntEntry(std::string label, int* value, float speed = 1.0f) : value(value), speed(speed), text(std::to_string(*value)), AttributeElement(label, UI_INT_SLIDER)
        {
            initialMousePos = glm::vec3(0); saveValue = 0; slideStarted = false;
        }

        // Element Functions
        void setValue(int value) { *(this->value) = value; }
        void setValue(int* value) { this->value = value; }
        void RenderElement(UIRenderer* renderer, ElementPosition position, float textSize) override;
        void OnClick(StateMachine* state) override;
        void OnHold(StateMachine* state) override;
        void OnRelease(StateMachine* state) override;

    private:
        int* value;
        TextInput text;
        float speed;

        // Interaction Elements
        glm::vec2 initialMousePos;
        bool slideStarted;
        float saveValue;
    };

    // Int Slider
    class IntSlider : public AttributeElement
    {
    public:
        IntSlider(std::string label, int* value, float speed = 1.0f, int min = 0, int max = 10) : value(value), speed(speed), min(min), max(max), text(std::to_string(*value)), AttributeElement(label, UI_INT_SLIDER)
        {
            initialMousePos = glm::vec3(0); saveValue = 0; slideStarted = false;
        }

        // Getters
        float getPercentage() { return smath::clamp(((float)*value - (float)min) / ((float)max - (float)min), 0.0f, 1.0f); }

        // Element Functions
        void setValue(int value) { *(this->value) = smath::clamp(value, min, max); }
        void setValue(int* value) { this->value = value; }
        void RenderElement(UIRenderer* renderer, ElementPosition position, float textSize) override;
        void OnClick(StateMachine* state) override;
        void OnHold(StateMachine* state) override;
        void OnRelease(StateMachine* state) override;

    private:
        int* value;
        TextInput text;
        int min;
        int max;
        float speed;

        // Interaction Elements
        glm::vec2 initialMousePos;
        bool slideStarted;
        float saveValue;
    };

    // Toggle/Boolean
    class Toggle : public AttributeElement
    {
    public:
        Toggle(std::string label, bool* value, ElementType type = UI_TOGGLE) : value(value), AttributeElement(label, type) {}

        // Element Functions
        void setToggle(bool value) { *(this->value) = value; }
        void toggleValue() { *value = !(*value); }
        void RenderElement(UIRenderer* renderer, ElementPosition position, float textSize) override;
        void OnClick(StateMachine* state) override;
        void OnHold(StateMachine* state) override;
        void OnRelease(StateMachine* state) override;

    private:
        bool* value;
    };

    // Text Entry
    class TextEntry : public AttributeElement
    {
    public:
        TextEntry(std::string label, std::string* value) : value(value), text(*value), AttributeElement(label, UI_TEXT_ENTRY) {}

        // Element Functions
        void setValue(std::string value) { *(this->value) = value; }
        void setValue(std::string* value) { this->value = value; }
        void RenderElement(UIRenderer* renderer, ElementPosition position, float textSize) override;
        void OnClick(StateMachine* state) override;
        void OnHold(StateMachine* state) override;
        void OnRelease(StateMachine* state) override;

    private:
        std::string* value;
        std::string saveValue;
        TextInput text;
    };

    // Dropdown
    class Dropdown : public AttributeElement
    {
    public:
        Dropdown(std::string label, int* value, std::vector<std::string> options) : value(value), options(options), optionSize(glm::vec2(0)), firstOptionYPos(0), droppedDown(false), AttributeElement(label, UI_DROPDOWN) {}

        // Element Functions
        void addOption(std::string option) { options.push_back(option); }
        void addOptions(std::vector<std::string> options) { this->options.insert(this->options.end(), options.begin(), options.end()); }
        void removeOption(int index) { options.erase(options.begin() + index); }
        void RenderElement(UIRenderer* renderer, ElementPosition position, float textSize) override;
        void OnClick(StateMachine* state) override;
        void OnHold(StateMachine* state) override;
        void OnRelease(StateMachine* state) override;

    private:
        int* value;
        bool droppedDown;
        glm::vec2 optionSize;
        float firstOptionYPos;
        std::vector<std::string> options; // Index corresponds to value
    };

    // Attribute Group (Parent Object)
    class Attribute
    {
    public:
        // Constructor & Deconstructor
        Attribute(std::string name, bool collapsed = false) : attributeName(name), collapsed(collapsed) { dropdown = new Toggle(name, &(this->collapsed), UI_TOGGLE); }
        ~Attribute()
        {
            for (int i = 0; i < elements.size(); i++)
            {
                delete elements[i];
                elements[i] = nullptr;
            }
            elements.clear();

            delete dropdown;
            dropdown = nullptr;
        }

        // Getters
        AttributeElement* getElement(int index) { return elements[index]; }
        std::string getName() { return attributeName; }
        bool getCollapsed() { return collapsed; }
        int getElementCount() { return elements.size(); }
        AttributeElement* getDropDownButton() { return dropdown; }

        // Setters
        void setName(std::string name) { this->attributeName = name; }
        void setCollapsed(bool collapsed) { this->collapsed = collapsed; }

        // Functions
        void addElement(AttributeElement* element) { elements.push_back(element); }
        void addFloatEntry(std::string label, float* value, float speed = 1.0f);
        void addFloatSlider(std::string label, float* value, float speed = 1.0f, float min = 0.0f, float max = 1.0f);
        void addIntEntry(std::string label, int* value, float speed = 1.0f);
        void addIntSlider(std::string label, int* value, float speed = 1.0f, int min = 0, int max = 10);
        void addToggle(std::string label, bool* value);
        void addTextEntry(std::string label, std::string* value);
        void addDropdown(std::string label, int* value, std::vector<std::string> options);

    private:
        std::string attributeName;
        bool collapsed;
        AttributeElement* dropdown;
        std::vector<AttributeElement*> elements;
    };


    // Hierarchy UI Elements
    // Hierarchy Text Entry
    class HierarchyTextEntry : public AttributeElement
    {
    public:
        HierarchyTextEntry(std::string label, std::string* value, obj::Object* object = nullptr) : value(value), text(*value), object(object), clickTime(-1.0f), textTriggered(false), AttributeElement(label, UI_TEXT_ENTRY) {}

        // Element Functions
        void setValue(std::string value) { *(this->value) = value; }
        void setValue(std::string* value) { this->value = value; }
        void RenderElement(UIRenderer* renderer, ElementPosition position, float textSize) override;
        void OnClick(StateMachine* state) override;
        void OnHold(StateMachine* state) override;
        void OnRelease(StateMachine* state) override;

    private:
        obj::Object* object;
        float clickTime;
        bool textTriggered;
        std::string* value;
        std::string saveValue;
        TextInput text;
    };


    // Viewport UI Elements
    class ViewNav : public AttributeElement
    {
    public:
        ViewNav(std::string label, float speed = 1.0f) : AttributeElement(label, UI_VIEW_NAV), speed(speed) { CreateMesh(); }
        ~ViewNav();

        // Setters
        void setTransform(glm::mat4 transform) { this->transform = transform; }

        // Functions
        void CreateMesh();
        void RenderElement(UIRenderer* renderer, ElementPosition position, float textSize) override;
        void OnClick(StateMachine* state) override;
        void OnHold(StateMachine* state) override;
        void OnRelease(StateMachine* state) override;

    private:
        // UI Stuff
        glm::vec2 initialMousePos;
        glm::vec2 storedCameraOrbit;
        bool slideStarted;
        float speed;
        enum ViewAxis { VIEW_NONE = -1, VIEW_POSITIVE_X, VIEW_NEGATIVE_X, VIEW_POSITIVE_Y, VIEW_NEGATIVE_Y, VIEW_POSITIVE_Z, VIEW_NEGATIVE_Z };
        ViewAxis getClosestAxis(glm::vec2 position, float distanceClamp);

        // Rendering Stuff
        Mesh* navMesh;
        glm::vec3 screenPos;
        glm::mat4 transform;
        shdr::Shader* navShader;
    };
    // Tools Window
    // Select Tool
    // Move Tool
    // Rotate Tool
    // Scale Tool
    // Add Item Tool
}
#endif // !UI Elements
