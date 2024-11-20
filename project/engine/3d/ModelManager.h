#pragma once
#include "Model.h"
#include "ModelCommon.h"
#include "DirectXCommon.h"
#include "SrvManager.h"

#include <map>
#include <format>

/// <summary>
/// テクスチャマネージャー
/// </summary>
class ModelManager
{
private:// シングルトン設計
	static ModelManager* instance;

	/// <summary>
	/// コンストラクタ、デストラクタの隠蔽
	/// </summary>
	ModelManager() = default;
	~ModelManager() = default;

	/// <summary>
	/// コピーコンストラクタの封印
	/// </summary>
	/// <param name=""></param>
	ModelManager(ModelManager&) = delete;

	/// <summary>
	/// コピー代入演算の封印
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	ModelManager& operator=(ModelManager&) = delete;

public:
	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns></returns>
	static ModelManager* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(DirectXCommon* dxCommon);

	/// <summary>
	/// 終了
	/// </summary>
	void Finalize();
	
	/// <summary>
	/// モデルファイルの読み込み
	/// </summary>
	/// <param name="filePath"></param>
	void LoadModel(const std::string& filePath);

	/// <summary>
	/// モデルの検索
	/// </summary>
	/// <param name="filePath"></param>
	/// <returns></returns>
	Model* FindModel(const std::string& filePath);

private:

	// モデルデータ
	std::map<std::string, std::unique_ptr<Model>> models_;

	ModelCommon* pModelCommon_ = nullptr;

};

