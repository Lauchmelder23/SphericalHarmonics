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
	const glm::mat4 GetModelMatrix();

	float* GetColorVPtr();

public:
	glm::vec3 color;

private:
	static Shader* defaultShader;
};