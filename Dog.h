#pragma once
#include "IAnimal.h"

class Dog : public IAnimal
{
public:

	Dog();
	~Dog();

	void MakeSound() override;
};

