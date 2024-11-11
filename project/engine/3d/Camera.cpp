#include "Camera.h"
#include "WinApp.h"
#include "ImGuiManager.h"

Camera::Camera()
	: transform_({ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} })
	,fovY_(0.45f)
	,aspectRatio_(float(WinApp::kClientWidth)/float(WinApp::kClientHeight))
	,nearClip_(0.1f)
	,farClip_(100.0f)
	,worldMatrix_(MyMath::MakeAffineMatrix(transform_.scale,transform_.rotate,transform_.translate))
	,viewMatrix_(MyMath::Inverse(worldMatrix_))
	,projectionMatrix_(MyMath::MakePerspectiveFovMatrix(fovY_,aspectRatio_,nearClip_,farClip_))
	,viewProjectionMatrix_(MyMath::Multiply(viewMatrix_,projectionMatrix_))
{}

void Camera::Update()
{

	// transformからアフィン変換行列を計算
	worldMatrix_ = MyMath::MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);

	// worldMatrixの逆行列
	viewMatrix_ = MyMath::Inverse(worldMatrix_);

	// プロジェクション行列計算
	projectionMatrix_ = MyMath::MakePerspectiveFovMatrix(fovY_, aspectRatio_, nearClip_, farClip_);

	// 合成行列
	viewProjectionMatrix_ = MyMath::Multiply(viewMatrix_, projectionMatrix_);

}

void Camera::ImGui()
{
#ifdef _DEBUG
	ImGui::Begin("Camera");
	ImGui::DragFloat3("translate", &transform_.translate.x, 0.1f);
	ImGui::DragFloat3("rotate", &transform_.rotate.x, 0.05f);
	ImGui::End();
#endif //_DEBUG
}
