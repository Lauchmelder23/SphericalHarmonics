#include "Model.hpp"

#include <glad/glad.h>

Model::Model() :
	vertices({}), indices({}), vao(0), ebo(0), vbo(0), modelMatrix({1.0f})
{
}

Model::Model(const std::vector<float>& vertices, const std::vector<unsigned int>& indices) :
	vertices(vertices), indices(indices), vao(0), vbo(0), ebo(0)
{
	CreateVAO();
}

Model::Model(std::vector<float>&& vertices, std::vector<unsigned int>&& indices) :
	vertices(vertices), indices(indices), vao(0), vbo(0), ebo(0)
{
	CreateVAO();
}

Model::~Model()
{
	glDeleteBuffers(1, &ebo);
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}

void Model::Draw()
{
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Model::CreateVAO()
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	UpdateBufferData();
	DefineVAOLayout();

	glBindVertexArray(0);
}

void Model::UpdateBufferData()
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
}

void Model::DefineVAOLayout()
{
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
}
