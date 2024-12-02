#include "Command.h"
#include "Player.h"

void MoveRightCommand::Exec(Player& player)
{
	player.MoveRight();
}

void MoveLeftCommand::Exec(Player& player)
{
	player.MoveLeft();
}

ICommand::~ICommand()
{
}
