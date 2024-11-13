#include "GameScene.h"

void GameScene::Initialize()
{
	//-------------------------------------
	// オーディオファイルの読み込み
	//-------------------------------------



	//-------------------------------------
	// テクスチャファイルの読み込み
	//-------------------------------------

	TextureManager::GetInstance()->LoadTexture("resource/uvChecker.png");
	TextureManager::GetInstance()->LoadTexture("resource/monsterBall.png");
	TextureManager::GetInstance()->LoadTexture("resource/eto_tora_family.png");
	
	//-------------------------------------
	// 3dモデルの読み込み
	//-------------------------------------
	
	ModelManager::GetInstance()->LoadModel("plane.obj");
	ModelManager::GetInstance()->LoadModel("axis.obj");

	//-------------------------------------
	// カメラの初期化
	//-------------------------------------

	pCamera_ = new Camera();
	pCamera_->SetRotate({ 0.3f,0.0f,0.0f });
	pCamera_->SetTranslate({ 0.0f,4.0f,-10.0f });

	//-------------------------------------
	// スプライト生成
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

		pSprite->Initialize(SpriteCommon::GetInstance() , texturePath);

		Vector2 position = pSprite->GetPosition();
		position = Vector2(i * 300.0f, i + 50.0f);
		pSprite->SetPosition(position);

		pSprites_.push_back(pSprite);
	}

	//-------------------------------------
	// 3dオブジェクト生成
	//-------------------------------------

	for (uint32_t i = 0; i < 2; ++i) {
		Object3d* pObject3d = new Object3d();

		pObject3d->Initialize(Object3dCommon::GetInstance());

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

	//-------------------------------------
	// BGM再生開始
	//-------------------------------------



}

void GameScene::Finalize()
{
	//-------------------------------------
	// 解放処理
	//-------------------------------------

	for (uint32_t i = 0; i < pObjects3d_.size(); i++) {
		delete pObjects3d_[i];
	}

	for (uint32_t i = 0; i < pSprites_.size(); ++i) {
		delete pSprites_[i];
	}

	delete pCamera_;
}

void GameScene::Update()
{
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

	for (uint32_t i = 0; i < pObjects3d_.size(); i++) {
		pObjects3d_[i]->Update();
	}

	//-------------------------------------
	// ImGui（デバッグテキスト）の更新
	//-------------------------------------

	pCamera_->ImGui();
	for (uint32_t i = 0; i < pSprites_.size(); ++i) {
		pSprites_[i]->ImGui();
	}


}

void GameScene::Draw()
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

	for (uint32_t i = 0; i < pObjects3d_.size(); i++) {
		pObjects3d_[i]->Draw();
	}
}
