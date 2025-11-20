#ifndef HIERARCHY_ELEMENT
#pragma once

#include <iostream>
#include <vector>
#include "../../../object/object.h"

enum HierarchyType { OBS_OBJECT, OBS_LIGHT, OBS_CAMERA, OBS_EMPTY };

class HierarchyElement
{
public:
    // Constructor
    HierarchyElement(HierarchyType type, HierarchyElement* parent, bool displayed = true, bool rendered = true);
    ~HierarchyElement() { EraseObject(); }

    // Getters
    int getID() { return ID; }
    HierarchyElement* getParent() { return parent; }
    std::vector<HierarchyElement*> getChildren() { return children; }
    HierarchyElement* getChild(int index) { return children[index]; }
    HierarchyType getType() { return type; }


    // Functions
    void changeParent(HierarchyElement* parent);
    void addChild(HierarchyElement* child);
    void removeChild(int ID);
    void EraseObject(bool deleteChildren = false);

    // Virtual Function(s)
    //virtual void DrawElementUI() = 0;

private:
    int ID;
    std::string label;
    HierarchyType type;
    HierarchyElement* parent;
    std::vector<HierarchyElement*> children;

    // UI Interaction Elements
    bool displayed;
    bool rendered;

    static int nextID;
};

// Object Hierarchy Element
class HierarchyObject : public HierarchyElement
{
public:
    // Constuctor & Deconstructor
    HierarchyObject(obj::Object* object, HierarchyElement* parent, bool displayed = true, bool rendered = true)
        : object(object), HierarchyElement(OBS_OBJECT, parent, displayed, rendered) {}
    ~HierarchyObject();

    // Getters
    obj::Object* getObject() { return object; }

private:
    obj::Object* object;
};

#endif // !HIERARCHY_ELEMENT
