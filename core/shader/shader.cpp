#include "shader.h"
using namespace shdr;

// Constructor
Shader::Shader(const char* vertexShaderSource, const char* fragmentShaderSource)
{
    // Loading in shader files
    std::ifstream vertexShaderFile(vertexShaderSource);
    std::ifstream fragmentShaderFile(fragmentShaderSource);

    if (vertexShaderFile.fail())
    {
        std::cout << "Vertex Shader File Failed to Open" << std::endl;
    }
    if (fragmentShaderFile.fail())
    {
        std::cout << "Fragment Shader File Failed to Open" << std::endl;
    }

    // Converting shader files into C strings
    std::stringstream vertexShaderStream, fragmentShaderStream;
    vertexShaderStream << vertexShaderFile.rdbuf();
    fragmentShaderStream << fragmentShaderFile.rdbuf();
    std::string vertexShaderString = vertexShaderStream.str();
    std::string fragmentShaderString = fragmentShaderStream.str();
    const char* vertexShaderCode = vertexShaderString.c_str();
    const char* fragmentShaderCode = fragmentShaderString.c_str();

    // Closing Files
    vertexShaderFile.close();
    fragmentShaderFile.close();

    // Assigning the Shaders
    // Vertex Shader
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderCode, NULL);
    glCompileShader(vertexShader);

    // Fragment Shader
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderCode, NULL);
    glCompileShader(fragmentShader);

    CreateShader();
}

Shader::Shader(const char* vertexShaderCode, const char* fragmentShaderCode, int shaderText)
{
    // Assigning the Shaders
    // Vertex Shader
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderCode, NULL);
    glCompileShader(vertexShader);

    // Fragment Shader
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderCode, NULL);
    glCompileShader(fragmentShader);

    CreateShader();
}

Shader::Shader()
{
    const char* vertexShaderCode = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aNormal;
        layout (location = 2) in vec2 atexCoord;
  
        out vec3 normal;
        out vec2 texCoord;

        uniform mat4 transformation;
        uniform mat4 projection;

        void main()
        {
            gl_Position = projection * transformation * vec4(aPos, 1);
            normal = aNormal;
            texCoord = atexCoord;
        }      
    )";
    const char* fragmentShaderCode = R"(
        #version 330 core
        out vec4 FragColor;  
        in vec3 normal;
        in vec2 texCoord;

        void main()
        {
            vec3 light = vec3(0, 1, 1);
            float shading = (dot(normal, light) + 1) / 2;
            vec3 color = vec3(0.9, 0.9, 0.9) * shading;

            FragColor = vec4(color, 1);
        }
    )";

    // Creating the Shaders
    // Vertex Shader
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderCode, NULL);
    glCompileShader(vertexShader);

    // Fragment Shader
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderCode, NULL);
    glCompileShader(fragmentShader);

    CreateShader();
}

// Destructor
Shader::~Shader()
{
    deallocateShader();
}

// Class Functions
void Shader::useShader()
{
    glUseProgram(shaderProgram);
}

void Shader::deallocateShader()
{
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glDeleteProgram(shaderProgram);
}

void Shader::CreateShader()
{
    // Creating the Shaders

    // Check For Errors, and output it to the screen (if there are any)
    int success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << std::endl << "VERTEX SHADER ERROR " << infoLog << std::endl;
    }

    // Check For Errors, and output it to the screen (if there are any)
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << std::endl << "FRAGMENT SHADER ERROR " << infoLog << std::endl;
    }

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << std::endl << "SHADER LINKING ERROR " << infoLog << std::endl;
    }
}

Shader& Shader::operator=(const Shader &aShader)
{
    this->shaderProgram = aShader.shaderProgram;
    this->vertexShader = aShader.vertexShader;
    this->fragmentShader = aShader.fragmentShader;
    void CreateShader();

    return *this;
}


// Setting Values
void Shader::setBool(const std::string& name, bool value) const
{
    glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), (int)value);
}
void Shader::setInt(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), value);
}
void Shader::setFloat(const std::string& name, float value) const
{
    glUniform1f(glGetUniformLocation(shaderProgram, name.c_str()), value);
}
void Shader::setVec2(const std::string& name, const glm::vec2& value) const
{
    glUniform2fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, &value[0]);
}
void Shader::setVec2(const std::string& name, float x, float y) const
{
    glUniform2f(glGetUniformLocation(shaderProgram, name.c_str()), x, y);
}
void Shader::setVec3(const std::string& name, const glm::vec3& value) const
{
    glUniform3fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, &value[0]);
}
void Shader::setVec3(const std::string& name, float x, float y, float z) const
{
    glUniform3f(glGetUniformLocation(shaderProgram, name.c_str()), x, y, z);
}
void Shader::setVec4(const std::string& name, const glm::vec4& value) const
{
    glUniform4fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, &value[0]);
}
void Shader::setVec4(const std::string& name, float x, float y, float z, float w) const
{
    glUniform4f(glGetUniformLocation(shaderProgram, name.c_str()), x, y, z, w);
}
void Shader::setMat2(const std::string& name, const glm::mat2& mat) const
{
    glUniformMatrix2fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
void Shader::setMat3(const std::string& name, const glm::mat3& mat) const
{
    glUniformMatrix3fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
void Shader::setMat4(const std::string& name, const glm::mat4& mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

Texture2D::Texture2D(const char* filePath, int filterMode, int wrapMode)
{
    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_2D, m_id);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMode);
    stbi_set_flip_vertically_on_load(true);
    // load and generate the texture
    unsigned char* data = stbi_load(filePath, &m_width, &m_height, &m_nrChannels, 0);
    if (data)
    {
        int format = ((m_nrChannels == 4) ? GL_RGBA : (m_nrChannels == 3) ? GL_RGB : (m_nrChannels == 2) ? GL_RG : GL_RED);
        glTexImage2D(GL_TEXTURE_2D, 0, format, m_width, m_height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
}
Texture2D::~Texture2D()
{
    glDeleteTextures(1, &m_id);
}
void Texture2D::Bind(unsigned int slot)
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_id);    
}