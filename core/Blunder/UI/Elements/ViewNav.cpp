#include "../UIElements.h"
using namespace ui;

// ViewNav Element
void ViewNav::CreateMesh()
{
    // Create Mesh
    // Mesh vertices & indices
    std::vector<Vertex> vertices =
    {
        // Position                     Color                     TexCoord (not used)
        Vertex({glm::vec3(0, 0, 0),    colors::red.rgb(),       glm::vec2(0) }),
        Vertex({glm::vec3(1, 0, 0),    colors::red.rgb(),       glm::vec2(0) }),  //  X : 1
        Vertex({glm::vec3(0, 0, 0),    colors::darkRed.rgb(),   glm::vec2(0) }),
        Vertex({glm::vec3(-1, 0, 0),   colors::darkRed.rgb(),   glm::vec2(0) }),  // -X : 3
        Vertex({glm::vec3(0, 0, 0),    colors::green.rgb(),     glm::vec2(0) }),
        Vertex({glm::vec3(0, 1, 0),    colors::green.rgb(),     glm::vec2(0) }),  //  Y : 5
        Vertex({glm::vec3(0, 0, 0),    colors::darkGreen.rgb(), glm::vec2(0) }),
        Vertex({glm::vec3(0, -1, 0),   colors::darkGreen.rgb(), glm::vec2(0) }),  // -Y : 7
        Vertex({glm::vec3(0, 0, 0),    colors::blue.rgb(),      glm::vec2(0) }),
        Vertex({glm::vec3(0, 0, 1),    colors::blue.rgb(),      glm::vec2(0) }),  //  Z : 9
        Vertex({glm::vec3(0, 0, 0),    colors::darkBlue.rgb(),  glm::vec2(0) }),
        Vertex({glm::vec3(0, 0, -1),   colors::darkBlue.rgb(),  glm::vec2(0) }),  // -Z : 11
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
    navMesh = new Mesh(vertices, indices);

    // Creating Shaders
    const char* navVertexShader = R"(
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

    const char* navFragmentShader = R"(
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
    navShader = new shdr::Shader(navVertexShader, navFragmentShader, 1);

}
ViewNav::~ViewNav()
{
    delete navMesh;
    delete navShader;
    navMesh = nullptr;
    navShader = nullptr;
}

// Update Function
void ViewNav::UpdateElement(const ElementPosition& newPosition)
{
    this->position = newPosition;

    // TODO: Update interactable
}

// Functions
void ViewNav::RenderElement(UIRenderer* renderer, float textSize)
{
    float xCenter = position.left_x + navSize * 0.5f - position.split;
    float yCenter = position.bottom_y + navSize * 0.5f;

    // Updating screen position
    screenPos = glm::vec3(xCenter, navSize * 0.5f, navSize) + glm::vec3(position.parentWindow->offset, 0.0f);

    // Adding background when highlighted
    if (highlighted || clicked)
    {
        renderer->addQuad(glm::vec3(xCenter, yCenter, -0.99999f), glm::vec2(navSize), colors::lightgrey.rgb(), position.parentWindow->offset, UI_NO_TEXTURE, QUAD_CIRCLE);
    }

    // Unique data for each axis
    glm::vec3 axisColors[] = { colors::white.rgb(), colors::red.rgb(), colors::darkRed.rgb(), colors::green.rgb(), colors::darkGreen.rgb(), colors::blue.rgb(), colors::darkBlue.rgb() };
    std::string axisTitles[] = { "", "+X", "-X", "+Y", "-Y", "+Z", "-Z" };

    // Adding quads for each point
    glm::vec4 axises[7] =
    {
        glm::vec4(0, 0, 0, 1),   // POSITIVE_X
        glm::vec4(1, 0, 0, 1),   // POSITIVE_X
        glm::vec4(-1, 0, 0, 1),  // NEGATIVE_X
        glm::vec4(0, 1, 0, 1),   // POSITIVE_Y
        glm::vec4(0, -1, 0, 1),  // NEGATIVE_Y
        glm::vec4(0, 0, 1, 1),   // POSITIVE_Z
        glm::vec4(0, 0, -1, 1),  // NEGATIVE_Z
    };
    for (int i = 0; i < 7; i++)
    {
        // Getting the coordinate of the axis point
        glm::vec3 axisPos = glm::vec3(xCenter, yCenter, 0) + glm::vec3(navSize * 0.5f, navSize * 0.5f, -1) * glm::vec3(transform * axises[i]);

        // Adding the quad for each axis
        renderer->addQuad(axisPos, glm::vec2(navSize * 0.15f), axisColors[i], position.parentWindow->offset, UI_NO_TEXTURE, QUAD_CIRCLE);

        // (Conditionally) Adding the text 
        if (highlighted || clicked)
        {
            glm::vec3 textColor = (i % 2 == 0 ? glm::vec3(1) : glm::vec3(0));
            renderer->addText(axisTitles[i], axisPos + glm::vec3(0, 0, 0.01f), navSize * 0.0015f, textColor, position.parentWindow->offset, CENTER);
        }
    }

    // Setting up viewport to be drawn to
    glViewport(position.left_x, position.bottom_y, navSize, navSize);

    // Shader settings
    navShader->useShader();
    navShader->setMat4("transform", transform);

    // Drawing lines
    navMesh->DrawMesh(true, false, false);
}
void ViewNav::OnClick(StateMachine* state)
{
    // Storing Initial Mouse Pos and camera settings
    slideStarted = false;
    initialMousePos = state->getMouse()->mousePos;
    storedCameraOrbit = state->getCamera()->getAngles();
}
void ViewNav::OnHold(StateMachine* state)
{
    // Checking if value should slide with mouse
    if (glm::length(state->getMouse()->mousePos - initialMousePos) > 1.0f || slideStarted)
    {
        // Setting state to be UI Transforming
        if (state->getState() != SM_UI_INTERACT)
        {
            state->changeState(SM_UI_INTERACT);
        }

        slideStarted = true;
        glm::vec2 mouseDelta = state->getMouse()->mouseDelta;
        storedCameraOrbit += mouseDelta * speed;
        state->getCamera()->setAngles(storedCameraOrbit);
    }
}
void ViewNav::OnRelease(StateMachine* state)
{
    if (slideStarted)
    {
        state->exitState();
        slideStarted = false;
    }
    else
    {
        // Getting the mouse position relative to the center of the ViewNav
        glm::vec2 relativeMousePos = (state->getMouse()->mousePos - glm::vec2(screenPos)) / (0.5f * screenPos.z) * glm::vec2(1, -1);

        // Checking which point the mouse is closest to (if at all)
        ViewAxis closestAxis = getClosestAxis(relativeMousePos, 0.2f);

        // Setting the View Axis (if a axis button was clicked)
        if (closestAxis != VIEW_NONE)
        {
            if (closestAxis == VIEW_POSITIVE_X)
            {
                state->getCamera()->setAngles(0, smath::PI / 2);
            }
            else if (closestAxis == VIEW_NEGATIVE_X)
            {
                state->getCamera()->setAngles(smath::PI, smath::PI / 2);
            }
            else if (closestAxis == VIEW_POSITIVE_Y)
            {
                state->getCamera()->setAngles(smath::PI / 2, 0);
            }
            else if (closestAxis == VIEW_NEGATIVE_Y)
            {
                state->getCamera()->setAngles(smath::PI / 2, smath::PI);
            }
            else if (closestAxis == VIEW_POSITIVE_Z)
            {
                state->getCamera()->setAngles(smath::PI / 2, smath::PI / 2);
            }
            else if (closestAxis == VIEW_NEGATIVE_Z)
            {
                state->getCamera()->setAngles(-smath::PI / 2, smath::PI / 2);
            }
        }
    }
}
ViewNav::ViewAxis ViewNav::getClosestAxis(glm::vec2 position, float distanceClamp) const
{
    // Checking which point the position is closest to (if it's close at all)
    ViewAxis closestAxis = VIEW_NONE;
    float depth = 0; // For checking if an axis is "above" another

    // Temporary array of each possible axis
    glm::vec4 axises[6] =
    {
        glm::vec4(1, 0, 0, 1),   // POSITIVE_X
        glm::vec4(-1, 0, 0, 1),  // NEGATIVE_X
        glm::vec4(0, 1, 0, 1),   // POSITIVE_Y
        glm::vec4(0, -1, 0, 1),  // NEGATIVE_Y
        glm::vec4(0, 0, 1, 1),   // POSITIVE_Z
        glm::vec4(0, 0, -1, 1),  // NEGATIVE_Z
    };

    // Checking each axis
    for (int i = 0; i < 6; i++)
    {
        glm::vec3 testPos = glm::vec3(transform * axises[i]);
        if (glm::distance(position, glm::vec2(testPos)) <= distanceClamp && testPos.z < depth)
        {
            closestAxis = (ViewAxis)i;
            depth - testPos.z;
        }
    }

    // Returning closest axis
    return closestAxis;

}