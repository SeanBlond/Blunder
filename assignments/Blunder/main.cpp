#include <iostream>
#include <stdio.h>
#include <math.h>

#include <ew/external/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Blunder/ObjectSystem/ObjectSystem.h>
#include <Blunder/StateMachine/StateMachine.h>
#include <Blunder/UI/UIElements.h>
#include <Blunder/UI/UIWindows.h>
#include <Camera/camera.h>
#include <shader/shader.h>
#include <object/object.h>
#include <math/smath.h>
#include <line/line.h>
#include <Blunder/Keystroke.h>
#include <Font/font.h>
#include <Blunder/Time.h>
#include <Blunder/Color.h>

// Screen Info
int SCREEN_WIDTH = 1500;
int SCREEN_HEIGHT = 1000;

// Time Manager
TimeManager Time;

// Mouse Data
bool wrapMouse = false;
Mouse mouse;

// Function Prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_position_management(GLFWwindow* window);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void viewManagement(GLFWwindow* window);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void character_callback(GLFWwindow* window, unsigned int codepoint);

// Camera Data
OrbitCamera camera(1.25f, 2.0f, 7.5f);

// State Machine Info
StateMachine state(&mouse);

// Setting up Object System
obs::ObjectSystem objectSystem;
int obj::Object::nextID = 0;

int main() {
    printf("Initializing...\n");
    if (!glfwInit()) {
        printf("GLFW failed to init!");
        return 1;
    }
    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Blunder v0.0.1", NULL, NULL);
    if (window == NULL) {
        printf("GLFW failed to create window");
        return 1;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGL(glfwGetProcAddress)) {
        printf("GLAD Failed to load GL headers");
        return 1;
    }

    // Loading software image
    GLFWimage images[1];
    images[0].pixels = stbi_load("assets/icon.png", &images[0].width, &images[0].height, 0, 4); // Load as RGBA
    if (!images[0].pixels) {
        std::cout << "Error Reading Icon" << std::endl;
    }
    else 
    {
        glfwSetWindowIcon(window, 1, images);
        stbi_image_free(images[0].pixels); // Free the image data after setting the icon
    }

    //Initialization goes here!
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Callbacks
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetCharCallback(window, character_callback);

    // Creating Shader
    shdr::Shader testShader("assets/vertex.glsl", "assets/Lighting.glsl");

    // Creating Default Objects
    printf("Adding Test Object\n");
    objectSystem.addObject(new obj::Object("cube", mesh::createCube(1.0f, 1.0f, 1.0f, 1), &testShader));
    objectSystem.addObject(new obj::Object("sphere", mesh::createSphere(1.0f, 8), &testShader, glm::vec3(-2, 0, 0)));
    //objectSystem.addObject(new obj::Object("cylinder", mesh::createCylinder(1.0f, 2.0f, 12), &testShader, glm::vec3(2, 0, 0)));
    //objectSystem.addObject(new obj::Object("torus", mesh::createTorus(1.0f, 0.25f, 8, 8), &testShader, glm::vec3(0, 0, -2.5f)));

    // Testing hierarchy stuff
    objectSystem.addFolder("Test 1");
    objectSystem.addFolder("Test 2");
    objectSystem.testOutput();

    // Selecting the default cube
    state.selectObject(objectSystem.getSelectedFolder()->getHierarchyElement(0)->getObject());
    obj::Object* selectedObject = state.getSelectedObject();

    // Attribute UI Creation
    AttributeWindow attributeUI(0.25f * SCREEN_HEIGHT, SCREEN_HEIGHT, "assets/fonts/Lato-Regular.ttf", 64);

    ui::Attribute positionAttribute("Position");
    positionAttribute.addFloatEntry("X", &(selectedObject->transform.position.x));
    positionAttribute.addFloatEntry("Y", &(selectedObject->transform.position.y));
    positionAttribute.addFloatEntry("Z", &(selectedObject->transform.position.z));
    attributeUI.addAttribute(&positionAttribute);

    ui::Attribute rotationAtrribute("Rotation");
    rotationAtrribute.addFloatEntry("X", &(selectedObject->transform.rotation.x));
    rotationAtrribute.addFloatEntry("Y", &(selectedObject->transform.rotation.y));
    rotationAtrribute.addFloatEntry("Z", &(selectedObject->transform.rotation.z));
    attributeUI.addAttribute(&rotationAtrribute);

    ui::Attribute scaleAtrribute("Scale");
    scaleAtrribute.addFloatEntry("X", &(selectedObject->transform.scale.x));
    scaleAtrribute.addFloatEntry("Y", &(selectedObject->transform.scale.y));
    scaleAtrribute.addFloatEntry("Z", &(selectedObject->transform.scale.z));
    attributeUI.addAttribute(&scaleAtrribute);

    attributeUI.GenerateInteractables();

    // Hierarchy UI Creation
    //HierarchyWindow hierarchyUI(0.25f * SCREEN_HEIGHT, SCREEN_HEIGHT, "assets/fonts/Lato-Regular.ttf", 64, &objectSystem);
    // hierarchyUI.GenerateInteractables();


    // Creating Axis Line
    Line xAxisLine(glm::vec3(-100.0f, 0.0f, 0.0f), glm::vec3(100.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), 3.0f);
    Line yAxisLine(glm::vec3(0.0f, -100.0f, 0.0f), glm::vec3(0.0f, 100.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 3.0f);
    Line zAxisLine(glm::vec3(0.0f, 0.0f, -100.0f), glm::vec3(0.0f, 0.0f, 100.0f), glm::vec3(0.0f, 0.0f, 1.0f), 3.0f);

    // Creating Projection Matrix
    glm::mat4 projection = camera.getProjectionMatrix((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);

    //Render loop
    while (!glfwWindowShouldClose(window)) {
        // Managing Inputs
        mouse_position_management(window);
        viewManagement(window);
        glfwPollEvents();

        // Window Management
        glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);
        glfwSetWindowSizeLimits(window, (SCREEN_HEIGHT / 2), 400, GLFW_DONT_CARE, GLFW_DONT_CARE);

        //Clear framebuffer
        glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // UI Math
        float uiwidth = (0.25f) * SCREEN_HEIGHT;
        glViewport(uiwidth, 0, SCREEN_WIDTH - (2.0f * uiwidth), SCREEN_HEIGHT);

        // Time Management
        Time.UpdateTime(glfwGetTime());

        // Rotating Camera View
        float aspectRatio = (SCREEN_WIDTH - (2.0f * uiwidth)) / SCREEN_HEIGHT;
        projection = camera.getProjectionMatrix(aspectRatio, 0.1f, 100.0f);;
        glm::mat4 view = camera.getViewMatrix();

        // State Machine Managment
        state.manageStateMachine(mouse.mouseDelta, camera);

        // Rendering 3D Stuff
        glEnable(GL_DEPTH);

        // Drawing the Meshes
        objectSystem.Render(projection, view);
        
        // Drawing Axis Lines
        xAxisLine.setTransformation(projection * view);
        xAxisLine.drawLine();
        yAxisLine.setTransformation(projection * view);
        yAxisLine.drawLine();
        zAxisLine.setTransformation(projection * view);
        zAxisLine.drawLine();

        //Drawing Attribute UI
        glViewport(0, 0, uiwidth, SCREEN_HEIGHT);
        glDisable(GL_DEPTH);
        attributeUI.DrawAttributeWindow();

        // Managing Attribute UI
        attributeUI.setDimensions(uiwidth, (float)SCREEN_HEIGHT);
        attributeUI.ManageUIInteraction(window, &state);

        // Drawing Hierarchy UI
        glViewport(SCREEN_WIDTH - uiwidth, 0, uiwidth, SCREEN_HEIGHT);
        glDisable(GL_DEPTH);
        //hierarchyUI.DrawAttributeWindow();

        // Managing Hierarchy UI
        //hierarchyUI.setDimensions(uiwidth, (float)SCREEN_HEIGHT);
        //hierarchyUI.ManageUIInteraction(window, &state);

        glfwSwapBuffers(window);
    }

    // De-Allocating Resources
    testShader.deallocateShader();

    printf("Shutting down...\n");
}

// Processes Key Input
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // Exiting Application
    if (key::quitWindow.getKey(window, key) && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Prioritize Keys while transforming
    if (state.getTransforming())
    {
        wrapMouse = true;

        // Changing Transformation Axis
        if (key::xAxis.getKey(window, key) && action == GLFW_PRESS)
        {
            state.changeAxis(glm::vec3(1.0f, 0.0f, 0.0f));
        }
        else if (key::yAxis.getKey(window, key) && action == GLFW_PRESS)
        {
            state.changeAxis(glm::vec3(0.0f, 1.0f, 0.0f));
        }
        else if (key::zAxis.getKey(window, key) && action == GLFW_PRESS)
        {
            state.changeAxis(glm::vec3(0.0f, 0.0f, 1.0f));
        }

        // Transformations Exits
        else if (key::completeTransform.getKey(window, key) && action == GLFW_PRESS)
        {
            wrapMouse = false;
            state.completeState();
        }
        else if (key::exitTransform.getKey(window, key) && action == GLFW_PRESS)
        {
            wrapMouse = false;
            state.exitState();
        }
    }

    // Prioritizing UI Typing
    if (state.getState() == SM_UI_TYPING)
    {
        if (key::backspaceText.getKey(window, key) && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            state.getTextInput()->remove();
        }
        else if (key::shiftCursorLeft.getKey(window, key) && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            state.getTextInput()->resetSelection();
            state.getTextInput()->shiftCursor(-1);
        }
        else if (key::shiftCursorRight.getKey(window, key) && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            state.getTextInput()->resetSelection();
            state.getTextInput()->shiftCursor(1);
        }
        else if (key::shiftSelectionLeft.getKey(window, key) && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            state.getTextInput()->updateSelection();
            state.getTextInput()->shiftCursor(-1);
        }
        else if (key::shiftSelectionRight.getKey(window, key) && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            state.getTextInput()->updateSelection();
            state.getTextInput()->shiftCursor(1);
        }
        else if (key::copySelected.getKey(window, key) && action == GLFW_PRESS)
        {
            std::string copiedText = state.getTextInput()->returnSelected();

            if (copiedText.length() > 0)
                glfwSetClipboardString(NULL, copiedText.c_str());
        }
        else if (key::cutSelected.getKey(window, key) && action == GLFW_PRESS)
        {
            std::string copiedText = state.getTextInput()->cutSelected();

            if (copiedText.length() > 0)
                glfwSetClipboardString(NULL, copiedText.c_str());
        }
        else if (key::pasteClipboard.getKey(window, key) && action == GLFW_PRESS)
        {
            state.getTextInput()->insert(glfwGetClipboardString(NULL));
        }
        else if (key::selectAll.getKey(window, key) && action == GLFW_PRESS)
        {
            state.getTextInput()->selectAll();
        }

        // Typing  Exits
        else if (key::completeText.getKey(window, key) && action == GLFW_PRESS)
        {
            state.completeState();
        }
        else if (key::exitText.getKey(window, key) && action == GLFW_PRESS)
        {
            state.exitState();
        }
    }

    // Reset Transformations
    else if (key::resetPosition.getKey(window, key) && action == GLFW_PRESS)
    {
        state.getSelectedObject()->resetPosition();
    }
    else if (key::resetRotation.getKey(window, key) && action == GLFW_PRESS)
    {
        state.getSelectedObject()->resetRotation();
    }
    else if (key::resetScale.getKey(window, key) && action == GLFW_PRESS)
    {
        state.getSelectedObject()->resetScale();
    }

    // Changing State
    else if (key::select.getKey(window, key) && action == GLFW_PRESS && !state.getUIInteraction())
    {
        state.changeState(SM_SELECT);
    }
    else if (key::translate.getKey(window, key) && action == GLFW_PRESS && !state.getUIInteraction())
    {
        state.changeState(SM_TRANSLATE);
    }
    else if (key::rotate.getKey(window, key) && action == GLFW_PRESS && !state.getUIInteraction())
    {
        state.changeState(SM_ROTATE);
    }
    else if (key::scale.getKey(window, key) && action == GLFW_PRESS && !state.getUIInteraction())
    {
        state.changeState(SM_SCALE);
    }

    // View Management
    else if (key == GLFW_KEY_F && action == GLFW_PRESS)
        camera.setPivot(state.getSelectedObject()->transform.position);
}

// Process Mouse Position
void mouse_position_management(GLFWwindow* window)
{
    double xposIn, yposIn;
    glfwGetCursorPos(window, &xposIn, &yposIn);

    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    float sensitivity = 0.5f;
    mouse.UpdateMouse(xpos, ypos, sensitivity);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && state.getTransforming())
        state.completeState();
}

void viewManagement(GLFWwindow* window)
{
    // View Management
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE))
    {
        // Checking if Pivot should be moved
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT))
        {
            glm::vec3 globalUp = glm::cross(camera.getCameraNormal(), glm::vec3(0, 1, 0));
            glm::vec3 right = glm::cross(camera.getCameraNormal(), globalUp);
            glm::vec3 up = glm::cross(camera.getCameraNormal(), right);
            camera.addPivot((up * (mouse.mouseDelta.x / 100.0f)) + (right * (mouse.mouseDelta.y / 100.0f)));
        }
        // Changing Orbit Rotation
        else
        {
            camera.addAngles(mouse.mouseDelta * glm::vec2(0.01f));
        }
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    float newDistance = camera.getDistance() + (yoffset * -0.25f * camera.getDistance());
    newDistance = smath::clamp(newDistance, 0.01f, 75.0f);
    camera.setDistance(newDistance);
}

void character_callback(GLFWwindow* window, unsigned int codepoint)
{
    // Checking if there is text to type into
    if (state.getState() == SM_UI_TYPING)
    {
        state.getTextInput()->insert((char)codepoint);
    }
}