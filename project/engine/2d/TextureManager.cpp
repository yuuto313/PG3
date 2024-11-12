#include "TextureManager.h"
#include "StringUtility.h"

// ImGuiで0番目を使用するため、1番から使用
uint32_t TextureManager::kSRVIndexTop = 1;

TextureManager* TextureManager::instance = nullptr;

TextureManager* TextureManager::GetInstance()
{
	if (instance == nullptr) {
		instance = new TextureManager;
	}

	return instance;
}

void TextureManager::Initialize(DirectXCommon* dxCommon)
{
	dxCommon_ = dxCommon;

	// SRV1の数と同数
	textureDatas_.reserve(SrvManager::kMaxCount);
}

void TextureManager::Finalize()
{
	delete instance;
	instance = nullptr;
}

void TextureManager::LoadTexture(const std::string& filePath)
{
	//-------------------------------------
	// 読み込み済みテクスチャを検索
	//-------------------------------------

	if (textureDatas_.contains(filePath)) {
		return;
	}

	assert(SrvManager::GetInstance()->CheckAllocate());

	//-------------------------------------
	// テクスチャファイルを読んでプログラムで扱えるようにする
	//-------------------------------------

	DirectX::ScratchImage image{};
	std::wstring filePathW = StringUtility::ConvertString(filePath);
	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr));

	//-------------------------------------
	// ミップマップの作成
	//-------------------------------------

	DirectX::ScratchImage mipImages{};
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
	assert(SUCCEEDED(hr));

	//-------------------------------------
	// テクスチャデータ追加
	//-------------------------------------
	
	// テクスチャデータを追加して書き込む
	TextureData& textureData = textureDatas_[filePath];

	//-------------------------------------
	// テクスチャデータ書き込み
	//-------------------------------------

	textureData.metadata = mipImages.GetMetadata();
	textureData.resource = dxCommon_->CreateTextureResource(textureData.metadata);
	dxCommon_->UploadTextureData(textureData.resource, mipImages);

	//-------------------------------------
	// SRVの設定を行う
	//-------------------------------------

	// SRV確保
	textureData.srvIndex = SrvManager::GetInstance()->Allocate();
	// SRVハンドルを取得
	textureData.srvHandleCPU = SrvManager::GetInstance()->GetCPUDescriptorHandle(textureData.srvIndex);
	textureData.srvHandleGPU = SrvManager::GetInstance()->GetGPUDescriptorHandle(textureData.srvIndex);

	//-------------------------------------
	// 設定を基にSRVの生成
	//-------------------------------------

	SrvManager::GetInstance()->CreateSRVforTexture2D(textureData.srvIndex,textureData.resource.Get(), textureData.metadata.format, UINT(textureData.metadata.mipLevels));

}

D3D12_GPU_DESCRIPTOR_HANDLE TextureManager::GetSrvHandleGPU(const std::string& filePath)
{
	// テクスチャデータの参照を取得
	TextureData& textureData = textureDatas_[filePath];
	return textureData.srvHandleGPU;
}

const DirectX::TexMetadata& TextureManager::GetMetaData(const std::string& filePath)
{
	// テクスチャデータの参照を取得
	TextureData& textureData = textureDatas_[filePath];
	return textureData.metadata;
}

uint32_t TextureManager::GetSrvIndex(const std::string& filePath)
{
	// テクスチャデータの参照を取得
	TextureData& textureData = textureDatas_[filePath];
	return textureData.srvIndex;

}
