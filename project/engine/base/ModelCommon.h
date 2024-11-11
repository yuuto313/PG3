#pragma once
#include "DirectXCommon.h"

/// <summary>
/// 3Dモデル共通部
/// </summary>
class ModelCommon
{
public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(DirectXCommon* dxCommon);

	DirectXCommon* GetDxCommon()const { return pDxCommon_; }

private:
	DirectXCommon* pDxCommon_;

};

