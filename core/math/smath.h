#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <stdio.h>
#include <math.h>
#include <vector>

#include "../ew/external/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace smath
{
    constexpr float PI = 3.141593;
    constexpr float TAU = PI * 2.0f;
    constexpr float DEG2RAD = (PI / 180.0f);
    constexpr float RAD2DEG = (180.0f / PI);
    inline float Radians(float degrees) 
    {
        return degrees * DEG2RAD;
    }
    inline float Degrees(float radians) 
    {
        return radians * RAD2DEG;
    }

    inline glm::mat4 rotate(glm::vec3 angles)
    {
        glm::mat4 xRotation, yRotation, zRotation;
        xRotation = glm::mat4(1.0f);
        yRotation = glm::mat4(1.0f);
        zRotation = glm::mat4(1.0f);

        // Creating the X-Rotation  Matrix
        xRotation = glm::mat4(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, cos(angles.x), sin(angles.x), 0.0f,
            0.0f, -sin(angles.x), cos(angles.x), 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );

        // Creating the Y-Rotation  Matrix
        yRotation = glm::mat4(
            cos(angles.y), 0.0f, -sin(angles.y), 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            sin(angles.y), 0.0f, cos(angles.y), 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );

        // Creating the X-Rotation  Matrix
        zRotation = glm::mat4(
            cos(angles.z), sin(angles.z), 0.0f, 0.0f,
            -sin(angles.z), cos(angles.z), 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );

        glm::mat4 rotationMatrix = (xRotation * zRotation * yRotation);

        return rotationMatrix;
    }

    inline glm::mat4 translate(glm::vec3 position)
    {
        glm::mat4 translation = glm::mat4(0);
        translation[0][0] = 1;
        translation[0][1] = 0;
        translation[0][2] = 0;
        translation[0][3] = 0;

        translation[1][0] = 0;
        translation[1][1] = 1;
        translation[1][2] = 0;
        translation[1][3] = 0;

        translation[2][0] = 0;
        translation[2][1] = 0;
        translation[2][2] = 1;
        translation[2][3] = 0;

        translation[3][0] = position.x;
        translation[3][1] = position.y;
        translation[3][2] = position.z;
        translation[3][3] = 1;

        return translation;
    }

    inline glm::mat4 scale(glm::vec3 scalar)
    {
        glm::mat4 scale = glm::mat4(0);
        scale[0][0] = scalar.x;
        scale[0][1] = 0;
        scale[0][2] = 0;
        scale[0][3] = 0;

        scale[1][0] = 0;
        scale[1][1] = scalar.y;
        scale[1][2] = 0;
        scale[1][3] = 0;

        scale[2][0] = 0;
        scale[2][1] = 0;
        scale[2][2] = scalar.z;
        scale[2][3] = 0;

        scale[3][0] = 0;
        scale[3][1] = 0;
        scale[3][2] = 0;
        scale[3][3] = 1;

        return scale;
    }

    inline glm::mat4 perspective(float fov, float aspectRatio, float n, float f)
    {
        fov = smath::Radians(fov);

        float t = tan(fov / 2.0f) * n;
        float b = -t;
        float r = t * aspectRatio;
        float l = -t * aspectRatio;

        // Creating the Matrix
        glm::mat4 perspMatrix(0);
        perspMatrix[0][0] = 2 * n / (r - l);
        perspMatrix[0][1] = 0;
        perspMatrix[0][2] = 0;
        perspMatrix[0][3] = 0;

        perspMatrix[1][0] = 0;
        perspMatrix[1][1] = 2 * n / (t - b);
        perspMatrix[1][2] = 0;
        perspMatrix[1][3] = 0;

        perspMatrix[2][0] = (r + l) / (r - l);
        perspMatrix[2][1] = (t + b) / (t - b);
        perspMatrix[2][2] = -(f + n) / (f - n);
        perspMatrix[2][3] = -1;

        perspMatrix[3][0] = 0;
        perspMatrix[3][1] = 0;
        perspMatrix[3][2] = -2 * f * n / (f - n);
        perspMatrix[3][3] = 0;

        return perspMatrix;
    }
    
    inline glm::mat4 orthographic(float l, float r, float b, float t)
    {
        // Creating the Matrix
        glm::mat4 orthMatrix(0);
        orthMatrix[0][0] = 2 / (r - l);
        orthMatrix[0][1] = 0;
        orthMatrix[0][2] = 0;
        orthMatrix[0][3] = 0;

        orthMatrix[1][0] = 0;
        orthMatrix[1][1] = 2 / (t - b);
        orthMatrix[1][2] = 0;
        orthMatrix[1][3] = 0;

        orthMatrix[2][0] = 0;
        orthMatrix[2][1] = 0;
        orthMatrix[2][2] = -1;
        orthMatrix[2][3] = 0;

        orthMatrix[3][0] = -((r + l) / (r - l));
        orthMatrix[3][1] = -((t + b) / (t - b));
        orthMatrix[3][2] = 0;
        orthMatrix[3][3] = 1;

        return orthMatrix;
    }

    inline glm::mat4 orthographic(float l, float r, float b, float t, float n, float f)
    {
        // Creating the Matrix
        glm::mat4 orthMatrix(0);
        orthMatrix[0][0] = 2 / (r - l);
        orthMatrix[0][1] = 0;
        orthMatrix[0][2] = 0;
        orthMatrix[0][3] = 0;

        orthMatrix[1][0] = 0;
        orthMatrix[1][1] = 2 / (t - b);
        orthMatrix[1][2] = 0;
        orthMatrix[1][3] = 0;

        orthMatrix[2][0] = 0;
        orthMatrix[2][1] = 0;
        orthMatrix[2][2] = -2 / (f - n);
        orthMatrix[2][3] = 0;

        orthMatrix[3][0] = -((r + l) / (r - l));
        orthMatrix[3][1] = -((t + b) / (t - b));
        orthMatrix[3][2] = -((f + n) / (f - n));
        orthMatrix[3][3] = 1;

        return orthMatrix;
    }

    inline glm::mat4 lookat(glm::vec3 cameraPosition, glm::vec3 lookPosition, glm::vec3 up)
    {
        // Creating Forward Vector
        glm::vec3 forward = (lookPosition - cameraPosition);
        forward = glm::normalize(forward);

        // Creating Sideways Vector
        glm::vec3 sideways = glm::cross(forward, up);
        sideways = glm::normalize(sideways);

        // Creating a Relative Up Vector
        glm::vec3 relativeUp = glm::cross(sideways, forward);
        relativeUp = glm::normalize(relativeUp);

        // Creating the Transformation matrix
        glm::mat4 lookAtMatrix;
        lookAtMatrix[0][0] = sideways.x;
        lookAtMatrix[0][1] = relativeUp.x;
        lookAtMatrix[0][2] = -forward.x;
        lookAtMatrix[0][3] = 0;

        lookAtMatrix[1][0] = sideways.y;
        lookAtMatrix[1][1] = relativeUp.y;
        lookAtMatrix[1][2] = -forward.y;
        lookAtMatrix[1][3] = 0;

        lookAtMatrix[2][0] = sideways.z;
        lookAtMatrix[2][1] = relativeUp.z;
        lookAtMatrix[2][2] = -forward.z;
        lookAtMatrix[2][3] = 0;
        
        lookAtMatrix[3][0] = 0;
        lookAtMatrix[3][1] = 0;
        lookAtMatrix[3][2] = 0;
        lookAtMatrix[3][3] = 1;

        lookAtMatrix *= translate(-cameraPosition);

        return lookAtMatrix;

    }


    // min, max, and clamp 

    template <typename T>
    inline T clamp(T value, T min, T max)
    {
        if (value < min)
            value = min;
        if (value > max)
            value = max;
        return value;
    }

    template <typename T>
    inline T max(T value1, T value2)
    { 
        // value1 is highest
        if (value1 >= value2)
            return value1;

        // value2 is highest
        return value2;
    }

    template <typename T>
    inline T max(T value1, T value2, T value3)
    { 
        if (value3 > max(value1, value2))
            return value3;

        return max(value1, value2);
    }

    template <typename T>
    inline T min(T value1, T value2)
    { 
        // value1 is lowest
        if (value1 <= value2)
            return value1;

        // value2 is lowest
        return value2;
    }

    template <typename T>
    inline T min(T value1, T value2, T value3)
    { 
        if (value3 < min(value1, value2))
            return value3;

        return min(value1, value2);
    }

    inline void outputMat4(glm::mat4 matrix)
    {
        for (int i = 0; i < 16; i++)
        {
            if (i % 4 == 0 && i != 0)
                std::cout << std::endl;

            std::cout << (matrix[i % 4][i / 4]) << " ";
        }
        std::cout << std::endl;
    }
    inline void outputVec3(glm::vec3 vector)
    {

        std::cout << "(" << (vector.x) << ", " << (vector.y) << ", " << (vector.z) << ")" << std::endl;
    }

    template <typename T>
    inline int vectorFind(std::vector<T> vector, T value)
    {
        for (int i = 0; i < vector.size(); i++)
        {
            if (vector[i] == value)
                return i;
        }
        return -1;
    }
}