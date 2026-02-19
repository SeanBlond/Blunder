#include "WindowManager.h"

// Locked Window Class Functions
// Constructor & Deocnstructor
LockedWindow::LockedWindow(ui::UIWindow* window, LockedWindow* parent, glm::vec2 dimensions, ChildWindowPosition position)
{
	this->window = window;
	this->parent = parent;
	this->dimensions = dimensions;
	this->childPosition = position;

	// Zeroing values
	leftWindow = rightWindow = topWindow = bottomWindow = nullptr;
	leftWidth = rightWidth = topHeight = bottomHeight = 0.0f;
	offset = percentageUsed = glm::vec2(0);

	UpdateDimensions();
	window->GenerateInteractables();
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
	{
		//std::cout << "No parent offset: " << ((leftWindow ? leftWidth : 0.0f) * dimensions.x) << std::endl;
		return ((leftWindow ? leftWidth : 0.0f) * dimensions.x);
	}

	if (childPosition == POS_RIGHT)
	{
		return ((1.0f - parent->getRightWidth()) * parent->getDimensions().x);
	}

	return 0.0f;
}
float LockedWindow::getYOffset() const
{
	if (!parent)
		return ((bottomWindow ? bottomHeight : 0.0f) * dimensions.y);

	return parent->getBottomHeight() * parent->getDimensions().y;
}

// Setters
void LockedWindow::setLeftWindow(ui::UIWindow* window, float width)
{ 
	this->leftWindow = new LockedWindow(window, this, glm::vec2(0), POS_LEFT);
	leftWindow->setParent(this);
	setLeftWidth(width);
}
void LockedWindow::setRightWindow(ui::UIWindow* window, float width)
{
	this->rightWindow = new LockedWindow(window, this, glm::vec2(0), POS_RIGHT);
	rightWindow->setParent(this);
	setRightWidth(width);
}
void LockedWindow::setTopWindow(ui::UIWindow* window, float height)
{
	this->topWindow = new LockedWindow(window, this, glm::vec2(0), POS_TOP);
	topWindow->setParent(this);
	setTopHeight(height);
}
void LockedWindow::setBottomWindow(ui::UIWindow* window, float height)
{ 
	this->bottomWindow = new LockedWindow(window, this, glm::vec2(0), POS_BOTTOM);
	bottomWindow->setParent(this); 
	setBottomHeight(height);
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
	glm::vec2 mainWindowDimensions = getMainWindowDimensions();
	glm::vec2 windowOffset = getOffset();
	window->setDimensions(mainWindowDimensions, windowOffset);

	//std::cout << std::endl << "Window Dimensions Update" << std::endl;
	//std::cout << "Set dimensions " << smath::outputVec2(mainWindowDimensions) << " and offset" << smath::outputVec2(windowOffset) << std::endl;
	//std::cout << "Set Window Corners " << smath::outputVec4(window->getPosition().getCorners()) << std::endl;
}
ui::UIWindow* LockedWindow::checkForCollisions(glm::vec2 position)
{
	// Checking the main screen
	if (smath::checkUICollision(position, getMainWindowCorners()))
	{
		//std::cout << childPosition << ": Checking " << smath::outputVec2(position) << " in " << smath::outputVec4(getMainWindowCorners()) << std::endl;
		return this->window;
	}

	// Checking connected windows
	if (leftWindow)
	{
		ui::UIWindow* checkedWindow = leftWindow->checkForCollisions(position);
		if (checkedWindow != nullptr)
			return checkedWindow;
	}
	if (rightWindow)
	{
		ui::UIWindow* checkedWindow = rightWindow->checkForCollisions(position);
		if (checkedWindow != nullptr)
			return checkedWindow;
	}
	if (topWindow)
	{
		ui::UIWindow* checkedWindow = topWindow->checkForCollisions(position);
		if (checkedWindow != nullptr)
			return checkedWindow;
	}
	if (bottomWindow)
	{
		ui::UIWindow* checkedWindow = bottomWindow->checkForCollisions(position);
		if (checkedWindow != nullptr)
			return checkedWindow;
	}

	// No Collision Detected
	return nullptr;
}
void LockedWindow::DrawWindows(ui::UIRenderer* renderer)
{
	// Drawing the main window
	//std::cout << "Drawing window with dimensions " << smath::outputVec4(window->getPosition().getCorners()) << std::endl;
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
WindowManager::WindowManager(StateMachine* state)
{
	this->state = state;
	selectedWindow = nullptr;
	rootLockedWindow = nullptr;
	popUpWindow = nullptr;
	storedScreenSize = glm::vec2(0);
}
WindowManager::~WindowManager()
{
	delete rootLockedWindow;
	rootLockedWindow = nullptr;

	closePopUp();
}

// Functions
void WindowManager::UpdateWindows(GLFWwindow* window, glm::vec2 screenSize)
{
	// Checking if screen size needs to be updated
	if (storedScreenSize != screenSize)
	{
		rootLockedWindow->setDimensions(screenSize);
	}

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
		selectedWindow = rootLockedWindow->checkForCollisions(mousePos);
		if (selectedWindow)
			selectedWindow->ManageInteraction(window, state);		
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
	storedScreenSize = screenSize;

	// Viewport UI
	rootLockedWindow = new LockedWindow(new ui::ViewportWindow(screenSize.x, screenSize.y, 0.0f, 0.0f, state, state->getCamera()), nullptr, screenSize, LockedWindow::POS_NONE);

	// Attribute UI
	rootLockedWindow->setLeftWindow(new ui::AttributeWindow(screenSize.x, screenSize.y, 0.0f, 0.0f, state->getSelectedObject()), 0.333f);

	// Hierarchy UI
	rootLockedWindow->getLeftWindow()->setRightWindow(new ui::HierarchyWindow(screenSize.x, screenSize.y, 0.0f, 0.0f, state), 0.5f);
}