#include "InputHandler.h"
#include "Input.h"

std::unique_ptr<ICommand> InputHandler::HandleInput()
{
    if (Input::GetInstance()->PushKey(DIK_A)) {
        return std::make_unique<MoveLeftCommand>();
    }
    if (Input::GetInstance()->PushKey(DIK_D)) {
        return std::make_unique<MoveRightCommand>();
    }
    return nullptr;
}