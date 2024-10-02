#include "DirectXCommon.h"
#include <cassert>
#include <format>

#include "Logger.h"
#include "StringUtility.h"

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxcompiler.lib")

using namespace Microsoft::WRL;

void DirectXCommon::Initialize(WinApp* winApp)
{
	// NULL検出
	assert(winApp);
	// メンバ変数に記録
	winApp_ = winApp;

	// デバイス生成
	InitializeDXGIDevice();

	// コマンド関連の生成
	InitializeCommand();
	// スワップチェーンの生成
	CreateSwapChain();

	// 深度バッファの生成
	CreateDepthBuffer();
	// 各種デスクリプタヒープの生成
	InitializeDescriptorHeap();
	// レンダーターゲットビューの初期化
	InitializeRenderTargetView();
	// 深度ステンシルビューの初期化
	InitializeDepthStencilView();
	// フェンスの生成
	CreateFence();
	// ビューポート矩形の初期化
	InitializeViewPort();
	// シザリング矩形の初期化
	InitializeScissorRect();
	// DCXコンパイラの生成
	InitializeDXCCompiler();
	// ImGuiの初期化
	InitializeImGui();
}

void DirectXCommon::PreDraw()
{
	//--------------------------------
	// バックバッファの番号取得
	//--------------------------------

	// これから書き込むバックバッファのインデックスを取得
	UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();

	//--------------------------------
	// リソースバリアで書き込み可能に変更
	//--------------------------------

	D3D12_RESOURCE_BARRIER barrier{};

	// 今回のバリアはTransition
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	// Noneにしておく
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	// バリアを張る対象のリソース。現在のバックバッファに対して行う
	barrier.Transition.pResource = swapChainResources_[backBufferIndex].Get();
	// 遷移前（現在）の	ResourceState
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	// 遷移後のResourceState
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	// TransitionBarrierを張る
	commandList_->ResourceBarrier(1, &barrier);

	//--------------------------------
	// 描画先のRTVとDSVを指定する
	//--------------------------------

	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();

	// 描画先のRTVとDSVを指定する
	commandList_->OMSetRenderTargets(1, &rtvHandles_[backBufferIndex], false, &dsvHandle);

	//--------------------------------
	// 画面全体の色をクリア
	//--------------------------------

	// 指定した色で画面全体をクリアする
	float clearColor[] = { 0.1f,0.25f,0.5f,1.0f };//青っぽい色。RGBAの順
	commandList_->ClearRenderTargetView(rtvHandles_[backBufferIndex], clearColor, 0, nullptr);

	//--------------------------------
	// 画面全体の深度をクリア
	//--------------------------------
	
	commandList_->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	//--------------------------------
	// SRV用のデスクリプタヒープを指定する
	//--------------------------------

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeaps[] = { srvDescriptorHeap_.Get() };
	commandList_->SetDescriptorHeaps(1, descriptorHeaps->GetAddressOf());

	//--------------------------------
	// ビューポート領域の設定
	//--------------------------------

	commandList_->RSSetViewports(1, &viewport_);

	//--------------------------------
	// シザー矩形の設定
	//--------------------------------

	commandList_->RSSetScissorRects(1, &scissorRect_);

}

void DirectXCommon::PostDraw()
{
	//--------------------------------
	// バックバッファの番号取得
	//--------------------------------

	UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();

	//--------------------------------
	// リソースバリアで表示状態に変更
	//--------------------------------

	D3D12_RESOURCE_BARRIER barrier{};

	// PreDrawと同じものを設定
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = swapChainResources_[backBufferIndex].Get();
	// 画面に描く処理はすべて終わり、画面に映すので状態を遷移
	// 今回はRenderTargetからPresentにする
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	// TransitionBarrierを張る
	commandList_->ResourceBarrier(1, &barrier);

	//--------------------------------
	// グラフィックスコマンドをクローズ
	//--------------------------------

	HRESULT hr = commandList_->Close();
	assert(SUCCEEDED(hr));

	//--------------------------------
	// GPUコマンドの実行
	//--------------------------------

	Microsoft::WRL::ComPtr<ID3D12CommandList> commandLists[] = { commandList_ };
	commandQueue_->ExecuteCommandLists(1, commandLists->GetAddressOf());

	//--------------------------------
	// GPU画面の交換を通知
	//--------------------------------

	// GPUとOSに画面の交換を行うよう通知する
	swapChain_->Present(1, 0);

	//--------------------------------
	// Fenceの値を更新
	//--------------------------------

	// Fenceの値を更新
	fenceVal_++;

	//--------------------------------
	// コマンドキューにシグナルを送る
	//--------------------------------
	
	// GPUがここまでたどり着いたときに、Fenceの値を指定した値に記入するようにSignalを送る
	commandQueue_->Signal(fence_.Get(), fenceVal_);

	//--------------------------------
	// コマンド完了待ち
	//--------------------------------

	if (fence_->GetCompletedValue() < fenceVal_) {
		// 指定したSignalにたどり着いていないので、たどり着くまで待つようにイベントを設定する
		fence_->SetEventOnCompletion(fenceVal_, fenceEvent_);
		// イベントを待つ
		WaitForSingleObject(fenceEvent_, INFINITE);
	}

	//--------------------------------
	// コマンドアロケーターのリセット
	//--------------------------------

	hr = commandAllocator_->Reset();
	assert(SUCCEEDED(hr));

	//--------------------------------
	// コマンドリストのリセット
	//--------------------------------

	hr = commandList_->Reset(commandAllocator_.Get(), nullptr);
	assert(SUCCEEDED(hr));

}

D3D12_CPU_DESCRIPTOR_HANDLE DirectXCommon::GetSRVCPUDescriptorHandle(uint32_t index)
{
	return GetCPUDescriptorHandle(srvDescriptorHeap_, descriptorSizeSRV_, index);
}

D3D12_GPU_DESCRIPTOR_HANDLE DirectXCommon::GetSRVGPUDescriptorHandel(uint32_t index)
{
	return GetGPUDescriptorHandle(srvDescriptorHeap_, descriptorSizeSRV_, index);
}

void DirectXCommon::InitializeDXGIDevice()
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

	result = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));

	assert(SUCCEEDED(result));

	//使用するアダプタ（GPU）用の変数。最初にnullptrを入れておく
	Microsoft::WRL::ComPtr<IDXGIAdapter4> useAdapter = nullptr;
	//良い順にアダプタを読む
	for (UINT i = 0; dxgiFactory_->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter)) != DXGI_ERROR_NOT_FOUND; ++i) {
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
		result = D3D12CreateDevice(useAdapter.Get(), featureLevels[i], IID_PPV_ARGS(&device_));
		//指定した機能レベルでデバイスが生成できたかを確認
		if (SUCCEEDED(result)) {
			//生成できたのでログ出力を行ってループを抜ける
			Logger::Log(std::format("FeatureLevel:{}\n", featureLevelStrings[i]));
			break;
		}
	}

	//デバイスの生成がうまくいかなかったので起動できない
	assert(device_ != nullptr);
	Logger::Log("Complete createD3D12Device!!!\n"); //初期化完了のログを出す
}

void DirectXCommon::InitializeCommand()
{
	HRESULT result{};

	//-------------------------------------
	//コマンドアロケータを生成する(命令保存用もメモリー）
	//-------------------------------------

	result = device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator_));
	//コマンドアロケータの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(result));

	//-------------------------------------
	//コマンドリストを生成する
	//-------------------------------------

	result = device_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_.Get(), nullptr, IID_PPV_ARGS(&commandList_));
	//コマンドリストの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(result));

	//-------------------------------------
	//コマンドキューを生成する	
	//-------------------------------------

	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	result = device_->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue_));
	//コマンドキューの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(result));

}

void DirectXCommon::CreateSwapChain()
{
	HRESULT result{};

	//-------------------------------------
	//スワップチェーンを生成する
	//-------------------------------------

	//画面の幅。ウィンドウのクライアント領域を同じものにしておく
	swapChainDesc_.Width = WinApp::kClientWidth;
	// 画面の高さ。ウィンドウのクライアント領域を同じものにしておく
	swapChainDesc_.Height = WinApp::kClientHeight;
	//色の形式
	swapChainDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	//マルチサンプルしない
	swapChainDesc_.SampleDesc.Count = 1;
	//描画のターゲットとして利用する
	swapChainDesc_.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	//ダブルバッファ
	swapChainDesc_.BufferCount = 2;
	//モニターにうつしたら、中身を破棄
	swapChainDesc_.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	//コマンドキュー、ウィンドウハンドル、設定を渡して生成する
	result = dxgiFactory_->CreateSwapChainForHwnd(commandQueue_.Get(), winApp_->GetHwnd(), &swapChainDesc_, nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(swapChain_.GetAddressOf()));
	assert(SUCCEEDED(result));
}

void DirectXCommon::CreateDepthBuffer()
{
	//深度バッファリソースを設定
	depthStencilResource_ = CreateDepthStencilTextureResource(WinApp::kClientWidth, WinApp::kClientHeight);
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

	//深度値のクリア設定
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;//1.0f(最大値)でクリア
	depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;//フォーマット。Resourceと合わせる

	//Resourceの生成
	Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
	HRESULT hr = device_->CreateCommittedResource(
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
	descriptorSizeSRV_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	descriptorSizeRTV_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	descriptorSizeDSV_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	//-------------------------------------
	//RTV用のデスクリプタヒープの生成
	//-------------------------------------

	//RTV用のヒープでデスクリプタの数は2。RTVはShader内で触るものではないので、ShaderVisibleはFalse
	rtvDescriptorHeap_ = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);

	//-------------------------------------
	//SRV用のデスクリプタヒープの生成
	//-------------------------------------

	//SRV用のヒープでデスクリプタの数は128。SRVはShader内で触るものなので、ShaderVisibleはTrue
	srvDescriptorHeap_ = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true);

	//-------------------------------------
	//DSV用のデスクリプタヒープの生成
	//-------------------------------------

	//DSV用のヒープでディスクリプタヒープの数は1。DSVはShader内で触るものではないので、ShaderVisibleはfalse
	dsvDescriptorHeap_ = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);
}

ComPtr<ID3D12DescriptorHeap> DirectXCommon::CreateDescriptorHeap(const D3D12_DESCRIPTOR_HEAP_TYPE& heapType, const UINT& numDescriptors, bool shaderVisible)
{
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.Type = heapType;
	descriptorHeapDesc.NumDescriptors = numDescriptors;
	descriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	HRESULT hr = device_->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));
	assert(SUCCEEDED(hr));
	return descriptorHeap;
}

void DirectXCommon::InitializeRenderTargetView()
{
	HRESULT result{};

	//-------------------------------------
	//SwapChainからResourceを引っ張ってくる
	//-------------------------------------

	//swapChainResources_[2] = { nullptr };
	result = swapChain_->GetBuffer(0, IID_PPV_ARGS(&swapChainResources_[0]));
	//うまく取得出来なければ起動できない
	assert(SUCCEEDED(result));
	result = swapChain_->GetBuffer(1, IID_PPV_ARGS(&swapChainResources_[1]));
	assert(SUCCEEDED(result));

	//-------------------------------------
	//RTVを作る
	//-------------------------------------

	//RTVの設定
	rtvDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;  //出力結果をSRGBに変換して書き込む
	rtvDesc_.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;  //2dテクスチャとして書き込む
	//ディスクリプタの先頭を取得する
	D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle = GetCPUDescriptorHandle(rtvDescriptorHeap_, descriptorSizeRTV_, 0);
	
	// for文に変更予定

	//まずは１つ目を作る。１つ目は最初のところに作る。作る場所はこちらで指定してあげる必要がある
	rtvHandles_[0] = rtvStartHandle;
	device_->CreateRenderTargetView(swapChainResources_[0].Get(), &rtvDesc_, rtvHandles_[0]);
	//２つ目のディスクラプタハンドルを得る（自力で）
	rtvHandles_[1].ptr = rtvHandles_[0].ptr + device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	//２つ目を作る
	device_->CreateRenderTargetView(swapChainResources_[1].Get(), &rtvDesc_, rtvHandles_[1]);
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
	device_->CreateDepthStencilView(depthStencilResource_.Get(), &dsvDesc, dsvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart());

}

void DirectXCommon::CreateFence()
{
	HRESULT result{};

	//-------------------------------------
	//FenceとEventを作成する
	//-------------------------------------

	result = device_->CreateFence(fenceVal_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
	assert(SUCCEEDED(result));

	//FenceのSignalを待つためのイベントを作成する
	fenceEvent_ = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(fenceEvent_ != nullptr);
}

void DirectXCommon::InitializeViewPort()
{
	//-------------------------------------
	// ビューポート矩形の設定
	//-------------------------------------

	//クライアント領域のサイズと一緒にして画面全体に表示
	viewport_.Width = WinApp::kClientWidth;
	viewport_.Height = WinApp::kClientHeight;
	viewport_.TopLeftX = 0;
	viewport_.TopLeftY = 0;
	viewport_.MinDepth = 0.0f;
	viewport_.MaxDepth = 1.0f;
}

void DirectXCommon::InitializeScissorRect()
{
	//-------------------------------------
	// シザリング矩形の初期化
	//-------------------------------------

	//基本的にビューポートと同じ矩形が構成されるようにする
	scissorRect_.left = 0;
	scissorRect_.right = WinApp::kClientWidth;
	scissorRect_.top = 0;
	scissorRect_.bottom = WinApp::kClientHeight;
}

void DirectXCommon::InitializeDXCCompiler()
{
	HRESULT result{};

	//-------------------------------------
	// DXCの初期化
	//-------------------------------------

	//dxcCompilerを初期化
	result = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils_));
	assert(SUCCEEDED(result));
	result = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler_));
	assert(SUCCEEDED(result));

	//現時点でincludeはしないが、includeに対応するための設定を行っておく
	result = dxcUtils_->CreateDefaultIncludeHandler(&includeHandler_);
	assert(SUCCEEDED(result));
}

void DirectXCommon::InitializeImGui()
{
	//-------------------------------------
	// ImGuiの初期化
	//-------------------------------------

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(winApp_->GetHwnd());
	ImGui_ImplDX12_Init(device_.Get(), swapChainDesc_.BufferCount, rtvDesc_.Format, srvDescriptorHeap_.Get(), srvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart(), srvDescriptorHeap_->GetGPUDescriptorHandleForHeapStart());

}
