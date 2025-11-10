#include "object.h"

using namespace obj;

Object::Object(const char* name, Mesh* mesh, shdr::Shader* shader, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
{
    this->name = name;
    this->mesh = mesh;
    this->transform = Transform(position, rotation, scale);
    this->shader = shader;
    this->displayed = true;
    this->parent = nullptr;
    originalPosition = position;
    originalRotation = rotation;
    originalScale = scale;

    // ID Management
    id = nextID;
    nextID++;
}
Object::Object(const char* name, Mesh* mesh, Object* parent, shdr::Shader* shader, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
{
    this->name = name;
    this->mesh = mesh;
    this->transform = Transform(position, rotation, scale);
    this->shader = shader;
    this->displayed = true;
    setParent(parent);
    originalPosition = position;
    originalRotation = rotation;
    originalScale = scale;

    // ID Management
    id = nextID;
    nextID++;
}

// Destructor
Object::~Object()
{
    delete mesh;
    mesh = nullptr;
}

// Getters
std::string Object::getName() const
{
    return name;
}
Mesh* Object::getMesh() const
{
    return this->mesh;
}

// Functions
void Object::DrawMesh(bool lines, bool points)
{
    shader->useShader();
    glm::mat4 model = glm::mat4(1.0f);

    // Checking for Parent Transformation
    if (parent != nullptr)
        model *= parent->transform.getTransformMatrix();

    // Calculating Transformations
    model *= transform.getTransformMatrix();

    shader->setMat4("model", model);
    mesh->DrawMesh(lines, points);
}

void Object::resetPosition()
{
    transform.position = originalPosition;
}
void Object::resetRotation()
{
    transform.rotation = originalRotation;
}
void Object::resetScale()
{
    transform.scale = originalScale;
}
void Object::setParent(Object* parent)
{
    if (this->parent != nullptr)
        removeParent();

    this->parent = parent;
    parent->addChild(this);
}
void Object::addChild(Object* child)
{
    children.push_back(child);
}
void Object::removeParent()
{
    parent->removeChild(this);
    parent = nullptr;
}
void Object::removeChild(Object* child)
{
    int childIndex = findChild(child->getID());

    if (childIndex == -1)
        return;
    else
    {
        children.erase(children.begin() + childIndex);
    }
}
int Object::findChild(int id)
{
    for (int i = 0; i < children.size(); i++)
    {
        if (children[i]->getID() == id)
            return id;
    }
    return -1;
}

// Operator Overloads
Object Object::operator=(const Object& aObject)
{
    this->name = aObject.name;
    this->mesh = aObject.mesh;
    this->transform = aObject.transform;

    return *this;
}