#pragma once
#include "DirectXCommon.h"

/// <summary>
/// スプライン共通部
/// </summary>
class SpriteCommon
{
public:

	//-------------基本処理-------------//

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="dxCommon"></param>
	void Initialize(DirectXCommon* dxCommon);

	/// <summary>
	/// 共通描画設定
	/// </summary>
	void Draw();

	//-------------ゲッター・セッター-------------//

	DirectXCommon* GetDxCommon()const { dxCommon_; }

private:

	//-------------メンバ変数-------------//

	DirectXCommon* dxCommon_;

	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature_;

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc_{};
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob_;
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob_;
	D3D12_BLEND_DESC blendDesc_{};
	D3D12_RASTERIZER_DESC rasterizerDesc_{};
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc_{};
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_;

	//-------------メンバ関数-------------//

	/// <summary>
	/// ルートシグネイチャの作成
	/// </summary>
	void CreateRootSignature();

	/// <summary>
	/// グラフィックスパイプラインの生成
	/// </summary>
	void CreateGraphicsPipeline();
};

