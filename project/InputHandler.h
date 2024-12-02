#pragma once
#include <Command.h>
#include <memory>

/// <summary>
/// 入力を取りまとめるクラス
/// </summary>
class InputHandler
{
public:
	std::unique_ptr<ICommand> HandleInput();

private:

};

