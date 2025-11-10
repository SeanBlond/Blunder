#ifndef CAMERA
#pragma once

#include <glm/glm.hpp>
#include "../math/smath.h"

class Camera
{
public:
    // Constructor
    Camera(float fov, float speed, bool doPerspective = true) : fov(fov), speed(speed), doPerspective(doPerspective) {}
    
    // Getters
    float getFOV() { return fov; }
    float getSpeed() { return speed; }
    bool getDoPerspective() { return doPerspective; }

    // Setters
    void setFOV(float fov) { this->fov = smath::clamp(fov, 1.0f, 120.0f); }
    void setSpeed(float speed) { this->speed = speed; }
    void setDoPerspective(bool doPerspective) { this->doPerspective = doPerspective; }
    void addFOV(float fov) { this->fov += fov; this->fov = smath::clamp(this->fov, 1.0f, 120.0f); }

    // Virtual Functions
    virtual glm::vec3 getCameraNormal() = 0;
    virtual glm::vec3 getPosition() = 0;
    virtual glm::mat4 getViewMatrix() = 0;

    // Functions
    glm::mat4 getProjectionMatrix(float width, float height, float nearPlane, float farPlane)
    {
        return (doPerspective ? smath::perspective(fov, (width / height), nearPlane, farPlane) : smath::orthographic(fov, (width / height), nearPlane, farPlane));
    }
    glm::mat4 getProjectionMatrix(float aspectRatio, float nearPlane, float farPlane) 
    { 
        return (doPerspective ? smath::perspective(fov, aspectRatio, nearPlane, farPlane) : smath::orthographic(fov, aspectRatio, nearPlane, farPlane));
    }

protected:
    float fov;
    float speed;
    bool doPerspective;
};

class OrbitCamera : public Camera
{
public:
    // Constructors
    OrbitCamera(float phi, float theta, float distance, glm::vec3 pivot = glm::vec3(0.0f), float fov = 60.0f, float speed = 6.0f, bool doPerspective = true)
        : phi(phi), theta(theta), distance(distance), pivot(pivot), Camera(fov, speed, doPerspective) {}

    // Getters
    glm::vec3 getPivot();
    float getPhi();
    float getTheta();
    float getDistance();
    glm::vec3 getCameraNormal() override;
    glm::vec3 getPosition() override;
    glm::mat4 getViewMatrix() override;

    // Setters
    void setPivot(glm::vec3 pivot);
    void addPivot(glm::vec3 pivot);

    void setPhi(float phi);
    void setTheta(float theta);
    void setAngles(glm::vec2 angles);
    void setAngles(float phi, float theta);
    void addAngles(glm::vec2 angles);
    void addAngles(float phi, float theta);

    void setDistance(float distance);
    void addDistance(float distance);

private:
    glm::vec3 pivot;
    float phi;
    float theta;
    float distance;
};

class FlyCamera : public Camera
{
public:
    // Constructors
    FlyCamera(glm::vec3 cameraPos, float fov = 60.0f, float yaw = -90.0f, float pitch = 0.0f, float speed = 6.0f, bool doPerspective = true)
        : cameraPos(cameraPos), yaw(yaw), pitch(pitch), Camera(fov, speed, doPerspective) { cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); }

    // Getters
    float getYaw();
    float getPitch();

    // Setters
    void setCameraPos(glm::vec3 pos);
    void setYaw(float yaw);
    void setPitch(float pitch);
    void addYaw(float yaw);
    void addPitch(float pitch);

    // Functions
    glm::vec3 generateFront();
    void moveForward(float value);
    void moveSideways(float value);
    void moveUp(float value);
    void processMouseMovement(glm::vec2 mouseDelta);
    void processKeyInput(GLFWwindow* window, float deltaTime);
    glm::vec3 getCameraNormal() override;
    glm::vec3 getPosition() override;
    glm::mat4 getViewMatrix() override;

private:
    glm::vec3 cameraPos;
    glm::vec3 cameraUp;
    float yaw;
    float pitch;
};

#endif // !CAMERA
