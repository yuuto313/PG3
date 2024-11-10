#pragma once
#include "ModelCommon.h"
#include "VertexData.h"
#include "Material.h"

#include <vector>

//ModelData構造体
struct ModelData {
	std::vector<VertexData> vertices;
	MaterialData material;
};

/// <summary>
/// 3Dモデル
/// </summary>
class Model
{
public:// 基本処理

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="modelCommon"></param>
	void Initialize(ModelCommon* modelCommon, const std::string& directoryPath, const std::string& filename);

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

private:// メンバ変数

	// ModelCommonのポインタ
	ModelCommon* pModelCommon_;

	// objファイルのデータ
	ModelData modelData_;

	// バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;

	// バッファリソース内のデータを指すポインタ
	VertexData* vertexData_ = nullptr;
	Material* materialData_ = nullptr;

	// バッファリソースの使い道を補足するバッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;

private:// メンバ関数

	/// <summary>
	/// 頂点データ作成
	/// </summary>
	void CreateVertexData();

	/// <summary>
	/// マテリアルデータ作成
	/// </summary>
	void CreateMaterialData();

	/// <summary>
	/// .mtlファイルの読み取り
	/// </summary>
	/// <param name="directoryPath"></param>
	/// <param name="filename"></param>
	/// <returns></returns>
	static MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);

	/// <summary>
	/// .objファイルの読み取り
	/// </summary>
	/// <param name="directoryPath"></param>
	/// <param name="filename"></param>
	/// <returns></returns>
	static ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);
};

