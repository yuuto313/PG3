#include <stdio.h>
#include <iostream>
#include <Windows.h>
#include <time.h>
#include <functional>

#include "Dog.h"
#include "Cat.h"

int main(int argc,char* argv[]) {	

	std::vector<std::unique_ptr<IAnimal>> animals;

	animals.push_back(std::make_unique<Dog>());
	animals.push_back(std::make_unique<Cat>());

	for (auto& animal : animals) {
		animal->MakeSound();
	}

	return 0;
}