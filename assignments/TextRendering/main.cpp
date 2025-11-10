#include <iostream>
#include <map>
#include <string>

#include <shader/shader.h>
#include <ew/external/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Font/font.h>
#include <math/smath.h>
#include <object/object.h>
#include <Blunder/UI/TextInput.h>
#include <Blunder/Keystroke.h>

#include <ft2build.h>
#include FT_FREETYPE_H


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void character_callback(GLFWwindow* window, unsigned int codepoint);

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

ui::TextInput text("123456789");

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGL(glfwGetProcAddress)) {
        printf("GLAD Failed to load GL headers");
        return 1;
    }

    // OpenGL state
    // ------------
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Callback
    glfwSetCharCallback(window, character_callback);
    glfwSetKeyCallback(window, key_callback);

    // compile and setup the shader
    // ----------------------------
    glm::mat4 projection = smath::orthographic(0.0f, static_cast<float>(SCR_WIDTH), 0.0f, static_cast<float>(SCR_HEIGHT));

    // Setting Up Renderer
    ui::UIRenderer renderer("assets/fonts/Lato-Regular.ttf", 64, projection);
    text.setTyping(true);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        text.renderText(&renderer, 10.0f, 690.0f, 0.45f, glm::vec3(0.5f, 0.8f, 0.2f));
        //lato.RenderText(text.getText(), 10.0f, 690.0f, 0.45f, glm::vec3(0.5f, 0.8f, 0.2f));
        //std::string cursorText = "Current Cursor Position: " + std::to_string(text.getCursor());
        //lato.RenderText(cursorText, 10.0f, 650.0f, 0.45f, glm::vec3(0.5f, 0.8f, 0.2f));

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key::quitWindow.getKey(window, key) && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (key::backspaceText.getKey(window, key) && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        text.remove();
    }
    else if (key::shiftCursorLeft.getKey(window, key) && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        text.resetSelection();
        text.shiftCursor(-1);
    }
    else if (key::shiftCursorRight.getKey(window, key) && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        text.resetSelection();
        text.shiftCursor(1);
    }
    else if (key::shiftSelectionLeft.getKey(window, key) && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        text.updateSelection();
        text.shiftCursor(-1);
    }
    else if (key::shiftSelectionRight.getKey(window, key) && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        text.updateSelection();
        text.shiftCursor(1);
    }
    else if (key::copySelected.getKey(window, key) && action == GLFW_PRESS)
    {
        std::string copiedText = text.returnSelected();

        if (copiedText.length() > 0)
            glfwSetClipboardString(NULL, copiedText.c_str());
    }
    else if (key::cutSelected.getKey(window, key) && action == GLFW_PRESS)
    {
        std::string copiedText = text.cutSelected();

        if (copiedText.length() > 0)
            glfwSetClipboardString(NULL, copiedText.c_str());
    }
    else if (key::pasteClipboard.getKey(window, key) && action == GLFW_PRESS)
    {
        text.insert(glfwGetClipboardString(NULL));
    }
    else if (key::selectAll.getKey(window, key) && action == GLFW_PRESS)
    {
        text.setCursor(0);
        text.setSelecting(true);
        text.setSelection(-(int)text.getText().length());
        text.updateSelection();
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void character_callback(GLFWwindow* window, unsigned int codepoint)
{
    text.insert((char)codepoint);
}