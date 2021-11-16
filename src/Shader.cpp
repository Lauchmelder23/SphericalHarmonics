#include "Shader.hpp"

#include <string>
#include <iostream>
#include <glad/glad.h>

Shader::Shader()
{
	const std::string vertexShaderSource = R"(
		#version 460 core

		layout(location = 0) in vec3 position;

		void main()
		{
			gl_Position = vec4(position, 1.0f);
		}	
		)";

	const std::string fragmentShaderSource = R"(
		#version 460 core

		out vec4 FragColor;

		void main()
		{	
			FragColor = vec4(1.0f, 0.1f, 0.1f, 1.0f);
		}
	)";

	CreateProgram(vertexShaderSource, fragmentShaderSource);
}

Shader::Shader(const std::string& vertexShaderSourceCode, const std::string& fragmentShaderSourceCode)
{
	CreateProgram(vertexShaderSourceCode, fragmentShaderSourceCode);
}

void Shader::SetMatrix(const std::string& name, const float* data)
{
	unsigned int location = glGetUniformLocation(program, name.c_str());
	glUniformMatrix4fv(location, 1, GL_FALSE, data);
}

void Shader::Bind()
{
	glUseProgram(program);
}

void Shader::CreateProgram(const std::string& vertexShaderSourceCode, const std::string& fragmentShaderSourceCode)
{
	int result;

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	const char* vertexShaderSourceCstring = vertexShaderSourceCode.c_str();
	glShaderSource(vertexShader, 1, &vertexShaderSourceCstring, NULL);
	glCompileShader(vertexShader);
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		char errorMessage[512];
		glGetShaderInfoLog(vertexShader, 512, NULL, errorMessage);

		std::cerr << "Failed to compile vertex shader: " << std::endl << errorMessage << std::endl;
		glDeleteShader(vertexShader);
		exit(-1);
	}

	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	const char* fragmentShaderSourceCstring = fragmentShaderSourceCode.c_str();
	glShaderSource(fragmentShader, 1, &fragmentShaderSourceCstring, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		char errorMessage[512];
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorMessage);

		std::cerr << "Failed to compile fragment shader: " << std::endl << errorMessage << std::endl;
		glDeleteShader(fragmentShader);
		glDeleteShader(vertexShader);
		exit(-1);
	}

	program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &result);
	if (result == GL_FALSE)
	{
		char errorMessage[512];
		glGetProgramInfoLog(program, 512, NULL, errorMessage);

		std::cerr << "Failed to link shader program: " << std::endl << errorMessage << std::endl;
		glDeleteShader(fragmentShader);
		glDeleteShader(vertexShader);
		glDeleteProgram(program);
		exit(-1);
	}

	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);
}
