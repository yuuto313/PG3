#pragma once
#include <windows.h>
#include <cstdint>

/// <summary>
/// WindowsAPI
/// </summary>
class WinApp
{
private:// シングルトン設計
	static WinApp* instance;

	/// <summary>
	/// コンストラクタ、デストラクタの隠蔽
	/// </summary>
	WinApp() = default;
	~WinApp() = default;

	/// <summary>
	/// コピーコンストラクタの封印
	/// </summary>
	/// <param name=""></param>
	WinApp(WinApp&) = delete;

	/// <summary>
	/// コピー代入演算の封印
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	WinApp& operator=(WinApp&) = delete;

public:

	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	/// <summary>
	///	シングルトンインスタンスの取得
	/// </summary>
	/// <returns></returns>
	static WinApp* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// メッセージの処理
	/// </summary>
	/// <returns></returns>
	bool ProcessMessage();

	/// <summary>
	/// 終了
	/// </summary>
	void Finalize();

	/// <summary>
	/// hwndゲッター
	/// </summary>
	/// <returns></returns>
	HWND GetHwnd()const { return hwnd_; }

	HINSTANCE GetHInstance()const { return wc_.hInstance; }

	//クライアント領域サイズ
	static const int32_t kClientWidth = 1280;
	static const int32_t kClientHeight = 720;

private:
	//ウィンドウハンドル
	HWND hwnd_ = nullptr;

	//ウィンドウクラスの設定
	WNDCLASS wc_{};
};

