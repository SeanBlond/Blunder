#include "WindowManager.h"

// Locked Window Class Functions
// Constructor & Deocnstructor
LockedWindow::LockedWindow(ui::UIWindow* window, LockedWindow* parent, glm::vec2 dimensions)
{
	this->window = window;
	this->parent = parent;
	this->dimensions = dimensions;

}
LockedWindow::~LockedWindow()
{
	// Deleting Window
	delete window;
	window = nullptr;

	// Deleting Connected Windows
	delete leftWindow;
	leftWindow = nullptr;
	delete rightWindow;
	rightWindow = nullptr;
	delete topWindow;
	topWindow = nullptr;
	delete bottomWindow;
	bottomWindow = nullptr;
}

// Getters
float LockedWindow::getXOffset() const
{
	if (!parent)
		return (leftWidth * dimensions.x);

	return ((1.0f - parent->getRightWidth()) * parent->getDimensions().x) + parent->getXOffset();
}
float LockedWindow::getYOffset() const
{
	return parent->getBottomHeight() * parent->getDimensions().y;
}

// Setters
void LockedWindow::setLeftWindow(LockedWindow* leftWindow) 
{ 
	this->leftWindow = leftWindow;
	leftWindow->setParent(this);
	
	UpdateDimensions(); 
}
void LockedWindow::setRightWindow(LockedWindow* rightWindow) 
{ 
	this->rightWindow = rightWindow;
	rightWindow->setParent(this); 
	
	UpdateDimensions(); 
}
void LockedWindow::setTopWindow(LockedWindow* topWindow) 
{ 
	this->topWindow = topWindow;
	topWindow->setParent(this); 
	
	UpdateDimensions(); 
}
void LockedWindow::setBottomWindow(LockedWindow* bottomWindow) 
{ 
	this->bottomWindow = bottomWindow;
	bottomWindow->setParent(this); 
	
	UpdateDimensions(); 
}

// Functions
void LockedWindow::UpdateDimensions()
{
	// Calcuting percantage used
	glm::vec2 tempPercentageUsed = glm::vec2(0);
	// X
	if (leftWindow)
		tempPercentageUsed.x += leftWidth;
	if (rightWindow)
		tempPercentageUsed.x += rightWidth;
	// Y
	if (topWindow)
		tempPercentageUsed.y += topHeight;
	if (bottomWindow)
		tempPercentageUsed.y += bottomHeight;

	percentageUsed = tempPercentageUsed;

	// Updating window sizes
	float topBottomWindowWidth = dimensions.x * (1.0f - percentageUsed.x);
	if (leftWindow)
		leftWindow->setDimensions(glm::vec2(leftWidth * dimensions.x, dimensions.y));
	if (rightWindow)
		rightWindow->setDimensions(glm::vec2(rightWidth * dimensions.x, dimensions.y));
	if (topWindow)
		topWindow->setDimensions(glm::vec2(topBottomWindowWidth, topHeight * dimensions.y));
	if (bottomWindow)
		bottomWindow->setDimensions(glm::vec2(topBottomWindowWidth, bottomHeight * dimensions.y));

	// Updating UI Window dimensions
	glm::vec2 windowDimensions = (glm::vec2(1) - percentageUsed) * dimensions;
	glm::vec2 windowOffset = getOffset();
	window->setDimensions(windowDimensions, windowOffset);
}
ui::UIWindow* LockedWindow::checkForCollisions(glm::vec2 position)
{
	// Checking the main screen
	if (smath::checkUICollision(position, getScreenCorners()))
		return window;

	// Checking connected windows
	if (leftWindow)
		return leftWindow->checkForCollisions(position);
	if (rightWindow)
		return rightWindow->checkForCollisions(position);
	if (topWindow)
		return topWindow->checkForCollisions(position);
	if (bottomWindow)
		return bottomWindow->checkForCollisions(position);

	// No Collision Detected
	return nullptr;
}
void LockedWindow::DrawWindows(ui::UIRenderer* renderer)
{
	// Drawing the main window
	window->DrawWindow(renderer);

	// Drawing connected windows
	if (leftWindow)
		leftWindow->DrawWindows(renderer);
	if (rightWindow)
		rightWindow->DrawWindows(renderer);
	if (topWindow)
		topWindow->DrawWindows(renderer);
	if (bottomWindow)
		bottomWindow->DrawWindows(renderer);
}


// Window Manager Class Functions
// Constructor & Deocnstructor
WindowManager::WindowManager(StateMachine* state, glm::vec2 screenSize)
{
	this->state = state;
	selectedWindow = nullptr;
	rootLockedWindow = nullptr;
}
WindowManager::~WindowManager()
{
	delete rootLockedWindow;
	rootLockedWindow = nullptr;

	closePopUp();
}

// Functions
void WindowManager::UpdateWindows(glm::vec2 screenSize)
{
	// Getting mouse position
	glm::vec2 mousePos = state->getMouse()->mousePos;

	// TODO:
	// if transforming, have the mouse position wrap around the window on both axis

	// If the pop-up window exists, only check it
	if (popUpWindow)
	{
		// Check for collision
		glm::vec4 checkCorners = popUpWindow->getPosition().getCorners();
		if (smath::checkUICollision(mousePos, checkCorners))
		{
			selectedWindow = popUpWindow;
		}
	}
	else
	{
		// Checking free window collision

		// Checking locked window collision

		// Setting selectedWindow to nullptr if none is selected
	}

	// TODO:
	// Create UpdateWindow functions in the UIWindow class that get called when the window is running, but not being interacted with
}
void WindowManager::DrawWindows(ui::UIRenderer* renderer)
{
	// Drawing the Locked Windows
	rootLockedWindow->DrawWindows(renderer);

	// Drawing the free windows
	for (int i = 0; i < freeWindows.size(); i++)
	{
		freeWindows[i]->DrawWindow(renderer);
	}

	// Drawing the pop-up window (if it exists)
	if (popUpWindow)
		popUpWindow->DrawWindow(renderer);
}
void WindowManager::CreateDefaultWindows(glm::vec2 screenSize)
{
	//rootLockedWindow = new LockedWindow(new ui::ViewportWindow(screenSize.x, screenSize.y, 0.0f, 0.0f, ), nullptr, glm::vec2(0));
}