
#include "DirectXCommon.h"
#include <cassert>

#include "Logger.h"
#include "StringUtilty.h"
#include <format>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxcompiler.lib")

using namespace Microsoft::WRL;

void DirectXCommon::Initialize(WinApp* winApp)
{
	//NULL検出
	assert(winApp);
	//メンバ変数に記録
	this->winApp = winApp;

	//デバイス生成
	InitializeDevice();
	//コマンド関連の生成
	InitializeCommand();
	//スワップチェーンの生成
	InitializeSwapChain();
	//深度バッファの生成
	InitialzeDepthBuffer();
	//各種デスクリプタヒープの生成
	InitializeDescriptorHeap();
	//レンダーターゲットビューの初期化
	InitializeRenderTargetView();
	//深度ステンシルビューの初期化
	InitializeDepthStencilView();
	//フェンスの生成
	InitializeFence();
	//ビューポート矩形の初期化
	InitializeViewPort();
	//シザリング矩形の初期化
	InitializeScissorRect();
	//DCXコンパイラの生成
	InitializeDXCCompiler();
	//ImGuiの初期化
	InitializeImGui();
}

D3D12_CPU_DESCRIPTOR_HANDLE DirectXCommon::GetSRVCPUDescriptorHandle(uint32_t index)
{
	return GetCPUDescriptorHandle(srvDescriptorHeap, descriptorSizeSRV, index);
}

D3D12_GPU_DESCRIPTOR_HANDLE DirectXCommon::GetSRVGPUDescriptorHandel(uint32_t index)
{
	return GetGPUDescriptorHandle(srvDescriptorHeap, descriptorSizeSRV, index);
}

void DirectXCommon::InitializeDevice()
{

	HRESULT result{};

#ifdef _DEBUG


	//-------------------------------------
	//デバッグレイヤー
	//-------------------------------------

	Microsoft::WRL::ComPtr<ID3D12Debug1> debugController = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		//デバッグレイヤーを有効化する
		debugController->EnableDebugLayer();
		//さらにGPU側でもチェックを行うようにする
		debugController->SetEnableGPUBasedValidation(TRUE);
	}


#endif

	//-------------------------------------
	//DXGIFactoryの生成
	//-------------------------------------

	result = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));

	assert(SUCCEEDED(result));

	//使用するアダプタ（GPU）用の変数。最初にnullptrを入れておく
	Microsoft::WRL::ComPtr<IDXGIAdapter4> useAdapter = nullptr;
	//良い順にアダプタを読む
	for (UINT i = 0; dxgiFactory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter)) != DXGI_ERROR_NOT_FOUND; ++i) {
		//アダプタの情報を取得する
		DXGI_ADAPTER_DESC3 adapterDesc{};
		result = useAdapter->GetDesc3(&adapterDesc);
		assert(SUCCEEDED(result));   //取得できないのは一大事
		//ソフトウェアアダプタでなければ採用！
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			//採用したアダプタの情報をログに出力、wstringの方なので注意
			Logger::Log(StringUtility::ConvertString(std::format(L"USE Adapter:{}\n", adapterDesc.Description))); //???
			break;
		}
		useAdapter = nullptr; //ソフトウェアアダプタの場合は見なかったことにする
	}
	//適切なアダプタが見つからなかったので起動できない
	assert(useAdapter != nullptr);

	//機能レベルとログ出力用の文字列
	D3D_FEATURE_LEVEL featureLevels[] = {
	D3D_FEATURE_LEVEL_12_2,D3D_FEATURE_LEVEL_12_1,D3D_FEATURE_LEVEL_12_0
	};
	const char* featureLevelStrings[] = { "12.2","12.1",{"12.0"} };
	//高い順に生成できるか試す
	for (size_t i = 0; i < _countof(featureLevels); ++i) {
		//採用したアダプタでデバイスを生成
		result = D3D12CreateDevice(useAdapter.Get(), featureLevels[i], IID_PPV_ARGS(&device));
		//指定した機能レベルでデバイスが生成できたかを確認
		if (SUCCEEDED(result)) {
			//生成できたのでログ出力を行ってループを抜ける
			Logger::Log(std::format("FeatureLevel:{}\n", featureLevelStrings[i]));
			break;
		}
	}

	//デバイスの生成がうまくいかなかったので起動できない
	assert(device != nullptr);
	Logger::Log("Complete createD3D12Device!!!\n"); //初期化完了のログを出す
}

void DirectXCommon::InitializeCommand()
{
	HRESULT result{};

	//-------------------------------------
	//コマンドアロケータを生成する(命令保存用もメモリー）
	//-------------------------------------

	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator = nullptr;
	result = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));
	//コマンドアロケータの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(result));

	//-------------------------------------
	//コマンドリストを生成する
	//-------------------------------------

	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList = nullptr;
	result = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList));
	//コマンドリストの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(result));

	//-------------------------------------
	//コマンドキューを生成する	
	//-------------------------------------

	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue = nullptr;
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	result = device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue));
	//コマンドキューの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(result));

}

void DirectXCommon::InitializeSwapChain()
{
	HRESULT result{};

	//-------------------------------------
	//スワップチェーンを生成する
	//-------------------------------------

	//画面の幅。ウィンドウのクライアント領域を同じものにしておく
	swapChainDesc.Width = WinApp::kClientWidth;
	// 画面の高さ。ウィンドウのクライアント領域を同じものにしておく
	swapChainDesc.Height = WinApp::kClientHeight;
	//色の形式
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	//マルチサンプルしない
	swapChainDesc.SampleDesc.Count = 1;
	//描画のターゲットとして利用する
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	//ダブルバッファ
	swapChainDesc.BufferCount = 2;
	//モニターにうつしたら、中身を破棄
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	//コマンドキュー、ウィンドウハンドル、設定を渡して生成する
	result = dxgiFactory->CreateSwapChainForHwnd(commandQueue.Get(), winApp->GetHwnd(), &swapChainDesc, nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(swapChain.GetAddressOf()));
	assert(SUCCEEDED(result));
}

void DirectXCommon::InitialzeDepthBuffer()
{
	//深度バッファリソースを設定
	depthStencilResource = CreateDepthStencilTextureResource(WinApp::kClientWidth, WinApp::kClientHeight);
}

ComPtr<ID3D12Resource> DirectXCommon::CreateDepthStencilTextureResource(const int32_t width, const uint32_t height)
{

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

	//深度置のクリア設定
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;//1.0f(最大値)でクリア
	depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;//フォーマット。Resourceと合わせる

	//Resourceの生成
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

void DirectXCommon::InitializeDescriptorHeap()
{

	HRESULT result{};

	//-------------------------------------
	//DescriptorSizeを取得しておく
	//-------------------------------------
	descriptorSizeSRV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	descriptorSizeRTV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	descriptorSizeDSV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	//-------------------------------------
	//RTV用のデスクリプタヒープの生成
	//-------------------------------------

	//RTV用のヒープでデスクリプタの数は2。RTVはShader内で触るものではないので、ShaderVisibleはFalse
	rtvDescriptorHeap = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);

	//-------------------------------------
	//SRV用のデスクリプタヒープの生成
	//-------------------------------------

	//SRV用のヒープでデスクリプタの数は128。SRVはShader内で触るものなので、ShaderVisibleはTrue
	srvDescriptorHeap = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true);

	//-------------------------------------
	//DSV用のデスクリプタヒープの生成
	//-------------------------------------

	//DSV用のヒープでディスクリプタヒープの数は1。DSVはShader内で触るものではないので、ShaderVisibleはfalse
	dsvDescriptorHeap = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);
}

ComPtr<ID3D12DescriptorHeap> DirectXCommon::CreateDescriptorHeap(const D3D12_DESCRIPTOR_HEAP_TYPE& heapType, const UINT& numDescriptors, bool shaderVisible)
{
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.Type = heapType;
	descriptorHeapDesc.NumDescriptors = numDescriptors;
	descriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	HRESULT hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));
	assert(SUCCEEDED(hr));
	return descriptorHeap;
}

void DirectXCommon::InitializeRenderTargetView()
{
	HRESULT result{};

	//-------------------------------------
	//SwapChainからResourceを引っ張ってくる
	//-------------------------------------

	swapChainResources[2] = { nullptr };
	result = swapChain->GetBuffer(0, IID_PPV_ARGS(&swapChainResources[0]));
	//うまく取得出来なければ起動できない
	assert(SUCCEEDED(result));
	result = swapChain->GetBuffer(1, IID_PPV_ARGS(&swapChainResources[1]));
	assert(SUCCEEDED(result));

	//-------------------------------------
	//RTVを作る
	//-------------------------------------

	//RTVの設定
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;  //出力結果をSRGBに変換して書き込む
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;  //2dテクスチャとして書き込む
	//ディスクリプタの先頭を取得する
	D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle = GetCPUDescriptorHandle(rtvDescriptorHeap, descriptorSizeRTV, 0);
	
	//まずは１つ目を作る。１つ目は最初のところに作る。作る場所はこちらで指定してあげる必要がある
	rtvHandles[0] = rtvStartHandle;
	device->CreateRenderTargetView(swapChainResources[0].Get(), &rtvDesc, rtvHandles[0]);
	//２つ目のディスクラプタハンドルを得る（自力で）
	rtvHandles[1].ptr = rtvHandles[0].ptr + device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	//２つ目を作る
	device->CreateRenderTargetView(swapChainResources[1].Get(), &rtvDesc, rtvHandles[1]);
}

D3D12_CPU_DESCRIPTOR_HANDLE DirectXCommon::GetCPUDescriptorHandle(ComPtr<ID3D12DescriptorHeap> descriptorHeap, const uint32_t descriptorSize, const uint32_t index)
{
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriptorSize * index);
	return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE DirectXCommon::GetGPUDescriptorHandle(ComPtr<ID3D12DescriptorHeap> descriptorHeap, const uint32_t descriptorSize, const uint32_t index)
{
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += (descriptorSize * index);
	return handleGPU;
}

void DirectXCommon::InitializeDepthStencilView()
{
	//-------------------------------------
	//DSVを作る
	//-------------------------------------

	//DEVの設定
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;//Format。基本的にはResourceに合わせる
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;//2dTexture
	//DSVHeapの先頭にDSVを作る
	device->CreateDepthStencilView(depthStencilResource.Get(), &dsvDesc, dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
}

void DirectXCommon::InitializeFence()
{
	HRESULT result{};

	//-------------------------------------
	//FenceとEventを作成する
	//-------------------------------------

	uint64_t fenceValue = 0;

	result = device->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
	assert(SUCCEEDED(result));

	//FenceのSignalを待つためのイベントを作成する
	HANDLE fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(fenceEvent != nullptr);
}

void DirectXCommon::InitializeViewPort()
{
	//クライアント領域のサイズと一緒にして画面全体に表示
	viewport.Width = WinApp::kClientWidth;
	viewport.Height = WinApp::kClientHeight;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
}

void DirectXCommon::InitializeScissorRect()
{
	//基本的にビューポートと同じ矩形が構成されるようにする
	scissorRect.left = 0;
	scissorRect.right = WinApp::kClientWidth;
	scissorRect.top = 0;
	scissorRect.bottom = WinApp::kClientHeight;
}

void DirectXCommon::InitializeDXCCompiler()
{
	HRESULT result{};

	//-------------------------------------
	//DXCの初期化
	//-------------------------------------

	//dxcCompilerを初期化
	result = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
	assert(SUCCEEDED(result));
	result = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler));
	assert(SUCCEEDED(result));

	//現時点でincludeはしないが、includeに対応するための設定を行っておく
	result = dxcUtils->CreateDefaultIncludeHandler(&includeHandler);
	assert(SUCCEEDED(result));
}

void DirectXCommon::InitializeImGui()
{
	//-------------------------------------
	//ImGuiの初期化
	//-------------------------------------
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(winApp->GetHwnd());
	ImGui_ImplDX12_Init(device.Get(), swapChainDesc.BufferCount, rtvDesc.Format, srvDescriptorHeap.Get(), srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());

}
