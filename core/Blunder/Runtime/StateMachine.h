#ifndef STATE_MACHINE
#pragma once

#include <vector>

#include "../../object/object.h"
#include "../../Camera/camera.h"
#include "../UI/TextInput.h"
#include "../UI/UIPositioning.h"
#include "../Scene/Scene.h"

enum EditingState { SM_NONE, SM_SELECT, SM_TRANSLATE, SM_ROTATE, SM_SCALE, SM_UI_INTERACT, SM_UI_TYPING, SM_UI_TRANSLATING };

class WindowManager;

struct Mouse 
{
    Mouse(glm::vec2 pos = glm::vec2(0)) : mousePos(pos), previousMouse(pos), mouseDelta(glm::vec2(0)), rawData(false) {}
    glm::vec2 mousePos;
    glm::vec2 mouseDelta;
    bool rawData;

    void UpdateMouse(GLFWwindow* window, float sensitivity) 
    { 
        double xposIn, yposIn;
        glfwGetCursorPos(window, &xposIn, &yposIn);

        previousMouse = mousePos; 
        mousePos = glm::vec2(xposIn, yposIn); 
        mouseDelta = (previousMouse - mousePos) * sensitivity * glm::vec2(-1, 1);
    }

private:
    glm::vec2 previousMouse;
};

class StateMachine
{
public:
    // Cosntructor & Destructor
    StateMachine(scn::Scene* scene, Mouse* mouse, OrbitCamera* activeCamera, glm::ivec2 dimensions = glm::ivec2(0));
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
    glm::ivec2 getWindowDimensions() { return windowDimensions; }
    WindowManager* getWindowManager() { return windowManager; }

    // Setters
    void setAxis(const glm::vec3 axis) { this->stateAxis = axis; }
    void setTextInput(ui::TextInput* textInput) { this->textInput = textInput; }
    void setWindowDimensions(glm::ivec2 dimensions) { this->windowDimensions = dimensions; }
    void setWindowManager(WindowManager* windowManager) { this->windowManager = windowManager; }

    //Functions
    void selectObject(obj::Object* newObject);
    void changeState(EditingState newState);
    void completeState();
    void exitState();
    void manageStateMachine();
    void changeAxis(const glm::vec3 axis);
    void UpdateMouse(GLFWwindow* window);

private:
    EditingState currentState;
    bool transforming;
    bool UIInteracting;
    scn::Scene* scene;
    obj::Object* selectedObject;
    Mouse* mouse;
    OrbitCamera* activeCamera;
    WindowManager* windowManager;
    ui::TextInput* textInput;
    glm::vec3 saveValue;
    glm::vec3 stateAxis;
    glm::ivec2 windowDimensions;
};

#endif