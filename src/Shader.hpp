#pragma once

#include <string>

class Shader
{
public:
	Shader();
	Shader(const std::string& vertexShaderSourceCode, const std::string& fragmentShaderSourceCode);
	~Shader();
	
	void SetMatrix(const std::string& name, const float* data);

	void Bind();

private:
	void CreateProgram(const std::string& vertexShaderSourceCode, const std::string& fragmentShaderSourceCode);

private:
	unsigned int program;
};