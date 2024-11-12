#include "GameSystem.h"

//-------------------------------------
// main関数
//-------------------------------------

//windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ポインタ	
	GameSystem* pGameSystem = nullptr;

#pragma region 基盤システムの初期化

	pGameSystem = new GameSystem();
	pGameSystem->Initialize();

#pragma endregion 基盤システムの初期化

	while (true) {
		// 終了リクエストが来たらループを抜ける
		if (pGameSystem->IsEndRequest()) {
			// ゲームループを抜ける
			break;
		}

		//-------------------------------------
		// 毎フレーム更新処理
		//-------------------------------------
		
		pGameSystem->Update();

		//-------------------------------------
		// 描画
		//-------------------------------------

		pGameSystem->Draw();

	}

	//-------------------------------------
	// 終了・解放処理
	//-------------------------------------

	pGameSystem->Finalize();
	delete pGameSystem;

	return 0;
}