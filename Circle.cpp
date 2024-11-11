#include "Circle.h"
#include <iostream>

#define PI 3.141592

Circle::Circle()
{
}

Circle::~Circle()
{
}

void Circle::Info()
{
	std::cout << "円の半径 = " << radius_ << std::endl;
}

void Circle::Size()
{
	// 円の面積を求める
	area_ = static_cast<float>(PI) * (radius_ * radius_);
}

void Circle::Draw()
{
	std::cout << "円の面積 = "<< area_ << std::endl;
}
