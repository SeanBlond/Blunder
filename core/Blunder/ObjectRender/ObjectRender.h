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

namespace obr
{
    enum RenderMode { OR_SIMPLE, OR_XRAY, OR_SHADED, OR_RENDER };

    class ObjectReference
    {
    public:
        ObjectReference(obj::Object* object) : object(object), showChildren(false) {}

        // Getters
        obj::Object* getObject() { return object; }
        bool getShowChildren() { return showChildren; }
        
        // Setters
        void setObject(obj::Object* object) { this->object = object; }
        void setShowChildren(bool showChildren) { this->showChildren = showChildren; }

        // Functions
        void renderObjectReference(ui::UIRenderer* renderer, float xpos, float ypos);

    private:
        obj::Object* object;
        bool showChildren;
    };

    class ObjectRender
    {
    public:
        // Constructor & deconstructor
        ObjectRender(RenderMode mode = OR_SIMPLE, bool renderTriangles = true, bool backfaceCull = true)
            : mode(mode), renderTriangles(renderTriangles), backfaceCull(backfaceCull) {}

        // Getters
        obj::Object* getObject(int ID) { return objects[findObject(ID)]; }
        std::vector<obj::Object*> getObjects() { return objects; }
        int getObjectCount() { return objects.size(); }
        RenderMode getMode() { return mode; }

        // Setters
        void setMode(RenderMode mode) { this->mode = mode; }

        // Functions
        void addObject(obj::Object* object) { objects.push_back(object); }
        int findObject(int ID); // returns index of the object;
        void removeObject(int ID) { objects.erase(objects.begin() + findObject(ID)); }
        void renderObjects(glm::mat4 projection, glm::mat4 view);

        void testOutput();

    private:
        std::vector<obj::Object*> objects;
        std::vector<obj::Folder*> folders;
        RenderMode mode;
        bool renderTriangles;
        bool backfaceCull;
    };
}

#endif // !OBJECT_RENDER
