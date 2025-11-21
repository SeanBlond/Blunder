#include "ObjectSystem.h"
using namespace obs;

// Constructor & Deocnstructor
ObjectSystem::ObjectSystem()
{
    rootFolder = new Folder("Scene");
    selectedFolder = rootFolder;
    renderSettings.backfaceCull = true;
    renderSettings.renderTriangles = true;
    renderSettings.mode = OR_SIMPLE;
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

// Render Function :)
void ObjectSystem::RenderHierarchyElement(HierarchyElement* element, glm::mat4 projection, glm::mat4 view)
{
    // Checking if main element should be rendered
    if (element->getDisplayed())
    {
        element->getObject()->shader->useShader();
        element->getObject()->shader->setMat4("projection", projection);
        element->getObject()->shader->setMat4("view", view);
        element->getObject()->DrawMesh(!renderSettings.renderTriangles);
    }

    // Rendering each child of the element
    for (int i = 0; i < element->getChildrenSize(); i++)
    {
        RenderHierarchyElement(element->getChild(i), projection, view);
    }
}
void ObjectSystem::RenderFolder(Folder* folder, glm::mat4 projection, glm::mat4 view)
{
    // Checking if folder is set to be displayed
    if (folder->getDisplayed())
    {
        // Rendering each folder
        for (int i = 0; i < folder->getChildFoldersSize(); i++)
        {
            RenderFolder(folder->getChildFolder(i), projection, view);
        }

        // Rendering each element
        for (int i = 0; i < folder->getHierarchyElementSize(); i++)
        {
            RenderHierarchyElement(folder->getHierarchyElement(i), projection, view);
        }
    }
}
void ObjectSystem::Render(glm::mat4 projection, glm::mat4 view)
{
    // Implement a lot of different stuff (in the future of course)

    // Rendering starts from the Root Folder
    RenderFolder(rootFolder, projection, view);
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