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
	leftWindow->getWindow()->OpenWindow();
}
void LockedWindow::setRightWindow(ui::UIWindow* window, float width)
{
	this->rightWindow = new LockedWindow(window, this, glm::vec2(0), POS_RIGHT);
	rightWindow->setParent(this);
	setRightWidth(width);
	rightWindow->getWindow()->OpenWindow();
}
void LockedWindow::setTopWindow(ui::UIWindow* window, float height)
{
	this->topWindow = new LockedWindow(window, this, glm::vec2(0), POS_TOP);
	topWindow->setParent(this);
	setTopHeight(height);
	topWindow->getWindow()->OpenWindow();
}
void LockedWindow::setBottomWindow(ui::UIWindow* window, float height)
{
	this->bottomWindow = new LockedWindow(window, this, glm::vec2(0), POS_BOTTOM);
	bottomWindow->setParent(this);
	setBottomHeight(height);
	bottomWindow->getWindow()->OpenWindow();
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

	// Resizing UI Window dimensions
	glm::vec2 mainWindowDimensions = getMainWindowDimensions();
	glm::vec2 windowOffset = getOffset();
	window->setPosition(mainWindowDimensions, windowOffset);
	window->ResizeWindow();
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
	state->setWindowManager(this);
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
void WindowManager::UpdateWindows(GLFWwindow* window, glm::ivec2 screenSize)
{
	// Checking if screen size needs to be updated
	if (storedScreenSize != screenSize)
	{
		// Updating pop up window position (if applicable)
		if (popUpWindow)
		{
			// Updating position to stay in the same relative position to the screen size
			glm::vec2 relativeOffset = popUpWindow->getPosition().offset / (glm::vec2)storedScreenSize;
			popUpWindow->setOffset(relativeOffset * (glm::vec2)screenSize);
		}

		// RTesizing free window positions
		for (int i = 0; i < freeWindows.size(); i++)
		{
			// Updating position to stay in the same relative position to the screen size
			glm::vec2 relativeOffset = freeWindows[i]->getPosition().offset / (glm::vec2)storedScreenSize;
			freeWindows[i]->setOffset(relativeOffset * (glm::vec2)screenSize);
			freeWindows[i]->ResizeWindow();
		}

		// Updating root locked window size
		rootLockedWindow->setDimensions((glm::vec2)screenSize);

		// Updating stored screen size
		storedScreenSize = screenSize;

		// Updating State Machine window size
		state->setWindowDimensions(screenSize);
	}

	// Updating selected window if the UI is not actively being interacted with
	ui::UIWindow* newSelectedWindow = selectedWindow;
	if (state->getState() != SM_UI_TRANSLATING)
	{
		// Getting mouse position
		glm::vec2 mousePos = state->getMouse()->mousePos;

		// Inverting mouse pos so that (0, 0) is at bottom left instead of top left
		mousePos.y = screenSize.y - mousePos.y;

		// If the pop-up window exists, only check it
		if (popUpWindow)
		{
			// Check for collision
			if (smath::checkUICollision(mousePos, popUpWindow->getPosition().getCorners()))
			{
				newSelectedWindow = popUpWindow;
			}
			// Checking if the window should be closed (clicked outside of the screen)
			else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1))
			{
				if (selectedWindow == popUpWindow)
					selectedWindow = nullptr;

				closePopUp();
				newSelectedWindow = nullptr;
			}
		}
		// Checking non-pop-up windows
		else
		{
			// Checking free windows
			bool freeWindowSelected = false;
			if (freeWindows.size() > 0)
			{
				// Looping through each open window to check for collision
				for (int i = 0; i < freeWindows.size(); i++)
				{
					// Checking for collision
					if (smath::checkUICollision(mousePos, freeWindows[i]->getPosition().getCorners()))
					{
						freeWindowSelected = true;
						newSelectedWindow = freeWindows[i];
					}
				}
			}

			// Checking locked window collision (only if no free window was selected)
			if (!freeWindowSelected)
			{
				newSelectedWindow = rootLockedWindow->checkForCollisions(mousePos);
			}
		}
	}

	// Unselecting previous window (if applicable)
	if (selectedWindow && selectedWindow != newSelectedWindow)
		selectedWindow->UnselectWindow();
	
	// Setting new selected window
	selectedWindow = newSelectedWindow;

	// Managing selected window interaction (if there is a selected window)
	if (selectedWindow)
		selectedWindow->ManageInteraction(window, state);

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
void WindowManager::CreateDefaultWindows(glm::ivec2 screenSize)
{
	storedScreenSize = screenSize;

	// Viewport UI
	rootLockedWindow = new LockedWindow(new ui::ViewportWindow(screenSize.x, screenSize.y, 0.0f, 0.0f, state), nullptr, screenSize, LockedWindow::POS_NONE);

	// Attribute UI
	//rootLockedWindow->setLeftWindow(new ui::AttributeWindow(screenSize.x, screenSize.y, 0.0f, 0.0f, state->getSelectedObject()), 0.1667f);

	// Hierarchy UI
	//rootLockedWindow->setRightWindow(new ui::HierarchyWindow(screenSize.x, screenSize.y, 0.0f, 0.0f, state), 0.1667);
}