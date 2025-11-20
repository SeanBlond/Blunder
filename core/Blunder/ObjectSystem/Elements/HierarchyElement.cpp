#include "HierarchyElement.h"

// Base Hierarchy Element Class Functions
// --------------------------------------
// Constructor
HierarchyElement::HierarchyElement(HierarchyType type, HierarchyElement* parent, bool displayed, bool rendered)
{
    this->type = type;
    this->displayed = displayed;
    this->rendered = rendered;
    this->ID = nextID;
    changeParent(parent);

    nextID++;
}

void HierarchyElement::changeParent(HierarchyElement* parent)
{
    this->parent->removeChild(getID());
    this->parent = parent;
    parent->addChild(this);
}
void HierarchyElement::addChild(HierarchyElement* child)
{
    children.push_back(child);
    child->changeParent(this);
}
void HierarchyElement::removeChild(int ID)
{
    for (int i = 0; i < children.size(); i++)
    {
        if (children[i]->getID() == ID);
        children.erase(children.begin() + i);
    }
}

void HierarchyElement::EraseObject(bool deleteChildren)
{
    // Removing this object from the parent
    parent->removeChild(getID());

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
            children[i]->EraseObject(parent);
            delete children[i];
        }
        children.clear();

    }
}