#pragma once

#include <glm/vec3.hpp>
#include "Model.hpp"

class Shader;
class Camera;

class Axis : public Model
{
public:
	Axis(const glm::vec3& direction, float length);

	void BindDefaultShader(Camera& camera);

private:
	static Shader* defaultShader;
};