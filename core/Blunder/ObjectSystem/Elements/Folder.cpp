#include "Folder.h"


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
            for (int i = 0; i < element->getChildren().size(); i++)
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