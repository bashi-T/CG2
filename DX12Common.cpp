#include "DX12Common.h"

DX12Common* DX12Common::GetInstance()
{
	if (instance == NULL)
	{
		instance = new DX12Common;
	}
	return instance;
}

void DX12Common::DeleteInstance()
{
	if (instance != NULL)
	{
		delete instance;
	}
	instance = NULL;
}

void DX12Common::Init(int32_t width, int32_t height)
{

#ifdef _DEBUG
	DebugLayer();
#endif

	DX12Common::MakeDXGIFactory();
	DX12Common::ChoseUseAdapter();
	DX12Common::MakeD3D12Device();
	depthStencilResource = CreatedepthstencilTextureResource(
		device.Get(),
		width,
		height);

#ifdef _DEBUG
	InfoQueue(device.Get());
#endif
	DX12Common::MakeScreen();
	DX12Common::MakeFence();
}

void DX12Common::MakeDXGIFactory()
{
	hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));
	assert(SUCCEEDED(hr));
}

void DX12Common::ChoseUseAdapter()
{
	for (UINT i = 0; dxgiFactory->EnumAdapterByGpuPreference(
		i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
		IID_PPV_ARGS(&useAdapter)) != DXGI_ERROR_NOT_FOUND; i++)
	{
		DXGI_ADAPTER_DESC3 adapterDesc{};
		hr = useAdapter->GetDesc3(&adapterDesc);
		assert(SUCCEEDED(hr));

		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE))
		{
			//debug_->Log(debug_->ConvertString(
			//	std::format(L"Use Adapter:{}\n",
			//		adapterDesc.Description)));
			break;
		}
		useAdapter = nullptr;
	}
}

void DX12Common::MakeD3D12Device()
{
	D3D_FEATURE_LEVEL featureLevels[] =
	{
	D3D_FEATURE_LEVEL_12_2,
	D3D_FEATURE_LEVEL_12_1,
	D3D_FEATURE_LEVEL_12_0
	};
	const char* featureLevelStrings[] =
	{
		"12.2",
		"12.1",
		"12.0"
	};
	for (size_t i = 0; i < _countof(featureLevels); ++i)
	{
		hr = D3D12CreateDevice(useAdapter.Get(), featureLevels[i],
			IID_PPV_ARGS(&device));
		if (SUCCEEDED(hr))
		{
			//debug_->Log(std::format("featureLevel {}\n",
			//	featureLevelStrings[i]));
			break;
		}
	}
	assert(device != nullptr);
	//debug_->Log("Complete create D3D12Device\n");
}

void DX12Common::MakeCommandQueue()
{
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	hr = device->CreateCommandQueue(
		&commandQueueDesc, IID_PPV_ARGS(&commandQueue));
	assert(SUCCEEDED(hr));
}

void DX12Common::MakeCommandList()
{
	hr = device->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));
	assert(SUCCEEDED(hr));
	hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
		commandAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList));
	assert(SUCCEEDED(hr));
}

void DX12Common::MakeSwapchain(int32_t width, int32_t height, HWND hwnd_)
{
	swapChainDesc.Width = width;
	swapChainDesc.Height = height;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	hr = dxgiFactory->CreateSwapChainForHwnd
	(
		commandQueue.Get(),
		hwnd_,
		&swapChainDesc,
		nullptr,
		nullptr,
		reinterpret_cast<IDXGISwapChain1**>(swapChain.GetAddressOf())
	);
	assert(SUCCEEDED(hr));
}

void DX12Common::MakeDescriptorHeap()
{
	rtvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvDescriptorHeapDesc.NumDescriptors = 2;
	hr = device->CreateDescriptorHeap(&rtvDescriptorHeapDesc,
		IID_PPV_ARGS(&rtvDescriptorHeap));
	assert(SUCCEEDED(hr));
}

void DX12Common::BringResources()
{
	hr = swapChain->GetBuffer(0, IID_PPV_ARGS(&swapChainResources[0]));
	assert(SUCCEEDED(hr));
	hr = swapChain->GetBuffer(1, IID_PPV_ARGS(&swapChainResources[1]));
	assert(SUCCEEDED(hr));
}

void DX12Common::MakeRTV()
{
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	const uint32_t descriptorSizeRTV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle =
		GetCPUDescriptorHandle(rtvDescriptorHeap.Get(), descriptorSizeRTV, 0);

	rtvHandles[0] = rtvStartHandle;
	device->CreateRenderTargetView(
		swapChainResources[0], &rtvDesc, rtvHandles[0]);

	rtvHandles[1].ptr = rtvHandles[0].ptr + device->
		GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	device->CreateRenderTargetView(
		swapChainResources[1], &rtvDesc, rtvHandles[1]);
}

void DX12Common::MakeScreen()
{
	MakeCommandQueue();
	MakeCommandList();
	MakeSwapchain(
		WinAPP::GetClientWidth(),
		WinAPP::GetClientHeight(),
		WinAPP::GetHWND()
	);
	MakeDescriptorHeap();

	rtvDescriptorHeap = CreateDescriptorHeap(
		device.Get(),
		D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
		2,
		false);
	srvDescriptorHeap = CreateDescriptorHeap(
		device.Get(),
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
		128,
		true);
	dsvDescriptorHeap = CreateDescriptorHeap(
		device.Get(),
		D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
		1,
		false);

	BringResources();
	MakeRTV();
	MakeDSV();
}

void DX12Common::DrawScreen()
{
	backBufferIndex = swapChain->GetCurrentBackBufferIndex();
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = swapChainResources[backBufferIndex];
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	commandList->ResourceBarrier(1, &barrier);

	commandList->ClearDepthStencilView(
		dsvHandle,
		D3D12_CLEAR_FLAG_DEPTH,
		1.0f,
		0,
		0,
		nullptr);

	commandList->OMSetRenderTargets(1, &rtvHandles[backBufferIndex],
		false, nullptr);
	commandList->ClearRenderTargetView(
		rtvHandles[backBufferIndex],
		clearColor, 0, nullptr);
}

void DX12Common::ClearScreen()
{
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	commandList->ResourceBarrier(1, &barrier);

	hr = commandList->Close();
	assert(SUCCEEDED(hr));

	ComPtr<ID3D12CommandList> commandLists[] =
	{
		commandList.Get()
	};
	commandQueue->ExecuteCommandLists(1, commandLists->GetAddressOf());
	swapChain->Present(1, 0);
	fenceValue++;
	commandQueue->Signal(fence.Get(), fenceValue);

	if (fence->GetCompletedValue() < fenceValue)
	{
		fence->SetEventOnCompletion(fenceValue, fenceEvent);
		WaitForSingleObject(fenceEvent, INFINITE);
	}

	hr = commandAllocator->Reset();
	assert(SUCCEEDED(hr));
	hr = commandList->Reset(commandAllocator.Get(), nullptr);
	assert(SUCCEEDED(hr));
}

void DX12Common::MakeFence()
{
	hr = device->CreateFence(fenceValue,
		D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
	assert(SUCCEEDED(hr));
	fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(fenceEvent != nullptr);
}

//void DX12Common::DX12Release()
//{
//#ifdef _DEBUG
	//debugController->Release();
//#endif
//}

ComPtr<ID3D12DescriptorHeap> DX12Common::CreateDescriptorHeap(
	ID3D12Device* device,
	D3D12_DESCRIPTOR_HEAP_TYPE heapType,
	UINT numDesctiptors,
	bool shaderVisible)
{
	ComPtr<ID3D12DescriptorHeap> descriptorHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC DescriptorHeapDesc{};
	DescriptorHeapDesc.Type = heapType;
	DescriptorHeapDesc.NumDescriptors = numDesctiptors;
	DescriptorHeapDesc.Flags =
		shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	hr = device->CreateDescriptorHeap(&DescriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));
	assert(SUCCEEDED(hr));
	return descriptorHeap;
}


void DX12Common::MakeDSV()
{
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	device->CreateDepthStencilView(
		depthStencilResource.Get(),
		&dsvDesc,
		dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	const uint32_t descriptorSizeDSV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	dsvHandle = GetCPUDescriptorHandle(dsvDescriptorHeap.Get(), descriptorSizeDSV, 0);
}


ComPtr<ID3D12Resource> DX12Common::CreatedepthstencilTextureResource(ID3D12Device* device, int32_t width, int32_t height)
{
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = width;
	resourceDesc.Height = height;
	resourceDesc.MipLevels = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;
	depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	ComPtr<ID3D12Resource> resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthClearValue,
		IID_PPV_ARGS(&resource));
	assert(SUCCEEDED(hr));
	return resource;
}

D3D12_CPU_DESCRIPTOR_HANDLE DX12Common::GetCPUDescriptorHandle(
	ID3D12DescriptorHeap* descriptorHeap,
	uint32_t descriptorSize,
	uint32_t index)
{
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriptorSize * index);
	return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE DX12Common::GetGPUDescriptorHandle(
	ID3D12DescriptorHeap* descriptorHeap,
	uint32_t descriptorSize,
	uint32_t index)
{
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += (descriptorSize * index);
	return handleGPU;
}

void DX12Common::DebugLayer()
{
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		debugController->EnableDebugLayer();
		debugController->SetEnableGPUBasedValidation(TRUE);
	}
}

void DX12Common::InfoQueue(ID3D12Device* device)
{
	ComPtr<ID3D12InfoQueue> InfoQueue = nullptr;
	if (SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&InfoQueue))))
	{
		InfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		InfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		D3D12_MESSAGE_ID denyIds[] =
		{
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
		};
		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		D3D12_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;
		InfoQueue->PushStorageFilter(&filter);
	}
}