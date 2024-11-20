#pragma once
#include <Windows.h>
#include <imgui.h>
#include <imgui_impl_dx12.h>
#include <imgui_impl_win32.h>
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

#include "DirectXCommon.h"
#include "WinApp.h"
#include "SrvManager.h"

/// <summary>
/// ImGuiを管理するクラス(仮実装。9章で変更予定)
/// </summary>
class ImGuiManager
{
private:// シングルトン設計
	static ImGuiManager* instance;

	/// <summary>
	/// コンストラクタ、デストラクタの隠蔽
	/// </summary>
	ImGuiManager() = default;
	~ImGuiManager() = default;

	/// <summary>
	/// コピーコンストラクタの封印
	/// </summary>
	/// <param name=""></param>
	ImGuiManager(ImGuiManager&) = delete;

	/// <summary>
	/// コピー代入演算の封印
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	ImGuiManager& operator=(ImGuiManager&) = delete;
public:
	//-------------基本処理-------------//

	/// <summary>
	///	シングルトンインスタンスの取得
	/// </summary>
	/// <returns></returns>
	static ImGuiManager* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="dxCommon"></param>
	/// <param name="winApp"></param>
	void Initialize(DirectXCommon* dxCommon, WinApp* winApp);

	/// <summary>
	/// フレームが始まる旨を告げる
	/// </summary>
	void Begin();

	/// <summary>
	/// 実際に開発用UIを出す
	/// </summary>
	void UpdateGameUI();

	/// <summary>
	/// ImGuiの内部コマンドを生成する
	/// </summary>
	void End();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();

private:

	//-------------メンバ変数-------------//

	// Initializeの引数で借りてくる
	DirectXCommon* dxCommon_ = nullptr;
	WinApp* winApp_ = nullptr;

};

