#include "ObjectRender.h"
using namespace obr;


int ObjectRender::findObject(int ID)
{
    // Using Bilinear search, as array will not be sorted :(
    for (int i = 0; i < objects.size(); i++)
    {
        if (objects[i]->getID() == ID)
            return i;
    }
}
void ObjectRender::renderObjects(glm::mat4 projection, glm::mat4 view)
{
    // Implement a lot of different stuff (in the future of course)

    // Rendering Objects in the System
    for (int i = 0; i < objects.size(); i++)
    {
        if (objects[i]->getDisplayed())
        {
            objects[i]->shader->useShader();
            objects[i]->shader->setMat4("projection", projection);
            objects[i]->shader->setMat4("view", view);
            objects[i]->DrawMesh(!renderTriangles);
        }
    }
}

void ObjectRender::testOutput()
{
    std::cout << "Object Hierarchy" << std::endl;
    std::vector<int> printedIDs;
    for (int i = 0; i < objects.size(); i++)
    {
        // Checking if object has already been listed
        int index = smath::vectorFind(printedIDs, objects[i]->getID());

        if (index != -1)
            continue;

        std::cout << "o " << objects[i]->getName() << std::endl;
        printedIDs.push_back(objects[i]->getID());
        for (int j = 0; j < objects[i]->getChildren().size(); j++)
        {
            std::cout << "  o " << objects[i]->getChildren()[j]->getName() << std::endl;
            printedIDs.push_back(objects[i]->getChildren()[j]->getID());
        }
    }
}