#include "OYFramework.h"
#include "MyGame.h"

//-------------------------------------
// main関数
//-------------------------------------

//windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	OYFramework* pGame = new MyGame();

	pGame->Run();

	delete pGame;

	return 0;
}