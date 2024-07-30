#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include "WinApp.h"

//スライド17~

/// <summary>
/// DIrectX基盤
/// </summary>
class DirectXCommon
{
public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(WinApp* winApp);

private:
	//WindowsAPI
	WinApp* winApp = nullptr;

	//DirectX12デバイス
	Microsoft::WRL::ComPtr<ID3D12Device> device;
	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain;


	/// <summary>
	/// デバイスの生成
	/// </summary>
	/// <param name="result"></param>
	void InitializeDevice(HRESULT result);

	/// <summary>
	/// コマンド関連の初期化
	/// </summary>
	/// <param name="result"></param>
	void InitializeCommand(HRESULT result);

	/// <summary>
	/// スワップチェーンの初期化
	/// </summary>
	/// <param name="result"></param>
	void InitializeSwapChain(HRESULT result);
};

