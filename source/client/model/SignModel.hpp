#pragma once

#include "Model.hpp"

class SignModel : public Model
{
public:
	SignModel();
	~SignModel();

	void render(float = 0, float = 0, float = 0, float = 0, float = 0, float = 0) override;

public:
	ModelPart m_cube;
	ModelPart m_cube2;
};
