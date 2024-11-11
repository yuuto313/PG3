#include "Animal.h"

Animal::Animal()
{
	std::cout << "生成" << std::endl;
}

Animal::~Animal()
{
	std::cout << "破棄" << std::endl;
}

void Animal::MakeSound()
{
	std::cout << "動物の鳴き声" << std::endl;
}
