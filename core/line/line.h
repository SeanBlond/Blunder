#ifndef LINE
#pragma once

#include <iostream>
#include <vector>
#include "../ew/external/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>


class Line 
{
public:
    // Constructor
    Line();
    Line(glm::vec3 startPos, glm::vec3 endPos, glm::vec3 color = glm::vec3(1.0f), float thickness = 1.0f);
    ~Line();

    // Setters
    void setPositions(glm::vec3 start, glm::vec3 end);
    void setStartPositions(glm::vec3 start);
    void setEndPositions(glm::vec3 end);
    void setColor(glm::vec3 color);
    void setThickness(float value);
    void setTransformation(glm::mat4 transform);

    // Getters
    glm::vec3 getStartPosition();
    glm::vec3 getEndPosition();
    glm::vec3 getColor();
    float getThickness();
    glm::mat4 getTransformation();

    // Functions
    void drawLine();

private:
    glm::vec3 startPosition;
    glm::vec3 endPosition;
    glm::vec3 lineColor;
    float lineThickness;
    glm::mat4 transformation;
    unsigned int VAO, VBO;
    std::vector<float> vertices;
    unsigned int shaderProgram;

    void createLine();
};

#endif // !LINE