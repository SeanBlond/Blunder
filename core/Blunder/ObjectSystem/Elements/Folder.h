#ifndef FOLDER
#pragma once

#include <iostream>
#include "HierarchyElement.h"

class Folder
{
public:
    // Constructor & Deconstructor
    Folder(std::string name, Folder* parentFolder = nullptr, bool displayed = true, bool rendered = true) : name(name), parentFolder(parentFolder), displayed(displayed), rendered(rendered) {}
    ~Folder() { EraseFolder(); }

    // Getters
    std::string getName() { return name; }
    HierarchyElement* getHierarchyElement(int index) { return elements[index]; }
    int getHierarchyElementSize() { return elements.size(); }
    Folder* getParentFolder() { return parentFolder; }
    Folder* getChildFolder(int index) { return childrenFolders[index]; }
    int getChildFoldersSize() { return childrenFolders.size(); }
    bool getDisplayed() { return displayed; }
    bool getRendered() { return rendered; }
    

    // Setters
    void setName(std::string name) { this->name = name; }
    void setDisplayed(bool displayed) { this->displayed = displayed; }
    void setRendered(bool rendered) { this->rendered = rendered; }

    // Functions
    void addElement(HierarchyElement* element);
    void removeElement(HierarchyElement* element, bool deleteChildren = false);
    void moveElement(int index, Folder* folder);
    void moveElement(HierarchyElement* element, Folder* folder);
    void changeParentFolder(Folder* parent);
    void addChildFolder(Folder* child);
    void removeChildFolder(Folder* index, bool deleteElements = false);
    void EraseFolder();

private:
    std::string name;
    Folder* parentFolder;
    std::vector<Folder*> childrenFolders;
    std::vector<HierarchyElement*> elements;

    // UI Interaction Elements
    bool displayed;
    bool rendered;
    
    // Sorting Elements by Alphabetical order
    void SortElements();
};

#endif // !FOLDER
