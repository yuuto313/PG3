#pragma once
#include "IShape.h"

class Rect : public IShape
{
public:

	Rect();
	~Rect();

	void Info() override;

	void Size() override;

	void Draw() override;

private:

	float width_ = 5.0f;
	float height_ = 3.0f;

};

