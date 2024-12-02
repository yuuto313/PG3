#include "GameScene.h"
#include "SceneManager.h"

void GameScene::Initialize()
{
	
	//-------------------------------------
	// テクスチャファイルの読み込み
	//-------------------------------------

	TextureManager::GetInstance()->LoadTexture("resource/uvChecker.png");
	
	//-------------------------------------
	// スプライト生成
	//-------------------------------------
	
	sprite_ = std::make_unique<Sprite>();
	sprite_->Initialize(SpriteCommon::GetInstance(), "resource/uvChecker.png");

	//-------------------------------------
	// キーにコマンドを割り当てる
	//-------------------------------------

	inputHandler_ = std::make_unique<InputHandler>();

	//-------------------------------------
	// プレイヤーを生成
	//-------------------------------------

	player_ = std::make_unique<Player>();

}

void GameScene::Finalize()
{
	//-------------------------------------
	// 解放処理
	//-------------------------------------

	sprite_.reset();

}

void GameScene::Update()
{

	//-------------------------------------
	// キーを取得してそれに応じたコマンドを実行する
	//-------------------------------------

	// ger Input
	iCommand_ = inputHandler_->HandleInput();

	// set Command
	if (this->iCommand_) {
		iCommand_->Exec(*player_);
	}

	player_->Update();
	Vector2 position = player_->GetPosition();
	sprite_->SetPosition(position);

	//-------------------------------------
	// スプライトの更新
	//-------------------------------------

	sprite_->Update();

}

void GameScene::ImGui()
{
	
}

void GameScene::Draw()
{
	//-------------------------------------
	// Sprite個々の描画
	//-------------------------------------

	sprite_->Draw();

}
