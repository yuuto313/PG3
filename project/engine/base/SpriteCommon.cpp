#include "SpriteCommon.h"
#include "Logger.h"
#include "ImGuiManager.h"

SpriteCommon* SpriteCommon::instance = nullptr;

SpriteCommon* SpriteCommon::GetInstance()
{
	if (instance == nullptr) {
		instance = new SpriteCommon;
	}

	return instance;
}

void SpriteCommon::Initialize(DirectXCommon* dxCommon)
{
	// 引数で受け取ってメンバ変数に記録する
	dxCommon_ = dxCommon;

	//-------------------------------------
	// グラフィックスパイプラインの生成
	//-------------------------------------

	CreateGraphicsPipeline();

}

void SpriteCommon::Finalize()
{
	delete instance;
	instance = nullptr;
}

void SpriteCommon::ImGui()
{
#ifdef _DEBUG
	ImGui::Begin("BlendDesc");
	const char* items[] = { "None","Normal","kBlendModeAdd","kBlendModeSubtract","kBlendModeMultily","kBlendModeScreen" };
	static int item_current = static_cast<int>(blendMode_);
	if (ImGui::Combo("Blend", &item_current, items, IM_ARRAYSIZE(items))) {
		blendMode_ = static_cast<BlendMode>(item_current);
	}
	ImGui::End();
#endif //_DEBUG

	//-------------------------------------
	// ブレンドモードが変更された場合、パイプラインステートを再作成
	//-------------------------------------
	static BlendMode previousBlendMode = blendMode_;
	if (previousBlendMode != blendMode_) {
		SetBlendState(blendDesc_, blendMode_);
		graphicsPipelineStateDesc_.BlendState = blendDesc_;
		graphicsPipelineState_ = CreateGraphicsPipelineState(dxCommon_->GetDevice(), graphicsPipelineStateDesc_);
		previousBlendMode = blendMode_;
	}

}


void SpriteCommon::PreDraw()
{
	// RootSignatureを設定。PSOに設定してるけど別途設定が必要
	dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature_.Get());
	// PSOを設定
	dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState_.Get());
	// 形状を設定。PSOに設定してるものとはまた別。同じものを設定すると考えておけばいい
	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void SpriteCommon::SetBlendState(D3D12_BLEND_DESC& blendDesc, BlendMode blendMode)
{
	//すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	//ブレンドを有効にするか無効にするかの指定
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	switch (blendMode)
	{
	case BlendMode::kBlendModeNone:
		blendDesc.RenderTarget[0].BlendEnable = FALSE;
		break;

	case BlendMode::kBlendModeNormal:
		blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		break;

	case BlendMode::kBlendModeAdd:
		//加算合成 
		//Result=SrcColor*SrcAlpha+DestColor
		//ピクセルシェーダーを出力するRGB値に対して実行する操作を指定する
		blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		//SrcBlend操作とDestBlend操作を組み合わせる方法を定義する
		blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
		break;

	case BlendMode::kBlendModeSubtract:
		//減算合成
		//Result=DescColor-SrcColor*SrcAlpha
		blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
		blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
		break;

	case BlendMode::kBlendModeMultily:
		//乗算合成
		//Result=SrcColor*DestColor
		blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
		break;

	case BlendMode::kBlendModeScreen:
		//スクリーン合成
		//Result=(1-DestColor)*SrcColor+DestColor
		blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
		blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
		break;

	default:
		break;
	}

	//α値ブレンド設定。基本的に使わないのでこのまま

	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
}

void SpriteCommon::CreateRootSignature()
{
	//-------------------------------------
	// DescriptorRangeを作成
	//-------------------------------------

	descriptorRange_[0].BaseShaderRegister = 0;// 0から始まる
	descriptorRange_[0].NumDescriptors = 1;// 数は1つ
	descriptorRange_[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;// SRVを使う
	descriptorRange_[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;// Offsetを自動計算

	//-------------------------------------
	// RootSignatureを作成
	//-------------------------------------
	// ShaderとResourceをどのように関連付けるかを示したオブジェクト

	// RootSignature作成
	descriptionRootSignature_.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// RootParameter作成。複数設定できるので配列。今回はPixelShaderのMaterialとVertexShaderのTransform
	
	rootParameters_[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; // CBVを使う
	rootParameters_[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	rootParameters_[0].Descriptor.ShaderRegister = 0; // レジスタ番号0とバインド

	rootParameters_[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; // CBVを使う
	rootParameters_[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX; // VertexShaderで使う
	rootParameters_[1].Descriptor.ShaderRegister = 0; // レジスタ番号0を使う

	//-------------------------------------
	// DescriptorTableを作成
	//-------------------------------------
	// DescriptorRangeをまとめたもの
	rootParameters_[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;// DescriptorTableを使う
	rootParameters_[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderで使う
	rootParameters_[2].DescriptorTable.pDescriptorRanges = descriptorRange_;// Tableの中身の配列を指定
	rootParameters_[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange_);// Tableで利用する数

	//-------------------------------------
	// 平行光源をShaderで使う
	//-------------------------------------
	rootParameters_[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;// CBVを使う
	rootParameters_[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderで使う
	rootParameters_[3].Descriptor.ShaderRegister = 1;// レジスタ番号１を使う

	//-------------------------------------
	// Samplerの設定
	//-------------------------------------

	staticSamplers_[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;// バイリニアフィルタ
	staticSamplers_[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;// 0~1の範囲外をリピート
	staticSamplers_[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers_[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers_[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;// 比較しない
	staticSamplers_[0].MaxLOD = D3D12_FLOAT32_MAX;// ありったけのMipmapを使う
	staticSamplers_[0].ShaderRegister = 0;// レジスタ番号0を使う
	staticSamplers_[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;// PixelShaderで使う
	descriptionRootSignature_.pStaticSamplers = staticSamplers_;
	descriptionRootSignature_.NumStaticSamplers = _countof(staticSamplers_);

	descriptionRootSignature_.pParameters = rootParameters_; // ルートパラメータ配列へのポインタ
	descriptionRootSignature_.NumParameters = _countof(rootParameters_); // 配列の長さ

	// シリアライズしてバイナリする

	HRESULT hr = D3D12SerializeRootSignature(&descriptionRootSignature_, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Logger::Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}

	//バイナリを元に作成
	hr = dxCommon_->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
	assert(SUCCEEDED(hr));

	//-------------------------------------
	// InputLayoutを作成
	//-------------------------------------
	// VertexShaderへ渡す頂点データがどのようなものかを指定するオブジェクト

	//InputLayoutの設定
	inputElementDescs_[0].SemanticName = "POSITION";
	inputElementDescs_[0].SemanticIndex = 0;
	inputElementDescs_[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs_[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs_[1].SemanticName = "TEXCOORD";
	inputElementDescs_[1].SemanticIndex = 0;
	inputElementDescs_[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs_[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs_[2].SemanticName = "NORMAL";
	inputElementDescs_[2].SemanticIndex = 0;
	inputElementDescs_[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs_[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputLayoutDesc_.pInputElementDescs = inputElementDescs_;
	inputLayoutDesc_.NumElements = _countof(inputElementDescs_);

	//-------------------------------------
	// BlendStateを作成
	//-------------------------------------
	// PixelShaderからの出力を画面にどのように書き込むかを設定する項目

	// BlendStateの設定(NormalBlend)
	SetBlendState(blendDesc_, blendMode_);

	//-------------------------------------
	// RasterizerStateを作成
	//-------------------------------------
	// Rasterizerに対する設定

	// RasterizerStateの設定
	// カリングしない(裏面も表示させる)
	rasterizerDesc_.CullMode = D3D12_CULL_MODE_NONE;
	// 三角形の中を塗りつぶす
	rasterizerDesc_.FillMode = D3D12_FILL_MODE_SOLID;

	//-------------------------------------
	// ShaderをCompilerする
	//-------------------------------------

	// Shaderをコンパイラする
	vertexShaderBlob_ = dxCommon_->CompileShader(L"Resources/shaders/Object3D.VS.hlsl", L"vs_6_0");
	assert(vertexShaderBlob_ != nullptr);

	pixelShaderBlob_ = dxCommon_->CompileShader(L"Resources/shaders/Object3D.PS.hlsl", L"ps_6_0");
	assert(pixelShaderBlob_ != nullptr);


	//-------------------------------------
	// DepthStencilState
	//-------------------------------------

	// DepthStencilStateの設定
	// Depthの機能を有効化する
	depthStencilDesc_.DepthEnable = true;
	// 書き込みします
	depthStencilDesc_.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	// 比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc_.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

}

void SpriteCommon::CreateGraphicsPipeline()
{
	//-------------------------------------
	// ルートシグネイチャの生成
	//-------------------------------------

	CreateRootSignature();

	//-------------------------------------
	// PSOを生成する
	//-------------------------------------

	graphicsPipelineStateDesc_.pRootSignature = rootSignature_.Get();// RootSignature
	graphicsPipelineStateDesc_.InputLayout = inputLayoutDesc_;// InputLayer
	graphicsPipelineStateDesc_.VS = { vertexShaderBlob_->GetBufferPointer(),vertexShaderBlob_->GetBufferSize() };// VertexShader
	graphicsPipelineStateDesc_.PS = { pixelShaderBlob_->GetBufferPointer(),pixelShaderBlob_->GetBufferSize() };// PixelShader
	graphicsPipelineStateDesc_.BlendState = blendDesc_;// BlendState
	graphicsPipelineStateDesc_.RasterizerState = rasterizerDesc_;// RasterizerState
	graphicsPipelineStateDesc_.DepthStencilState = depthStencilDesc_;// depthStencilState
	// 書き込むRTVの情報
	graphicsPipelineStateDesc_.NumRenderTargets = 1;
	graphicsPipelineStateDesc_.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	graphicsPipelineStateDesc_.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	// 利用するトポロジ（形状）のタイプ。三角形
	graphicsPipelineStateDesc_.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	// どのように画面に色を打ち込むかの設定（気にしない）
	graphicsPipelineStateDesc_.SampleDesc.Count = 1;
	graphicsPipelineStateDesc_.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	// 実際に生成
	HRESULT hr = dxCommon_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc_, IID_PPV_ARGS(&graphicsPipelineState_));
	assert(SUCCEEDED(hr));

}

Microsoft::WRL::ComPtr<ID3D12PipelineState> SpriteCommon::CreateGraphicsPipelineState(Microsoft::WRL::ComPtr<ID3D12Device> device, const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc)
{
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
	HRESULT hr = device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&pipelineState));
	assert(SUCCEEDED(hr));
	return pipelineState;
}
