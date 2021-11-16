#include <iostream>
#include <chrono>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include "Model.hpp"
#include "Orbital.hpp"
#include "Shader.hpp"
#include "Camera.hpp"

struct UserData
{
	glm::mat4* projectionMatrix;
	Camera* camera;
	float frametime;
	double lastX, lastY;
	bool mouseMovedBefore;
	bool cursorEnabled;
};

void OnFramebufferResize(GLFWwindow* window, int width, int height);
void OnMouseMoved(GLFWwindow* window, double xpos, double ypos);
void OnKeyPressed(GLFWwindow* window, int key, int scancode, int action, int mode);

void ProcessInput(GLFWwindow* window);

int main(int argc, char** argv)
{
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(1200, 800, "Orbital Demonstation", NULL, NULL);
	if (window == NULL)
	{
		std::cerr << "Failed to create GLFW window" << std::endl;
		return -1;
	}

	glfwMakeContextCurrent(window);

	glfwSetFramebufferSizeCallback(window, OnFramebufferResize);
	glfwSetCursorPosCallback(window, OnMouseMoved);
	glfwSetKeyCallback(window, OnKeyPressed);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	Model triangle(
		{
			-1.0f, -1.0f, 1.0f,			// Front Bottom, Left
			-1.0f,  1.0f, 1.0f,			// Front Top Left
			 1.0f,  1.0f, 1.0f,			// Front Top Right
			 1.0f, -1.0f, 1.0f,			// Front Bottom Right

			-1.0f, -1.0f, -1.0f,		// Back Bottom, Left
			-1.0f,  1.0f, -1.0f,		// Back Top Left
			 1.0f,  1.0f, -1.0f,		// Back Top Right
			 1.0f, -1.0f, -1.0f,		// Back Bottom Right
		},
		{
			0, 1, 2,
			0, 2, 3,

			3, 2, 6,
			3, 6, 7,
			
			4, 5, 1,
			4, 1, 0,

			1, 5, 6,
			1, 6, 2,

			4, 0, 3,
			4, 3, 7,

			7, 6, 5,
			7, 5, 4
		}
	);

	Orbital orbital(4, 2);

	glm::mat4 modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(3.0f));
	Camera camera;
	camera.SetPosition(glm::vec3(0.0f, 0.0f, 3.0f));

	glm::mat4 projectionMatrix = glm::perspective(glm::radians(110.0f), 1200.0f / 800.0f, 0.1f, 100.0f);

	Shader shader(
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

	UserData data = {
		&projectionMatrix,
		&camera,
		0.0,
		0.0, 0.0,
		false,
		false
	};
	glfwSetWindowUserPointer(window, &data);

	std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

	glViewport(0, 0, 1200, 800);
	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	while (!glfwWindowShouldClose(window))
	{
		std::chrono::duration<float> framedelta = std::chrono::duration_cast<std::chrono::duration<float>>(std::chrono::system_clock::now() - start);
		data.frametime = framedelta.count();
		start = std::chrono::system_clock::now();

		glfwPollEvents();
		ProcessInput(window);

		// modelMatrix = glm::rotate(modelMatrix, glm::radians(1.0f), glm::vec3(1.0f, 0.8f, -0.2f));

		glClearColor(0.0f, 0.0f, 0.1f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.Bind();
		shader.SetMatrix("model", &modelMatrix[0][0]);
		shader.SetMatrix("view", camera.GetViewMatrix());
		shader.SetMatrix("projection", &projectionMatrix[0][0]);
		// triangle.Draw();
		orbital.Draw();

		glfwSwapBuffers(window);
	}

	glfwDestroyWindow(window);

	return 0;
}

void OnFramebufferResize(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	UserData* data = (UserData*)glfwGetWindowUserPointer(window);
	*(data->projectionMatrix) = glm::perspective(glm::radians(110.0f), (float)width / (float)height, 0.1f, 100.0f);
}

void OnMouseMoved(GLFWwindow* window, double xpos, double ypos)
{
	UserData* data = (UserData*)glfwGetWindowUserPointer(window);
	float sensitivity = 5.0f;

	if (!data->mouseMovedBefore)
	{
		data->lastX = xpos;
		data->lastY = ypos;
		data->mouseMovedBefore = true;
	}

	double deltaX = xpos - data->lastX;
	double deltaY = data->lastY - ypos;
	data->lastX = xpos;
	data->lastY = ypos;
	
	if (!data->cursorEnabled)
	{
		data->camera->HandleMouseMoved(deltaX, deltaY, sensitivity, data->frametime);
	}
}

void OnKeyPressed(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	UserData* data = (UserData*)glfwGetWindowUserPointer(window);

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		data->cursorEnabled = !data->cursorEnabled;
		glfwSetInputMode(window, GLFW_CURSOR, (data->cursorEnabled ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED));
	}
}

void ProcessInput(GLFWwindow* window)
{
	UserData* data = (UserData*)glfwGetWindowUserPointer(window);
	float cameraSpeed = 3.0f;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		data->camera->MoveForward(cameraSpeed, data->frametime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		data->camera->MoveForward(-cameraSpeed, data->frametime);

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		data->camera->MoveRight(cameraSpeed, data->frametime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		data->camera->MoveRight(-cameraSpeed, data->frametime);

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		data->camera->MoveUp(cameraSpeed, data->frametime);
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		data->camera->MoveUp(-cameraSpeed, data->frametime);
}
