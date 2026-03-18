#ifndef CUBE_SIMULATION
#pragma once

#include <vector>
#include "object.h"

class Cube
{
public:
	// Constructor & Deconstructor
	Cube(shdr::Shader* shader, glm::vec3 size = glm::vec3(1.0f)) : velocity(glm::vec3(0)) { CreateCube(shader, size); }
	~Cube()
	{
		delete object;
		object = nullptr;
	}

	// Accessors
	glm::vec3 getVelocity() { return velocity; }

	// Modifiers
	void addVelocity(glm::vec3 velocity) { this->velocity += velocity; }
	void setVelocity(glm::vec3 velocity) { this->velocity = velocity; }

	// Functions
	void CreateCube(shdr::Shader* shader, glm::vec3 size);
	void SetCube(glm::vec3 position, glm::vec3 velocity = glm::vec3(0), glm::vec3 rotation = glm::vec3(0));
	void UpdatePosition(float deltaTime) { object->transform.addPosition(velocity * deltaTime); }
	void UpdateSim(float deltaTime);
	void DrawCube(glm::mat4 projection, glm::mat4 view);

private:
	glm::vec3 velocity;
	std::vector<glm::vec3> corners;
	obj::Object* object;

	static glm::vec3 gravity;
	static glm::vec3 groundNormal;
	static float friction;
};


#endif // !CUBE_SIMULATION
