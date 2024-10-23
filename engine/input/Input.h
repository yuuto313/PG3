#pragma once
#include <wrl.h>
#include <cassert>
#include <windows.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include "WinApp.h"

/// <summary>
/// 入力
/// </summary>
class Input
{
public:
	//namespace省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(WinApp* winApp);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// キーの押下をチェック
	/// </summary>
	/// <param name="keyNumber"></param>
	/// <returns></returns>
	bool PushKey(BYTE keyNumber);

	/// <summary>
	/// キーのトリガーをチェック
	/// </summary>
	/// <param name="keyNumber"></param>
	/// <returns></returns>
	bool TriggerKey(BYTE keyNumber);
private:
	//DirectInputのインスタンス生成
	ComPtr<IDirectInput8> directInput_;

	//キーボードデバイス生成
	ComPtr<IDirectInputDevice8> keyboard_;

	//全キーの状態
	BYTE key_[256] = {};

	//前回の全キーの状態
	BYTE keyPre_[256] = {};

	//WindowsAPI
	WinApp* winApp_ = nullptr;
};

