#include "SRVManager.h"

const uint32_t SRVManager::kMaxSRVCount = 512;
const uint32_t SRVManager::kSRVIndexTop = 0;

void SRVManager::Initialize()
{
	MakeFence();
	descriptorHeap = DX12Common::GetInstance()->CreateDescriptorHeap(
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
		kMaxSRVCount,
		true);
	descriptorSize = DX12Common::GetInstance()->GetDevice()->
		GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = float(WinAPP::clientWidth_);
	viewport.Height = float(WinAPP::clientHeight_);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	scissorRect.left = LONG(0.0f);
	scissorRect.right = LONG(WinAPP::clientWidth_);
	scissorRect.top = LONG(0.0f);
	scissorRect.bottom = LONG(WinAPP::clientHeight_);

}

uint32_t SRVManager::Allocate()
{
	assert(CheckNumTexture(useIndex));
	int index = useIndex;
	useIndex++;
	return index;
}

D3D12_CPU_DESCRIPTOR_HANDLE SRVManager::GetCPUDescriptorHandle(uint32_t index)
{
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriptorSize * index);
	return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE SRVManager::GetGPUDescriptorHandle(uint32_t index)
{
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += (descriptorSize * index);
	return handleGPU;
}

void SRVManager::CreateSRVforTexture2D(
	uint32_t srvIndex,
	ID3D12Resource* pResource,
	DXGI_FORMAT Format,
	UINT MipLevels)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = UINT(MipLevels);

	DX12Common::GetInstance()->GetDevice().Get()->CreateShaderResourceView(
		pResource, &srvDesc, GetCPUDescriptorHandle(srvIndex));
}

void SRVManager::CreateSRVforStructuredBuffer(
	uint32_t srvIndex,
	ID3D12Resource* pResource,
	UINT numElements,
	UINT structureByteStride)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC instancingSrvDesc{};
	instancingSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
	instancingSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	instancingSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	instancingSrvDesc.Buffer.FirstElement = 0;
	instancingSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	instancingSrvDesc.Buffer.NumElements = numElements;
	instancingSrvDesc.Buffer.StructureByteStride = structureByteStride;

	DX12Common::GetInstance()->GetDevice().Get()->CreateShaderResourceView(
		pResource, &instancingSrvDesc, GetCPUDescriptorHandle(srvIndex));
}

void SRVManager::SetGraphicsRootDescriptorTable(UINT RootParamaterIndex, uint32_t srvIndex)
{
	DX12Common::GetInstance()->GetCommandList()->SetGraphicsRootDescriptorTable(RootParamaterIndex, GetGPUDescriptorHandle(srvIndex));
}

bool SRVManager::CheckNumTexture(uint32_t textureIndex)
{
	if (kMaxSRVCount > textureIndex)
	{
		return true;
	}
	else {
		return false;
	}
}

void SRVManager::PreDraw()
{
	backBufferIndex = DX12Common::GetInstance()->GetSwapChain()->GetCurrentBackBufferIndex();

	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	//barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = DX12Common::GetInstance()->GetSwapChainResources()[backBufferIndex].Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	DX12Common::GetInstance()->GetCommandList()->ResourceBarrier(1, &barrier);
	
	//DX12Common::GetInstance()->GetCommandList()->OMSetRenderTargets(1,
	//	&DX12Common::GetInstance()->GetRtvHandles(backBufferIndex), false, nullptr);

	DX12Common::GetInstance()->GetCommandList()->ClearRenderTargetView(
		DX12Common::GetInstance()->GetRtvHandles(backBufferIndex),
		clearColor, 0, nullptr);

	DX12Common::GetInstance()->GetCommandList()->ClearDepthStencilView(
		DX12Common::GetInstance()->GetDsvHandle(),
		D3D12_CLEAR_FLAG_DEPTH,
		1.0f,
		0,
		0,
		nullptr);

	ComPtr<ID3D12DescriptorHeap> descriptorHeaps[] =
	{
		descriptorHeap.Get()
	};
	DX12Common::GetInstance()->GetCommandList()->
		SetDescriptorHeaps(1, descriptorHeaps->GetAddressOf());
	DX12Common::GetInstance()->GetCommandList()->RSSetViewports(1, &viewport);
	DX12Common::GetInstance()->GetCommandList()->RSSetScissorRects(1, &scissorRect);
}

void SRVManager::PostDraw()
{
	backBufferIndex = DX12Common::GetInstance()->GetSwapChain()->GetCurrentBackBufferIndex();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	DX12Common::GetInstance()->GetCommandList()->ResourceBarrier(1, &barrier);

	HRESULT hr = DX12Common::GetInstance()->GetCommandList()->Close();
	assert(SUCCEEDED(hr));

	ComPtr<ID3D12CommandList> commandLists[] =
	{
		DX12Common::GetInstance()->GetCommandList().Get()
	};
	DX12Common::GetInstance()->GetCommandQueue()->ExecuteCommandLists(1, commandLists->GetAddressOf());
	DX12Common::GetInstance()->GetSwapChain()->Present(1, 0);
	fenceValue++;
	DX12Common::GetInstance()->GetCommandQueue()->Signal(fence.Get(), fenceValue);

	if (fence->GetCompletedValue() < fenceValue)
	{
		fence->SetEventOnCompletion(fenceValue, fenceEvent);
		WaitForSingleObject(fenceEvent, INFINITE);
	}

	hr = DX12Common::GetInstance()->GetCommandAllocator()->Reset();
	assert(SUCCEEDED(hr));
	hr = DX12Common::GetInstance()->GetCommandList()->Reset(DX12Common::GetInstance()->GetCommandAllocator().Get(), nullptr);
	assert(SUCCEEDED(hr));
}
SRVManager* SRVManager::GetInstance()
{
	if (instance == NULL)
	{
		instance = new SRVManager;
	}
	return instance;
}
void SRVManager::MakeFence()
{
	HRESULT hr = DX12Common::GetInstance()->GetDevice()->CreateFence(fenceValue,
		D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
	assert(SUCCEEDED(hr));
	fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(fenceEvent != nullptr);
}