#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

/// <summary>
/// DIrectX基盤
/// </summary>
class DirectXCommon
{
public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

private:
	//DirectX12デバイス
	Microsoft::WRL::ComPtr<ID3D12Device> device;
	Microsoft::WRL::ComPtr<IDXGIFactory> dxgiFactory;
};

