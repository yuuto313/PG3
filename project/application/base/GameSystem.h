#pragma once

#include "Framework.h"
#include "D3DResourceLeakChecker.h"
#include "WinApp.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "Audio.h"
#include "Camera.h"
#include "ImGuiManager.h"
#include "SpriteCommon.h"
#include "Sprite.h"
#include "Object3dCommon.h"
#include "Object3d.h"
#include "SrvManager.h"
#include "ModelManager.h"
#include "TextureManager.h"

/// <summary>
/// ゲームシーン
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
	/// 描画
	/// </summary>
	void Draw() override;

private:
	//ポインタ
	D3DResourceLeakChecker* pLeakChecke_ = nullptr;
	WinApp* pWinApp_ = nullptr;

	DirectXCommon* pDxCommon_ = nullptr;

	Input* pInput_ = nullptr;
	Audio* pAudio_ = nullptr;
	Camera* pCamera_ = nullptr;

	ImGuiManager* pImguiManager_ = nullptr;

	SpriteCommon* pSpriteCommon_ = nullptr;
	std::vector<Sprite*> pSprites_;

	Object3dCommon* pObject3dCommon_ = nullptr;
	std::vector<Object3d*> pObjects3d_;

};

