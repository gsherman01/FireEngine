#pragma once

#include <glm/glm.hpp>
#include <string>

class Shader
{
public:
    Shader();
    ~Shader();

    bool LoadFromFiles(const std::string& vertexPath, const std::string& fragmentPath);
    void Use() const;

    void SetMat4(const std::string& name, const glm::mat4& value) const;
    void SetVec3(const std::string& name, const glm::vec3& value) const;
    void SetFloat(const std::string& name, float value) const;
    void SetInt(const std::string& name, int value) const;

    unsigned int GetProgram() const { return m_program; }

private:
    unsigned int m_program;

    bool CompileShader(unsigned int shader, const std::string& source) const;
    bool LinkProgram(unsigned int vertexShader, unsigned int fragmentShader);
    bool ReadTextFile(const std::string& path, std::string& output) const;
};
