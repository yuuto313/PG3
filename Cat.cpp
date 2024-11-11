#include "Cat.h"

Cat::Cat()
{
	std::cout << "猫が出てきた！" << std::endl;
}

Cat::~Cat()
{
	std::cout << "猫はどこかへ行ってしまった…" << std::endl;
}

void Cat::MakeSound()
{
	std::cout << "ニャー！" << std::endl;
}
