#pragma once

#include "Framework.h"
#include "DirectXCommon.h"
#include "Camera.h"
#include "SpriteCommon.h"
#include "Sprite.h"
#include "Object3dCommon.h"
#include "Object3d.h"
#include "SrvManager.h"
#include "ModelManager.h"
#include "TextureManager.h"

/// <summary>
/// ゲーム全体
/// </summary>
class GameSystem : public Framework
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
	/// 描画前処理
	/// </summary>
	void PreDraw() override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw() override;

	/// <summary>
	/// 描画後処理
	/// </summary>
	void PostDraw() override;

private:
	//ポインタ
	DirectXCommon* pDxCommon_ = nullptr;
	WinApp* pWinApp_ = nullptr;

	Camera* pCamera_ = nullptr;

	SpriteCommon* pSpriteCommon_ = nullptr;
	std::vector<Sprite*> pSprites_;

	Object3dCommon* pObject3dCommon_ = nullptr;
	std::vector<Object3d*> pObjects3d_;

};

