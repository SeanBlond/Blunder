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

    class ObjectSystem
    {
    public:
        ObjectSystem();

    private:
        std::vector<Folder*> folders;
    };
}
#endif // !OBJECTSYSTEM
