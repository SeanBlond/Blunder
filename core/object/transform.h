#ifndef TRASNFORM
#pragma once

#include <iostream>
#include <glm/glm.hpp>

class Transform 
{
public:
	// Constructors
	Transform(glm::vec3 position = glm::vec3(0), glm::vec3 rotation = glm::vec3(0), glm::vec3 scale = glm::vec3(1)) : position(position), rotation(rotation), scale(scale) {}

	// Modifiers
	// Position
	void addPosition(glm::vec3 position) { this->position += position; }
	void addPosition(float x, float y, float z) { this->position += glm::vec3(x, y, z); }
	void addPosition(float position) { this->position += glm::vec3(position); }
	void addPositionX(float position) { this->position.x += position; }
	void addPositionY(float position) { this->position.y += position; }
	void addPositionZ(float position) { this->position.z += position; }

	// Rotation
	void addRotation(glm::vec3 rotation) { this->rotation += rotation; }
	void addRotation(float x, float y, float z) { this->rotation += glm::vec3(x, y, z); }
	void addRotation(float rotation) { this->rotation += glm::vec3(rotation); }
	void addRotationX(float rotation) { this->rotation.x += rotation; }
	void addRotationY(float rotation) { this->rotation.y += rotation; }
	void addRotationZ(float rotation) { this->rotation.z += rotation; }

	// Scale
	void addScale(glm::vec3 scale) { this->scale += scale; }
	void addScale(float x, float y, float z) { this->scale += glm::vec3(x, y, z); }
	void addScale(float scale) { this->scale += glm::vec3(scale); }
	void addScaleX(float scale) { this->scale.x += scale; }
	void addScaleY(float scale) { this->scale.y += scale; }
	void addScaleZ(float scale) { this->scale.z += scale; }

	// Functions
	Transform& operator=(const Transform& transform)
	{
		this->position = transform.position;
		this->rotation = transform.rotation;
		this->scale = transform.scale;

		return *this;
	}
	glm::mat4 getTransformMatrix()
	{
		return (glm::mat4(1) * smath::translate(position) * smath::rotate(rotation) * smath::scale(scale));
	}

	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
};

#endif // !TRASNFORM
