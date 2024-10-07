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


	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;

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

