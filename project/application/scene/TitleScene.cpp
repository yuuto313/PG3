#include "TitleScene.h"
#include "SceneManager.h"
#include "ImGuiManager.h"

void TitleScene::Initialize()
{
	//-------------------------------------
	// オーディオファイルの読み込み
	//-------------------------------------

	soundData_ = Audio::GetInstance()->SoundLoadWave("Resources/sound/Alarm01.wav");

	//-------------------------------------
	// BGM再生開始
	//-------------------------------------

	Audio::GetInstance()->SoundPlayWave(soundData_);

	//-------------------------------------
	// テクスチャファイルの読み込み
	//-------------------------------------

	TextureManager::GetInstance()->LoadTexture("resource/uvChecker.png");

	//-------------------------------------
	// スプライト生成
	//-------------------------------------

	sprite_ = std::make_unique<Sprite>();
	sprite_->Initialize(SpriteCommon::GetInstance(), "resource/uvChecker.png");

	position_ = sprite_->GetPosition();
	position_ = Vector2(100.0f, 100.0f);
	sprite_->SetPosition(position_);

}

void TitleScene::Finalize()
{
	if(sprite_) {
		sprite_.reset();
	}

	Audio::GetInstance()->SoundUnload(&soundData_);

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
	sprite_->SetPosition(position_);
}

void TitleScene::ImGui()
{
	ImGui::Begin("Info");
	ImGui::Text("ENTER : GameScene\n");
	ImGui::End();

	ImGui::SetNextWindowSize(ImVec2(500.0f, 100.0f), ImGuiCond_Always);
	ImGui::Begin("09_03_Assignment");
	ImGui::SliderFloat2("position", &position_.x, 0.0f, 500.0f);
	ImGui::End();
}

void TitleScene::Draw()
{
	//-------------------------------------
	// Sprite個々の描画
	//-------------------------------------

	sprite_->Draw();
}
