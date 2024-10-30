#include <Windows.h>
#include <cstdint>
#include <string>
#include <format>
#define _USE_MATH_DEFINES
#include <math.h>

#include <d3d12.h>
#pragma comment(lib,"d3d12.lib")
#include <dxgi1_6.h>
#pragma comment(lib,"dxgi.lib")
#include <cassert>
#include <dxgidebug.h>
#pragma comment(lib,"dxguid.lib")

#include <dxcapi.h>
#pragma comment(lib,"dxcompiler.lib")

#include "externals/DirectXTex/DirectXTex.h"

#include <fstream>
#include <sstream>

#include <wrl.h>

#include "Input.h"
#include "WinApp.h"
#include "DirectXCommon.h"
#include "ImGuiManager.h"
#include "SpriteCommon.h"
#include "Sprite.h"
#include "TextureManager.h"
#include "Audio.h"

#include "Logger.h"
#include "StringUtility.h"
#include "D3DResourceLeakChecker.h"
#include "MyMath.h"


//平行光源
typedef struct {
	Vector4 color;//ライトの色
	Vector3 direction;//ライトの向き
	float intensity;//輝度
}DirectionalLight;

//MaterialData構造体
typedef struct {
	std::string textureFilePath;
}MaterialData;

//ModelData構造体
typedef struct {
	std::vector<VertexData> vertices;
	MaterialData material;
}ModelData;

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
//mtlファイルを読み込む関数
//-------------------------------------

MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename) {
	//1.中で必要になる変数の宣言
	//構築するマテリアルデータ
	MaterialData materialData;
	//ファイルから読んだ1行を格納するもの
	std::string line;

	//2.ファイルを開く
	std::ifstream file(directoryPath + "/" + filename);
	//とりあえず開けなかったら止める
	assert(file.is_open());

	//3.実際にファイルを読み、MaterialDataを構築していく
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

//-------------------------------------
//objファイルを読み込む関数
//-------------------------------------

ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename) {
	//1.必要となる変数の宣言
	ModelData modelData;;//構築するModelData
	std::vector<Vector4> positions;//位置
	std::vector<Vector3> normals;//法線
	std::vector<Vector2> texcoords;//テクスチャの座標
	std::string line;//ファイルから読んだ一行を格納するもの

	//2/ファイルを開く
	std::ifstream file(directoryPath + "/" + filename);
	assert(file.is_open());//開けなかったら止める

	//3.ファイルを読み、ModelDataを構築
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
		} else if(identifier=="vt") {
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

	for (uint32_t i = 0; i < 3; ++i) {
		Sprite* pSprite = new Sprite();
		pSprite->Initialize(pSpriteCommon, pDxCommon, "resource/uvChecker.png");

		Vector2 position = pSprite->GetPosition();
		position = Vector2(i * 300.0f, i + 50.0f);
		pSprite->SetPosition(position);

		pSprites.push_back(pSprite);
	}

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

	//-------------------------------------
	//ModelDataを使ったResourceの作成
	//-------------------------------------

	////モデル読み込み
	//ModelData modelData = LoadObjFile("resource", "axis.obj");
	////頂点リソースを作る
	//Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource = dxCommon->CreateBufferResource(sizeof(VertexData) * modelData.vertices.size());
	////頂点バッファビューを作成する
	//D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	////リソースの先頭アドレスから使う
	//vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	////使用するリソースのサイズは頂点のサイズ
	//vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * modelData.vertices.size());
	////１頂点当たりのサイズ
	//vertexBufferView.StrideInBytes = sizeof(VertexData);

	////頂点リソースにデータを書き込む
	//VertexData* vertexData = nullptr;
	////書き込むためのアドレスを取得
	//vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	////頂点データをリソースにコピー
	//std::memcpy(vertexData, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size());

	//-------------------------------------
	//平行光源用のResourceを作成
	//-------------------------------------

	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource = pDxCommon->CreateBufferResource(sizeof(DirectionalLight));
	DirectionalLight* directionalLightData = nullptr;
	directionalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));
	//デフォルトの値は以下にしておく
	directionalLightData->color = { 1.0f,1.0f,1.0f };
	directionalLightData->direction = MyMath::Normalize({ 0.0f,-1.0f,0.0f });
	directionalLightData->intensity = 1.0f;

	//テクスチャ切り替え用のbool変数
	bool useMonsterBall = true;

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
		//ゲームの更新処理でパラメータを変更したいタイミングでImGuiの処理を行う
		//-------------------------------------
		//開発用UIの処理。実際に開発用のUIを出す場合はここをゲーム固有の処理に置き換える
		
		pImguiManager->UpdateGameUI();
		pSpriteCommon->ImGui();

		for (uint32_t i = 0; i < pSprites.size(); ++i) {
			pSprites[i]->ImGui();
		}
	
		//-------------------------------------
		// ライトの向きを正規化
		//-------------------------------------

		directionalLightData->direction = MyMath::Normalize(directionalLightData->direction);
		
		//-------------------------------------
		// 描画前処理
		//-------------------------------------

		// DirectXの描画準備。すべての描画に共通のグラフィックスコマンドを積む
		pDxCommon->PreDraw();

		// Spriteの描画準備。Spriteの描画に共通のグラフィックスコマンドを積む
		pSpriteCommon->SetCommonDrawing();

		//-------------------------------------
		// Sprite個々の描画
		//-------------------------------------
		
		for (uint32_t i = 0; i < pSprites.size(); ++i) {
			pSprites[i]->Draw(directionalLightResource);
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