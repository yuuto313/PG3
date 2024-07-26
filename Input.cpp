#include "Input.h"

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

void Input::Initialize(WinApp* winApp)
{
	//借りてきたWinAppのインスタンスを記録
	winApp_ = winApp;

	HRESULT result;

	result = DirectInput8Create(winApp->GetHInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput_, nullptr);
	assert(SUCCEEDED(result));
	result = directInput_->CreateDevice(GUID_SysKeyboard, &keyboard_, NULL);
	assert(SUCCEEDED(result));

	//入力データ形式のセット
	result = keyboard_->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(result));

	//排他制御レベルのセット
	result = keyboard_->SetCooperativeLevel(winApp->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));
}

void Input::Update()
{
	//前回のキー入力を保存
	memcpy(keyPre_, key_, sizeof(key_));

	//キーボード情報の取得開始
	keyboard_->Acquire();
	//全キーの入力情報を取得する
	keyboard_->GetDeviceState(sizeof(key_), key_);

}

bool Input::PushKey(BYTE keyNumber)
{
	//指定キーを押していればtrueを返す
	if (key_[keyNumber]) {
		return true;
	} else {
		return false;
	}
}

bool Input::TriggerKey(BYTE keyNumber)
{
	if (!keyPre_[keyNumber] && key_[keyNumber]) {
		return true;
	} else {
		return false;
	}
}
