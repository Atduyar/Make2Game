#include "Shader.h"

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <GL/glew.h>

#include "Renderer.h"


Shader::Shader(const std::string& filepath)
	: m_RendererID(0), m_FilePath(filepath)
{
    const ShaderProgramSource source = ParseShader(filepath);
    m_RendererID = CreateShader(source.VertexSource, source.FragmentSource);
}

Shader::~Shader()
{
    glDeleteProgram(m_RendererID);
}



ShaderProgramSource Shader::ParseShader(const std::string& filePath) {
    std::ifstream stream(filePath);
    if (stream.fail()) {
        std::cout << "ERROR: filePath not find!" << std::endl;
    }

    enum class ShaderType {
        NONE = -1,
        VERTEX = 0,
        FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;
    while (std::getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos) {
            if (line.find("vertex") != std::string::npos) {
                type = ShaderType::VERTEX;
            }
            else if (line.find("fragment") != std::string::npos) {
                type = ShaderType::FRAGMENT;
            }
        }
        else {
            ss[(int)type] << line << "\n";
        }
    }

    //std::cout << ss[(int)ShaderType::VERTEX].str() <<  std::endl;
    //std::cout << ss[(int)ShaderType::FRAGMENT].str() <<  std::endl;

    return { ss[(int)ShaderType::VERTEX].str(), ss[(int)ShaderType::FRAGMENT].str() };
}


uint32_t Shader::CompileShader(uint32_t type, const std::string& source) {
    const char* src = source.c_str();

    uint32_t id = glCreateShader(type);
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    //TODO: Error Hendiling
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cerr << "Fail " << __func__ << ":" << std::endl;
        std::cerr << message << std::endl;
    }

    return id;
}

uint32_t Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
    const uint32_t program = glCreateProgram();

    uint32_t vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    uint32_t fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);

    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

void Shader::Bind() const
{
    glUseProgram(m_RendererID);
}

void Shader::Unbind() const
{
    glUseProgram(0);
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3) const
{
    glUniform4f(GetUniformLocation(name), v0, v1, v2, v3);
}
void Shader::SetUniform4i(const std::string& name, int v0, int v1, int v2, int v3) const
{
    glUniform4i(GetUniformLocation(name), v0, v1, v2, v3);
}
void Shader::SetUniform1i(const std::string& name, int v0) const
{
    glUniform1i(GetUniformLocation(name), v0);
}
void Shader::SetUniformMat4f(const std::string& name, const glm::mat4& matrix) const
{
    glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]);
}
int Shader::GetUniformLocation(const std::string& name) const
{
    if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
        return m_UniformLocationCache[name];

    const int location = glGetUniformLocation(m_RendererID, name.c_str());
    if (location == -1)
        std::cout << "Warning: Uniform '" << name << "' doesn't exist!" << std::endl;
    m_UniformLocationCache[name] = location;
    return  location;
}
