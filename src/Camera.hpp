#pragma once

#include <glm/glm.hpp>

class Camera
{
public:
	Camera();

	float* GetViewMatrix();

	void SetPosition(const glm::vec3& position);

	void MoveForward(float amount, float frametime);
	void MoveRight(float amount, float frametime);
	void MoveUp(float amount, float frametime);

	void HandleMouseMoved(double deltaX, double deltaY, float sensitivity, float frametime);

private:
	glm::mat4 viewMatrix;
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 yawPitchRoll;
};