#ifndef SHADER
#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "../ew/external/stb_image.h"
#include "../ew/external/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>


namespace shdr
{
    class Shader
    {
    public:
        // Constructor Prototype
        Shader(const char* vertexShaderSource, const char* fragmentShaderSource);
        Shader(const char* vertexShaderCode, const char* fragmentShaderCode, int shaderText);
        Shader();

        // Destructor
        ~Shader();

        // Shader Functions
        void useShader();
        void deallocateShader();

        Shader& operator=(const Shader &aShader);

        // Setting Values
        void setBool(const std::string& name, bool value) const;
        void setInt(const std::string& name, int value) const;
        void setFloat(const std::string& name, float value) const;
        void setVec2(const std::string& name, const glm::vec2& value) const;
        void setVec2(const std::string& name, float x, float y) const;
        void setVec3(const std::string& name, const glm::vec3& value) const;
        void setVec3(const std::string& name, float x, float y, float z) const;
        void setVec4(const std::string& name, const glm::vec4& value) const;
        void setVec4(const std::string& name, float x, float y, float z, float w) const;
        void setMat2(const std::string& name, const glm::mat2& mat) const;
        void setMat3(const std::string& name, const glm::mat3& mat) const;
        void setMat4(const std::string& name, const glm::mat4& mat) const;

        unsigned int shaderProgram;
    private:
        unsigned int vertexShader;
        unsigned int fragmentShader;

        void CreateShader();
    };



    class Texture2D {
        public:
            Texture2D(const char* filePath, int filterMode, int wrapMode);
            ~Texture2D();
            void Bind(unsigned int slot = 0); //Bind to a specific texture unit

        private:
            unsigned int m_id; //GL texture handle
            int m_width, m_height, m_nrChannels;
    };
};

#endif // !SHADER