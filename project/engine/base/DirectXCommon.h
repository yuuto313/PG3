#pragma once
#include <windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxcapi.h>
#include <wrl.h>
#include <array>
#include <string>
#include <chrono>
#include <vector>

#include "externals/DirectXTex/DirectXTex.h"
#include "externals/DirectXTex/d3dx12.h"

#include "WinApp.h"

/// <summary>
/// DirectX基盤
/// </summary>
class DirectXCommon
{
public:
	//namespace省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	// 最大SRV数(最大テクスチャ枚数)
	static const uint32_t kMaxSRVCount;

	//-------------基本処理-------------//

	DirectXCommon();
	~DirectXCommon();

	/// <summary>
	/// シェーダーのコンパイル
	/// </summary>
	/// <param name="filePath"></param>
	/// <param name="profile"></param>
	/// <returns></returns>
	Microsoft::WRL::ComPtr<IDxcBlob> CompileShader(const std::wstring& filePath, const wchar_t* profile);

	/// <summary>
	/// バッファリソースの生成
	/// </summary>
	/// <param name="sizeInBytes"></param>
	/// <returns></returns>
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(size_t sizeInBytes);

	/// <summary>
	/// テクスチャリソースの生成
	/// </summary>
	/// <param name="metaData"></param>
	/// <returns></returns>
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(const DirectX::TexMetadata& metadata);

	/// <summary>
	/// テクスチャデータの転送
	/// </summary>
	/// <param name="texture"></param>
	/// <param name="mipImages"></param>
	/// <returns></returns>
	void UploadTextureData(const Microsoft::WRL::ComPtr<ID3D12Resource>& texture, const DirectX::ScratchImage& mipImages);


	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(WinApp* winApp);

	/// <summary>
	/// 描画前処理
	/// </summary>
	void PreDraw();

	/// <summary>
	/// 描画後処理
	/// </summary>
	void PostDraw();

	//-------------ゲッター・セッター-------------//

	/// <summary>
	/// デバイスの取得
	/// </summary>
	/// <returns>デバイス</returns>
	ID3D12Device* GetDevice() const { return device_.Get(); }

	/// <summary>
	/// SRVを取得
	/// </summary>
	/// <returns></returns>
	ID3D12DescriptorHeap* GetSrvDescriptorHeap()const { return srvDescriptorHeap_.Get(); }

	uint32_t GetDescriptorSizeSRV()const { return descriptorSizeSRV_; }

	/// <summary>
	/// コマンドリストの取得
	/// </summary>
	/// <returns></returns>
	ID3D12GraphicsCommandList* GetCommandList()const { return commandList_.Get(); }

	/// <summary>
	/// swapChainDescの取得
	/// </summary>
	/// <returns></returns>
	DXGI_SWAP_CHAIN_DESC1 GetSwapChainDesc() { return swapChainDesc_; }

	/// <summary>
	/// RTVDescの取得
	/// </summary>
	/// <returns></returns>
	D3D12_RENDER_TARGET_VIEW_DESC GetRTVDesc() { return rtvDesc_; }

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
	//-------------メンバ変数-------------//
	
	//WindowsAPI
	WinApp* winApp_ = nullptr;

	//Direct3D関連
	Microsoft::WRL::ComPtr<ID3D12Device> device_ = nullptr;
	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_ = nullptr;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc_ = {};
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap_ = nullptr;
	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, 2> swapChainResources_ = {};
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc_ = {};
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles_[2] = {};
	Microsoft::WRL::ComPtr<ID3D12Fence> fence_ = {};
	// フェンス値
	UINT64 fenceVal_ = {};
	HANDLE fenceEvent_ = {};

	D3D12_RESOURCE_BARRIER barrier_ = {};
	D3D12_VIEWPORT viewport_ = {};
	D3D12_RECT scissorRect_ = {};

	IDxcUtils* dxcUtils_ = {};
	IDxcCompiler3* dxcCompiler_ = {};
	IDxcIncludeHandler* includeHandler_ = {};

	uint32_t descriptorSizeSRV_ = {};
	uint32_t descriptorSizeRTV_ = {};
	uint32_t descriptorSizeDSV_ = {};

	
	// 記録時間(FPS固定用)
	std::chrono::steady_clock::time_point reference_ = {};

	//-------------メンバ関数-------------//

	/// <summary>
	/// デバイスの生成
	/// </summary>
	/// <param name="result"></param>
	void InitializeDXGIDevice();

	/// <summary>
	/// コマンド関連の生成
	/// </summary>
	/// <param name="result"></param>
	void InitializeCommand();

	/// <summary>
	/// スワップチェーンの生成
	/// </summary>
	/// <param name="result"></param>
	void CreateSwapChain();

	/// <summary>
	/// 深度バッファの生成
	/// </summary>
	void CreateDepthBuffer();

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
	void CreateFence();

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
	void InitializeDXCCompiler();

	/// <summary>
	/// FPS固定初期化
	/// </summary>
	void InitializeFixFPS();

	/// <summary>
	/// FPS固定更新
	/// </summary>
	void UpdateFixFPS();
};

