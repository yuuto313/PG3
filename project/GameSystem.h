#pragma once
#include "Input.h"
#include "WinApp.h"
#include "DirectXCommon.h"
#include "ImGuiManager.h"
#include "SpriteCommon.h"
#include "Sprite.h"
#include "TextureManager.h"
#include "Object3dCommon.h"
#include "Object3d.h"
#include "ModelManager.h"
#include "Camera.h"
#include "SrvManager.h"

#include "Audio.h"

#include "Logger.h"
#include "StringUtility.h"
#include "D3DResourceLeakChecker.h"
#include "MyMath.h"

/// <summary>
/// ゲームシーン
/// </summary>
class GameSystem
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

