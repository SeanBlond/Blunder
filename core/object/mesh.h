#ifndef MESH
#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include "../ew/external/glad.h"
#include <GLFW/glfw3.h>
#include "../shader/shader.h"
#include "../math/smath.h"

enum PrimitiveType { MESH_NONE, MESH_PLANE, MESH_CUBE, MESH_SPHERE, MESH_CYLINDER, MESH_TORUS };

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoord;
};

// Values that primitive meshes could have
struct FloatAttribute
{
    FloatAttribute(std::string label, float value) : attributeLabel(label), attributeValue(value) {}
    std::string attributeLabel;
    float attributeValue;
};
struct IntAttribute
{
    IntAttribute(std::string label, int value) : attributeLabel(label), attributeValue(value) {}
    std::string attributeLabel;
    int attributeValue;
};

class Mesh
{
public:
    unsigned int VAO;

    // Constructors
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, PrimitiveType type = MESH_NONE);
    Mesh(Mesh* mesh);

    // Getters
    PrimitiveType getType() { return type; }
    std::vector<FloatAttribute*> getFloatAttributes() { return floatAttributes; }
    std::vector<IntAttribute*> getIntAttributes() { return intAttributes; }

    // Functions
    void DrawMesh(bool lines = false, bool points = false);
    void updateMesh(const Mesh& mesh);
    void outputMesh();
    void addFloatAttribute(std::string label, float value);
    void addIntAttribute(std::string label, int value);

    // Operators
    Mesh operator=(const Mesh& aMesh);

private:
    unsigned int VBO, EBO;
    PrimitiveType type;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    std::vector<FloatAttribute*> floatAttributes;
    std::vector<IntAttribute*> intAttributes;

    void createMesh();
};

namespace mesh
{
    // Mesh Building Functions
    extern Mesh* loadFromFile(std::string filePath);
    extern Mesh* createSphere(float radius, int segments);
    extern Mesh* createCylinder(float height, float radius, int segments);
    extern Mesh* createPlane(float width, float height, int segments);
    extern Mesh* createTorus(float majorRadius, float minorRadius, int majorSegments, int minorSegments);
    extern Mesh* createCube(float length, float width, float height, int segments);
};

#endif