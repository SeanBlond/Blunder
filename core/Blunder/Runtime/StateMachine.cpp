#include "StateMachine.h"

// Cosntructor & Destructor
StateMachine::StateMachine(scn::Scene* scene, Mouse* mouse, OrbitCamera* activeCamera, glm::ivec2 dimensions)
{
    this->activeCamera = activeCamera;
    this->scene = scene;
    this->mouse = mouse;
    this->windowDimensions = dimensions;
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
void StateMachine::manageStateMachine()
{
    glm::vec2 mouseDelta = (mouse ? mouse->mouseDelta : glm::vec2(0));

    // Determining what to do for each state
    switch (currentState)
    {
    case SM_NONE:
        break;

    case SM_UI_INTERACT:
        UIInteracting = true;
        break;

    case SM_UI_TRANSLATING:
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
            glm::vec3 globalUp = glm::cross(activeCamera->getCameraNormal(), glm::vec3(0, 1, 0));
            glm::vec3 right = glm::cross(activeCamera->getCameraNormal(), globalUp);
            glm::vec3 up = glm::cross(activeCamera->getCameraNormal(), right);
            translateVector += (up * -((mouseDelta.x) / 100.0f)) + (right * -(mouseDelta.y / 100.0f));
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

void StateMachine::UpdateMouse(GLFWwindow* window)
{
    // TODO: Make sensitivty a member variable
    float sensitivity = 0.5f;

    // Checking if mouse should be hidden when ui interacting
    if (currentState == SM_UI_TRANSLATING && glfwRawMouseMotionSupported())
    {
        if (!mouse->rawData)
        {
            std::cout << "Raw Data Mode: ON" << std::endl;
            mouse->rawData = true;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
        }
    }
    else
    {
        if (mouse->rawData)
        {
            std::cout << "Raw Data Mode: OFF" << std::endl;
            mouse->rawData = false;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
        }
    }

    // Updating mouse data
    mouse->UpdateMouse(window, sensitivity);
}