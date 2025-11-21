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
#include "Elements/Folder.h"
#include "Elements/HierarchyElement.h"

namespace obs
{

    enum RenderMode { OR_SIMPLE, OR_XRAY, OR_SHADED, OR_RENDER };
    struct RenderSettings
    {
        RenderMode mode;
        bool renderTriangles;
        bool backfaceCull;
    };

    class ObjectSystem
    {
    public:
        // Constructor & Deocnstructor
        ObjectSystem();
        ~ObjectSystem();

        // Getters
        Folder* getSelectedFolder() { return selectedFolder; }
        Folder* getRootFolder() { return rootFolder; }
        RenderSettings getRenderSettings() { return renderSettings; }

        // Setters
        void setSelectedFolder(Folder* folder) { this->selectedFolder = folder; }
        void setRenderSettings(RenderSettings renderSettings) { this->renderSettings = renderSettings; }

        //Functions
        void addObject(obj::Object* object);
        void addHierarchyElement(HierarchyElement* element);
        void addFolder(std::string name);
        void RenderHierarchyElement(HierarchyElement* element, glm::mat4 projection, glm::mat4 view);
        void RenderFolder(Folder* folder, glm::mat4 projection, glm::mat4 view);
        void Render(glm::mat4 projection, glm::mat4 view);

    private:
        Folder* selectedFolder;
        Folder* rootFolder;
        RenderSettings renderSettings;
    };
}
#endif // !OBJECTSYSTEM
