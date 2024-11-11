#include "SrvManager.h"
#include <cassert>

const uint32_t SrvManager::kMaxCount_ = 512;

void SrvManager::Initialize(DirectXCommon* dxCommon)
{
	// 引数で受け取ってメンバ変数に記録する
	this->pDxCommon_ = dxCommon;

	// デスクリプタヒープの生成
	descriptorHeap_ = pDxCommon_->CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, kMaxCount_, true);

	// デスクリプタ1個分のサイズを取得して記録
	desctiptorSize_ = pDxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

}

uint32_t SrvManager::Allocate()
{
	// 上限に達しているかチェックしてassert
	assert(useIndex < kMaxCount_);

	// returnする番号を一旦記録しておく
	int index = useIndex;

	// 次回のために番号を1進める
	useIndex++;

	// 上で記録した番号をreturn
	return index;

}

bool SrvManager::CheckAllocate()
{
	return false;
}

void SrvManager::PreDraw()
{

	// 描画用のDescriptorHeapの設定
	ID3D12DescriptorHeap* descriptorHeaps[] = { descriptorHeap_.Get() };
	pDxCommon_->GetCommandList()->SetDescriptorHeaps(1, descriptorHeaps);

}

void SrvManager::CreateSRVforTexture2D(uint32_t srvIndex, ID3D12Resource* pResource, DXGI_FORMAT format, UINT mipLevels)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};

	srvDesc.Format = format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = mipLevels;

	pDxCommon_->GetDevice()->CreateShaderResourceView(pResource, &srvDesc, GetCPUDescriptorHandle(srvIndex));
}

void SrvManager::CreateSRVforStructuredBuffer(uint32_t srvIndex, ID3D12Resource* pResource, UINT numElements, UINT structureByteStride)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};

	srvDesc.Format = DXGI_FORMAT_UNKNOWN; // Structured Bufferの場合はDXGI_FORMAT_UNKNOWN
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.NumElements = numElements;
	srvDesc.Buffer.StructureByteStride = structureByteStride;
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

	pDxCommon_->GetDevice()->CreateShaderResourceView(pResource, &srvDesc, GetCPUDescriptorHandle(srvIndex));
}

D3D12_CPU_DESCRIPTOR_HANDLE SrvManager::GetCPUDescriptorHandle(uint32_t index)
{
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap_->GetCPUDescriptorHandleForHeapStart();

	handleCPU.ptr += (desctiptorSize_ * index);

	return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE SrvManager::GetGPUDescriptorHandle(uint32_t index)
{
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap_->GetGPUDescriptorHandleForHeapStart();

	handleGPU.ptr += (desctiptorSize_ * index);

	return handleGPU;
}

void SrvManager::SetGraphicsRootDescriptorTable(UINT rootParameter, uint32_t srvIndex)
{
	pDxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(rootParameter, GetGPUDescriptorHandle(srvIndex));
}
