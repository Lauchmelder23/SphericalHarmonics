#include <iostream>
#include <chrono>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include "Orbital.hpp"
#include "CoordinateSystem.hpp"
#include "Shader.hpp"
#include "Camera.hpp"

struct UserData
{
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

void DrawOrbitalSettings(Orbital& orbital);
void DrawGeneralSettings(Camera& camera);
void DrawMathematicalSettings(CoordinateSystem& cs);

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

	CoordinateSystem csystem;

	// Create some orbital and set up its transformation matrix
	// TODO: the matrix should probably be part of Model
	Orbital orbital(2, 1);

	// Set up a camera 
	// TODO: should the projection matrix be part of the camera?
	Camera camera(110.0f, 1200.0f / 800.0f);
	camera.SetPosition(glm::vec3(0.0f, 0.0f, 4.0f));

	glm::vec3 clearColor(0.0f, 0.0f, 0.05f);

	// Data that we want to be able to access from anywhere
	UserData data = {
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
		glClearColor(clearColor.x, clearColor.y, clearColor.z, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Start new ImGui Frame 
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		orbital.BindDefaultShader(camera);
		orbital.Draw();

		csystem.BindDefaultShader(camera);
		csystem.Draw();

		ImGui::Begin("Settings");

		DrawOrbitalSettings(orbital);
		DrawGeneralSettings(camera);
		DrawMathematicalSettings(csystem);

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
	data->camera->UpdatePerspective(110.0f, (float)width / (float)height);
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

void DrawOrbitalSettings(Orbital& orbital)
{
	if (ImGui::CollapsingHeader("Orbital Settings"))
	{

		if (ImGui::TreeNode("Properties"))
		{
			ImGui::SliderInt("l", &orbital.l, 0, 8);
			if (orbital.m > orbital.l)
				orbital.m = orbital.l;
			else if (orbital.m < -orbital.l)
				orbital.m = -orbital.l;

			ImGui::SliderInt("m", &orbital.m, -orbital.l, orbital.l);

			ImGui::SliderInt("Resolution", (int*)&orbital.resolution, 10, 1000);

			if (ImGui::Button("Generate"))
			{
				orbital.UpdateModel();
			}

			ImGui::TreePop();
			ImGui::Separator();
		}

		if (ImGui::TreeNode("Appearance"))
		{
			ImGui::ColorEdit3("Positive Value Color", orbital.GetPositiveColorVPtr());
			ImGui::ColorEdit3("Negative Value Color", orbital.GetNegativeColorVPtr());

			ImGui::TreePop();
			ImGui::Separator();
		}
	}
}

void DrawGeneralSettings(Camera& camera)
{
	if(ImGui::CollapsingHeader("Camera Settings"))
	{

	}
}

void DrawMathematicalSettings(CoordinateSystem& cs)
{
	if (ImGui::CollapsingHeader("Coordinate System Settings"))
	{
		if (ImGui::TreeNode("Axes"))
		{
			ImGui::ColorEdit3("x-Axis color", cs.GetAxis(0)->GetColorVPtr());
			ImGui::ColorEdit3("y-Axis color", cs.GetAxis(1)->GetColorVPtr());
			ImGui::ColorEdit3("z-Axis color", cs.GetAxis(2)->GetColorVPtr());

			ImGui::TreePop();
			ImGui::Separator();
		}
	}
}
