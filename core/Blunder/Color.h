#ifndef COLOR
#pragma once

#include <iostream>
#include <math.h>
#include <glm/glm.hpp>
#include "../math/smath.h"

class Color
{
public:
	// Constructors
	Color() : color(glm::vec4(1)) {}
	Color(float r, float g, float b, float a = 1.0f) : color(glm::vec4(r, g, b, a)) {}
	Color(glm::vec3 color, float a = 1.0f) : color(glm::vec4(color, a)) {}
	Color(glm::vec4 color) : color(color) {}
	Color(float color, float a = 1.0f) : color(glm::vec4(glm::vec3(color), a)) {}

	// Getters
	glm::vec3 rgb() const { return glm::vec3(color); }
	glm::vec4 rgba() const { return color; };
	glm::vec3 hsv() const { return RGBtoHSV(color); }
	glm::vec4 hsva() const { return RGBAtoHSVA(color); }
	float alpha() const { return color.w; }
	std::string hex() const { return RGBAtoHEX(color); }
	float r() const { return color.x; }
	float g() const { return color.y; }
	float b() const { return color.z; }
	float h() const { return hsv().x; }
	float s() const { return hsv().y; }
	float v() const { return hsv().z; }
	float a() const { return color.w; }

	// Setters
	void setRGB(const glm::vec3 rgb) { color = glm::vec4(rgb, color.w); }
	void setRGBA(const glm::vec4 rgba) { color = rgba; }
	void setHSV(const glm::vec3 hsv) {color = glm::vec4(HSVtoRGB(hsv), color.w); }
	void setHSVA(const glm::vec4 hsva) { color = HSVAtoRGBA(hsva); }
	void setHex(const std::string hex) { glm::vec4 testConvert = HEXtoRGBA(hex); color = (testConvert == glm::vec4(-1) ? color : testConvert); }
	void setHue(const float hue)               { setHSV(glm::vec3(hue, s(),        v())); }
	void setSaturation(const float saturation) { setHSV(glm::vec3(h(), saturation, v())); }
	void setValue(const float value)           { setHSV(glm::vec3(h(), s(),        value)); }
	void setAlpha(const float alpha) { color.w = alpha; }

	// RGB <--> HSV Functions
	static glm::vec3 RGBtoHSV(glm::vec3 rgb) 
	{
		// Necessary Values for 
		const float chromaMax = smath::max(rgb.x, rgb.y, rgb.z);
		const float chromaMin = smath::min(rgb.x, rgb.y, rgb.z);
		const float chromaDelta = chromaMax - chromaMin;

		// Calculating saturation
		const float saturation = (chromaMax <= 0.0f ? 0 : chromaDelta / chromaMax);

		// Calculating saturation
		const float value = chromaMax;

		// Calculating hue
		float hue = 0.0f;
		if (chromaDelta <= 0.0f) // NaN
			return glm::vec3(hue, saturation, value);
		if (chromaMax == rgb.x)
			hue = 60.0f * fmod((rgb.y - rgb.z) / chromaDelta, 6.0f);
		else if (chromaMax == rgb.y)
			hue = 60.0f * (((rgb.z - rgb.x) / chromaDelta) + 2.0f);
		else if (chromaMax == rgb.z)
			hue = 60.0f * (((rgb.x - rgb.y) / chromaDelta) + 4.0f);

		// Making sure hue is above 0
		if (hue < 0.0f)
		{
			hue += 360.0f;
		}

		// Converting hue to 0-1
		hue /= 360.0f;

		return glm::vec3(hue, saturation, value);
	}
	static glm::vec4 RGBAtoHSVA(glm::vec4 rgba) { return glm::vec4(RGBtoHSV(rgba), rgba.w); }
	static glm::vec3 HSVtoRGB(glm::vec3 hsv) 
	{
		// Getting hue (and other variables)
		const float hue = hsv.x * 360.0f;
		const float chroma = hsv.y * hsv.z;
		const float huePrime = fmod(hue / 60.0f, 6.0f);
		const float x = chroma * (1.0f - abs(fmod(huePrime, 2.0f) - 1.0f));
		const float m = hsv.z - chroma;

		// Calculating rgb
		glm::vec3 rgb = glm::vec3(0);

		if (huePrime <= 1.0f)
			rgb = glm::vec3(chroma, x, 0);
		else if (huePrime <= 2.0f)
			rgb = glm::vec3(x, chroma, 0);
		else if (huePrime <= 3.0f)
			rgb = glm::vec3(0, chroma, x);
		else if (huePrime <= 4.0f)
			rgb = glm::vec3(0, x, chroma);
		else if (huePrime <= 5.0f)
			rgb = glm::vec3(x, 0, chroma);
		else if (huePrime <= 6.0f)
			rgb = glm::vec3(chroma, 0, x);

		return rgb + glm::vec3(m);
	}
	static glm::vec4 HSVAtoRGBA(glm::vec4 hsva) { return glm::vec4(HSVtoRGB(hsva), hsva.w); }

	// RGB & HSV <--> Hex Functions
	static std::string RGBtoHEX(glm::vec3 rgb)
	{
		// Converting floats to integer values between 0 and 255
		int r = static_cast<int>(smath::clamp(rgb.x, 0.0f, 1.0f) * 255);
		int g = static_cast<int>(smath::clamp(rgb.y, 0.0f, 1.0f) * 255);
		int b = static_cast<int>(smath::clamp(rgb.z, 0.0f, 1.0f) * 255);

		std::string result = '#' + smath::decToHexa(r, 2) + smath::decToHexa(g, 2) + smath::decToHexa(b, 2);
		return result;
	}
	static std::string RGBAtoHEX(glm::vec4 rgba)
	{
		// Converting floats to integer values between 0 and 255
		int r = static_cast<int>(smath::clamp(rgba.x, 0.0f, 1.0f) * 255);
		int g = static_cast<int>(smath::clamp(rgba.y, 0.0f, 1.0f) * 255);
		int b = static_cast<int>(smath::clamp(rgba.z, 0.0f, 1.0f) * 255);
		int a = static_cast<int>(smath::clamp(rgba.w, 0.0f, 1.0f) * 255);

		std::string result = '#' + smath::decToHexa(r, 2) + smath::decToHexa(g, 2) + smath::decToHexa(b, 2) + smath::decToHexa(a, 2);
		return result;
	}
	static std::string HSVtoHEX(glm::vec3 hsv)
	{
		// Converting to RGB then converting to Hex
		return RGBtoHEX(HSVtoRGB(hsv));
	}
	static std::string HSVAtoHEX(glm::vec4 hsva)
	{
		// Converting to RGBA then converting to Hex
		return RGBAtoHEX(HSVAtoRGBA(hsva));
	}
	static glm::vec3 HEXtoRGB(std::string hex)
	{
		// Checking if first character is a hashtag, then removing it
		if (hex[0] == '#')
		{
			hex.erase(hex.begin());
		}

		// Checking if there are the correct amount of hexadecimal digits
		if (hex.size() != 6)
			return glm::vec3(-1); // -1 used as error value, since RGB is 0-1

		// Value the color will be stored in
		int decimalValues[3];

		// Looping through each digit pair
		for (int i = 0; i < 3; i++)
		{
			// Getting the individual hex values
			int hexIndex = i * 2;
			std::string tempHex = { hex[hexIndex], hex[hexIndex + 1] };

			// Converting the hex values
			decimalValues[i] = smath::hexaToDec(tempHex);
		}

		// Returning the output
		return glm::vec3((float)decimalValues[0], (float)decimalValues[1], (float)decimalValues[2]) * (1.0f / 255.0f);
	}
	static glm::vec4 HEXtoRGBA(std::string hex)
	{
		// Checking if first character is a hashtag, then removing it
		if (hex[0] == '#')
		{
			hex.erase(hex.begin());
		}

		// Checking if there are the correct amount of hexadecimal digits
		if (hex.size() != 8)
			return glm::vec4(-1); // -1 used as error value, since RGB is 0-1

		// Value the color will be stored in
		int decimalValues[4];

		// Looping through each digit pair
		for (int i = 0; i < 4; i++)
		{
			// Getting the individual hex values
			int hexIndex = i * 2;
			std::string tempHex = { hex[hexIndex], hex[hexIndex + 1] };

			// Converting the hex values
			decimalValues[i] = smath::hexaToDec(tempHex);
		}

		// Returning the output
		return glm::vec4((float)decimalValues[0], (float)decimalValues[1], (float)decimalValues[2], (float)decimalValues[3]) * (1.0f / 255.0f);
	}
	static glm::vec3 HEXtoHSV(std::string hex)
	{
		// Converting from hex to rgb, then to hsv
		return RGBtoHSV(HEXtoRGB(hex));
	}
	static glm::vec4 HEXtoHSVA(std::string hex)
	{
		// Converting from hex to rgba, then to hsva
		return RGBAtoHSVA(HEXtoRGBA(hex));
	}

	// Operators
	Color& operator=(const Color& color) { this->color = color.color; return *this; }

private:
	glm::vec4 color;
};

// Custom Struct for Color Interaction
struct InteractingColor
{
	InteractingColor(Color* selectedColor) : selectedColor(selectedColor), interacting(false) {}
	Color* selectedColor;
	bool interacting;
};

namespace colors
{
	const Color black(0);
	const Color darkerGrey(0.256f);
	const Color murkyGrey(0.3f); // Essentially only used for viewport background color
	const Color darkGrey(0.514f);
	const Color grey(0.434f);
	const Color lightgrey(0.354f);
	const Color lightestgrey(0.825f);
	const Color white(1);
	
	const Color blunderGreen(0.57f, 0.66f, 0.33f);
	const Color selectionBlue(0.33f, 0.57f, 0.66f);
	
	const Color red(1, 0, 0);
	const Color green(0, 1, 0);
	const Color blue(0, 0, 1);
	const Color darkRed(0.3f, 0, 0);
	const Color darkGreen(0, 0.3f, 0);
	const Color darkBlue(0, 0, 0.3f);
	const Color lightRed(1, 0.3f, 0.3f);
	const Color lightGreen(0.3f, 1, 0.3f);
	const Color lightBlue(0.3f, 0.3f, 1);
}


#endif // !COLOR
