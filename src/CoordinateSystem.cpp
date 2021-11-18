#include "CoordinateSystem.hpp"

#include <glm/gtc/type_ptr.hpp>

#include "Shader.hpp"
#include "Axis.hpp"
#include "Camera.hpp"

Shader* CoordinateSystem::defaultShader = nullptr;

CoordinateSystem::CoordinateSystem() :
	modelMatrix(1.0f)
{
	if (defaultShader == nullptr)
	{
		defaultShader = new Shader(
			R"(
			#version 460 core

			layout(location = 0) in vec3 position;

			out vec3 outColor;

			uniform vec3 axisColor;

			uniform mat4 axisModel;
			uniform mat4 model;
			uniform mat4 view;
			uniform mat4 projection;

			void main()
			{
				outColor = axisColor;
				gl_Position = projection * view * model * axisModel * vec4(position, 1.0f);
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
	}

	axes[0] = new Axis(glm::vec3(1.0f, 0.0f, 0.0f), 4.0f);
	axes[0]->color = glm::vec3(1.0f, 0.0f, 0.0f);

	axes[1] = new Axis(glm::vec3(0.0f, 1.0f, 0.0f), 4.0f);
	axes[1]->color = glm::vec3(0.0f, 1.0f, 0.0f);

	axes[2] = new Axis(glm::vec3(0.0f, 0.0f, 1.0f), 4.0f);
	axes[2]->color = glm::vec3(0.0f, 0.0f, 1.0f);

}

CoordinateSystem::~CoordinateSystem()
{
	delete axes[2];
	delete axes[1];
	delete axes[0];
}

void CoordinateSystem::BindDefaultShader(Camera& camera)
{
	defaultShader->Bind();

	defaultShader->SetMatrix("model", glm::value_ptr(modelMatrix));
	defaultShader->SetMatrix("view", glm::value_ptr(camera.GetViewMatrix()));
	defaultShader->SetMatrix("projection", glm::value_ptr(camera.GetProjectionMatrix()));
}

void CoordinateSystem::Draw()
{
	for (Axis* axis : axes)
	{
		defaultShader->SetMatrix("axisModel", glm::value_ptr(axis->GetModelMatrix()));
		defaultShader->SetVector3("axisColor", glm::value_ptr(axis->color));
		axis->Draw();
	}
}
