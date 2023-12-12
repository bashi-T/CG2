#pragma once
#include"WindowApp.h"
#include "list"
#include<cassert>
#include"Debug.h"
#include<d3d12.h>
#include<dxgi1_6.h>
#include<DirectXTex.h>
#include<WRL.h>
#include<array>
#include<chrono>
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

class DX12Common final
{
public:
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	void Init(int32_t width, int32_t height, WinAPP* winApp);
	void update();
	void MakeDXGIFactory();
	void ChoseUseAdapter();
	void MakeD3D12Device();

	void MakeCommandQueue();
	void MakeCommandList();

	void MakeSwapchain(int32_t width, int32_t height, HWND hwnd_);
	void MakeDescriptorHeap();
	void BringResources();
	void MakeRTV();
	void MakeDSV();

	void MakeScreen(WinAPP* winApp);
	void PreDraw();
	void PostDraw();
	void MakeFence();
	void DX12Release();
	ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(
		D3D12_DESCRIPTOR_HEAP_TYPE heapType,
		UINT numDescriptors,
	    bool shaderVisible);
	ComPtr<ID3D12Resource> CreatedepthstencilTextureResource(
		int32_t width,
		int32_t height);
	static D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(
		ID3D12DescriptorHeap* descriptorHeap,
		uint32_t descriptorSize,
		uint32_t index);
	static D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(
		ID3D12DescriptorHeap* descriptorHeap,
		uint32_t descriptorSize,
		uint32_t index);

	static D3D12_CPU_DESCRIPTOR_HANDLE GetSRVCPUDescriptorHandle(
		uint32_t index);
	static D3D12_GPU_DESCRIPTOR_HANDLE GetSRVGPUDescriptorHandle(
		uint32_t index);

	void DebugLayer();
	void InfoQueue(ID3D12Device* device);


	ComPtr<ID3D12Debug1> GetDebugController() { return debugController; }
	ComPtr<ID3D12DebugDevice> GetDebugDevice() { return debugDevice; }

	UINT GetBackBufferIndex() { return backBufferIndex; }
	HANDLE GetFenceEvent() { return fenceEvent; }
	uint64_t fenceValue = 0;

	static DX12Common* GetInstance();
	static void DeleteInstance();

	D3D12_CPU_DESCRIPTOR_HANDLE GetRtvHandles(int32_t i) { return rtvHandles[i]; }
	D3D12_CPU_DESCRIPTOR_HANDLE GetDsvHandle() { return dsvHandle; }
	
	ComPtr<ID3D12Device> GetDevice() { return device; }
	ComPtr<ID3D12GraphicsCommandList> GetCommandList() { return commandList; }
	DXGI_SWAP_CHAIN_DESC1 GetSwapChainDesc() { return swapChainDesc; }
	D3D12_RENDER_TARGET_VIEW_DESC GetRtvDesc() { return rtvDesc; }
	ComPtr<ID3D12DescriptorHeap> GetSrvDescriptorHeap() { return srvDescriptorHeap; }
	ComPtr<ID3D12DescriptorHeap> GetRtvDescriptorHeap() { return rtvDescriptorHeap; }
	ComPtr<ID3D12DescriptorHeap> GetDsvDescriptorHeap() { return dsvDescriptorHeap; }
		
	~DX12Common() {
		swapChain.Reset();
		device.Reset();
	}
private:
	DX12Common() = default;
	//~DX12Common() = default;
	DX12Common(const DX12Common& obj) = delete;
	DX12Common& oparator(const DX12Common&obj) = delete;
	static inline DX12Common* instance;

	Debug* debug_ = nullptr;
	WinAPP* winApp_ = nullptr;

	ComPtr<ID3D12Device> device = nullptr;
	ComPtr<IDXGIFactory7> dxgiFactory = nullptr;
	HRESULT hr = NULL;
	ComPtr<IDXGIAdapter4> useAdapter = nullptr;
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle;
	float clearColor[4] = { 0.1f, 0.25f, 0.5f, 1.0f };

	ComPtr<ID3D12CommandQueue> commandQueue = nullptr;
	ComPtr<ID3D12CommandAllocator> commandAllocator = nullptr;
	ComPtr<ID3D12GraphicsCommandList> commandList = nullptr;
	ComPtr<IDXGISwapChain4> swapChain = nullptr;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC rtvDescriptorHeapDesc{};
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap = nullptr;
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap;

	std::array<ComPtr<ID3D12Resource>, 10> swapChainResources;
	ComPtr<ID3D12Fence> fence = nullptr;
	HANDLE fenceEvent;
	D3D12_RESOURCE_BARRIER barrier{};
	UINT backBufferIndex;
	ComPtr<ID3D12Resource> depthStencilResource;

	ComPtr<IDXGIDebug1> debug;
	ComPtr<ID3D12DebugDevice> debugDevice;
	ComPtr<ID3D12Debug1> debugController = nullptr;

	D3D12_VIEWPORT viewport{};
	D3D12_RECT scissorRect{};

	void InitializefixFPS();
	void UpdateFixFPS();

	std::chrono::steady_clock::time_point reference_;

};

