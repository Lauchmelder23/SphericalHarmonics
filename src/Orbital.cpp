#include "Orbital.hpp"

#define TWO_PI       6.28318530718
#define PI           3.14159265359

#include <cmath>
#include <complex>

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.hpp"
#include "Camera.hpp"

// Write some shaders to display the orbitals (too lazy to put them in files)
Shader* Orbital::defaultShader = nullptr; 

std::complex<double> SphericalHarmonic(unsigned int l, unsigned int m, float theta, float phi);
unsigned int Fac(unsigned int n);

Orbital::Orbital(unsigned int l, unsigned int m) :
	l(l), m(m)
{
	if (defaultShader == nullptr)
	{
		defaultShader = new Shader(
			R"(
			#version 460 core

			layout(location = 0) in vec3 position;
			layout(location = 1) in vec3 color;

			out vec3 outColor;

			uniform mat4 model;
			uniform mat4 view;
			uniform mat4 projection;

			void main()
			{
				outColor = color;
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

	UpdateModel();
	CreateVAO();

	modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(3.0f));
}

void Orbital::BindDefaultShader(Camera& camera)
{
	defaultShader->Bind();
	defaultShader->SetMatrix("model", glm::value_ptr(modelMatrix));
	defaultShader->SetMatrix("view", glm::value_ptr(camera.GetViewMatrix()));
	defaultShader->SetMatrix("projection", glm::value_ptr(camera.GetProjectionMatrix()));
}

void Orbital::UpdateModel()
{
	unsigned int verticesPerRing = 70;
	unsigned int rings = 70;

	for (int ring = 0; ring <= rings; ring++)
	{
		for (int vertex = 0; vertex < verticesPerRing; vertex++)
		{
			float phi = vertex * TWO_PI / verticesPerRing;
			float theta = ring * PI / rings;

			std::complex value = SphericalHarmonic(l, m, theta, phi);
			double distance = std::abs(std::real(value));
			// std::cout << "(" << theta << ", " << phi << ") -> " << distance << std::endl;
			vertices.push_back(distance * std::cos(phi) * std::sin(theta));
			vertices.push_back(distance * std::sin(phi) * std::sin(theta));
			vertices.push_back(distance * std::cos(theta));

			if (std::real(value) < 0)
			{
				vertices.push_back(0.85f);
				vertices.push_back(0.1f);
				vertices.push_back(0.1f);
			}
			else
			{
				vertices.push_back(0.1f);
				vertices.push_back(0.85f);
				vertices.push_back(0.1f);
			}
		}
	}

	for (int ring = 0; ring < rings; ring++)
	{
		for (int vertex = 0; vertex < verticesPerRing; vertex++)
		{
			indices.push_back(verticesPerRing * ring + vertex);
			indices.push_back(verticesPerRing * ring + ((vertex + 1) % verticesPerRing));
			indices.push_back(verticesPerRing * (ring + 1) + ((vertex + 1) % verticesPerRing));

			indices.push_back(verticesPerRing * ring + vertex);
			indices.push_back(verticesPerRing * (ring + 1) + ((vertex + 1) % verticesPerRing));
			indices.push_back(verticesPerRing * (ring + 1) + vertex);
		}
	}
}

void Orbital::DefineVAOLayout()
{
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
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
