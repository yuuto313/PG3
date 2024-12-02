#pragma once
#include "BaseScene.h"
#include "SpriteCommon.h"
#include "Object3dCommon.h"
#include "TextureManager.h"
#include "ModelManager.h"
#include "Sprite.h"
#include "Object3d.h"
#include "Camera.h"
#include "Input.h"

#include "Player.h"
#include "InputHandler.h"
#include <Command.h>

/// <summary>
/// ゲームプレイシーン
/// </summary>
class GameScene : public BaseScene
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override;

	/// <summary>
	/// 終了
	/// </summary>
	void Finalize() override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

	/// <summary>
	/// ImGui
	/// </summary>
	void ImGui() override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw() override;

private:
	std::unique_ptr<Sprite> sprite_;

	std::unique_ptr<Player> player_;
	std::unique_ptr<ICommand> iCommand_;
	std::unique_ptr<InputHandler> inputHandler_;
};

