#pragma once

#include <array>

#include <glm/matrix.hpp>

#include "Axis.hpp"

class Shader;
class Camera;

class CoordinateSystem
{
public:
	CoordinateSystem();
	~CoordinateSystem();

	void BindDefaultShader(Camera& camera);
	void Draw();

	Axis* GetAxis(unsigned int index) { return axes[index]; };

private:
	std::array<Axis*, 3> axes;
	glm::mat4 modelMatrix;

	static Shader* defaultShader;
};