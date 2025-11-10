#include "camera.h"


// ORBIT CAMERA
// Getters
glm::vec3 OrbitCamera::getPivot()
{
    return pivot;
}
float OrbitCamera::getPhi()
{
    return phi;
}
float OrbitCamera::getTheta()
{
    return theta;
}
float OrbitCamera::getDistance()
{
    return distance;
}
glm::vec3 OrbitCamera::getCameraNormal()
{
    return glm::normalize(pivot - getPosition());
}
glm::vec3 OrbitCamera::getPosition()
{
    glm::vec3 cameraPosition = glm::vec3(
        distance * sin(phi) * cos(theta) + pivot.x,
        distance * cos(phi) + pivot.y,
        distance * sin(phi) * sin(theta) + pivot.z
    );
    return cameraPosition;
}
glm::mat4 OrbitCamera::getViewMatrix()
{
    glm::vec3 cameraPosition = glm::vec3(
        distance * sin(phi) * cos(theta) + pivot.x,
        distance * cos(phi) + pivot.y,
        distance * sin(phi) * sin(theta) + pivot.z
    );

    glm::mat4 viewMatrix = smath::lookat(cameraPosition, pivot, glm::vec3(0.0f, 1.0f, 0.0f));

    return viewMatrix;
}

// Setters
void OrbitCamera::setPivot(glm::vec3 pivot)
{
    this->pivot = pivot;
}
void OrbitCamera::addPivot(glm::vec3 pivot)
{
    this->pivot += pivot;
}

void OrbitCamera::setPhi(float phi)
{
    this->phi = phi;
}
void OrbitCamera::setTheta(float theta)
{
    this->theta = theta;
}
void OrbitCamera::setAngles(glm::vec2 angles)
{
    this->theta = angles.x;
    this->phi = angles.y;
}
void OrbitCamera::setAngles(float theta, float phi)
{
    this->theta = theta;
    this->phi = phi;
}
void OrbitCamera::addAngles(glm::vec2 angles)
{
    this->theta += angles.x;
    this->phi += angles.y;
    this->phi = smath::clamp(this->phi, 0.01f, 3.1f);
}
void OrbitCamera::addAngles(float theta, float phi)
{
    this->theta += theta;
    this->theta = smath::clamp(this->theta, -6.2f, 6.2f);
    this->phi += phi;
}

void OrbitCamera::setDistance(float distance)
{
    this->distance = distance;
}
void OrbitCamera::addDistance(float distance)
{
    this->distance += distance;
}



// FLY CAMERA
// Getters
float FlyCamera::getYaw()
{
    return yaw;
}
float FlyCamera::getPitch()
{
    return pitch;
}

// Setters
void FlyCamera::setCameraPos(glm::vec3 pos)
{
    this->cameraPos = pos;
}
void FlyCamera::setYaw(float yaw)
{
    this->yaw = yaw;
}
void FlyCamera::setPitch(float pitch)
{
    this->pitch = smath::clamp(pitch, -89.0f, 89.0f);
}
void FlyCamera::addYaw(float yaw)
{
    this->yaw += yaw;
}
void FlyCamera::addPitch(float pitch)
{
    this->pitch += pitch;
    this->pitch = smath::clamp(this->pitch, -89.0f, 89.0f);
}

// Functions
glm::vec3 FlyCamera::generateFront()
{
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(front);
    return front;
}
void FlyCamera::moveForward(float value)
{
    cameraPos += value * generateFront();
}
void FlyCamera::moveSideways(float value)
{
    cameraPos -= glm::normalize(glm::cross(generateFront(), cameraUp)) * value;
}
void FlyCamera::moveUp(float value)
{
    cameraPos += glm::normalize(cameraUp) * value;
}
glm::vec3 FlyCamera::getCameraNormal()
{
    return glm::normalize(generateFront() - cameraPos);
}
glm::vec3 FlyCamera::getPosition()
{
    return cameraPos;
}
glm::mat4 FlyCamera::getViewMatrix()
{
    return smath::lookat(cameraPos, cameraPos + generateFront(), cameraUp);
}
void FlyCamera::processMouseMovement(glm::vec2 mouseDelta)
{
    addYaw(mouseDelta.x);
    addPitch(mouseDelta.y);
}
void FlyCamera::processKeyInput(GLFWwindow* window, float deltaTime)
{
    // Moving the Camera
    float cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        cameraSpeed = speed * 2;
    else
        cameraSpeed = speed;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        moveForward(cameraSpeed * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        moveForward(-cameraSpeed * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        moveSideways(cameraSpeed * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        moveSideways(-cameraSpeed * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        moveUp(-cameraSpeed * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        moveUp(cameraSpeed * deltaTime);

    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
        doPerspective = true;
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
        doPerspective = false;
}