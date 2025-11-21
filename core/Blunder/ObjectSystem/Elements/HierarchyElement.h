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
    HierarchyElement(obj::Object* object, HierarchyType type, HierarchyElement* parent, bool displayed = true, bool rendered = true);
    ~HierarchyElement() { EraseObject(true); }

    // Getters
    HierarchyElement* getParent() { return parent; }
    std::vector<HierarchyElement*> getChildren() { return children; }
    HierarchyElement* getChild(int index) { return children[index]; }
    HierarchyType getType() { return type; }
    obj::Object* getObject() { return object; }
    std::string getName() { return object->getName(); }

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

    // UI Interaction Elements
    bool displayed;
    bool rendered;
};
#endif // !HIERARCHY_ELEMENT
