#pragma once
#include "SpriteCommon.h"
#include "Object3dCommon.h"
#include "TextureManager.h"
#include "ModelManager.h"
#include "Sprite.h"
#include "Object3d.h"
#include "Camera.h"

/// <summary>
/// ゲームプレイシーン
/// </summary>
class GameScene
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 終了
	/// </summary>
	void Finalize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

private:
	Camera* pCamera_ = nullptr;
	std::vector<Sprite*> pSprites_;
	std::vector<Object3d*> pObjects3d_;
};

