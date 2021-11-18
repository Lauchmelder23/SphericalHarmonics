#pragma once

#include "Model.hpp"

class Shader;
class Camera;

class Orbital : public Model
{
public:
	Orbital(unsigned int l, unsigned int m);

	void BindDefaultShader(Camera& camera);

private:
	void UpdateModel();
	void DefineVAOLayout() final override;

private:
	unsigned int l, m;
	static Shader* defaultShader;
};