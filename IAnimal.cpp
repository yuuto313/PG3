#include "IAnimal.h"

IAnimal::IAnimal()
{
	std::cout << "生成" << std::endl;
}

IAnimal::~IAnimal()
{
	std::cout << "破棄" << std::endl;
}

void IAnimal::MakeSound()
{
	std::cout << "動物の鳴き声" << std::endl;
}
