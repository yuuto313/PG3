#pragma once
#include <windows.h>
#include <cstdint>

/// <summary>
/// WindowsAPI
/// </summary>
class WinApp
{
public:
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// hwndゲッター
	/// </summary>
	/// <returns></returns>
	HWND GetHwnd()const { return hwnd; }

	HINSTANCE GetHInstance()const { return wc.hInstance; }

	//クライアント領域サイズ
	static const int32_t kClientWidth = 1280;
	static const int32_t kClientHeight = 720;

private:
	//ウィンドウハンドル
	HWND hwnd = nullptr;

	//ウィンドウクラスの設定
	WNDCLASS wc{};
};

