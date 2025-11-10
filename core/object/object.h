#ifndef OBJECT
#pragma once

#include "mesh.h"
#include "../math/smath.h"
#include "transform.h"


namespace obj
{
    class Object
    {
    public:
        shdr::Shader* shader;

        // Constructor
        Object(const char* name, Mesh* mesh, shdr::Shader* shader, glm::vec3 position = glm::vec3(0), glm::vec3 rotation = glm::vec3(0), glm::vec3 scale = glm::vec3(1));
        Object(const char* name, Mesh* mesh, Object* parent, shdr::Shader* shader, glm::vec3 position = glm::vec3(0), glm::vec3 rotation = glm::vec3(0), glm::vec3 scale = glm::vec3(1));

        // Destructor
        ~Object();

        // Getters
        std::string getName() const;
        Mesh* getMesh() const;
        bool getDisplayed() const { return displayed; }
        Object* getParent() const { return parent; }
        int getID() const { return id; }
        std::vector<Object*> getChildren() { return children; }

        // Setters
        void setMesh(Mesh* mesh) { this->mesh = mesh; }
        void setDisplayed(bool displayed) { this->displayed = displayed; }
        void setParent(Object* parent);

        // Functions
        void DrawMesh(bool lines = false, bool points = false);
        void resetPosition();
        void resetRotation();
        void resetScale();
        void toggleDisplayed() { displayed = !displayed; }
        void addChild(Object* child);
        void removeParent();
        void removeChild(Object* child);
        int findChild(int id);
        bool hasChildren() { return (children.size() > 0); }

        // Operator Overloads
        Object operator=(const Object& aObject);

        Transform transform;
    private:
        std::string name;
        Mesh* mesh;
        Object* parent;
        std::vector<Object*> children;
        bool displayed;
        int id;
        static int nextID;
        glm::vec3 originalPosition;
        glm::vec3 originalRotation;
        glm::vec3 originalScale;
    };

    class Folder
    {
    public:
        Folder();

    private:
        std::string name;
        bool display;
        std::vector<Object*> members;
    };
};

#endif