#pragma once

#include "OYFramework.h"
#include "DirectXCommon.h"
#include "SpriteCommon.h"
#include "Object3dCommon.h"

#include "BaseScene.h"
#include "TitleScene.h"
#include "GameScene.h"

/// <summary>
/// ゲーム全体
/// </summary>
class MyGame : public OYFramework
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
	// ポインタ
	SpriteCommon* pSpriteCommon_ = nullptr;
	Object3dCommon* pObject3dCommon_ = nullptr;

	// シーン
	BaseScene* pGameScene_ = nullptr;
	BaseScene* pTitleScene_ = nullptr;

};

