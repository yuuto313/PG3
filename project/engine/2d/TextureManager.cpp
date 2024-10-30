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
	textureDatas_.reserve(DirectXCommon::kMaxSRVCount);
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

	auto it = std::find_if(
		textureDatas_.begin(),
		textureDatas_.end(),
		[&](TextureData& textureData) {return textureData.filePath == filePath; }
	);

	if (it != textureDatas_.end()) {
		// 読み込み済みなら早期return
		return;
	}

	// テクスチャ枚数上限チェック
	assert(textureDatas_.size() + kSRVIndexTop < DirectXCommon::kMaxSRVCount);

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
	
	// テクスチャデータを追加
	textureDatas_.resize(textureDatas_.size() + 1);
	// 追加したテクスチャデータの参照を取得する
	TextureData& textureData = textureDatas_.back();

	//-------------------------------------
	// テクスチャデータ書き込み
	//-------------------------------------

	textureData.filePath = filePath;
	textureData.metadata = mipImages.GetMetadata();
	textureData.resource = dxCommon_->CreateTextureResource(textureData.metadata);
	dxCommon_->UploadTextureData(textureData.resource, mipImages);

	//-------------------------------------
	// デスクリプタハンドルの計算
	//-------------------------------------

	// テクスチャデータの要素番号をSRVのインデックスとする
	uint32_t srvIndex = static_cast<uint32_t>(textureDatas_.size() - 1) + kSRVIndexTop;

	textureData.srvHandleCPU = dxCommon_->GetSRVCPUDescriptorHandle(srvIndex);
	textureData.srvHandleGPU = dxCommon_->GetSRVGPUDescriptorHandel(srvIndex);

	//-------------------------------------
	// SRVの設定を行う
	//-------------------------------------

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = textureData.metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = UINT(textureData.metadata.mipLevels);

	// SRVを作成するDescriptorHeapの場所を決める
	textureData.srvHandleCPU = dxCommon_->GetSrvDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
	textureData.srvHandleGPU = dxCommon_->GetSrvDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();

	// 先頭はImGuiが使っているのでその次を使う
	textureData.srvHandleCPU.ptr += dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	textureData.srvHandleGPU.ptr += dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	//-------------------------------------
	// 設定を基にSRVの生成
	//-------------------------------------

	dxCommon_->GetDevice()->CreateShaderResourceView(textureData.resource.Get(), &srvDesc, textureData.srvHandleCPU);

}

uint32_t TextureManager::GetTextureIndexByFilePath(const std::string& filePath)
{
	// 読み込み済みテクスチャデータ検索
	auto it = std::find_if(
		textureDatas_.begin(),
		textureDatas_.end(),
		[&](TextureData& textureData) {return textureData.filePath == filePath; }
	);

	if (it != textureDatas_.end()) {
		// 読み込み済みなら要素番号を返す
		uint32_t textureIndex = static_cast<uint32_t>(std::distance(textureDatas_.begin(), it));
		return textureIndex;
	} else {
		// 検索がヒットしない場合は事前に適切に読み込みをしていないので停止させる
		assert(0);
		return 0;
	}

}

D3D12_GPU_DESCRIPTOR_HANDLE TextureManager::GetSrvHandleGPU(uint32_t textureIndex)
{
	// 範囲外指定違反チェック
	assert(textureIndex < textureDatas_.size());

	// テクスチャデータの参照を取得
	TextureData& textureData = textureDatas_[textureIndex];
	return textureData.srvHandleGPU;
}

const DirectX::TexMetadata& TextureManager::GetMetaData(uint32_t textureIndex)
{
	// 範囲外指定違反チェック
	assert(textureIndex < textureDatas_.size());

	// テクスチャデータの参照を取得
	TextureData& textureData = textureDatas_[textureIndex];
	return textureData.metadata;
}


