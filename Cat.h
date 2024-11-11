#pragma once
#include "IAnimal.h"

class Cat : public IAnimal
{
public:

	Cat();
	~Cat();

	void MakeSound() override;
};

