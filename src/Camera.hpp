#pragma once

#include <glm/glm.hpp>

class Camera
{
public:
	Camera(float fov, float aspectRatio);

	void SetPosition(const glm::vec3& position);
	void UpdatePerspective(float fov, float aspectRatio);

	void MoveForward(float amount, float frametime);
	void MoveRight(float amount, float frametime);
	void MoveUp(float amount, float frametime);

	void HandleMouseMoved(double deltaX, double deltaY, float sensitivity, float frametime);

	const glm::mat4& GetViewMatrix();
	const glm::mat4& GetProjectionMatrix() const;

private:
	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;

	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 yawPitchRoll;
};