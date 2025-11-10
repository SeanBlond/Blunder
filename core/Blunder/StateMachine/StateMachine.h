#ifndef STATE_MACHINE
#define STATE_MACHINE

#include "../../object/object.h"
#include "../../Camera/camera.h"
#include "../UI/TextInput.h"


enum EditingState { SM_NONE, SM_SELECT, SM_TRANSLATE, SM_ROTATE, SM_SCALE, SM_UI_INTERACT, SM_UI_TYPING };

struct Mouse 
{
    Mouse(glm::vec2 pos = glm::vec2(0)) : mousePos(pos), previousMouse(pos) { mouseDelta = glm::vec2(0); }
    glm::vec2 mousePos;
    glm::vec2 mouseDelta;

    void UpdateMouse(float x, float y, float sensitivity) 
    { 
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
    StateMachine(Mouse* mouse);
    ~StateMachine();

    // Getters
    EditingState getState() { return currentState; }
    obj::Object* getSelectedObject() { return selectedObject; }
    bool getTransforming() { return transforming; }
    bool getUIInteraction() { return UIInteracting; }
    ui::TextInput* getTextInput() { return textInput; }

    // Setters
    void setAxis(const glm::vec3 axis) { this->stateAxis = axis; }
    void setTextInput(ui::TextInput* textInput) { this->textInput = textInput; }

    //Functions
    void selectObject(obj::Object* newObject);
    void changeState(EditingState newState);
    void completeState();
    void exitState();
    void manageStateMachine(glm::vec2 mouseDelta, OrbitCamera camera);
    void changeAxis(const glm::vec3 axis);

private:
    EditingState currentState;
    obj::Object* selectedObject;
    Mouse* mouse;
    ui::TextInput* textInput;
    glm::vec3 saveValue;
    glm::vec3 stateAxis;
    bool transforming;
    bool UIInteracting;
};

#endif