#ifndef OBJECT_RENDER
#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include "../../ew/ewMath/ewMath.h"
#include "../../ew/external/glad.h"
#include <GLFW/glfw3.h>

#include "../UI/UIRenderer.h"
#include "../../math/smath.h"
#include "../../shader/shader.h"
#include "../../object/object.h"

namespace obs
{
    enum RenderMode { OR_SIMPLE, OR_XRAY, OR_SHADED, OR_RENDER };

    class ObjectRender
    {
    public:
        // Constructor & deconstructor
        ObjectRender(RenderMode mode = OR_SIMPLE, bool renderTriangles = true, bool backfaceCull = true)
            : mode(mode), renderTriangles(renderTriangles), backfaceCull(backfaceCull) {}

        // Getters
        RenderMode getMode() { return mode; }

        // Setters
        void setMode(RenderMode mode) { this->mode = mode; }

        // Functions
        void renderObjects(glm::mat4 projection, glm::mat4 view);

    private:
        RenderMode mode;
        bool renderTriangles;
        bool backfaceCull;
    };
}

#endif // !OBJECT_RENDER
