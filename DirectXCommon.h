#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxcapi.h>
#include <wrl.h>
#include <array>

#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"

#include "WinApp.h"

//最後まで実装したが、エラーが出る
//分かれてる関数をもう一度確認
//どこを実装するとエラーになるか確認

/// <summary>
/// DIrectX基盤
/// </summary>
class DirectXCommon
{
public:
	//namespace省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(WinApp* winApp);

	/// <summary>
	/// SRVの指定番号のCPUデスクリプタハンドルを取得する
	/// </summary>
	/// <param name="index"></param>
	/// <returns></returns>
	D3D12_CPU_DESCRIPTOR_HANDLE GetSRVCPUDescriptorHandle(uint32_t index);

	/// <summary>
	/// SRVの指定番号のGPUデスクリプタハンドルを取得する
	/// </summary>
	/// <param name="index"></param>
	/// <returns></returns>
	D3D12_GPU_DESCRIPTOR_HANDLE GetSRVGPUDescriptorHandel(uint32_t index);

private:
	//WindowsAPI
	WinApp* winApp = nullptr;

	//DirectX12デバイス
	ComPtr<ID3D12Device> device;
	ComPtr<IDXGIFactory7> dxgiFactory;
	ComPtr<ID3D12CommandAllocator> commandAllocator;
	ComPtr<ID3D12GraphicsCommandList> commandList;
	ComPtr<ID3D12CommandQueue> commandQueue;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
	ComPtr<IDXGISwapChain4> swapChain;
	ComPtr<ID3D12Resource> depthStencilResource;
	ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap;
	ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap;
	ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap;
	std::array<ComPtr<ID3D12Resource>, 2> swapChainResources;
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc;
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];
	ComPtr<ID3D12Fence> fence;
	D3D12_VIEWPORT viewport{};
	D3D12_RECT scissorRect{};
	IDxcUtils* dxcUtils;
	IDxcCompiler3* dxcCompiler;
	IDxcIncludeHandler* includeHandler;

	uint32_t descriptorSizeSRV;
	uint32_t descriptorSizeRTV;
	uint32_t descriptorSizeDSV;

	/// <summary>
	/// デバイスの生成
	/// </summary>
	/// <param name="result"></param>
	void InitializeDevice();

	/// <summary>
	/// コマンド関連の生成
	/// </summary>
	/// <param name="result"></param>
	void InitializeCommand();

	/// <summary>
	/// スワップチェーンの生成
	/// </summary>
	/// <param name="result"></param>
	void InitializeSwapChain();

	/// <summary>
	/// 深度バッファの生成
	/// </summary>
	void InitialzeDepthBuffer();

	/// <summary>
	/// 深度バッファリソースを作成する
	/// </summary>
	/// <param name="width"></param>
	/// <param name="height"></param>
	/// <returns></returns>
	ComPtr<ID3D12Resource> CreateDepthStencilTextureResource(const int32_t width, const uint32_t height);

	/// <summary>
	/// デスクリプタヒープの生成
	/// </summary>
	/// <param name="result"></param>
	void InitializeDescriptorHeap();

	/// <summary>
	/// デスクリプタヒープ生成関数
	/// </summary>
	/// <param name="heapType"></param>
	/// <param name="numDescriptors"></param>
	/// <param name="shaderVisible"></param>
	/// <returns></returns>
	ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(const D3D12_DESCRIPTOR_HEAP_TYPE& heapType, const UINT& numDescriptors, bool shaderVisible);

	/// <summary>
	/// レンダーターゲットビューの生成
	/// </summary>
	/// <param name="result"></param>
	void InitializeRenderTargetView();
	
	/// <summary>
	/// 指定番号のCPUデスクリプタハンドルを取得する
	/// </summary>
	/// <param name="descriptorHeap"></param>
	/// <param name="descriptorSize"></param>
	/// <param name="index"></param>
	/// <returns></returns>
	static D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap, const uint32_t descriptorSize, const uint32_t index);

	/// <summary>
	/// 指定番号のGPUデスクリプタハンドルを取得する
	/// </summary>
	/// <param name="descriptorHeap"></param>
	/// <param name="descriptorSize"></param>
	/// <param name="index"></param>
	/// <returns></returns>
	static D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap, const uint32_t descriptorSize, const uint32_t index);

	/// <summary>
	/// 深度ステンシルビューの初期化
	/// </summary>
	void InitializeDepthStencilView();

	/// <summary>
	/// フェンスの生成
	/// </summary>
	/// <param name="result"></param>
	void InitializeFence();

	/// <summary>
	/// ビューポート矩形の初期化
	/// </summary>
	void InitializeViewPort();

	/// <summary>
	/// シザリング矩形の初期化
	/// </summary>
	void InitializeScissorRect();

	/// <summary>
	/// DXCコンパイラの生成
	/// </summary>
	/// <param name="result"></param>
	void  InitializeDXCCompiler();

	/// <summary>
	/// ImGuiの初期化
	/// </summary>
	void InitializeImGui();
};

