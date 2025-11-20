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
        ObjectSystem();

        // Getters
        Folder* getSelectedFolder() { return selectedFolder; }
        Folder* getFolder(int index) { return folders[index]; }
        std::vector<Folder*> getFolders() { return folders; }
        ObjectRender* getRender() { return &render; }

        // Setters
        void setSelectedFolder(Folder* folder) { this->selectedFolder = folder; }

        //Functions
        void addObject(obj::Object object);
        void addHierarchyElement(HierarchyElement* element);

    private:
        Folder* selectedFolder;
        std::vector<Folder*> folders;
        ObjectRender renderer;
    };
}
#endif // !OBJECTSYSTEM
