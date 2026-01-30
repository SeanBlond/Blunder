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
    FloatAttribute(std::string label, float value, bool hasLimits, float lowerLimit, float upperLimit) : attributeLabel(label), attributeValue(value), hasLimits(hasLimits) { attributeLimits[0] = lowerLimit; attributeLimits[1] = upperLimit; }
    std::string attributeLabel;
    float attributeValue;
    bool hasLimits;
    float attributeLimits[2];
};
struct IntAttribute
{
    IntAttribute(std::string label, int value, bool hasLimits, int lowerLimit, int upperLimit) : attributeLabel(label), attributeValue(value), hasLimits(hasLimits) { attributeLimits[0] = lowerLimit; attributeLimits[1] = upperLimit; }
    std::string attributeLabel;
    int attributeValue;
    bool hasLimits;
    int attributeLimits[2];
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
    bool hasAttributes() { return (floatAttributes.size() + intAttributes.size()) > 0; }
    int getNumFloatAttributes() { return floatAttributes.size(); }
    int getNumIntAttributes() { return intAttributes.size(); }
    FloatAttribute* getFloatAttribute(int index) { index = smath::clamp(index, 0, (int)floatAttributes.size()); return floatAttributes[index]; }
    IntAttribute* getIntAttribute(int index) { index = smath::clamp(index, 0, (int)intAttributes.size()); return intAttributes[index]; }

    // Functions
    void DrawMesh(bool lines = false, bool points = false);
    void UpdateMesh(const Mesh& mesh);
    void outputMesh();
    void addFloatAttribute(std::string label, float value, bool hasLimits = false, float lowerLimit = 0, float upperLimit = 100);
    void addIntAttribute(std::string label, int value, bool hasLimits = false, int lowerLimit = 1, int upperLimit = 128);

    // Operators
    Mesh operator=(const Mesh& aMesh);

private:
    unsigned int VBO, EBO;
    PrimitiveType type;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    std::vector<FloatAttribute*> floatAttributes;
    std::vector<IntAttribute*> intAttributes;
    std::vector<FloatAttribute*> storedFloatAttributes;
    std::vector<IntAttribute*> storedIntAttributes;

    void createMesh();
    bool PrimitiveChanges();
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