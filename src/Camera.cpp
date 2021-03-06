#include "Camera.hpp"

#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(float fov, float aspectRatio) :
	viewMatrix(1.0f), position(0.0f), 
	front({ 0.0f, 0.0f, -1.0f }), up({ 0.0f, 1.0f, 0.0f }), 
	yawPitchRoll({ -90.0f, 0.0f, 0.0f })
{
	projectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, 0.1f, 100.0f);
}

void Camera::SetPosition(const glm::vec3& position)
{
	this->position = position;
}

void Camera::UpdatePerspective(float fov, float aspectRatio)
{
	projectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, 0.1f, 100.0f);
}

void Camera::MoveForward(float amount, float frametime)
{
	position += amount * front * frametime;
}

void Camera::MoveRight(float amount, float frametime)
{
	position += amount * glm::normalize(glm::cross(front, up)) * frametime;
}

void Camera::MoveUp(float amount, float frametime)
{
	position += amount * up * frametime;
}

void Camera::HandleMouseMoved(double deltaX, double deltaY, float sensitivity, float frametime)
{
	deltaX *= sensitivity * frametime;
	deltaY *= sensitivity * frametime;

	yawPitchRoll.x += deltaX;
	yawPitchRoll.y += deltaY;

	if (yawPitchRoll.y > 89.0f)
		yawPitchRoll.y = 89.0f;
	if (yawPitchRoll.y < -89.0f)
		yawPitchRoll.y = -89.0f;

	glm::vec3 direction;
	direction.x = cos(glm::radians(yawPitchRoll.x)) * cos(glm::radians(yawPitchRoll.y));
	direction.y = sin(glm::radians(yawPitchRoll.y));
	direction.z = sin(glm::radians(yawPitchRoll.x)) * cos(glm::radians(yawPitchRoll.y));
	front = glm::normalize(direction);
}

const glm::mat4& Camera::GetViewMatrix()
{
	viewMatrix = glm::lookAt(position, position + front, up);
	return viewMatrix;
}

const glm::mat4& Camera::GetProjectionMatrix() const
{
	return projectionMatrix;
}
