#include "Object3d.h"
#include "Object3dCommon.h"
#include "TextureManager.h"

#include <cassert>
#include <fstream>

void Object3d::Initialize(Object3dCommon* object3dCommon)
{

	// 引数で受けっとってメンバ変数に記録
	this->pObject3dCommon_ = object3dCommon;

	//-------------------------------------
	// モデル読み込み
	//-------------------------------------
	modelData_ = LoadObjFile("resources", "Plane.obj");

	//-------------------------------------
	// テクスチャ読み込み
	//-------------------------------------

	// .objの参照しているテクスチャファイル読み込み
	TextureManager::GetInstance()->LoadTexture(modelData_.material.textureFilePath);

	// 読み込んだテクスチャの番号を取得
	TextureManager::GetInstance()->GetTextureIndexByFilePath(modelData_.material.textureFilePath);

	//-------------------------------------
	// Transform情報を作る
	//-------------------------------------

	transform_ = { {1.0f,1.0f,1.f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	cameraTransform_ = { {1.0f,1.0f,1.0f},{0.3f,0.0f,0.0f},{0.0f,4.0f,-10.0f} };

	//-------------------------------------
	// 頂点データ作成
	//-------------------------------------

	CreateVertexData();

	//-------------------------------------
	// マテリアルデータ作成
	//-------------------------------------

	CreateMaterialData();

	//-------------------------------------
	// 座標変換行列データ作成
	//-------------------------------------

	CreateTrasnformationMatrixData();

	//-------------------------------------
	// 並行光源データ作成
	//-------------------------------------

	CreateDirectionalLightData();

}

void Object3d::Update()
{
	// 左下
	vertexData_[0].position = { left,bottom,0.0f,1.0f };
	vertexData_[0].texcoord = { tex_left,tex_bottom };
	vertexData_[0].normal = { 0.0f,0.0f,-1.0f };
	// 左上
	vertexData_[1].position = { left,top,0.0f,1.0f };
	vertexData_[1].texcoord = { tex_left,tex_top };
	vertexData_[1].normal = { 0.0f,0.0f,-1.0f };

	// 右下
	vertexData_[2].position = { right,bottom,0.0f,1.0f };
	vertexData_[2].texcoord = { tex_right,tex_bottom };
	vertexData_[2].normal = { 0.0f,0.0f,-1.0f };

	// 右上
	vertexData_[3].position = { right,top,0.0f,1.0f };
	vertexData_[3].texcoord = { tex_right,tex_top };
	vertexData_[3].normal = { 0.0f,0.0f,-1.0f };

	//-------------------------------------
	// Transform情報を作る
	//-------------------------------------

	CreateWVPMatrix();
}

void Object3d::CreateVertexData()
{
	//-------------------------------------
	// VertexResourceを作る
	//-------------------------------------

	vertexResource_ = pObject3dCommon_->GetDxCommon()->CreateBufferResource(sizeof(VertexData) * modelData_.vertices.size());

	//-------------------------------------
	// VertexBufferViewを作成する(値を設定するだけ)
	//-------------------------------------

	//リソースの先頭アドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点のサイズ
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * modelData_.vertices.size());
	//１頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	//-------------------------------------
	// VertexResourceにデータを書き込むためのアドレスを取得してVertexDataに割り当てる
	//-------------------------------------

	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	//頂点データをリソースにコピー
	std::memcpy(vertexData_, modelData_.vertices.data(), sizeof(VertexData) * modelData_.vertices.size());
	
}

void Object3d::CreateMaterialData()
{
	//-------------------------------------
	// マテリアルリソースを作る
	//-------------------------------------

	materialResource_ = pObject3dCommon_->GetDxCommon()->CreateBufferResource(sizeof(Material));

	//-------------------------------------
	// マテリアルリソースにデータを書き込むためのアドレスを取得してmaterialDataに割り当てる
	//-------------------------------------

	// 書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));

	//-------------------------------------
	// マテリアルデータの初期値を書き込む
	//-------------------------------------

	// 今回は白を書き込み
	materialData_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	// SpriteはLightingしないのでfalseを設定する
	materialData_->enableLighting = false;
	// 単位行列で初期化
	materialData_->uvTransform = MyMath::MakeIdentity4x4();

}

void Object3d::CreateTrasnformationMatrixData()
{
	//-------------------------------------
	// 座標変換行列リソースを作る
	//-------------------------------------

	transformationMatrixResource_ = pObject3dCommon_->GetDxCommon()->CreateBufferResource(sizeof(TransformationMatrix));

	//-------------------------------------
	// 座標変換行列リソースにデータを書き込むためのアドレスを取得してtransformationMatrixDataに割り当てる
	//-------------------------------------

	transformationMatrixResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixData_));

	//-------------------------------------
	// 単位行列を書き込んでおく
	//-------------------------------------

	transformationMatrixData_->WVP = MyMath::MakeIdentity4x4();
	transformationMatrixData_->World = MyMath::MakeIdentity4x4();
}

void Object3d::CreateDirectionalLightData()
{
	//-------------------------------------
	// 並行光源リソースを作る
	//-------------------------------------

	directionalLightResource_ = pObject3dCommon_->GetDxCommon()->CreateBufferResource(sizeof(DirectionalLight));

	//-------------------------------------
	// 並行光源リソースにデータを書き込むためのアドレスを取得して並行光源構造体のポインタに割り当てる
	//-------------------------------------

	directionalLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData_));

	//-------------------------------------
	// デフォルトを書き込んでおく
	//-------------------------------------

	directionalLightData_->color = { 1.0f,1.0f,1.0f };
	directionalLightData_->direction = MyMath::Normalize({ 0.0f,-1.0f,0.0f });
	directionalLightData_->intensity = 1.0f;
}

void Object3d::CreateWVPMatrix()
{
	//-------------------------------------
	// TransformからWorldMatrixを作る
	//-------------------------------------

	Matrix4x4 worldMatrix = MyMath::MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);

	//-------------------------------------
	// ViewMatrixを作って単位行列を代入
	//-------------------------------------

	Matrix4x4 viewMatrix = MyMath::MakeIdentity4x4();

	//-------------------------------------
	// ProjectionMatrixを作って並行投影行列を書き込む
	//-------------------------------------

	Matrix4x4 projectionMatrix = MyMath::MakeOrthographicMatrix(0.0f, 0.0f, float(WinApp::kClientWidth), float(WinApp::kClientHeight), 0.0f, 100.f);

	Matrix4x4 wvpMatrix = MyMath::Multiply(worldMatrix, MyMath::Multiply(viewMatrix, projectionMatrix));
	transformationMatrixData_->WVP = wvpMatrix;
	transformationMatrixData_->World = worldMatrix;
}

MaterialData Object3d::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename)
{
	//-------------------------------------
	// 1.中で必要になる変数の宣言
	//-------------------------------------

	//構築するマテリアルデータ
	MaterialData materialData;
	//ファイルから読んだ1行を格納するもの
	std::string line;

	//-------------------------------------
	// 2.ファイルを開く
	//-------------------------------------

	std::ifstream file(directoryPath + "/" + filename);
	//とりあえず開けなかったら止める
	assert(file.is_open());

	//-------------------------------------
	// 3.実際にファイルを読み、MaterialDataを構築していく
	//-------------------------------------
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		//identifilerに応じた処理
		if (identifier == "map_Kd") {
			std::string textureFilename;
			s >> textureFilename;
			//連結してファイルパスにする
			materialData.textureFilePath = directoryPath + "/" + textureFilename;
		}
	}
	return materialData;
}

ModelData Object3d::LoadObjFile(const std::string& directoryPath, const std::string& filename)
{
	//-------------------------------------
	// 1.中で必要になる変数の宣言
	//-------------------------------------

	ModelData modelData;;//構築するModelData
	std::vector<Vector4> positions;//位置
	std::vector<Vector3> normals;//法線
	std::vector<Vector2> texcoords;//テクスチャの座標
	std::string line;//ファイルから読んだ一行を格納するもの

	//-------------------------------------
	// 2.ファイルを開く
	//-------------------------------------

	std::ifstream file(directoryPath + "/" + filename);
	assert(file.is_open());//開けなかったら止める

	//-------------------------------------
	// 3.ファイルを読み、ModelDataを構築
	//-------------------------------------

	while (std::getline(file, line)) {
		//streamから１行読んでstd::stringに格納する関数。whileでファイルの最後まで１行ずつ読み込んでいく
		std::string identifier;
		//文字列を分析しながら読むためのクラス、空白を区切り文字として読んでいくことができる
		std::istringstream s(line);
		//先頭の識別子を読む
		s >> identifier;

		//identifierに応じた処理
		//"v":頂点位置
		//"vt":頂点テクスチャ座標
		//"vn":頂点法線
		//"f":面
		if (identifier == "v") {
			Vector4 position;
			s >> position.x >> position.y >> position.z;
			position.x *= -1.0f;
			position.w = 1.0f;
			positions.push_back(position);
		} else if (identifier == "vt") {
			Vector2 texcoord;
			s >> texcoord.x >> texcoord.y;
			texcoords.push_back(texcoord);
		} else if (identifier == "vn") {
			Vector3 normal;
			s >> normal.x >> normal.y >> normal.z;
			normal.x *= -1.0f;
			normals.push_back(normal);
		} else if (identifier == "f") {
			//三角形を作る
			VertexData triangle[3];
			//面は三角形限定。その他は未対応
			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
				std::string vertexDefinition;
				s >> vertexDefinition;
				//頂点の要素へのIndexは「位置/UV/法線」で格納されてるので、　分解してIndexを取得する
				std::istringstream v(vertexDefinition);
				uint32_t elementIndices[3];
				for (int32_t element = 0; element < 3; ++element) {
					std::string index;
					//区切りでインデックスを読んでいく
					std::getline(v, index, '/');
					elementIndices[element] = std::stoi(index);
				}
				//要素へのIndexから、実際の要素の値を取得して、頂点を構築する
				Vector4 position = positions[elementIndices[0] - 1];
				Vector2 texcoord = texcoords[elementIndices[1] - 1];
				Vector3 normal = normals[elementIndices[2] - 1];
				//右手系から左手系へ変換するためｘを反転させる
				//position.x *= -1.0f;
				//normal.x *= -1.0f;
				texcoord.y = 1.0f - texcoord.y;
				//VertexData vertex = { position,texcoord,normal };
				//modelData.verteces.push_back(vertex);
				triangle[faceVertex] = { position,texcoord,normal };
			}
			//頂点を逆順で登録することで、回り順を逆にする
			modelData.vertices.push_back(triangle[2]);
			modelData.vertices.push_back(triangle[1]);
			modelData.vertices.push_back(triangle[0]);
		} else if (identifier == "mtllib") {
			//materialTemplateLibraryファイル名を取得する
			std::string materialFilename;
			s >> materialFilename;
			//基本的にobjファイルと同一階層にmtlは存在させるので、ディレクトリ名とファイル名を渡す
			modelData.material = LoadMaterialTemplateFile(directoryPath, materialFilename);
		}
	}

	return modelData;
}
