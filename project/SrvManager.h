#pragma once
#include "DirectXCommon.h"

/// <summary>
/// SRV管理
/// </summary>
class SrvManager
{
public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(DirectXCommon* dxCommon);

	/// <summary>
	/// 確保
	/// </summary>
	/// <returns></returns>
	uint32_t Allocate();

	/// <summary>
	/// 確保可能チェック
	/// </summary>
	/// <returns></returns>
	bool CheckAllocate(size_t size);

	/// <summary>
	/// 描画前処理
	/// </summary>
	void PreDraw();

	/// <summary>
	/// SRV生成（テクスチャ用）
	/// </summary>
	/// <param name="srvIndex"></param>
	/// <param name="pResource"></param>
	/// <param name="format"></param>
	/// <param name="mipLevels"></param>
	void CreateSRVforTexture2D(uint32_t srvIndex, ID3D12Resource* pResource, DXGI_FORMAT format, UINT mipLevels);

	/// <summary>
	/// SRV生成（Structured Buffer用）
	/// </summary>
	/// <param name="srvIndex"></param>
	/// <param name="pResource"></param>
	/// <param name="numElements"></param>
	/// <param name="structureByteStride"></param>
	void CreateSRVforStructuredBuffer(uint32_t srvIndex, ID3D12Resource* pResource, UINT numElements, UINT structureByteStride);

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(uint32_t index);

	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(uint32_t index);

	/// <summary>
	/// SRVセットコマンド
	/// </summary>
	/// <param name="rootParameter"></param>
	/// <param name="srvIndex"></param>
	void SetGraphicsRootDescriptorTable(UINT rootParameter, uint32_t srvIndex);

private:
	DirectXCommon* pDxCommon_ = nullptr;

	// 最大SRV数（最大テクスチャ枚数）
	static const uint32_t kMaxCount_;
	// SRV用のデスクリプタサイズ
	uint32_t desctiptorSize_;
	// SRV用デスクリプタヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap_;

	// 次に使用するSRVインデックス
	uint32_t useIndex = 0;

};

