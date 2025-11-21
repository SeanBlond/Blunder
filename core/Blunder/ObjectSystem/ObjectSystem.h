#ifndef OBJECTSYSTEM
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
#include "ObjectRender.h"
#include "Elements/Folder.h"
#include "Elements/HierarchyElement.h"

namespace obs
{

    class ObjectSystem
    {
    public:
        // Constructor & Deocnstructor
        ObjectSystem();
        ~ObjectSystem();

        // Getters
        Folder* getSelectedFolder() { return selectedFolder; }
        Folder* getRootFolder() { return rootFolder; }
        ObjectRender* getRenderer() { return &renderer; }

        // Setters
        void setSelectedFolder(Folder* folder) { this->selectedFolder = folder; }

        //Functions
        void addObject(obj::Object* object);
        void addHierarchyElement(HierarchyElement* element);
        void addFolder(std::string name);

        void outputFolder(Folder* folder, int offset);
        void testOutput();

    private:
        Folder* selectedFolder;
        Folder* rootFolder;
        ObjectRender renderer;
    };
}
#endif // !OBJECTSYSTEM
