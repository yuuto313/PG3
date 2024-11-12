#pragma once
#include "DirectXCommon.h"

/// <summary>
/// SRV管理
/// </summary>
class SrvManager
{
private:// シングルトン設計
	static SrvManager* instance;

	/// <summary>
	/// コンストラクタ、デストラクタの隠蔽
	/// </summary>
	SrvManager() = default;
	~SrvManager() = default;

	/// <summary>
	/// コピーコンストラクタの封印
	/// </summary>
	/// <param name=""></param>
	SrvManager(SrvManager&) = delete;

	/// <summary>
	/// コピー代入演算の封印
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	SrvManager& operator=(SrvManager&) = delete;

public:
	/// <summary>
	///	シングルトンインスタンスの取得
	/// </summary>
	/// <returns></returns>
	static SrvManager* GetInstance();

	// 最大SRV数（最大テクスチャ枚数）
	static const uint32_t kMaxCount;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(DirectXCommon* dxCommon);

	/// <summary>
	/// 終了
	/// </summary>
	void Finalize();

	/// <summary>
	/// 確保
	/// </summary>
	/// <returns></returns>
	uint32_t Allocate();

	/// <summary>
	/// 確保可能チェック
	/// </summary>
	/// <returns></returns>
	bool CheckAllocate();

	/// <summary>
	/// 描画前処理
	/// </summary>
	void PreDraw();

	/// <summary>
	/// SRVを取得
	/// </summary>
	/// <returns></returns>
	ID3D12DescriptorHeap* GetSrvDescriptorHeap()const { return descriptorHeap_.Get(); }

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

	/// <summary>
	/// 指定番号のCPUディスクリプタハンドルを取得
	/// </summary>
	/// <param name="index"></param>
	/// <returns></returns>
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(uint32_t index);

	/// <summary>
	/// 指定番号のGPUディスクリプタハンドルを取得
	/// </summary>
	/// <param name="index"></param>
	/// <returns></returns>
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(uint32_t index);

	/// <summary>
	/// SRVセットコマンド
	/// </summary>
	/// <param name="rootParameter"></param>
	/// <param name="srvIndex"></param>
	void SetGraphicsRootDescriptorTable(UINT rootParameter, uint32_t srvIndex);

private:
	DirectXCommon* pDxCommon_ = nullptr;

	// SRV用のデスクリプタサイズ
	uint32_t desctiptorSize_;
	// SRV用デスクリプタヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap_;

	// 次に使用するSRVインデックス
	uint32_t useIndex = 0;

};

