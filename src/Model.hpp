#pragma once

#include <vector>
#include <glm/matrix.hpp>

class Model
{
public:
	Model();
	Model(const std::vector<float>& vertices, const std::vector<unsigned int>& indices);
	Model(std::vector<float>&& vertices, std::vector<unsigned int>&& indices);
	virtual ~Model();

	void Draw();

protected:
	void CreateVAO();
	void UpdateBufferData();
	virtual void DefineVAOLayout();

protected:
	std::vector<float> vertices;
	std::vector<unsigned int> indices;
	glm::mat4 modelMatrix;

private:
	unsigned int vbo, ebo, vao;
};