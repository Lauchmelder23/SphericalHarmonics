#pragma once

#include "Model.hpp"

class Orbital : public Model
{
public:
	Orbital(unsigned int l, unsigned int m);

private:
	void UpdateModel();
	void DefineVAOLayout() final override;

private:
	unsigned int l, m;
};