#include "HierarchyElement.h"

// Base Hierarchy Element Class Functions
// --------------------------------------
// Constructor
HierarchyElement::HierarchyElement(obj::Object* object, HierarchyType type, HierarchyElement* parent, bool displayed, bool rendered)
{
    this->object = object;
    this->type = type;
    this->displayed = displayed;
    this->rendered = rendered;
    this->parent = parent;
    this->dropdown = true;
}

void HierarchyElement::changeParent(HierarchyElement* parent)
{
    // Checking if parent exists
    if (parent != nullptr)
    {
        this->parent->removeChild(this);
        this->parent = parent;
        parent->addChild(this);
    }
}
void HierarchyElement::addChild(HierarchyElement* child)
{
    children.push_back(child);
    child->changeParent(this);
}
void HierarchyElement::removeChild(HierarchyElement* element)
{
    // Checking if element is in vector
    int index = smath::vectorFind(children, element);
    if (index != -1)
    {
        children.erase(children.begin() + index);
    }
}

void HierarchyElement::EraseObject(bool deleteChildren)
{
    // Removing this object from the parent
    if (parent != nullptr)
        parent->removeChild(this);

    // Deleting object data
    delete object;
    object = nullptr;

    // Moving children to the parent
    if (!deleteChildren)
    {
        for (int i = 0; i < children.size(); i++)
        {
            children[i]->changeParent(parent);
        }
    }
    // Deleting children with the object
    else
    {
        for (int i = 0; i < children.size(); i++)
        {
            delete children[i];
            children[i] = nullptr;
        }
        children.clear();
    }
}

void HierarchyElement::DrawElementUI()
{
    std::cout << "TODO: DRAW UI ELEMENT" << std::endl;
}