#ifndef MATERIAL
#pragma once

#include <iostream>
#include <fstream>
#include "shader.h"
#include "../ew/external/stb_image.h"
#include "../ew/external/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>


class Material
{
public:
    Material();

private:
    std::string name;
};

#endif // !MATERIAL
