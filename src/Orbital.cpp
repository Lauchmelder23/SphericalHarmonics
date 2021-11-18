#include "Orbital.hpp"

#define TWO_PI       6.28318530718
#define PI           3.14159265359

#include <cmath>
#include <complex>
#include <functional>

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.hpp"
#include "Camera.hpp"

// Write some shaders to display the orbitals (too lazy to put them in files)
Shader* Orbital::defaultShader = nullptr; 

std::complex<double> SphericalHarmonic(unsigned int l, unsigned int m, float theta, float phi);
unsigned int Fac(unsigned int n);

Orbital::Orbital(int l, int m) :
	l(l), m(m), positiveColor({ 1.0f, 1.0f, 0.5f }), negativeColor({ 0.5f, 1.0f, 1.0f }),
	resolution(70)
{
	if (defaultShader == nullptr)
	{
		defaultShader = new Shader(
			R"(
			#version 460 core

			layout(location = 0) in vec3 position;
			layout(location = 1) in uint sign;		// 1 = positive, 0 = negative

			out vec3 outColor;

			uniform mat4 model;
			uniform mat4 view;
			uniform mat4 projection;

			uniform vec3 positiveColor;
			uniform vec3 negativeColor;

			void main()
			{
				outColor = (sign > 0) ? positiveColor : negativeColor;
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

	CreateVAO();
	UpdateModel();

	// modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(3.0f));
}

void Orbital::BindDefaultShader(Camera& camera)
{
	defaultShader->Bind();
	defaultShader->SetMatrix("model", glm::value_ptr(modelMatrix));
	defaultShader->SetMatrix("view", glm::value_ptr(camera.GetViewMatrix()));
	defaultShader->SetMatrix("projection", glm::value_ptr(camera.GetProjectionMatrix()));

	defaultShader->SetVector3("positiveColor", glm::value_ptr(positiveColor));
	defaultShader->SetVector3("negativeColor", glm::value_ptr(negativeColor));
}

float* Orbital::GetPositiveColorVPtr()
{
	return glm::value_ptr(positiveColor);
}

float* Orbital::GetNegativeColorVPtr()
{
	return glm::value_ptr(negativeColor);
}

void Orbital::UpdateModel()
{
	vertices.clear();
	indices.clear();

	for (int ring = 0; ring <= resolution; ring++)
	{
		for (int vertex = 0; vertex < resolution; vertex++)
		{
			float phi = vertex * TWO_PI / resolution;
			float theta = ring * PI / resolution;

			std::complex value = SphericalHarmonic(l, std::abs(m), theta, phi);
			
			double distance = 0.0f;
			if(m < 0)
				distance = std::abs(std::imag(value));
			else 
				distance = std::abs(std::real(value));

			vertices.push_back(distance * std::cos(phi) * std::sin(theta));
			vertices.push_back(distance * std::sin(phi) * std::sin(theta));
			vertices.push_back(distance * std::cos(theta));

			if (m < 0)
				vertices.push_back(std::imag(value) >= 0);
			else
				vertices.push_back(std::real(value) >= 0);
			
		}
	}

	for (int ring = 0; ring < resolution; ring++)
	{
		for (int vertex = 0; vertex < resolution; vertex++)
		{
			indices.push_back(resolution * ring + vertex);
			indices.push_back(resolution * ring + ((vertex + 1) % resolution));
			indices.push_back(resolution * (ring + 1) + ((vertex + 1) % resolution));

			indices.push_back(resolution * ring + vertex);
			indices.push_back(resolution * (ring + 1) + ((vertex + 1) % resolution));
			indices.push_back(resolution * (ring + 1) + vertex);
		}
	}

	UpdateBufferData();
}

void Orbital::DefineVAOLayout()
{
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float) + 1 * sizeof(unsigned int), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, 3 * sizeof(float) + 1 * sizeof(unsigned int), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

std::complex<double> SphericalHarmonic(unsigned int l, unsigned int m, float theta, float phi)
{
	float N = std::sqrt((2 * l + 1) / 2.0f * Fac(l - m) / Fac(l + m));
	return std::complex(1.0 / std::sqrt(TWO_PI) * N * std::assoc_legendre(l, m, std::cos(theta)), 0.0) * std::exp(std::complex(0.0, m * (double)phi));
}

unsigned int Fac(unsigned int n)
{
	unsigned int prod = 1;
	for (int i = 2; i <= n; i++)
		prod *= i;

	return prod;
}
