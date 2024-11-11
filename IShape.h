#pragma once
class IShape
{
public:

	IShape();
	~IShape();

	virtual void Size() = 0;

	virtual void Draw() = 0;

protected:

	float area_;

};

