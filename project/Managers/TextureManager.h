#pragma once
#include<string>
#include<WRL.h>
#include "Commons/DX12Common.h"
#include"SRVManager.h"
#include<unordered_map>
#include"externals/DirectXTex/d3dx12.h"
#include<vector>

class TextureManager
{
public:
	struct TextureData
	{
		DirectX::TexMetadata metadata;
		ComPtr<ID3D12Resource> resource;
		uint32_t srvIndex = 0;
		D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU;
		D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU;
	};

	static TextureManager* GetInstance();
	void Finalize();
	void Initialize();
	void LoadTexture(const std::string& filePath);
	void LoadTextureforSRV(const std::string& filePath);
	ComPtr<ID3D12Resource> CreateTextureResource(const DirectX::TexMetadata& metadata);
	const DirectX::TexMetadata& GetMetaData(const std::string& filePath);
	uint32_t GetSrvIndex(const std::string& filePath);
	D3D12_GPU_DESCRIPTOR_HANDLE GetSRVHandleGPU(const std::string& filePath);
	ComPtr<ID3D12Resource> CreateBufferResource(size_t sizeInBytes);

	void UploadTextureData(
		ID3D12Resource* texture,
		const DirectX::ScratchImage& mipImages,
		const DirectX::TexMetadata& metadata);

	ComPtr<ID3D12Resource> UploadTextureData(
		ID3D12Resource* texture,
		const DirectX::ScratchImage& mipImages);

private:
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	static TextureManager* instance;

	TextureManager() = default;
	~TextureManager() = default;
	TextureManager(TextureManager&) = delete;
	TextureManager& operator=(TextureManager&) = delete;
	Debug* debug_;
	std::unordered_map<std::string, TextureData>textureDatas;
	HRESULT hr = NULL;
	ComPtr<ID3D12Resource> intermediateResources;

};

