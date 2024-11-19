#include "TitleScene.h"
#include "SceneManager.h"
#include "Audio.h"

void TitleScene::Initialize()
{
	//-------------------------------------
	// オーディオファイルの読み込み
	//-------------------------------------

	Audio::GetInstance()->SoundLoadWave("Resources/sound/Alarm01.wav");

	//-------------------------------------
	// BGM再生開始
	//-------------------------------------

	Audio::GetInstance()->SoundPlayWave()

	//-------------------------------------
	// テクスチャファイルの読み込み
	//-------------------------------------

	TextureManager::GetInstance()->LoadTexture("resource/uvChecker.png");

	//-------------------------------------
	// スプライト生成
	//-------------------------------------

	sprite_ = std::make_unique<Sprite>();
	sprite_->Initialize(SpriteCommon::GetInstance(), "resource/uvChecker.png");



}

void TitleScene::Finalize()
{
	if(sprite_) {
		sprite_.reset();
	}
}

void TitleScene::Update()
{
	//-------------------------------------
	// シーン切り替え依頼
	//-------------------------------------

	// enterキーを押したら
	if (Input::GetInstance()->TriggerKey(DIK_RETURN)) {
		// シーン切り替え
		SceneManager::GetInstance()->ChangeScene("GAME");
	}

	//-------------------------------------
	// スプライトの更新
	//-------------------------------------

	sprite_->Update();


}

void TitleScene::Draw()
{
	//-------------------------------------
	// Sprite個々の描画
	//-------------------------------------

	sprite_->Draw();
}
