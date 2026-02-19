#ifndef STATE_MACHINE
#pragma once

#include <vector>

#include "../../object/object.h"
#include "../../Camera/camera.h"
#include "../UI/TextInput.h"
#include "../UI/UIPositioning.h"
#include "../Scene/Scene.h"

enum EditingState { SM_NONE, SM_SELECT, SM_TRANSLATE, SM_ROTATE, SM_SCALE, SM_UI_INTERACT, SM_UI_TYPING };

struct Mouse 
{
    Mouse(glm::vec2 pos = glm::vec2(0)) : mousePos(pos), previousMouse(pos), mouseDelta(glm::vec2(0)) {}
    glm::vec2 mousePos;
    glm::vec2 mouseDelta;

    void UpdateMouse(float x, float y, float sensitivity) 
    { 
        previousMouse = mousePos; 
        mousePos = glm::vec2(x, y); 
        mouseDelta = (previousMouse - mousePos) * sensitivity * glm::vec2(-1, 1);
    }
    void UpdateMouse(float x, float y, float sensitivity, glm::vec4 bounds) 
    { 
        glm::vec2 dimensions = glm::vec2(bounds.z - bounds.x, bounds.w - bounds.y);
        //float wrappedX = 

        previousMouse = mousePos; 
        mousePos = glm::vec2(x, y); 
        mouseDelta = (previousMouse - mousePos) * sensitivity * glm::vec2(-1, 1);
    }

private:
    glm::vec2 previousMouse;
};

class StateMachine
{
public:
    // Cosntructor & Destructor
    StateMachine(scn::Scene* scene, Mouse* mouse, OrbitCamera* activeCamera);
    ~StateMachine();

    // Getters
    EditingState getState() const { return currentState; }
    obj::Object* getSelectedObject() { return selectedObject; }
    bool getTransforming() const { return transforming; }
    bool getUIInteraction() const { return UIInteracting; }
    ui::TextInput* getTextInput() { return textInput; }
    Mouse* getMouse() { return mouse; }
    OrbitCamera* getCamera() { return activeCamera; }
    scn::Scene* getScene() { return scene; }
    ui::WindowPosition* getSelectedWindowPosition() { return selectedWindowPosition; }

    // Setters
    void setAxis(const glm::vec3 axis) { this->stateAxis = axis; }
    void setTextInput(ui::TextInput* textInput) { this->textInput = textInput; }
    void setSelectedWindowPosition(ui::WindowPosition* position) { this->selectedWindowPosition = position; }

    //Functions
    void selectObject(obj::Object* newObject);
    void changeState(EditingState newState);
    void completeState();
    void exitState();
    void manageStateMachine();
    void changeAxis(const glm::vec3 axis);
    void UpdateMouse(float xPos, float yPos);

private:
    EditingState currentState;
    bool transforming;
    bool UIInteracting;
    scn::Scene* scene;
    obj::Object* selectedObject;
    Mouse* mouse;
    OrbitCamera* activeCamera;
    ui::TextInput* textInput;
    ui::WindowPosition* selectedWindowPosition;
    glm::vec3 saveValue;
    glm::vec3 stateAxis;
};

#endif