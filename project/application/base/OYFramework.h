#pragma once
#include "D3DResourceLeakChecker.h"
#include "DirectXCommon.h"
#include "SrvManager.h"
#include "WinApp.h"
#include "ImGuiManager.h"
#include "TextureManager.h"
#include "ModelManager.h"
#include "SpriteCommon.h"
#include "Object3dCommon.h"

#include "SceneManager.h"
#include "SceneFactory.h"

/// <summary>
/// ゲーム全体の骨組み
/// </summary>
class OYFramework
{
public:
	virtual ~OYFramework() = default;

	virtual void Initialize();

	virtual void Finalize();

	virtual void Update();

	virtual void PreDraw();

	virtual void Draw() = 0;

	virtual void PostDraw();

	virtual bool IsEndRequest() { return endRequest_; }

public:

	/// <summary>
	/// 実行
	/// </summary>
	void Run();

private:

	D3DResourceLeakChecker* pLeakChecke_ = nullptr;
	WinApp* pWinApp_ = nullptr;
	DirectXCommon* pDxCommon_ = nullptr;
	ImGuiManager* pImguiManager_ = nullptr;



protected:	
	// シーンファクトリー
	AbstractSceneFactory* sceneFactory_ = nullptr;

	bool endRequest_ = false;

};

