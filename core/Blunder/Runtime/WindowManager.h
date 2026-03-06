#ifndef WINDOW_MANAGER
#pragma once

#include "../../ew/external/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "../../math/smath.h"
#include "../UI/UIWindows.h"
#include "StateMachine.h"

class LockedWindow
{
public:
	// Data necessary for getting offset
	enum ChildWindowPosition { POS_NONE, POS_LEFT, POS_RIGHT, POS_BOTTOM, POS_TOP };

	// Constructor & Deconstructor
	LockedWindow(ui::UIWindow* window, LockedWindow* parent, glm::vec2 dimensions, ChildWindowPosition position);
	~LockedWindow();

	// Getters
	ui::UIWindow* getWindow() const { return this->window; }
	LockedWindow* getLeftWindow() const { return leftWindow; };
	LockedWindow* getRightWindow() const { return rightWindow; };
	LockedWindow* getTopWindow() const { return topWindow; };
	LockedWindow* getBottomWindow() const { return bottomWindow; };
	float getLeftWidth() const { return (leftWindow ? leftWidth : 0 ); }
	float getRightWidth() const { return (rightWindow ? rightWidth : 0 ); }
	float getTopHeight() const { return (topWindow ? topHeight : 0 ); }
	float getBottomHeight() const { return (bottomWindow ? bottomHeight : 0 ); }
	glm::vec2 getDimensions() const { return dimensions; }
	glm::vec2 getMainWindowDimensions() const { return (glm::vec2(1) - percentageUsed) * dimensions; }
	LockedWindow* getParent() { return parent; }
	float getXOffset() const;
	float getYOffset() const;
	glm::vec2 getOffset() const { return glm::vec2(getXOffset(), getYOffset()); }
	glm::vec4 getScreenCorners() const { return glm::vec4(getOffset(), getOffset() + getDimensions()); }
	glm::vec4 getMainWindowCorners() const { return glm::vec4(getOffset(), getOffset() + getMainWindowDimensions()); }

	// Setters
	void setDimensions(glm::vec2 dimensions) { this->dimensions = dimensions; UpdateDimensions(); }
	void setLeftWindow(ui::UIWindow* window, float width = 0.25f);
	void setRightWindow(ui::UIWindow* window, float width = 0.25f);
	void setTopWindow(ui::UIWindow* window, float height = 0.25f);
	void setBottomWindow(ui::UIWindow* window, float height = 0.25f);
	void setLeftWidth(float leftWidth) { this->leftWidth = smath::clamp(leftWidth, 0.0f, 1.0f - percentageUsed.x); UpdateDimensions(); }
	void setRightWidth (float rightWidth) { this->rightWidth = smath::clamp(rightWidth, 0.0f, 1.0f - percentageUsed.x); UpdateDimensions(); }
	void setTopHeight (float topHeight) { this->topHeight = smath::clamp(topHeight, 0.0f, 1.0f - percentageUsed.y); UpdateDimensions(); }
	void setBottomHeight (float bottomHeight) { this->bottomHeight = smath::clamp(bottomHeight, 0.0f, 1.0f - percentageUsed.y); UpdateDimensions(); }
	void setParent(LockedWindow* parent) { this->parent = parent; }

	// Functions
	void UpdateDimensions();
	ui::UIWindow* checkForCollisions(glm::vec2 position);
	void DrawWindows(ui::UIRenderer* renderer);

private:
	// Window data
	ChildWindowPosition childPosition;
	LockedWindow* parent;
	ui::UIWindow* window;
	glm::vec2 dimensions;
	glm::vec2 offset;
 	glm::vec2 percentageUsed; // Between 0.0 to 1.0

	// Connected Windows
	LockedWindow* leftWindow;
	LockedWindow* rightWindow;
	LockedWindow* topWindow;
	LockedWindow* bottomWindow;
	float leftWidth;
	float rightWidth;
	float topHeight;
	float bottomHeight;
};


class WindowManager
{
public:
	// Constructor & Deocnstructor
	WindowManager(StateMachine* state);
	~WindowManager();

	// Getters

	// Setters
	void setPopUpWindow(ui::UIWindow* window) { closePopUp(); this->popUpWindow = window; }

	// Functions
	void closePopUp() { if (popUpWindow) { popUpWindow->UnselectWindow(); delete popUpWindow; popUpWindow = nullptr; } }
	void addFreeWindow(ui::UIWindow* window) { freeWindows.push_back(window); window->UpdateWindow(); }
	void UpdateWindows(GLFWwindow* window, glm::ivec2 screenSize);
	void DrawWindows(ui::UIRenderer* renderer);
	void CreateDefaultWindows(glm::ivec2 screenSize);

private:
	// Window Layers
	LockedWindow* rootLockedWindow;
	std::vector<ui::UIWindow*> freeWindows;
	ui::UIWindow* popUpWindow;

	glm::ivec2 storedScreenSize;
	ui::UIWindow* selectedWindow;
	StateMachine* state;
};

#endif // !WINDOW_MANAGER
