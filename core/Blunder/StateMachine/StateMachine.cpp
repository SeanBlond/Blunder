#include "StateMachine.h"

// Cosntructor & Destructor
StateMachine::StateMachine(Mouse* mouse)
{
    this->mouse = mouse;
    currentState = SM_NONE;
    transforming = false;
    UIInteracting = false;
    selectedObject = nullptr;
    stateAxis = glm::vec3(0.0f, 0.0f, 0.0f);
}
StateMachine::~StateMachine()
{
    selectedObject = nullptr;
    delete selectedObject;
}

//Functions
void StateMachine::selectObject(obj::Object* newObject)
{
    selectedObject = newObject;
}
void StateMachine::changeState(EditingState newState)
{
    // Checking if new state is the same as the current state
    if (currentState == newState || selectedObject == nullptr)
        return;

    glm::vec3 tempAxis = glm::vec3(0);
    if (stateAxis.length() > 0.1f)
        tempAxis = stateAxis;

    exitState();
    currentState = newState;
    stateAxis = tempAxis;

    if (newState == SM_TRANSLATE)
        saveValue = selectedObject->transform.position;

    if (newState == SM_ROTATE)
        saveValue = selectedObject->transform.rotation;

    if (newState == SM_SCALE)
        saveValue = selectedObject->transform.scale;
}
void StateMachine::completeState()
{
    if (currentState == SM_UI_TYPING)
    {
        textInput->setTyping(false);
        textInput->setStored(true);
        textInput = nullptr;
    }

    currentState = SM_NONE;
    transforming = false;
    UIInteracting = false;
    stateAxis = glm::vec3(0);
    saveValue = glm::vec3(0.0f);
}
void StateMachine::exitState()
{
    if (currentState == SM_TRANSLATE)
        selectedObject->transform.position = (saveValue);

    else if (currentState == SM_ROTATE)
        selectedObject->transform.rotation = (saveValue);

    else if (currentState == SM_SCALE)
        selectedObject->transform.scale = (saveValue);

    else if (currentState == SM_UI_TYPING)
    {
        textInput->setTyping(false);
        textInput = nullptr;
    }

    currentState = SM_NONE;
    transforming = false;
    UIInteracting = false;
    saveValue = glm::vec3(0.0f);
    stateAxis = glm::vec3(0.0f);
}
void StateMachine::manageStateMachine(glm::vec2 mouseDelta, OrbitCamera camera)
{
    // Determining what to do for each state
    switch (currentState)
    {
    case SM_NONE:
        break;

    case SM_UI_INTERACT:
        UIInteracting = true;
        break;

    case SM_UI_TYPING:
        UIInteracting = true;
        break;

    case SM_TRANSLATE:
        transforming = true;
        glm::vec3 translateVector = selectedObject->transform.position;

        // Checking if transformation axis is set
        if (glm::length(stateAxis) >= 0.01f)
        {
            translateVector += stateAxis * (mouseDelta.x / 50.0f);
        }
        else
        {
            glm::vec3 globalUp = glm::cross(camera.getCameraNormal(), glm::vec3(0, 1, 0));
            glm::vec3 right = glm::cross(camera.getCameraNormal(), globalUp);
            glm::vec3 up = glm::cross(camera.getCameraNormal(), right);
            translateVector += (up * -(mouseDelta.x / 100.0f)) + (right * -(mouseDelta.y / 100.0f));
        }
        selectedObject->transform.position = (translateVector);
        break;

    case SM_ROTATE:
        transforming = true;
        glm::vec3 rotateVector = selectedObject->transform.rotation;

        // Checking if transformation axis is set
        if (glm::length(stateAxis) >= 0.01f)
        {
            rotateVector += stateAxis * (mouseDelta.x / 50.0f);
        }
        else
        {
            //rotateVector += camera.getCameraNormal() * -(mouseDelta.x / 50.0f);
        }
        selectedObject->transform.rotation = (rotateVector);
        break;

    case SM_SCALE:
        transforming = true;
        glm::vec3 scaleVector = selectedObject->transform.scale;

        // Checking if transformation axis is set
        if (glm::length(stateAxis) >= 0.01f)
        {
            scaleVector += stateAxis * (mouseDelta.x / 50.0f);
        }
        else
        {
            scaleVector += glm::vec3(mouseDelta.x / 50.0f);
        }
        selectedObject->transform.scale = (scaleVector);
    break;
    }
}
void StateMachine::changeAxis(const glm::vec3 axis)
{
    EditingState tempState = getState();
    exitState();
    setAxis(axis);
    changeState(tempState);
}