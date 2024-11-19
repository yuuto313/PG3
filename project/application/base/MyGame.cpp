#include "MyGame.h"
#include "SceneFactory.h"

void MyGame::Initialize()
{
#pragma region 基盤システムの初期化
	
	//-------------------------------------
	// 基底クラスの初期化
	//-------------------------------------

	OYFramework::Initialize();

	//-------------------------------------
	// キーボード入力の初期化
	//-------------------------------------

	Input::GetInstance()->Initialize(WinApp::GetInstance());

	//-------------------------------------
	// Audioの初期化
	//-------------------------------------

	Audio::GetInstance()->InitXAudio2();

	
#pragma endregion 基盤システムの初期化

#pragma region シーン

	//-------------------------------------
	// シーンの生成
	//-------------------------------------

	// シーンファクトリーを生成し、マネージャにセット
	sceneFactory_ = std::make_unique<SceneFactory>();
	SceneManager::GetInstance()->SetSceneFactory(sceneFactory_.get());
	// シーンマネージャに最初のシーンをセット
	SceneManager::GetInstance()->ChangeScene("TITLE");


#pragma endregion シーン

}

void MyGame::Finalize()
{
	//-------------------------------------
	// COMの終了処理
	//-------------------------------------

	CoUninitialize();

	//-------------------------------------
	// Audioクラスの後始末
	//-------------------------------------

	Audio::GetInstance()->ResetXAudio2();
	Audio::GetInstance()->Finalize();

	//-------------------------------------
	// Inputクラスの後始末
	//-------------------------------------

	Input::GetInstance()->Finalize();

	//-------------------------------------
	// 基底クラスの終了処理
	//-------------------------------------

	OYFramework::Finalize();

}

void MyGame::Update()
{	
	//-------------------------------------
	// 基底クラスの更新処理
	//-------------------------------------

	OYFramework::Update();

	//-------------------------------------
	// キーボード入力の更新
	//-------------------------------------

	Input::GetInstance()->Update();

}

void MyGame::PreDraw()
{	
	
	//-------------------------------------
	// 基底クラスの描画前処理
	//-------------------------------------

	OYFramework::PreDraw();

}

void MyGame::Draw()
{
	//-------------------------------------
	// シーンマネージャの描画処理
	//-------------------------------------

	SceneManager::GetInstance()->Draw();

}

void MyGame::PostDraw()
{
	//-------------------------------------
	// 基底クラスの描画後処理
	//-------------------------------------

	OYFramework::PostDraw();

}
