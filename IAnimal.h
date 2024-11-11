#pragma once
#include <memory>
#include <iostream>
#include <vector>

class IAnimal
{
public:
	IAnimal();
	virtual ~IAnimal();

	/// <summary>
	/// 純粋仮想関数
	/// </summary>
	virtual void MakeSound() = 0;

};

