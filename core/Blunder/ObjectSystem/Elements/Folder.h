#ifndef FOLDER
#pragma once

#include <iostream>
#include "HierarchyElement.h"

class Folder
{
public:
    Folder(std::string name, Folder* parentFolder = nullptr, bool displayed = true, bool rendered = true) : name(name), parentFolder(parentFolder), displayed(displayed), rendered(rendered) {}

    // Getters
    std::string getName() { return name; }
    std::vector<HierarchyElement*> getHierarchyElements() { return elements; }
    HierarchyElement* getHierarchyElement(int index) { return elements[index]; }
    Folder* getParentFolder() { return parentFolder; }

    // Setters
    void setName(std::string name) { this->name = name; }
    void setParentFolder(Folder* parentFolder) { this->parentFolder = parentFolder; }

    // Functions
    void addElement(HierarchyElement* element);
    void removeElement(HierarchyElement* element, bool deleteChildren = false);
    void moveElement(int index, Folder* folder);
    void moveElement(HierarchyElement* element, Folder* folder);

private:
    std::string name;
    Folder* parentFolder;
    std::vector<HierarchyElement*> elements;

    // UI Interaction Elements
    bool displayed;
    bool rendered;
};

#endif // !FOLDER
