#include "Input.h"
#include "WinApp.h"
#include "DirectXCommon.h"
#include "ImGuiManager.h"
#include "SpriteCommon.h"
#include "Sprite.h"
#include "TextureManager.h"
#include "Object3dCommon.h"
#include "Object3d.h"
#include "ModelCommon.h"
#include "Model.h"

#include "Audio.h"

#include "Logger.h"
#include "StringUtility.h"
#include "D3DResourceLeakChecker.h"
#include "MyMath.h"

//-------------------------------------
//DepthStencilTextureを作る
//-------------------------------------

Microsoft::WRL::ComPtr<ID3D12Resource> CreateDepthStencilTextureResource(Microsoft::WRL::ComPtr<ID3D12Device> device, const int32_t width,const uint32_t height) {
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = width;//Textureの幅
	resourceDesc.Height = height;//Textureの高さ
	resourceDesc.MipLevels = 1;//mipmapの数
	resourceDesc.DepthOrArraySize = 1;//奥行きor配列Textureの配列数
	resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;//DepthStencilとして利用可能なフォーマット
	resourceDesc.SampleDesc.Count = 1;//サンプリングカウント。1固定。
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;//２次元
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;//DepthStencilとして使う通知
	
	//利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;//VRAM上に作る

	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;//1.0f(最大値)でクリア
	depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;//フォーマット。Resourceと合わせる

	Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,//Heapの設定
		D3D12_HEAP_FLAG_NONE,//Heapの特殊な設定。特になし
		&resourceDesc, //Resourceの設定
		D3D12_RESOURCE_STATE_DEPTH_WRITE,//深度値を書き込む状態にしておく
		&depthClearValue, //Clear最適値
		IID_PPV_ARGS(&resource)//作成するResourceポインタへのポインタ
		);
	assert(SUCCEEDED(hr));

	return resource;
}

//-------------------------------------
//特定のインデックスのDescriptorHandleを取得する
//-------------------------------------
//CPUver.
D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap,const uint32_t descriptorSize,const uint32_t index) {
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriptorSize * index);
	return handleCPU;
}

//GPUver.
D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap,const uint32_t descriptorSize,const uint32_t index) {
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += (descriptorSize * index);
	return handleGPU;
}

//-------------------------------------
//main関数
//-------------------------------------

//windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	//ポインタ
	D3DResourceLeakChecker* pLeakChecke = nullptr;
	WinApp* pWinApp = nullptr;
	Input* pInput = nullptr;
	DirectXCommon* pDxCommon = nullptr;
	Audio* pAudio = nullptr;
	ImGuiManager* pImguiManager = nullptr;
	SpriteCommon* pSpriteCommon = nullptr;	
	std::vector<Sprite*> pSprites;
	Object3dCommon* pObject3dCommon = nullptr;
	std::vector<Object3d*> pObjects3d;
	ModelCommon* pModelCommon = nullptr;
	Model* pModel = nullptr;

#pragma region 基盤システムの初期化

	//-------------------------------------
	// リークチェッカー
	//-------------------------------------

	pLeakChecke->~D3DResourceLeakChecker();

	//-------------------------------------
	// WinAppAPIの初期化
	//-------------------------------------

	pWinApp = new WinApp();
	pWinApp->Initialize();

	//-------------------------------------
	// 入力の初期化
	//-------------------------------------

	pInput = new Input();
	pInput->Initialize(pWinApp);

	//-------------------------------------
	// DirectXの初期化
	//-------------------------------------

	pDxCommon = new DirectXCommon();
	pDxCommon->Initialize(pWinApp);

	//-------------------------------------
	// テクスチャマネージャの初期化
	//-------------------------------------

	TextureManager::GetInstance()->Initialize(pDxCommon);
	TextureManager::GetInstance()->LoadTexture("resource/uvChecker.png");
	TextureManager::GetInstance()->LoadTexture("resource/monsterBall.png");
	
	//-------------------------------------
	// Audioの初期化
	//-------------------------------------

	pAudio = new Audio();
	pAudio->InitXAudio2();

	//-------------------------------------
	// ImGuiの初期化
	//-------------------------------------

	pImguiManager = new ImGuiManager();
	pImguiManager->Initialize(pDxCommon, pWinApp);

	//-------------------------------------
	// スプライト共通部の初期化
	//-------------------------------------

	pSpriteCommon = new SpriteCommon();
	pSpriteCommon->Initialize(pDxCommon);

	//-------------------------------------
	// スプライトの初期化
	//-------------------------------------

	for (uint32_t i = 0; i < 1; ++i) {
		Sprite* pSprite = new Sprite();
		// スプライトごとに異なるテクスチャを割り当てる
		std::string texturePath;
		if (i == 0) {
			texturePath = "resource/monsterBall.png";
		} else if (i == 1) {
			texturePath = "resource/uvChecker.png";
		} else {
			texturePath = "resource/monsterBall.png";
		}


		pSprite->Initialize(pSpriteCommon, texturePath);

		Vector2 position = pSprite->GetPosition();
		position = Vector2(i * 300.0f, i + 50.0f);
		pSprite->SetPosition(position);

		pSprites.push_back(pSprite);
	}

	//-------------------------------------
	// 3dオブジェクト共通部の初期化
	//-------------------------------------

	pObject3dCommon = new Object3dCommon();
	pObject3dCommon->Initialize(pDxCommon);

	//-------------------------------------
	// 3dオブジェクトの初期化
	//-------------------------------------

	for (uint32_t i = 0; i < 2; ++i) {
		Object3d* pObject3d = new Object3d();

		pObject3d->Initialize(pObject3dCommon);

		Vector3 translate = pObject3d->GetTranslate();
		translate = Vector3(i * 1.0f, i + 1.0f);
		pObject3d->SetTranslate(translate);

		pObjects3d.push_back(pObject3d);
	}

	//-------------------------------------
	// 3dモデルの共通部の初期化
	//-------------------------------------
	
	pModelCommon = new ModelCommon();
	pModelCommon->Initialize(pDxCommon);

	//-------------------------------------
	// 3dモデルの初期化
	//-------------------------------------

	pModel = new Model();
	pModel->Initialize(pModelCommon);

#pragma endregion 基盤システムの初期化

#ifdef _DEBUG

	//-------------------------------------
	//エラーと警告の抑制
	//-------------------------------------

	Microsoft::WRL::ComPtr<ID3D12InfoQueue> infoQueue = nullptr;
	if (SUCCEEDED(pDxCommon->GetDevice()->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
		//やばいエラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		//エラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		//警告時に止まる
		//infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

		D3D12_MESSAGE_ID denyIds[] = {
			//Windows11でのDXGIデバッグレイヤーとDX12デバッグレイヤーの相互作用バグによるエラーメッセージ
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
		};

		//抑制するレベル
		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		D3D12_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;
		//指定したメッセージの表示を抑制する
		infoQueue->PushStorageFilter(&filter);

	}
#endif
	for (uint32_t i = 0; i < 2; i++) {
		pObjects3d[i]->SetModel(pModel);
	}

	while (true) {
		//Windowのメッセージ処理
		if (pWinApp->ProcessMessage()) {
			//ゲームループを抜ける
			break;
		}

		//-------------------------------------
		//フレームの始まる旨を告げる
		//-------------------------------------

		pImguiManager->Begin();

		//-------------------------------------
		// 入力処理の更新
		//-------------------------------------

		pInput->Update();
		
		//-------------------------------------
		// スプライトの更新
		//-------------------------------------
		
		for (uint32_t i = 0; i < pSprites.size(); ++i) {
			pSprites[i]->Update();
			float rotation = pSprites[i]->GetRotation();
			rotation += 0.01f;
			pSprites[i]->SetRotation(rotation);
		}

		//-------------------------------------
		// 3dオブジェクトの更新
		//-------------------------------------

		for (uint32_t i = 0; i < 2; i++) {
			pObjects3d[i]->Update();
		}

		//-------------------------------------
		// ゲームの更新処理でパラメータを変更したいタイミングでImGuiの処理を行う
		//-------------------------------------
		//開発用UIの処理。実際に開発用のUIを出す場合はここをゲーム固有の処理に置き換える
		
		pImguiManager->UpdateGameUI();
		pSpriteCommon->ImGui();

		for (uint32_t i = 0; i < pSprites.size(); ++i) {
			pSprites[i]->ImGui();
		}
		
		//-------------------------------------
		// 描画前処理
		//-------------------------------------

		// DirectXの描画準備。すべての描画に共通のグラフィックスコマンドを積む
		pDxCommon->PreDraw();

		//-------------------------------------
		// スプライト描画準備
		//-------------------------------------

		// Spriteの描画準備。Spriteの描画に共通のグラフィックスコマンドを積む
		pSpriteCommon->SetDraw();

		//-------------------------------------
		// Sprite個々の描画
		//-------------------------------------
		
		for (uint32_t i = 0; i < pSprites.size(); ++i) {
			pSprites[i]->Draw();
		}

		//-------------------------------------
		// 3dオブジェクト描画準備
		//-------------------------------------

		// 3dオブジェクトの描画準備。3dオブジェクトの描画に共通のグラフィックスコマンドを積む
		pObject3dCommon->SetDraw();

		//-------------------------------------
		// 3dオブジェクト個々の描画
		//-------------------------------------

		for (uint32_t i = 0; i < 2; i++) {
			pObjects3d[i]->Draw();
		}

		//-------------------------------------
		//ゲームの処理が終わり描画処理に入る前に、ImGuiの内部コマンドを生成する
		//-------------------------------------

		pImguiManager->End();

		//-------------------------------------
		// 画面表示できるようにする
		//-------------------------------------

		pImguiManager->Draw();
		
		//-------------------------------------
		// 描画後処理
		//-------------------------------------

		pDxCommon->PostDraw();
		
	}

	//-------------------------------------
    // COMの終了処理
    //-------------------------------------

	CoUninitialize();

	//-------------------------------------
	// テクスチャマネージャの終了処理
	//-------------------------------------

	TextureManager::GetInstance()->Finalize();

    //-------------------------------------
	// ImGuiの終了処理
	//-------------------------------------
	
	pImguiManager->Finalize();

	//-------------------------------------
	// Audioクラスの後始末
	//-------------------------------------

	pAudio->ResetXAudio2();

	//-------------------------------------
	// WindowsAPIの終了処理
	//-------------------------------------
	
	pWinApp->Finalize();

	//-------------------------------------
    // 解放処理
    //-------------------------------------

	
	delete pModel;
	delete pModelCommon;
	for (uint32_t i = 0; i < 2; i++) {
		delete pObjects3d[i];
	}
	delete pObject3dCommon;
	for (uint32_t i = 0; i < pSprites.size(); ++i) {
		delete pSprites[i];
	}
	delete pSpriteCommon;
	delete pImguiManager;
	delete pAudio;
	delete pDxCommon;
	delete pInput;
	delete pWinApp;

	return 0;
}