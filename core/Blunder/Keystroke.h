#ifndef KEYSTROKE
#pragma once

#include <GLFW/glfw3.h>

class Keystroke
{
public:
    // Constructor
    Keystroke(int key, bool control = false, bool shift = false, bool alt = false)
        : key(key), control(control), shift(shift), alt(alt) { keyPressed = false; }

    bool getKey(GLFWwindow* window, int keyInput)
    {
        return (keyInput == key) && getModifiers(window);
    }
    bool getModifiers(GLFWwindow* window)
    {
        bool controlActive = ((bool)glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) | (bool)glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL)) == control;
        bool shiftActive = ((bool)glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) | (bool)glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT)) == shift;
        bool altActive = ((bool)glfwGetKey(window, GLFW_KEY_LEFT_ALT) | (bool)glfwGetKey(window, GLFW_KEY_RIGHT_ALT)) == alt;
        return (controlActive && shiftActive) && altActive;
    }

private:
    bool control, shift, alt;
    int key;
    bool keyPressed;
};

namespace key
{
    // Tool Keys
    Keystroke select(GLFW_KEY_W);
    Keystroke translate(GLFW_KEY_G);
    Keystroke rotate(GLFW_KEY_R);
    Keystroke scale(GLFW_KEY_S);

    // Transformation Completions
    Keystroke completeTransform(GLFW_KEY_ENTER);
    Keystroke exitTransform(GLFW_KEY_ESCAPE);

    // Transformation Axis
    Keystroke xAxis(GLFW_KEY_X);
    Keystroke yAxis(GLFW_KEY_Y);
    Keystroke zAxis(GLFW_KEY_Z);

    // Reset Transform Keys
    Keystroke resetPosition(GLFW_KEY_G, false, false, true);
    Keystroke resetRotation(GLFW_KEY_R, false, false, true);
    Keystroke resetScale(GLFW_KEY_S, false, false, true);

    // Software Operations
    Keystroke quitWindow(GLFW_KEY_Q, true);

    // Text Editting Operations
    Keystroke backspaceText(GLFW_KEY_BACKSPACE);
    Keystroke deleteCharacter(GLFW_KEY_DELETE);
    Keystroke shiftCursorLeft(GLFW_KEY_LEFT);
    Keystroke shiftCursorRight(GLFW_KEY_RIGHT);
    Keystroke shiftSelectionLeft(GLFW_KEY_LEFT, false, true);
    Keystroke shiftSelectionRight(GLFW_KEY_RIGHT, false, true);
    Keystroke selectAll(GLFW_KEY_A, true);
    Keystroke copySelected(GLFW_KEY_C, true);
    Keystroke cutSelected(GLFW_KEY_X, true);
    Keystroke pasteClipboard(GLFW_KEY_V, true);
    Keystroke completeText(GLFW_KEY_ENTER);
    Keystroke exitText(GLFW_KEY_ESCAPE);
}

#endif // !Keystroke
