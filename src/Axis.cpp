#include "Axis.hpp"

#define TWO_PI       6.28318530718
#define PI           3.14159265359

#include <cmath>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.hpp"
#include "Camera.hpp"

Shader* Axis::defaultShader = nullptr;

Axis::Axis(const glm::vec3& direction, float length)
{
	if (defaultShader == nullptr)
	{
		defaultShader = new Shader(
			R"(
			#version 460 core

			layout(location = 0) in vec3 position;

			out vec3 outColor;

			uniform mat4 model;
			uniform mat4 view;
			uniform mat4 projection;

			void main()
			{
				outColor = vec3(0.6f, 0.6f, 0.6f);
				gl_Position = projection * view * model * vec4(position, 1.0f);
			}	
		)",

			R"(
			#version 460 core
			
			in vec3 outColor;
			out vec4 FragColor;

			void main()
			{	
				FragColor = vec4(outColor, 1.0f);
			}
		)"
		);
	}

	const unsigned int axisRingResolution = 80;

	vertices.push_back(0.0f);
	vertices.push_back(0.0f);
	vertices.push_back(length);
	for (int i = 0; i < axisRingResolution; i++)
	{
		float angle = TWO_PI * (float)i / (float)axisRingResolution;
		float x = std::cos(angle);
		float y = std::sin(angle);
		vertices.push_back(x);
		vertices.push_back(y);
		vertices.push_back(-length);

		vertices.push_back(x);
		vertices.push_back(y);
		vertices.push_back(length - 0.5f);

		vertices.push_back(2.0f * x);
		vertices.push_back(2.0f * y);
		vertices.push_back(length - 0.5f);

		indices.push_back(3 * i + 1);
		indices.push_back((3 * i + 1) % (axisRingResolution * 3) + 1);
		indices.push_back((3 * i + 4) % (axisRingResolution * 3) + 1);

		indices.push_back(3 * i + 1);
		indices.push_back((3*i + 4) % (axisRingResolution * 3) + 1);
		indices.push_back((3*i + 3) % (axisRingResolution * 3) + 1);

		indices.push_back((3 * i + 2) % (axisRingResolution * 3) + 1);
		indices.push_back(0);
		indices.push_back((3 * i + 5) % (axisRingResolution * 3) + 1);
	}

	CreateVAO();

	float angleBetweenVectors = std::acos(glm::dot(direction, glm::vec3(0.0f, 0.0f, 1.0f)) / glm::length(direction));
	if (angleBetweenVectors > 0.01)
	{
		glm::vec3 rotationAxis = glm::cross(direction, glm::vec3(0.0f, 0.0f, 1.0f));
		modelMatrix = glm::rotate(modelMatrix, -angleBetweenVectors, rotationAxis);
	}
	modelMatrix = glm::scale(modelMatrix, glm::vec3(0.03f, 0.03f, 1.0f));
}

void Axis::BindDefaultShader(Camera& camera)
{
	defaultShader->Bind();
	defaultShader->SetMatrix("model", glm::value_ptr(modelMatrix));
	defaultShader->SetMatrix("view", glm::value_ptr(camera.GetViewMatrix()));
	defaultShader->SetMatrix("projection", glm::value_ptr(camera.GetProjectionMatrix()));
}
