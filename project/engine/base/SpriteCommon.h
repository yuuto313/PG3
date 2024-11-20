#pragma once
#include "DirectXCommon.h"

//-------------------------------------
// BlendMode
//-------------------------------------

enum class BlendMode {
	// ブレンドなし
	kBlendModeNone,
	// 通常行αブレンド。デフォルト
	kBlendModeNormal,
	// 加算
	kBlendModeAdd,
	// 減算
	kBlendModeSubtract,
	// 乗算
	kBlendModeMultily,
	// スクリーン
	kBlendModeScreen,
	// 利用してはいけない
	kCountOfBlendMode,
};

/// <summary>
/// スプライン共通部
/// </summary>
class SpriteCommon
{
private:// シングルトン設計

	static SpriteCommon* instance;

	/// <summary>
	/// コンストラクタ、デストラクタの隠蔽
	/// </summary>
	SpriteCommon() = default;
	~SpriteCommon() = default;

	/// <summary>
	/// コピーコンストラクタの封印
	/// </summary>
	/// <param name=""></param>
	SpriteCommon(SpriteCommon&) = delete;

	/// <summary>
	/// コピー代入演算の封印
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	SpriteCommon& operator=(SpriteCommon&) = delete;

public:

	//-------------基本処理-------------//
	/// <summary>
	///	シングルトンインスタンスの取得
	/// </summary>
	/// <returns></returns>
	static SpriteCommon* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="dxCommon"></param>
	void Initialize(DirectXCommon* dxCommon);

	/// <summary>
	/// 終了
	/// </summary>
	void Finalize();

	/// <summary>
	/// 更新
	/// </summary>
	void ImGui();

	/// <summary>
	/// 共通描画設定
	/// </summary>
	void PreDraw();

	/// <summary>
	/// BlendStateの設定
	/// </summary>
	void SetBlendState(D3D12_BLEND_DESC& blendDesc, BlendMode blendMode);

	//-------------ゲッター・セッター-------------//

	DirectXCommon* GetDxCommon()const {return dxCommon_; }

private:

	//-------------メンバ変数-------------//

	BlendMode blendMode_ = BlendMode::kBlendModeNormal;

	DirectXCommon* dxCommon_;

	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature_;

	D3D12_DESCRIPTOR_RANGE descriptorRange_[1] = {};
	D3D12_ROOT_PARAMETER rootParameters_[4] = {};
	D3D12_STATIC_SAMPLER_DESC staticSamplers_[1] = {};
	ID3DBlob* signatureBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc_{};
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob_;
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob_;
	D3D12_INPUT_ELEMENT_DESC inputElementDescs_[3] = {};
	D3D12_BLEND_DESC blendDesc_{};
	D3D12_RASTERIZER_DESC rasterizerDesc_{};
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc_{};
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc_{};
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

	static Microsoft::WRL::ComPtr<ID3D12PipelineState> CreateGraphicsPipelineState(Microsoft::WRL::ComPtr<ID3D12Device> device, const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc);
};

