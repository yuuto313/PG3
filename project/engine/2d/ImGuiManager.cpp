#include "ImGuiManager.h"

ImGuiManager* ImGuiManager::instance = nullptr;

ImGuiManager* ImGuiManager::GetInstance()
{
#ifdef  _DEBUG
	if (instance == nullptr) {
		instance = new ImGuiManager();
	}
#endif //  _DEBUG
	return instance;
}

void ImGuiManager::Initialize(DirectXCommon* dxCommon,WinApp* winApp)
{
#ifdef  _DEBUG
	// 引数で受け取ってメンバ変数に記録する
	dxCommon_ = dxCommon;
	winApp_ = winApp;

	// ImGuiの初期
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsClassic();
	ImGui_ImplWin32_Init(winApp_->GetHwnd());
	ImGui_ImplDX12_Init(dxCommon_->GetDevice(), dxCommon_->GetBackBufferCount(), dxCommon_->GetRTVFormat(), SrvManager::GetInstance()->GetSrvDescriptorHeap(), SrvManager::GetInstance()->GetSrvDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(), SrvManager::GetInstance()->GetSrvDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());
#endif //  _DEBUG
}

void ImGuiManager::Begin()
{
#ifdef _DEBUG
	// ImGuiフレーム開始
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
#endif // _DEBUG
}

void ImGuiManager::UpdateGameUI()
{
#ifdef _DEBUG
	// 開発用UIの処理
	ImGui::ShowDemoWindow();
#endif // _DEBUG
}

void ImGuiManager::End()
{
#ifdef _DEBUG
	// 描画前準備
	ImGui::Render(); 
#endif // _DEBUG
}


void ImGuiManager::Draw()
{
#ifdef _DEBUG
	// 実際のcommandListのImGuiの描画コマンドを積む
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dxCommon_->GetCommandList());
#endif // _DEBUG
}

void ImGuiManager::Finalize()
{
#ifdef _DEBUG
	// ImGuiの終了処理
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
#endif // _DEBUG

	delete instance;
	instance = nullptr;

}
