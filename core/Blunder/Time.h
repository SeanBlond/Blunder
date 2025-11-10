#ifndef TIME
#pragma once

#include <iostream>

class TimeManager
{
public:
	static float time;
	static float timeScale;
	static float deltaTime;
	static int fps;
	static float hertz;

	void UpdateTime(float softwareTime)
	{
		lastTime = time;
		time = softwareTime * timeScale;
		deltaTime = time - lastTime;
		fps = (int)round(1.0f / deltaTime);
		hertz = (1.0f / deltaTime);
	}

private:
	static float lastTime;
};

float TimeManager::time = 0.0f;
float TimeManager::lastTime = 0.0f;
float TimeManager::timeScale = 0.0f;
float TimeManager::deltaTime = 0.0f;
int TimeManager::fps = 0;
float TimeManager::hertz = 0.0f;


#endif // !TIME
