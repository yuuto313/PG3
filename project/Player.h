#pragma once
#include "MyVector2.h"


class Player
{
public:

	Player();

	void Initialize();

	void Update();

	void Draw();

	void MoveRight();

	void MoveLeft();

	Vector2 GetPosition()const { return position_; }

private:

	Vector2 position_ = {};

	float speed_ = 1.0f;
};

