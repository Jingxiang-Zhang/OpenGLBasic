#pragma once
#include <string>
#include <unordered_map>
#include <glm/glm.hpp>

struct ShaderPromgramSource {
	std::string vertexSource;
	std::string fragmentSource;
};

class Shader 
{
private:
	std::string m_FilePath;
	unsigned int m_RenderID;
	// caching for uniforms
	std::unordered_map<std::string, int> m_UniformLocationChche;
public:
	Shader(const std::string& filepath);
	~Shader();

	void Bind() const;
	void UnBind() const;
	// set uniform
	void SetUniformMat4f(const std::string& name, const glm::mat4 matrix);
	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
	void SetUniform1i(const std::string& name, int value);
	void SetUniform1iv(const std::string& name, int length, int* value);

private:
	int GetUniformLocation(const std::string& name);
	ShaderPromgramSource ParseShader(const std::string filepath);
	unsigned int CompileShader(unsigned int type, const std::string& source);
	unsigned int CreateShader(const std::string vertexShader, const std::string fragmentShader);
};