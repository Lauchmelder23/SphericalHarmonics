#include <iostream>
#include <chrono>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

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
	// Initialize GLFW and let it know what OpenGL version/profile we're using
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create a new window 
	// TODO: Magic numbers kinda bad
	GLFWwindow* window = glfwCreateWindow(1200, 800, "Orbital Demonstation", NULL, NULL);
	if (window == NULL)
	{
		std::cerr << "Failed to create GLFW window" << std::endl;
		return -1;
	}

	// Make window context and set callbacks for resizing and keyboard/mouse events
	glfwMakeContextCurrent(window);

	glfwSetFramebufferSizeCallback(window, OnFramebufferResize);
	glfwSetCursorPosCallback(window, OnMouseMoved);
	glfwSetKeyCallback(window, OnKeyPressed);

	// Let GLFW trap the cursor (needed for proper camera movement)
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Load OpenGL functions from driver
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// Set up ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 460 core");

	ImGui::StyleColorsDark();

	// Create some orbital and set up its transformation matrix
	// TODO: the matrix should probably be part of Model
	Orbital orbital(2, 1);

	glm::mat4 modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(3.0f));

	// Set up a camera 
	// TODO: should the projection matrix be part of the camera?
	Camera camera;
	camera.SetPosition(glm::vec3(0.0f, 0.0f, 3.0f));

	glm::mat4 projectionMatrix = glm::perspective(glm::radians(110.0f), 1200.0f / 800.0f, 0.1f, 100.0f);

	// Write some shaders to display the orbitals (too lazy to put them in files)
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

	// Data that we want to be able to access from anywhere
	UserData data = {
		&projectionMatrix,	// Projection matrix
		&camera,			// The camera object
		0.0,				// Duration of the last frame
		0.0, 0.0,			// Mouse position of the last frame
		false,				// Has the mouse moved before
		false				// Is the cursor enabled
	};
	glfwSetWindowUserPointer(window, &data);

	// Set up a timer to calculate frametimes
	std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

	// Set viewport and depth buffer
	glViewport(0, 0, 1200, 800);
	glEnable(GL_DEPTH_TEST);
	while (!glfwWindowShouldClose(window))
	{
		// Calculate frametime
		std::chrono::duration<float> framedelta = std::chrono::duration_cast<std::chrono::duration<float>>(std::chrono::system_clock::now() - start);
		data.frametime = framedelta.count();
		start = std::chrono::system_clock::now();

		// Handle events
		glfwPollEvents();
		ProcessInput(window);

		// Clear screen
		glClearColor(0.0f, 0.0f, 0.05f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Start new ImGui Frame 
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// Use our shader and set the matrices
		shader.Bind();
		shader.SetMatrix("model", &modelMatrix[0][0]);
		shader.SetMatrix("view", camera.GetViewMatrix());
		shader.SetMatrix("projection", &projectionMatrix[0][0]);

		// Finally draw the orbital
		orbital.Draw();

		ImGui::Begin("Test Window");
		ImGui::Button("Test");
		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Update swapchain
		glfwSwapBuffers(window);
	}

	// cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

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
	float sensitivity = 6.5f;

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
