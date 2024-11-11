#include "Rect.h"

Rect::Rect()
{
}

Rect::~Rect()
{
}

void Rect::Info()
{
	std::cout << "矩形の横幅 = " << width_ << std::endl;
	std::cout << "矩形の縦幅 = " << height_ << std::endl;
}

void Rect::Size()
{
	// 矩形の面積を求める
	area_ = width_ * height_;
}

void Rect::Draw()
{
	std::cout << "矩形の面積 = " << area_ << std::endl;
}
