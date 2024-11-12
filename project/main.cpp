#include "Framework.h"
#include "GameSystem.h"

//-------------------------------------
// main関数
//-------------------------------------

//windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	Framework* pGame = new GameSystem();

	pGame->Run();

	delete pGame;

	return 0;
}