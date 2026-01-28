#ifndef TIME
#pragma once

#include <iostream>
#include <mutex>


class TimeManager
{
private:
	// Member variables
	float time, previousTime;
	static TimeManager* instancePtr;

	// Mutex to ensure thread safety
	static std::mutex mtx;

	// Private Constructor
	TimeManager() { time = previousTime = 0.0f; }

public:
	// Deleting the copy constructor to prevent copies
	TimeManager(const TimeManager& obj) = delete;

	// Static method to get the TimeManager instance
	static TimeManager* getInstance()
	{
		if (instancePtr == nullptr) {
			std::lock_guard<std::mutex> lock(mtx);
			if (instancePtr == nullptr)
				instancePtr = new TimeManager();
		}
		return instancePtr;
	}

	// Updating Values
	void UpdateTime(float softwareTime)
	{
		previousTime = time;
		time = softwareTime;
	}

	// Getters
	float getTime() { return time; }
	float getDeltaTime() { return (time - previousTime); }
	int getFPS() { return (int)round(1.0f / (time - previousTime)); }
	float getHertz() { return (1.0f / (time - previousTime)); }
};

#endif // !TIME
