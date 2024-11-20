#include "SceneManager.h"
#include <cassert>

SceneManager* SceneManager::instance = nullptr;

SceneManager* SceneManager::GetInstance()
{
	if (instance == nullptr) {
		instance = new SceneManager;
	}

	return instance;
}

void SceneManager::Finalize()
{
	// 最後のシーンの終了と解放
	scene_->Finalize();

	delete instance;
	instance = nullptr;
}

void SceneManager::Update()
{
	//-------------------------------------
	// シーン切り替え機構
	//-------------------------------------
	
	// 次のシーンの予約があるなら
	if (nextScene_) {
		// 旧シーンの終了
		if (scene_) {
			scene_->Finalize();
		}

		// シーン切り替え
		// nextScene_にははnullptrが入る
		scene_ = std::move(nextScene_);

		// シーンマネージャをセット
		scene_->SetSceneManager(this);

		// 次のシーンを初期化する
		scene_->Initialize();
	}

	//-------------------------------------
	// 実行中シーンを更新する
	//-------------------------------------

	scene_->Update();
	scene_->ImGui();

}

void SceneManager::Draw()
{
	//-------------------------------------
	// 実行中シーンを描画する
	//-------------------------------------

	scene_->Draw();

}

void SceneManager::ChangeScene(const std::string& sceneName)
{
	assert(sceneFactory_);
	assert(nextScene_ == nullptr);

	// 次のシーンを生成
	nextScene_ = sceneFactory_->CreateScene(sceneName);
}
