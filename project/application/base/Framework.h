#pragma once
#include "D3DResourceLeakChecker.h"
#include "DirectXCommon.h"
#include "SrvManager.h"
#include "WinApp.h"
#include "Input.h"
#include "Audio.h"
#include "ImGuiManager.h"
#include "SpriteCommon.h"
#include "TextureManager.h"
#include "ModelManager.h"

/// <summary>
/// ゲーム全体の骨組み
/// </summary>
class Framework
{
public:
	virtual ~Framework() = default;

	virtual void Initialize();

	virtual void Finalize();

	virtual void Update();

	virtual void PreDraw() = 0;

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
	Input* pInput_ = nullptr;
	Audio* pAudio_ = nullptr;
	ImGuiManager* pImguiManager_ = nullptr;
	SpriteCommon* pSpCpmmon_ = nullptr;

protected:

	bool endRequest_ = false;

};

