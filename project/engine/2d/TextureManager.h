#pragma once
#include <string>
#include <d3d12.h>
#include <wrl.h>

#include "externals/DirectXTex/DirectXTex.h"

#include "DirectXCommon.h"

/// <summary>
/// テクスチャマネージャー
/// </summary>
class TextureManager
{
private:
	/// <summary>
	/// テクスチャ一枚分のデータ
	/// </summary>
	struct TextureData
	{
		// 画像ファイルのパス
		std::string filePath;
		// 画像の幅や高さなどの情報
		DirectX::TexMetadata metadata;
		// テクスチャリソース
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		// SRV作成時に必要なCPUハンドル
		D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU;
		// 描画コマンドに必要なGPUハンドル
		D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU;
	};	
	
	// SRVインデックスの開始番号
	static uint32_t kSRVIndexTop;

private:// シングルトン設計
	static TextureManager* instance;

	/// <summary>
	/// コンストラクタ、デストラクタの隠蔽
	/// </summary>
	TextureManager() = default;
	~TextureManager() = default;

	/// <summary>
	/// コピーコンストラクタの封印
	/// </summary>
	/// <param name=""></param>
	TextureManager(TextureManager&) = delete;

	/// <summary>
	/// コピー代入演算の封印
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	TextureManager& operator=(TextureManager&) = delete;

public:
	/// <summary>
	///	シングルトンインスタンスの取得
	/// </summary>
	/// <returns></returns>
	static TextureManager* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(DirectXCommon* dxCommon);

	/// <summary>
	/// 終了
	/// </summary>
	void Finalize();

	/// <summary>
	/// テクスチャファイルの読み込み
	/// </summary>
	/// <param name="filePath"></param>
	/// <returns></returns>
	void LoadTexture(const std::string& filePath);

	/// <summary>
	/// SRVインデックスの開始番号を取得
	/// </summary>
	/// <param name="filepath"></param>
	/// <returns></returns>
	uint32_t GetTextureIndexByFilePath(const std::string& filePath);

	/// <summary>
	/// テクスチャ番号からGPUハンドルを取得
	/// </summary>
	/// <param name="textureIndex"></param>
	/// <returns></returns>
	D3D12_GPU_DESCRIPTOR_HANDLE GetSrvHandleGPU(uint32_t textureIndex);

	/// <summary>
	/// メタデータを取得
	/// </summary>
	/// <param name="textureIndex"></param>
	/// <returns></returns>
	const DirectX::TexMetadata& GetMetaData(uint32_t textureIndex);

private:
	// テクスチャデータ
	std::vector<TextureData> textureDatas_;

	DirectXCommon* dxCommon_ = nullptr;
};

