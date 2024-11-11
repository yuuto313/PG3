#include "Circle.h"
#include <iostream>

#define PI 3.141592

void Circle::Size()
{
	area_ = PI * (radius_ * radius_);
}

void Circle::Draw()
{
	std::cout << "円の面積は%fです" << std::endl;
}
