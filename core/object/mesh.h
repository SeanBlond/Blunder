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

class Mesh
{
public:
    unsigned int VAO;

    // Constructors
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, PrimitiveType type = MESH_NONE);
    Mesh(Mesh* mesh);

    // Getters
    PrimitiveType getType() { return type; }

    // Functions
    virtual void DrawMesh(bool lines = false, bool points = false);
    void updateMesh(Mesh* mesh);
    void outputMesh();

    // Operators
    Mesh operator=(const Mesh& aMesh);

private:
    unsigned int VBO, EBO;
    PrimitiveType type;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    void createMesh();
};

// Primitve Mesh Children (I think this is one my favorite comments I've ever written)
class PlaneMesh : public Mesh
{
public:
    // Constructor
    PlaneMesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, float width, float height, int subdivisions)
        : storedWidth(width), storedHeight(height), storedSubdivisions(subdivisions), Mesh(vertices, indices, MESH_PLANE) 
    {
        this->width = storedWidth;
        this->height = storedHeight;
        this->subdivisions = storedSubdivisions;
    }

    void DrawMesh(bool lines = false, bool points = false) override;

    float width;
    float height;
    int subdivisions;
private:
    float storedWidth;
    float storedHeight;
    int storedSubdivisions;
};
class CubeMesh : public Mesh
{
public:
    // Constructor
    CubeMesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, float width, float height, float length, int subdivisions)
        : storedWidth(width), storedHeight(height), storedLength(length), storedSubdivisions(subdivisions), Mesh(vertices, indices, MESH_CUBE) 
    {
        this->width = storedWidth;
        this->height = storedHeight;
        this->length = storedLength;
        this->subdivisions = storedSubdivisions;
    }

    void DrawMesh(bool lines = false, bool points = false) override;

    float width;
    float height;
    float length;
    int subdivisions;
private:
    float storedWidth;
    float storedHeight;
    float storedLength;
    int storedSubdivisions;
};
class SphereMesh : public Mesh
{
public:
    // Constructor
    SphereMesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, float radius, int subdivisions)
        : storedRadius(radius), storedSubdivisions(subdivisions), Mesh(vertices, indices, MESH_SPHERE)
    {
        this->radius = storedRadius;
        this->subdivisions = storedSubdivisions;
    }

    void DrawMesh(bool lines = false, bool points = false) override;

    float radius;
    int subdivisions;
private:
    float storedRadius;
    int storedSubdivisions;
};
class CylinderMesh : public Mesh
{
public:
    // Constructor
    CylinderMesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, float radius, float height, int subdivisions)
        : storedRadius(radius), storedHeight(height), storedSubdivisions(subdivisions), Mesh(vertices, indices, MESH_CYLINDER)
    {
        this->radius = storedRadius;
        this->height = storedHeight;
        this->subdivisions = storedSubdivisions;
    }

    void DrawMesh(bool lines = false, bool points = false) override;

    float radius;
    float height;
    int subdivisions;
private:
    float storedRadius;
    float storedHeight;
    int storedSubdivisions;
};
class TorusMesh : public Mesh
{
public:
    // Constructor
    TorusMesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, float majorRadius, float minorRadius, int majorSegments, int minorSegments)
        : storedMajorRadius(majorRadius), storedMinorRadius(minorRadius), storedMajorSegments(majorSegments), storedMinorSegments(minorSegments), Mesh(vertices, indices, MESH_TORUS)
    {
        this->majorRadius = storedMajorRadius;
        this->minorRadius = storedMinorRadius;
        this->majorSegments = storedMajorSegments;
        this->minorSegments = storedMinorSegments;
    }

    void DrawMesh(bool lines = false, bool points = false) override;

    float majorRadius;
    float minorRadius;
    int majorSegments;
    int minorSegments;
private:
    float storedMajorRadius;
    float storedMinorRadius;
    int storedMajorSegments;
    int storedMinorSegments;
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