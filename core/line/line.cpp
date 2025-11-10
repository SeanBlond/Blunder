#include "line.h"

// Constructor
Line::Line()
{
    this->startPosition = glm::vec3(0.0f);
    this->endPosition = glm::vec3(0.0f);
    this->lineColor = glm::vec3(1.0f);
    this->lineThickness = 1.0f;
    this->transformation = glm::mat4(1.0f);

    createLine();
}
Line::Line(glm::vec3 startPos, glm::vec3 endPos, glm::vec3 color, float thickness)
{
    this->startPosition = startPos;
    this->endPosition = endPos;
    this->lineColor = color;
    this->lineThickness = thickness;
    this->transformation = glm::mat4(1.0f);

    createLine();
}
Line::~Line()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
}

// Setters
void Line::setPositions(glm::vec3 start, glm::vec3 end)
{
    this->startPosition = start;
    this->endPosition = end;
}
void Line::setStartPositions(glm::vec3 start)
{
    this->startPosition = start;
}
void Line::setEndPositions(glm::vec3 end)
{
    this->endPosition = end;
}
void Line::setColor(glm::vec3 color)
{
    this->lineColor = color;
}
void Line::setThickness(float value)
{
    this->lineThickness = value;
}
void Line::setTransformation(glm::mat4 transform)
{
    this->transformation = transform;
}

// Getters
glm::vec3 Line::getStartPosition()
{
    return startPosition;
}
glm::vec3 Line::getEndPosition()
{
    return endPosition;
}
glm::vec3 Line::getColor()
{
    return lineColor;
}
float Line::getThickness()
{
    return lineThickness;
}
glm::mat4 Line::getTransformation()
{
    return transformation;
}

// Functions
void Line::drawLine()
{
    glUseProgram(shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "transformation"), 1, GL_FALSE, &transformation[0][0]);
    glUniform3fv(glGetUniformLocation(shaderProgram, "color"), 1, &lineColor[0]);


    glLineWidth(lineThickness);
    glBindVertexArray(VAO);
    glDrawArrays(GL_LINES, 0, 2);
    glBindVertexArray(0);
}
void Line::createLine()
{
    // Creating the Shader
    const char* vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        uniform mat4 transformation;
        void main()
        {
           gl_Position = transformation * vec4(aPos, 1.0);
        }
        )";

    const char* fragmentShaderSource = R"(
        #version 330 core
        out vec4 FragColor;
        uniform vec3 color;
        void main()
        {
           FragColor = vec4(color, 1.0f);
        }
        )";

    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors

    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors

    // link shaders
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    vertices = {
         startPosition.x, startPosition.y, startPosition.z,
         endPosition.x, endPosition.y, endPosition.z,
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}