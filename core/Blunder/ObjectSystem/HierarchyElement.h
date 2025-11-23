#ifndef HIERARCHY_ELEMENT
#pragma once

#include <iostream>
#include <vector>
#include "../../object/object.h"

enum HierarchyType { OBS_OBJECT, OBS_LIGHT, OBS_CAMERA, OBS_EMPTY };

class HierarchyInfo
{
public:
    // Constructor
    HierarchyInfo(bool displayed = true, bool rendered = true, bool dropdown = true) : displayed(displayed), rendered(rendered), dropdown(dropdown) {}

    // Getters
    bool getDisplayed() { return displayed; }
    bool getRendered() { return rendered; }
    bool getDropdown() { return dropdown; }
    bool* getDisplayedAddress() { return &displayed; }
    bool* getRenderedAddress() { return &rendered; }
    bool* getDropdownAddress() { return &dropdown; }

    // Setters
    void setDisplayed(bool displayed) { this->displayed = displayed; }
    void setRendered(bool rendered) { this->rendered = rendered; }
    void setDropdown(bool dropdown) { this->dropdown = dropdown; }

    // Overload Functions
    virtual std::string getName() = 0;
    virtual void setName(std::string name) = 0;

protected:
    // UI Interaction Elements
    bool displayed;
    bool rendered;
    bool dropdown;
};

class HierarchyElement : public HierarchyInfo
{
public:
    // Constructor
    HierarchyElement(obj::Object* object, HierarchyType type, HierarchyElement* parent, bool displayed = true, bool rendered = true);
    ~HierarchyElement() { EraseObject(true); }

    // Getters
    HierarchyElement* getParent() { return parent; }
    HierarchyElement* getChild(int index) { return children[index]; }
    int getChildrenSize() { return children.size(); }
    HierarchyType getType() { return type; }
    obj::Object* getObject() { return object; }
    std::string getName() override { return object->getName(); }
    bool hasChildren() { return children.size() > 0; }

    // Setters
    void setName(std::string name) override { this->object->setName(name); }

    // Functions
    void changeParent(HierarchyElement* parent);
    void addChild(HierarchyElement* child);
    void removeChild(HierarchyElement* element);
    void EraseObject(bool deleteChildren = false);
    void DrawElementUI();

private:
    obj::Object* object;
    HierarchyType type;
    HierarchyElement* parent;
    std::vector<HierarchyElement*> children;
};

class Folder : public HierarchyInfo
{
public:
    // Constructor & Deconstructor
    Folder(std::string name, Folder* parentFolder = nullptr, bool displayed = true, bool rendered = true) : name(name), parentFolder(parentFolder), displayed(displayed), rendered(rendered), dropdown(true) {}
    ~Folder() { EraseFolder(); }

    // Getters
    HierarchyElement* getHierarchyElement(int index) { return elements[index]; }
    int getHierarchyElementSize() { return elements.size(); }
    Folder* getParentFolder() { return parentFolder; }
    Folder* getChildFolder(int index) { return childrenFolders[index]; }
    int getChildFoldersSize() { return childrenFolders.size(); }
    bool hasChildren() { return (elements.size() > 0 || childrenFolders.size() > 0); }
    std::string getName() override { return name; }


    // Setters
    void setName(std::string name) override { this->name = name; }

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
    bool dropdown;

    // Sorting Elements by Alphabetical order
    void SortElements();
};

#endif // !HIERARCHY_ELEMENT
