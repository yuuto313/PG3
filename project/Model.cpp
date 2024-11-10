#include "Model.h"
#include "TextureManager.h"

#include <cassert>
#include <fstream>

void Model::Initialize(ModelCommon* modelCommon)
{
	// 引数で受け取ってメンバ変数に記録する
	this->pModelCommon_ = modelCommon;

	//-------------------------------------
	// モデル読み込み
	//-------------------------------------

	modelData_ = LoadObjFile("resource", "plane.obj");

	//-------------------------------------
	// 頂点データ作成
	//-------------------------------------

	CreateVertexData();

	//-------------------------------------
	// マテリアルデータ作成
	//-------------------------------------

	CreateMaterialData();

	//-------------------------------------
	// テクスチャ読み込み
	//-------------------------------------

	// .objの参照しているテクスチャファイル読み込み
	TextureManager::GetInstance()->LoadTexture(modelData_.material.textureFilePath);

	// 読み込んだテクスチャの番号を取得
	modelData_.material.textureIndex = TextureManager::GetInstance()->GetTextureIndexByFilePath(modelData_.material.textureFilePath);
}

void Model::Draw()
{
	//-------------------------------------
	// VertexBufferViewを設定する
	//-------------------------------------

	pModelCommon_->GetDxCommon()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);

	//-------------------------------------
	// マテリアルCBufferの場所を設定
	//-------------------------------------

	pModelCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	
	//-------------------------------------
	// SRVのDescriptorTableの先頭を設定
	//-------------------------------------

	pModelCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetSrvHandleGPU(modelData_.material.textureIndex));

	//-------------------------------------
	// 描画！
	//-------------------------------------

	pModelCommon_->GetDxCommon()->GetCommandList()->DrawInstanced(UINT(modelData_.vertices.size()), 1, 0, 0);

}

void Model::CreateVertexData()
{
	//-------------------------------------
	// VertexResourceを作る
	//-------------------------------------

	vertexResource_ = pModelCommon_->GetDxCommon()->CreateBufferResource(sizeof(VertexData) * modelData_.vertices.size());

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

void Model::CreateMaterialData()
{
	//-------------------------------------
	// マテリアルリソースを作る
	//-------------------------------------

	materialResource_ = pModelCommon_->GetDxCommon()->CreateBufferResource(sizeof(Material));

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

MaterialData Model::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename)
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

ModelData Model::LoadObjFile(const std::string& directoryPath, const std::string& filename)
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
			//position.x *= -1.0f;
			position.w = 1.0f;
			positions.push_back(position);
		} else if (identifier == "vt") {
			Vector2 texcoord;
			s >> texcoord.x >> texcoord.y;
			texcoords.push_back(texcoord);
		} else if (identifier == "vn") {
			Vector3 normal;
			s >> normal.x >> normal.y >> normal.z;
			//normal.x *= -1.0f;
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
