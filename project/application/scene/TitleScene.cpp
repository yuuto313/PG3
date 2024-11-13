#include "TitleScene.h"

void TitleScene::Initialize()
{
	//-------------------------------------
	// テクスチャファイルの読み込み
	//-------------------------------------

	TextureManager::GetInstance()->LoadTexture("resource/uvChecker.png");

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

		pSprite->Initialize(SpriteCommon::GetInstance(), texturePath);

		Vector2 position = pSprite->GetPosition();
		position = Vector2(i * 300.0f, i + 50.0f);
		pSprite->SetPosition(position);

		pSprites_.push_back(pSprite);
	}

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
