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
	// Transform情報を作る
	//-------------------------------------

	transform_ = { {1.0f,1.0f,1.f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	//-------------------------------------
	// デフォルトカメラをセットする
	//-------------------------------------

	this->pCamera_ = pObject3dCommon_->GetDefaultCamera();

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
	//-------------------------------------
	// Transform情報を作る
	//-------------------------------------

	CreateWVPMatrix();
}

void Object3d::Draw()
{

	//-------------------------------------
	// 座標変換行列の場所を設定
	//-------------------------------------
	
	pObject3dCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResource_->GetGPUVirtualAddress());

	//-------------------------------------
	// 並行光源CBufferの場所を設定
	//-------------------------------------

	pObject3dCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource_->GetGPUVirtualAddress());

	//-------------------------------------
	// 3Dモデルが割り当てられていれば描画する
	//-------------------------------------

	if (pModel_) {
		pModel_->Draw();
	}

}

void Object3d::SetModel(const std::string& filePath)
{
	pModel_ = ModelManager::GetInstance()->FindModel(filePath);
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
	// worldViewProjectionMatrixを作成
	//-------------------------------------

	if (pCamera_) {
		const Matrix4x4& viewProjectionMatrix = pCamera_->GetViewProjectionMatrix();
		worldViewProjectionMatrix_ = MyMath::Multiply(worldMatrix, viewProjectionMatrix);
	} else {
		worldViewProjectionMatrix_ = worldMatrix;
	}

	transformationMatrixData_->WVP = worldViewProjectionMatrix_;
	transformationMatrixData_->World = worldMatrix;
}
