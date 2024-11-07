#pragma once
#include "Material.h"
#include "VertexData.h"
#include "TransformationMatrix.h"
#include "MyTransform.h"

#include <d3d12.h>
#include <wrl.h>
#include <vector>

//ModelData構造体
struct ModelData {
	std::vector<VertexData> vertices;
	MaterialData material;
};

//平行光源
struct DirectionalLight{
	Vector4 color;//ライトの色
	Vector3 direction;//ライトの向き
	float intensity;//輝度
};

class Object3dCommon;

/// <summary>
/// 3Dオブジェクト
/// </summary>
class Object3d
{
public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Object3dCommon* object3dCommon);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

private:// メンバ変数

	Object3dCommon* pObject3dCommon_ = nullptr;

	// objファイルのデータ
	ModelData modelData_;

	// Transform
	Transform transform_;
	Transform cameraTransform_;

	// バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource_;

	// バッファリソース内のデータを指すポインタ
	VertexData* vertexData_ = nullptr;
	Material* materialData_ = nullptr;
	TransformationMatrix* transformationMatrixData_ = nullptr;
	DirectionalLight* directionalLightData_ = nullptr;

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
	/// 座標変換行列データ作成
	/// </summary>
	void CreateTrasnformationMatrixData();

	/// <summary>
	/// 並行光源データ作成
	/// </summary>
	void CreateDirectionalLightData();

	/// <summary>
	/// WVP行列を作成する
	/// </summary>
	void CreateWVPMatrix();

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

