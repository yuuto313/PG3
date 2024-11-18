#include "GameScene.h"
#include "SceneManager.h"

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

	camera_ = std::make_unique<Camera>();
	camera_->SetRotate({ 0.3f,0.0f,0.0f });
	camera_->SetTranslate({ 0.0f,4.0f,-10.0f });

	//-------------------------------------
	// スプライト生成
	//-------------------------------------

	sprites_.emplace_back(std::make_unique<Sprite>());
	sprites_.emplace_back(std::make_unique<Sprite>());
	sprites_.emplace_back(std::make_unique<Sprite>());

	int i = 0;

	for (const auto& sprite : sprites_) {
		// スプライトごとに異なるテクスチャを割り当てる
		std::string texturePath;
		if (i == 0) {
			texturePath = "resource/uvChecker.png";
		} else if (i == 1) {
			texturePath = "resource/eto_tora_family.png";
		} else {
			texturePath = "resource/monsterBall.png";
		}

		sprite->Initialize(SpriteCommon::GetInstance(), texturePath);

		Vector2 position = sprite->GetPosition();
		position = Vector2(i * 300.0f, i + 50.0f);
		sprite->SetPosition(position);

		sprites_.push_back(sprite);

		i++;
	
	}

	//-------------------------------------
	// 3dオブジェクト生成
	//-------------------------------------

	objects3d_.emplace_back(std::make_unique<Object3d>());
	objects3d_.emplace_back(std::make_unique<Object3d>());

	for (const auto& object3d : objects3d_) {

		object3d->Initialize(Object3dCommon::GetInstance());

		Vector3 translate = object3d->GetTranslate();
		translate = Vector3(i * 1.0f, i + 1.0f);
		object3d->SetTranslate(translate);

		// 異なるモデルを割り当てる
		std::string filePath;
		if (i == 0) {
			filePath = "axis.obj";
		} else if (i == 1) {
			filePath = "plane.obj";
		}

		object3d->SetModel(filePath);

		object3d->SetCamera(camera_.get());

		objects3d_.push_back(object3d);

		i++;
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

	
}

void GameScene::Update()
{
	//-------------------------------------
	// シーン切り替え依頼
	//-------------------------------------

	// enterキーを押したら
	if (Input::GetInstance()->TriggerKey(DIK_RETURN)) {
		// シーン切り替え
		SceneManager::GetInstance()->ChangeScene("TITLE");
	}

	//-------------------------------------
	// スプライトの更新
	//-------------------------------------

	for (const auto& sprite : sprites_) {
		sprite->Update();

		sprite->SetSize({ 600.0f,300.0f });

		float rotation = sprite->GetRotation();
		rotation += 0.01f;
		sprite->SetRotation(rotation);

		// ImGuiを表示
		sprite->ImGui();
	}

	//-------------------------------------
	// カメラの更新
	//-------------------------------------

	// 3dオブジェクトの更新より前に行う
	camera_->Update();

	//-------------------------------------
	// 3dオブジェクトの更新
	//-------------------------------------

	for (const auto& object3d : objects3d_) {
		object3d->Update();
	}

}

void GameScene::Draw()
{
	//-------------------------------------
	// Sprite個々の描画
	//-------------------------------------

	for (const auto& sprite : sprites_) {
		sprite->Draw();
	}

	//-------------------------------------
	// 3dオブジェクト個々の描画
	//-------------------------------------

	for (const auto& object3d : objects3d_) {
		object3d->Draw();
	}
}
