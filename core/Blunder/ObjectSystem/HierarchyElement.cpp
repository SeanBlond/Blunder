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


void Folder::addElement(HierarchyElement* element)
{
    elements.push_back(element);
}
void Folder::removeElement(HierarchyElement* element, bool deleteChildren)
{
    // Checking if element exists within folder
    int index = smath::vectorFind(elements, element);
    if (index != -1)
    {
        // Adding children to this folder
        if (!deleteChildren)
        {
            for (int i = 0; i < element->getChildrenSize(); i++)
            {
                addElement(element->getChild(i));
            }
        }

        // Normally deleting the element
        elements[index]->EraseObject(deleteChildren);
        delete elements[index];
        elements.erase(elements.begin() + index);
    }
}
void Folder::moveElement(int index, Folder* folder)
{
    // Checking if index is within the range of the vector
    if (index >= 0 && index < elements.size())
    {
        folder->addElement(elements[index]);
        removeElement(elements[index]);
    }
}
void Folder::moveElement(HierarchyElement* element, Folder* folder)
{
    // Checking if element exists within folder
    int index = smath::vectorFind(elements, element);
    if (index != -1)
    {
        moveElement(index, folder);
    }
}

void Folder::changeParentFolder(Folder* parent)
{
    // Checking if parent folder can be changed to
    if (parent != nullptr)
    {
        this->parentFolder->removeChildFolder(this, false);
        this->parentFolder = parent;
        this->parentFolder->addChildFolder(this);
    }
}
void Folder::addChildFolder(Folder* child)
{
    childrenFolders.push_back(child);
}
void Folder::removeChildFolder(Folder* folder, bool deleteElements)
{
    // Checking if folder exists within the vector
    int index = smath::vectorFind(childrenFolders, folder);
    if (index != -1)
    {
        // Checking if elements should not be deleted
        if (!deleteElements) // Instead of deleting elements, elements are added to this folder
        {
            // Adding each hierarchy element to this folder
            for (int i = 0; i < folder->getHierarchyElementSize(); i++)
            {
                folder->moveElement(0, this);
            }

            // Adding child folders of erased folder to this folder
            for (int i = 0; i < folder->getChildFoldersSize(); i++)
            {
                folder->getChildFolder(0)->changeParentFolder(this);
            }
        }
        else // Elements are deleted
        {
            folder->EraseFolder();
        }
        delete childrenFolders[index];
        childrenFolders.erase(childrenFolders.begin() + index);
    }
}
void Folder::EraseFolder()
{
    // Erasing all elements
    for (int i = 0; i < elements.size(); i++)
    {
        delete elements[i];
        elements[i] = nullptr;
    }
    elements.clear();

    // Erasing the folders
    for (int i = 0; i < childrenFolders.size(); i++)
    {
        delete childrenFolders[i];
        childrenFolders[i] = nullptr;
    }
    childrenFolders.clear();
}


// Folder Class Functions
// ----------------------
// Sorting Elements by Alphabetical order
void Folder::SortElements()
{
    // Right now, this function just uses insertion sort. In the future it can be changed to combine quick and insertion for better efficiency

    // Sorting the folders
    for (int i = 0; i < childrenFolders.size(); i++)
    {
        std::string currentValue = childrenFolders[i]->getName();
        int checkIndex = i - 1;

        while (checkIndex >= 0 && childrenFolders[checkIndex]->getName() > currentValue)
        {
            childrenFolders[checkIndex + 1] = childrenFolders[checkIndex];
            checkIndex--;
        }
        childrenFolders[checkIndex + 1] = childrenFolders[i];
    }

    // Sorting the elements
    for (int i = 0; i < elements.size(); i++)
    {
        std::string currentValue = elements[i]->getName();
        int checkIndex = i - 1;

        while (checkIndex >= 0 && elements[checkIndex]->getName() > currentValue)
        {
            elements[checkIndex + 1] = elements[checkIndex];
            checkIndex--;
        }
        elements[checkIndex + 1] = elements[i];
    }
}