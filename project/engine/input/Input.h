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
private:// シングルトン設計

	static Input* instance;

	/// <summary>
	/// コンストラクタ、デストラクタの隠蔽
	/// </summary>
	Input() = default;
	~Input() = default;

	/// <summary>
	/// コピーコンストラクタの封印
	/// </summary>
	/// <param name=""></param>
	Input(Input&) = delete;

	/// <summary>
	/// コピー代入演算の封印
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	Input& operator=(Input&) = delete;

public:
	//namespace省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	/// <summary>
	///	シングルトンインスタンスの取得
	/// </summary>
	/// <returns></returns>
	static Input* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(WinApp* winApp);

	/// <summary>
	/// 終了
	/// </summary>
	void Finalize();

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

