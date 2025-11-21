#include "ObjectSystem.h"
using namespace obs;

// Constructor & Deocnstructor
ObjectSystem::ObjectSystem()
{
    rootFolder = new Folder("Scene");
    selectedFolder = rootFolder;
}
ObjectSystem::~ObjectSystem()
{
    delete rootFolder;
    rootFolder = nullptr;
    selectedFolder = nullptr;
}

// Functions
void ObjectSystem::addObject(obj::Object* object)
{
    if (selectedFolder != nullptr)
    {
        selectedFolder->addElement(new HierarchyElement(object, OBS_OBJECT, nullptr));
    }
    else
    {
        rootFolder->addElement(new HierarchyElement(object, OBS_OBJECT, nullptr));
    }
}
void ObjectSystem::addHierarchyElement(HierarchyElement* element)
{
    if (selectedFolder != nullptr)
        selectedFolder->addElement(element);
    else
        rootFolder->addElement(element);
}
void ObjectSystem::addFolder(std::string name)
{
    if (selectedFolder != nullptr)
    {
        selectedFolder->addChildFolder(new Folder(name, selectedFolder));
    }
    else
    {
        rootFolder->addChildFolder(new Folder(name, rootFolder));
    }
}

void ObjectSystem::outputFolder(Folder* folder, int offset)
{
    std::cout << std::string((offset * 4), ' ') << folder->getName() << std::endl;
    for (int i = 0; i < folder->getChildFoldersSize(); i++)
    {
        outputFolder(folder->getChildFolder(i), offset + 1);
    }
    for (int i = 0; i < folder->getHierarchyElementSize(); i++)
    {
        std::cout << std::string((offset * 4), ' ') << folder->getHierarchyElement(i)->getName() << std::endl;
    }
}

void ObjectSystem::testOutput()
{
    std::cout << "Test Hierarchy Layout" << std::endl;
    outputFolder(rootFolder, 0);
}