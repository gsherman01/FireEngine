#include "Shader.h"

#include <glad/glad.h>

#include <fstream>
#include <iostream>
#include <sstream>

Shader::Shader() : m_program(0)
{
}

Shader::~Shader()
{
    if (m_program != 0)
    {
        glDeleteProgram(m_program);
    }
}

bool Shader::LoadFromFiles(const std::string& vertexPath, const std::string& fragmentPath)
{
    std::string vertexSource;
    std::string fragmentSource;

    if (!ReadTextFile(vertexPath, vertexSource) || !ReadTextFile(fragmentPath, fragmentSource))
    {
        return false;
    }

    const unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    if (!CompileShader(vertexShader, vertexSource) || !CompileShader(fragmentShader, fragmentSource))
    {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return false;
    }

    const bool linked = LinkProgram(vertexShader, fragmentShader);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return linked;
}

void Shader::Use() const
{
    glUseProgram(m_program);
}

void Shader::SetMat4(const std::string& name, const glm::mat4& value) const
{
    glUniformMatrix4fv(glGetUniformLocation(m_program, name.c_str()), 1, GL_FALSE, &value[0][0]);
}

void Shader::SetVec3(const std::string& name, const glm::vec3& value) const
{
    glUniform3f(glGetUniformLocation(m_program, name.c_str()), value.x, value.y, value.z);
}

void Shader::SetFloat(const std::string& name, float value) const
{
    glUniform1f(glGetUniformLocation(m_program, name.c_str()), value);
}

void Shader::SetInt(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(m_program, name.c_str()), value);
}

bool Shader::CompileShader(unsigned int shader, const std::string& source) const
{
    const char* sourcePtr = source.c_str();
    glShaderSource(shader, 1, &sourcePtr, nullptr);
    glCompileShader(shader);

    int success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (success == GL_FALSE)
    {
        char infoLog[1024] = {};
        glGetShaderInfoLog(shader, static_cast<int>(sizeof(infoLog)), nullptr, infoLog);
        std::cerr << "Shader compile error: " << infoLog << std::endl;
        return false;
    }

    return true;
}

bool Shader::LinkProgram(unsigned int vertexShader, unsigned int fragmentShader)
{
    if (m_program != 0)
    {
        glDeleteProgram(m_program);
        m_program = 0;
    }

    m_program = glCreateProgram();
    glAttachShader(m_program, vertexShader);
    glAttachShader(m_program, fragmentShader);
    glLinkProgram(m_program);

    int success = 0;
    glGetProgramiv(m_program, GL_LINK_STATUS, &success);

    if (success == GL_FALSE)
    {
        char infoLog[1024] = {};
        glGetProgramInfoLog(m_program, static_cast<int>(sizeof(infoLog)), nullptr, infoLog);
        std::cerr << "Program link error: " << infoLog << std::endl;
        return false;
    }

    return true;
}

bool Shader::ReadTextFile(const std::string& path, std::string& output) const
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cerr << "Failed to open shader file: " << path << std::endl;
        return false;
    }

    std::ostringstream stream;
    stream << file.rdbuf();
    output = stream.str();
    return true;
}
