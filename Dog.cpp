#include "Dog.h"

Dog::Dog()
{
	std::cout << "犬が出てきた！" << std::endl;
}

Dog::~Dog()
{
	std::cout << "犬はどこかへ行ってしまった…" << std::endl;
}

void Dog::MakeSound()
{
	std::cout << "ワン！" << std::endl;
}
