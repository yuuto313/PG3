#include "GameSystem.h"

void GameSystem::Initialize()
{
#pragma region 基盤システムの初期化

	//-------------------------------------
	// リークチェッカー
	//-------------------------------------

	pLeakChecke_->~D3DResourceLeakChecker();

	//-------------------------------------
	// WinAppAPIの初期化
	//-------------------------------------

	pWinApp_ = new WinApp();
	pWinApp_->Initialize();
	
	//-------------------------------------
	// DirectXの初期化
	//-------------------------------------

	pDxCommon_ = new DirectXCommon();
	pDxCommon_->Initialize(pWinApp_);

	//-------------------------------------
	// SRVマネージャの初期化
	//-------------------------------------

	SrvManager::GetInstance()->Initialize(pDxCommon_);

	//-------------------------------------
	// キーボード入力の初期化
	//-------------------------------------

	pInput_ = new Input();
	pInput_->Initialize(pWinApp_);

	//-------------------------------------
	// Audioの初期化
	//-------------------------------------

	pAudio_ = new Audio();
	pAudio_->InitXAudio2();

	//-------------------------------------
	// カメラの初期化
	//-------------------------------------

	pCamera_ = new Camera();
	pCamera_->SetRotate({ 0.3f,0.0f,0.0f });
	pCamera_->SetTranslate({ 0.0f,4.0f,-10.0f });
	
	//-------------------------------------
	// ImGui（デバッグテキスト）の初期化
	//-------------------------------------

	pImguiManager_ = new ImGuiManager();
	pImguiManager_->Initialize(pDxCommon_, pWinApp_);

	//-------------------------------------
	// テクスチャマネージャの初期化
	//-------------------------------------

	TextureManager::GetInstance()->Initialize(pDxCommon_);
	TextureManager::GetInstance()->LoadTexture("resource/uvChecker.png");
	TextureManager::GetInstance()->LoadTexture("resource/monsterBall.png");
	TextureManager::GetInstance()->LoadTexture("resource/eto_tora_family.png");

	//-------------------------------------
	// 3dモデルマネージャの初期化
	//-------------------------------------

	ModelManager::GetInstance()->Initialize(pDxCommon_);
	// .objファイルからモデルを読み込む
	ModelManager::GetInstance()->LoadModel("plane.obj");
	ModelManager::GetInstance()->LoadModel("axis.obj");

	//-------------------------------------
	// スプライト共通部の初期化
	//-------------------------------------

	pSpriteCommon_ = new SpriteCommon();
	pSpriteCommon_->Initialize(pDxCommon_);

	//-------------------------------------
	// スプライトの初期化
	//-------------------------------------

	for (uint32_t i = 0; i < 3; ++i) {
		Sprite* pSprite = new Sprite();
		// スプライトごとに異なるテクスチャを割り当てる
		std::string texturePath;
		if (i == 0) {
			texturePath = "resource/uvChecker.png";
		} else if (i == 1) {
			texturePath = "resource/eto_tora_family.png";
		} else {
			texturePath = "resource/monsterBall.png";
		}

		pSprite->Initialize(pSpriteCommon_, texturePath);

		Vector2 position = pSprite->GetPosition();
		position = Vector2(i * 300.0f, i + 50.0f);
		pSprite->SetPosition(position);

		pSprites_.push_back(pSprite);
	}

	//-------------------------------------
	// 3dオブジェクト共通部の初期化
	//-------------------------------------

	pObject3dCommon_ = new Object3dCommon();
	pObject3dCommon_->Initialize(pDxCommon_);

	//-------------------------------------
	// 3dオブジェクトの初期化
	//-------------------------------------

	for (uint32_t i = 0; i < 2; ++i) {
		Object3d* pObject3d = new Object3d();

		pObject3d->Initialize(pObject3dCommon_);

		Vector3 translate = pObject3d->GetTranslate();
		translate = Vector3(i * 1.0f, i + 1.0f);
		pObject3d->SetTranslate(translate);
		pObject3d->SetModel("axis.obj");
		pObject3d->SetCamera(pCamera_);

		pObjects3d_.push_back(pObject3d);
	}

#pragma endregion 基盤システムの初期化
}

void GameSystem::Finalize()
{
}

void GameSystem::Update()
{
}

void GameSystem::Draw()
{
}
