#include "SRVManager.h"

const uint32_t SRVManager::kMaxSRVCount = 512;
const uint32_t SRVManager::kSRVIndexTop = 0;

void SRVManager::Initialize()
{
	descriptorHeap = DX12Common::GetInstance()->CreateDescriptorHeap(
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
		kMaxSRVCount,
		true);
	descriptorSize = DX12Common::GetInstance()->GetDevice()->
		GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
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

void SRVManager::CreateSRV(
	uint32_t srvIndex,
	ID3D12Resource* pResource,
	const DirectX::TexMetadata& metadata,
	UINT MipLevels)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	if (metadata.IsCubemap())
	{
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.TextureCube.MostDetailedMip = 0;
		srvDesc.TextureCube.MipLevels = UINT_MAX;
		srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
	}else
	{
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = UINT(MipLevels);
	}

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
	instancingSrvDesc.Buffer.StructureByteStride =structureByteStride;

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

SRVManager* SRVManager::GetInstance()
{
	if (instance == NULL)
	{
		instance = new SRVManager;
	}
	return instance;
}
