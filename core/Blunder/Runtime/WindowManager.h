#ifndef WIsNDOW_MANAGER
#pragma once

#include "../../math/smath.h"
#include "../UI/UIWindows.h"
#include "StateMachine.h"

class LockedWindow
{
public:
	// Constructor & Deconstructor
	LockedWindow(ui::UIWindow* window, LockedWindow* parent, glm::vec2 dimensions);
	~LockedWindow();

	// Getters
	ui::UIWindow* getWindow() const { return window; }
	LockedWindow* getLeftWindow() const { return leftWindow; };
	LockedWindow* getRightWindow() const { return rightWindow; };
	LockedWindow* getTopWindow() const { return topWindow; };
	LockedWindow* getBottomWindow() const { return bottomWindow; };
	float getLeftWidth() const { return (leftWindow ? leftWidth : 0 ); }
	float getRightWidth() const { return (rightWindow ? rightWidth : 0 ); }
	float getTopHeight() const { return (topWindow ? topHeight : 0 ); }
	float getBottomHeight() const { return (bottomWindow ? bottomHeight : 0 ); }
	glm::vec2 getDimensions() const { return dimensions; }
	LockedWindow* getParent() { return parent; }
	float getXOffset() const;
	float getYOffset() const;
	glm::vec2 getOffset() const { return glm::vec2(getXOffset(), getYOffset()); }
	glm::vec4 getScreenCorners() const { return glm::vec4(getOffset(), getOffset() + getDimensions()); }

	// Setters
	void setDimensions(glm::vec2 dimensions) { this->dimensions = dimensions; UpdateDimensions(); }
	void setLeftWindow(LockedWindow* leftWindow);
	void setRightWindow(LockedWindow* rightWindow);
	void setTopWindow(LockedWindow* topWindow);
	void setBottomWindow(LockedWindow* bottomWindow);
	void setLeftWidth(float leftWidth) { this->leftWidth = smath::clamp(leftWidth, 0.0f, 1.0f - percentageUsed.x); UpdateDimensions(); }
	void setRightWidth (float rightWidth) { this->rightWidth = smath::clamp(rightWidth, 0.0f, 1.0f - percentageUsed.x); UpdateDimensions(); }
	void setTopHeight (float topHeight) { this->topHeight = smath::clamp(topHeight, 0.0f, 1.0f - percentageUsed.y); UpdateDimensions(); }
	void setBottomHeight (float bottomHeight) { this->bottomHeight = smath::clamp(bottomHeight, 0.0f, 1.0f - percentageUsed.y); UpdateDimensions(); }
	void setParent(LockedWindow* parent) { this->parent = parent; }

	// Functions
	void UpdateDimensions();
	ui::UIWindow* checkForCollisions(glm::vec2 position);
	void DrawWindows();

private:
	// Window data
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
	void closePopUp() { popUpWindow->UnselectWindow(); delete popUpWindow; popUpWindow = nullptr; }
	void addFreeWindow(ui::UIWindow* window) { freeWindows.push_back(window); }
	void UpdateWindows();
	void DrawWindows();
	void CreateDefaultWindows();

private:
	// Window Layers
	LockedWindow* rootLockedWindow;
	std::vector<ui::UIWindow*> freeWindows;
	ui::UIWindow* popUpWindow;
	ui::UIWindow* selectedWindow;

	StateMachine* state;
};

#endif // !WINDOW_MANAGER
