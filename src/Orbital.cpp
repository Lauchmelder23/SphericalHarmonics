#include "Orbital.hpp"

#include <glad/glad.h>

#define TWO_PI           6.28318530718
#define PI           3.14159265359

#include <cmath>
#include <complex>

std::complex<double> SphericalHarmonic(unsigned int l, unsigned int m, float theta, float phi);
unsigned int Fac(unsigned int n);

Orbital::Orbital(unsigned int l, unsigned int m) :
	l(l), m(m)
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

	CreateVAO();
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
