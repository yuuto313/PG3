#include "Player.h"

Player::Player()
{
}

void Player::Initialize()
{
	position_ = { 100.0f,50.0f };
}

void Player::Update()
{
}

void Player::Draw()
{
}

void Player::MoveRight()
{
	this->position_.x += this->speed_;
}

void Player::MoveLeft()
{
	this->position_.x -= this->speed_;
}
