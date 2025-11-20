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
        elements.erase(elements.begin() + smath::vectorFind(element));
    }
}
void Folder::moveElement(int index, Folder* folder)
{
    // Checking if index is within the range of the vector
    if (index >= 0 && index < elements.size())
    {
        folder->addElement(elements[index]);
        removeElement(index);
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