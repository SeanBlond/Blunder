#include "CubeSim.h"

void Cube::CreateCube(shdr::Shader* shader, glm::vec3 size)
{
	// Creating the Cube Object
	object = new obj::Object("Cube", mesh::createCube(size.x, size.y, size.z, 1), shader);

	// Adding the vertice corners to the list of collision corners
	corners.push_back(glm::vec3(-size.x * 0.5f,   -size.y * 0.5f,   -size.z * 0.5f));
	corners.push_back(glm::vec3(-size.x * 0.5f,   -size.y * 0.5f,    size.z * 0.5f));
	corners.push_back(glm::vec3(-size.x * 0.5f,    size.y * 0.5f,   -size.z * 0.5f));
	corners.push_back(glm::vec3(-size.x * 0.5f,    size.y * 0.5f,    size.z * 0.5f));
	corners.push_back(glm::vec3( size.x * 0.5f,   -size.y * 0.5f,   -size.z * 0.5f));
	corners.push_back(glm::vec3( size.x * 0.5f,   -size.y * 0.5f,    size.z * 0.5f));
	corners.push_back(glm::vec3( size.x * 0.5f,    size.y * 0.5f,   -size.z * 0.5f));
	corners.push_back(glm::vec3( size.x * 0.5f,    size.y * 0.5f,    size.z * 0.5f));
}
void Cube::SetCube(glm::vec3 position, glm::vec3 velocity, glm::vec3 rotation)
{
	// Storing the position in the object transform
	object->transform.position = position;

	// Storing the rotation in the object transform
	object->transform.rotation = rotation;

	// Setting the velocity
	this->velocity = velocity;
}
void Cube::UpdateSim(float deltaTime)
{
	// Updating velocity using gravitational acceleration
	glm::vec3 velocityDelta = deltaTime * -(gravity * gravity);
	addVelocity(velocityDelta);

	// Updating cube position
	UpdatePosition(deltaTime);

	// Checking each collision corner for a collision
	for (int i = 0; i < corners.size(); i++)
	{
		// Calculating the modified position of each corner
		glm::vec4 cornerPos = smath::translate(object->transform.position) * smath::rotate(object->transform.rotation) * glm::vec4(corners[i], 1);

		// Taking the dot product of each corner to check for collision with the ground
		float cornerDot = glm::dot(glm::vec3(cornerPos), groundNormal);

		// Dot Product is <= 0, collision detected
		if (cornerDot <= 0.0f)
		{
			std::cout << "Collision Detected" << std::endl;
		}
	}
}
void Cube::DrawCube(glm::mat4 projection, glm::mat4 view)
{
	// Setting the shader transformations
	object->shader->useShader();
	object->shader->setMat4("projection", projection);
	object->shader->setMat4("view", view);

	// Drawing the cube
	object->DrawMesh();
}