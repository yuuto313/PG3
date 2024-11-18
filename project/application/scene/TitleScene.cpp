#include "TitleScene.h"
#include "SceneManager.h"

void TitleScene::Initialize()
{
	//-------------------------------------
	// テクスチャファイルの読み込み
	//-------------------------------------

	TextureManager::GetInstance()->LoadTexture("resource/uvChecker.png");

	//-------------------------------------
	// スプライト生成
	//-------------------------------------

	Sprite* pSprite = new Sprite();

	pSprite->Initialize(SpriteCommon::GetInstance(), "resource/uvChecker.png");

	Vector2 position = pSprite->GetPosition();
	position = Vector2(300.0f, 50.0f);
	pSprite->SetPosition(position);

	pSprites_.push_back(pSprite);

}

void TitleScene::Finalize()
{
	//-------------------------------------
	// 解放処理
	//-------------------------------------

	for (uint32_t i = 0; i < pSprites_.size(); ++i) {
		delete pSprites_[i];
	}

}

void TitleScene::Update()
{
	//-------------------------------------
	// シーン切り替え依頼
	//-------------------------------------

	// enterキーを押したら
	if (Input::GetInstance()->TriggerKey(DIK_RETURN)) {
		// ゲームシーン（次のシーン）を生成
		BaseScene* scene = new GameScene();
		// シーン切り替え依頼
		SceneManager::GetInstance()->SetNextScene(scene);
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
}

void TitleScene::Draw()
{
	//-------------------------------------
	// Sprite個々の描画
	//-------------------------------------

	for (uint32_t i = 0; i < pSprites_.size(); ++i) {
		pSprites_[i]->Draw();
	}
}
