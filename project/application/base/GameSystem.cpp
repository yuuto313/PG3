#include "GameSystem.h"

void GameSystem::Initialize()
{
#pragma region 基盤システムの初期化

	//-------------------------------------
	// WinAppAPIの初期化
	//-------------------------------------

	pWinApp_ = WinApp::GetInstance();
	pWinApp_->Initialize();

	//-------------------------------------
	// DirectXの初期化
	//-------------------------------------

	pDxCommon_ = DirectXCommon::GetInstance();
	pDxCommon_->Initialize(pWinApp_);
	
	//-------------------------------------
	// 基底クラスの初期化
	//-------------------------------------

	Framework::Initialize();

	//-------------------------------------
	// カメラの初期化
	//-------------------------------------

	pCamera_ = new Camera();
	pCamera_->SetRotate({ 0.3f,0.0f,0.0f });
	pCamera_->SetTranslate({ 0.0f,4.0f,-10.0f });

	//-------------------------------------
	// テクスチャマネージャの初期化
	//-------------------------------------

	TextureManager::GetInstance()->LoadTexture("resource/uvChecker.png");
	TextureManager::GetInstance()->LoadTexture("resource/monsterBall.png");
	TextureManager::GetInstance()->LoadTexture("resource/eto_tora_family.png");

	//-------------------------------------
	// 3dモデルマネージャの初期化
	//-------------------------------------

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

		// 異なるモデルを割り当てる
		std::string filePath;
		if (i == 0) {
			filePath = "axis.obj";
		} else if (i == 1) {
			filePath = "plane.obj";
		}
		
		pObject3d->SetModel(filePath);

		pObject3d->SetCamera(pCamera_);

		pObjects3d_.push_back(pObject3d);
	}

#pragma endregion 基盤システムの初期化
}

void GameSystem::Finalize()
{
	//-------------------------------------
	// COMの終了処理
	//-------------------------------------

	CoUninitialize();

	//-------------------------------------
	// WindowsAPIの終了処理
	//-------------------------------------

	pWinApp_->Finalize();

	//-------------------------------------
	// DirectXCommonの終了処理
	//-------------------------------------

	pDxCommon_->Finalize();

	//-------------------------------------
	// 解放処理
	//-------------------------------------


	for (uint32_t i = 0; i < 2; i++) {
		delete pObjects3d_[i];
	}
	delete pObject3dCommon_;
	for (uint32_t i = 0; i < pSprites_.size(); ++i) {
		delete pSprites_[i];
	}
	delete pSpriteCommon_;
	delete pCamera_;

	//-------------------------------------
	// 基底クラスの終了処理
	//-------------------------------------

	Framework::Finalize();

}

void GameSystem::Update()
{	
	//-------------------------------------
	// 基底クラスの更新処理
	//-------------------------------------

	Framework::Update();

	//-------------------------------------
	// 終了リクエストを確認
	//-------------------------------------

	if (pWinApp_->ProcessMessage()) {
		endRequest_ = true;
	}

	//-------------------------------------
	// スプライトの更新
	//-------------------------------------

	for (uint32_t i = 0; i < pSprites_.size(); ++i) {
		pSprites_[i]->Update();

		pSprites_[i]->SetSize({ 600.0f,300.0f });

		float rotation = pSprites_[i]->GetRotation();
		rotation += 0.01f;
		pSprites_[i]->SetRotation(rotation);

	}

	//-------------------------------------
	// カメラの更新
	//-------------------------------------

	// 3dオブジェクトの更新より前に行う
	pCamera_->Update();
	
	//-------------------------------------
	// 3dオブジェクトの更新
	//-------------------------------------

	for (uint32_t i = 0; i < 2; i++) {
		pObjects3d_[i]->Update();
	}

	//-------------------------------------
	// ImGui（デバッグテキスト）の更新
	//-------------------------------------
	
	pCamera_->ImGui();
	
	pSpriteCommon_->ImGui();

	for (uint32_t i = 0; i < pSprites_.size(); ++i) {
		pSprites_[i]->ImGui();
	}
}

void GameSystem::PreDraw()
{	
	//-------------------------------------
	// 描画前処理
	//-------------------------------------

	// DirectXの描画準備。すべての描画に共通のグラフィックスコマンドを積む
	pDxCommon_->PreDraw();

	// DescriptorHeapを設定
	SrvManager::GetInstance()->PreDraw();
	
	//-------------------------------------
	// スプライト描画準備
	//-------------------------------------

	// Spriteの描画準備。Spriteの描画に共通のグラフィックスコマンドを積む
	pSpriteCommon_->SetDraw();
	
	//-------------------------------------
	// 3dオブジェクト描画準備
	//-------------------------------------

	// 3dオブジェクトの描画準備。3dオブジェクトの描画に共通のグラフィックスコマンドを積む
	pObject3dCommon_->SetDraw();

}

void GameSystem::Draw()
{
	//-------------------------------------
	// Sprite個々の描画
	//-------------------------------------

	for (uint32_t i = 0; i < pSprites_.size(); ++i) {
		pSprites_[i]->Draw();
	}

	//-------------------------------------
	// 3dオブジェクト個々の描画
	//-------------------------------------

	for (uint32_t i = 0; i < 2; i++) {
		pObjects3d_[i]->Draw();
	}

}

void GameSystem::PostDraw()
{
	//-------------------------------------
	// 基底クラスの描画後処理
	//-------------------------------------

	Framework::PostDraw();

	//-------------------------------------
	// 描画後処理
	//-------------------------------------

	pDxCommon_->PostDraw();
}
