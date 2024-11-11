#pragma once
#include "IShape.h"

class Circle : public IShape
{
public:

	Circle();
	~Circle();

	void Size() override;

	void Draw() override;

private:

	float radius_;


};

