#include "../UIWindows.h"
using namespace ui;

// Viewport Window Functions
void ViewportWindow::UpdateWindow()
{
    // Setting ViewNav Positions
    glm::vec4 navCorners = glm::vec4(
        position.getWidth() - viewNavElement.getNavSize() + position.getXOffset(),
        position.getHeight() - viewNavElement.getNavSize() + position.getYOffset(),
        position.getWidth() + position.getXOffset(),
        position.getHeight() + position.getYOffset()
    );
    ui::ElementPosition elementPos(navCorners, position.getXOffset(), &position);
    viewNavElement.UpdateElement(elementPos);
}
void ViewportWindow::DrawWindow(ui::UIRenderer* renderer)
{
    // Rendering the Scene to the Viewport
    RenderScene();

    // Setting ViewNav draw values
    glm::mat4 transform = smath::orthographic(-2, 2, -2, 2, 0.1f, 100.0f) * state->getCamera()->getViewMatrix() * smath::scale(glm::vec3(1.5f));
    viewNavElement.setTransform(transform);

    // Drawing the ViewNav
    viewNavElement.RenderElement(renderer, smallText());

    // Setting viewport size
    glm::vec4 viewportSize = glm::vec4(
        position.getXOffset(),
        position.getYOffset(),
        position.getWidth(),
        position.getHeight()
    );
    glViewport(viewportSize.x, viewportSize.y, viewportSize.z, viewportSize.w);
}
void ViewportWindow::ManageInteraction(GLFWwindow* window, StateMachine* state)
{
    // Getting the mouse Position (flipping the y because glfw is stupid)
    glm::vec2 mousePos = glm::vec2(
        state->getMouse()->mousePos.x,
        (float)(state->getWindowDimensions().y) - state->getMouse()->mousePos.y
    );

    // Checking ViewNav for collision
    if (viewNavElement.checkCollision(mousePos) && !state->getTransforming())
    {
        // Clicking an Element
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) && clickedElement == nullptr)
        {
            state->changeState(SM_UI_INTERACT);
            viewNavElement.clicked = true;
            clickedElement = &viewNavElement;
            clickedElement->OnClick(state);
        }

        // Highilighting an Element
        else
            viewNavElement.highlighted = true;
    }

    // Unhighlighting an Element
    else if (viewNavElement.highlighted)
        viewNavElement.highlighted = false;

    // Managing Clicked Element
    if (clickedElement != nullptr)
    {
        clickedElement->OnHold(state);

        // Unclicking an Element
        if (clickedElement->clicked && !glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1))
        {
            clickedElement->OnRelease(state);
            clickedElement->clicked = false;
            clickedElement = nullptr;
        }
    }
}
void ViewportWindow::UnselectWindow()
{
    // Unclicking element
    if (clickedElement != nullptr)
    {
        clickedElement->clicked = false;
        clickedElement->highlighted = false;
        clickedElement = nullptr;

    }
}
void ViewportWindow::RenderScene()
{
    // Getting the Matrices
    glm::mat4 projection = state->getCamera()->getProjectionMatrix(position.getAspectRatio(), 0.01f, 1000.0f);
    glm::mat4 view = state->getCamera()->getViewMatrix();

    // Rendering the scene
    glViewport(position.getXOffset(), position.getYOffset(), position.getWidth(), position.getHeight());
    state->getScene()->Render(projection, view);

    // Drawing Axis Lines
    viewportShader->useShader();
    viewportShader->setMat4("transform", projection * view);
    viewportMesh->DrawMesh(true, false);
}
void ViewportWindow::CreateMesh()
{
    // Create Mesh
    // Mesh vertices & indices
    std::vector<Vertex> vertices =
    {
        // Position                            Color                     TexCoord (not used)
        Vertex({glm::vec3(0, 0, 0) * 1000.0f,  colors::red.rgb(),        glm::vec2(0) }),
        Vertex({glm::vec3(1, 0, 0) * 1000.0f,  colors::red.rgb(),        glm::vec2(0) }),  //  X : 1
        Vertex({glm::vec3(0, 0, 0) * 1000.0f,  colors::darkRed.rgb(),    glm::vec2(0) }),
        Vertex({glm::vec3(-1, 0, 0) * 1000.0f,  colors::darkRed.rgb(),    glm::vec2(0) }),  // -X : 3
        Vertex({glm::vec3(0, 0, 0) * 1000.0f,  colors::green.rgb(),      glm::vec2(0) }),
        Vertex({glm::vec3(0, 1, 0) * 1000.0f,  colors::green.rgb(),      glm::vec2(0) }),  //  Y : 5
        Vertex({glm::vec3(0, 0, 0) * 1000.0f,  colors::darkGreen.rgb(),  glm::vec2(0) }),
        Vertex({glm::vec3(0, -1, 0) * 1000.0f,  colors::darkGreen.rgb(),  glm::vec2(0) }),  // -Y : 7
        Vertex({glm::vec3(0, 0, 0) * 1000.0f,  colors::blue.rgb(),       glm::vec2(0) }),
        Vertex({glm::vec3(0, 0, 1) * 1000.0f,  colors::blue.rgb(),       glm::vec2(0) }),  //  Z : 9
        Vertex({glm::vec3(0, 0, 0) * 1000.0f,  colors::darkBlue.rgb(),   glm::vec2(0) }),
        Vertex({glm::vec3(0, 0, -1) * 1000.0f,  colors::darkBlue.rgb(),   glm::vec2(0) }),  // -Z : 11
    };
    std::vector<unsigned int> indices =
    {
        0, 1, 0,
        2, 3, 2,
        4, 5, 4,
        6, 7, 6,
        8, 9, 8,
        10, 11, 10,
    };

    // Creating mesh object
    viewportMesh = new Mesh(vertices, indices);

    // Creating Shaders
    const char* viewportVertexShader = R"(
        #version 330 core
        layout (location = 0) in vec3 vertex;
        layout (location = 1) in vec3 LineColor;
        layout (location = 2) in vec2 TexCoord;
        out vec2 TexCoords;
        out vec3 Color;
        
        uniform mat4 transform;

        void main()
        {
            gl_Position = transform * vec4(vertex, 1.0);
            TexCoords = TexCoord;
            Color = LineColor;
        }  
        )";

    const char* viewportFragmentShader = R"(
        #version 330 core
        in vec2 TexCoords;
        in vec3 Color;
        out vec4 fragColor;

        void main()
        {    
            fragColor = vec4(Color, 1.0);
        }  
        )";

    // Creating shader object
    viewportShader = new shdr::Shader(viewportVertexShader, viewportFragmentShader, 1);
}