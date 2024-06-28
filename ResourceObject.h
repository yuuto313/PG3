#include <d3d12.h>
#include <wrl.h>

#pragma once
/// <summary>
/// ID3D12Resourceを自動で開放するクラス
/// </summary>
class ResourceObject
{
public:
	ResourceObject(Microsoft::WRL::ComPtr<ID3D12Resource> resource) :resource_(resource) {};
	~ResourceObject() {
		if (resource_) {
			//resource_->Release();
		}
	};

	Microsoft::WRL::ComPtr<ID3D12Resource> Get() {
		return resource_;
	}
private:
	Microsoft::WRL::ComPtr<ID3D12Resource> resource_;
};

