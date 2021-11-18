#pragma once

#include "Model.hpp"

class Shader;
class Camera;

class Orbital : public Model
{
public:
	Orbital(int l, int m);

	void BindDefaultShader(Camera& camera);
	float* GetPositiveColorVPtr();
	float* GetNegativeColorVPtr();
	void UpdateModel();

private:
	void DefineVAOLayout() final override;

public:
	glm::vec3 positiveColor, negativeColor;
	int l, m;

private:
	
	static Shader* defaultShader;
};