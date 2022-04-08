#pragma once

#include <string>
#include <unordered_map>

#include "glm/glm.hpp"

struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragmentSource;
};

class Shader
{
private:
	unsigned int m_RendererID;
	std::string m_FilePath;
	mutable std::unordered_map<std::string, int> m_UniformLocationCache;
public:
	Shader(const std::string& filepath);
	~Shader();
	
	void Bind() const;
	void Unbind() const;

	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3) const;
	void SetUniform4i(const std::string& name, int v0, int v1, int v2, int v3) const;
	void SetUniform1i(const std::string& name, int v0) const;
	void SetUniformMat4f(const std::string& name, const glm::mat4& matrix) const;

private:
	ShaderProgramSource ParseShader(const std::string& filePath);
	uint32_t CompileShader(uint32_t type, const std::string& source);
	uint32_t CreateShader(const std::string& vertexShader, const std::string& fragmentShader);

	int GetUniformLocation(const std::string& name) const;
};
