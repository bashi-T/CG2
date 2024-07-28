#pragma once
#include"DX12Common.h"
#include "externals/DirectXTex/DirectXTex.h"

class SRVManager
{
private:
	uint32_t descriptorSize = 0;
	ComPtr<ID3D12DescriptorHeap> descriptorHeap = nullptr;
	uint32_t useIndex = 0;
	UINT backBufferIndex = 0;
	static inline SRVManager* instance;
	HRESULT hr=NULL;

public:
	static const uint32_t kMaxSRVCount;
	static const uint32_t kSRVIndexTop;
	void Initialize();
	uint32_t Allocate();
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(uint32_t index);
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(uint32_t index);
	void CreateSRV(
		uint32_t srvIndex,
		ID3D12Resource* pResource,
		const DirectX::TexMetadata& metadata,
		UINT MipLevels);
	void CreateSRVforStructuredBuffer(
		uint32_t srvIndex,
		ID3D12Resource* pResource,
		UINT numElements,
		UINT structureByteStride);
	void SetGraphicsRootDescriptorTable(UINT RootParamaterIndex, uint32_t srvIndex);
	bool CheckNumTexture(uint32_t textureIndex);

	ComPtr<ID3D12DescriptorHeap> GetSrvDescriptorHeap() { return descriptorHeap; }
	static SRVManager* GetInstance();

};

