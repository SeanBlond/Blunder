#pragma once
#include <random>

namespace ew {
	constexpr float PI = 3.14159265359f;
	constexpr float TAU = 6.283185307179586f;
	constexpr float DEG2RAD = (PI / 180.0f);
	constexpr float RAD2DEG = (180.0f / PI);
	inline float Radians(float degrees) {
		return degrees * DEG2RAD;
	}
	inline float Degrees(float radians) {
		return radians * RAD2DEG;
	}
	inline float RandomRange(float min, float max) {
		float t = (float)rand() / RAND_MAX;
		return min + (max - min) * t;
	}
}
