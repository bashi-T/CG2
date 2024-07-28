#include "TextureManager.h"
TextureManager* TextureManager::instance = nullptr;

TextureManager* TextureManager::GetInstance()
{
	if (instance == nullptr)
	{
		instance = new TextureManager;
	}
	return instance;
}

void TextureManager::Finalize()
{
	delete instance;
	instance = nullptr;
}

void TextureManager::Initialize()
{
	textureDatas.reserve(SRVManager::kMaxSRVCount);
}

void TextureManager::LoadTexture(const std::string& filePath)
{
	if (textureDatas.contains(filePath))
	{
		return;
	}

	assert(SRVManager::GetInstance()->CheckNumTexture(uint32_t(textureDatas.size())));
	DirectX::ScratchImage image{};//テクスチャファイルをプログラムで扱えるように
	std::wstring filePathW = debug_->ConvertString(filePath);
	HRESULT hr;
	if (filePathW.ends_with(L".dds"))
	{
		hr = DirectX::LoadFromDDSFile(
			filePathW.c_str(),
			DirectX::DDS_FLAGS_NONE,
			nullptr,
			image);
	}else
	{
		hr = DirectX::LoadFromWICFile(
			filePathW.c_str(),
			DirectX::WIC_FLAGS_FORCE_SRGB,
			nullptr,
			image);
	}
	assert(SUCCEEDED(hr));

	DirectX::ScratchImage mipImages{};//MipMapの作成
	if (DirectX::IsCompressed(image.GetMetadata().format))
	{
		mipImages = std::move(image);
	}else
	{
		hr = DirectX::GenerateMipMaps(
			image.GetImages(),
			image.GetImageCount(),
			image.GetMetadata(),
			DirectX::TEX_FILTER_SRGB,
			4,
			mipImages);
	}
	assert(SUCCEEDED(hr));

	TextureData& textureData = textureDatas[filePath];
	textureData.metadata = mipImages.GetMetadata();
	textureData.resource = CreateTextureResource(textureData.metadata);
	UploadTextureData(textureData.resource.Get(), mipImages, textureData.metadata);

	textureData.srvIndex = SRVManager::GetInstance()->Allocate();
	textureData.srvHandleCPU = SRVManager::GetInstance()->
		GetCPUDescriptorHandle(textureData.srvIndex);

	textureData.srvHandleGPU = SRVManager::GetInstance()->
		GetGPUDescriptorHandle(textureData.srvIndex);

	SRVManager::GetInstance()->CreateSRV(
		textureData.srvIndex,
		textureData.resource.Get(),
		textureData.metadata,
		UINT(textureData.metadata.mipLevels));

	textureDatas[filePath] = textureData;
}

void TextureManager::LoadTextureforSRV(const std::string& filePath)
{
	if (textureDatas.contains(filePath))
	{
		return;
	}

	assert(SRVManager::GetInstance()->CheckNumTexture(uint32_t(textureDatas.size())));

	DirectX::ScratchImage image{};//テクスチャファイルをプログラムで扱えるように
	std::wstring filePathW = debug_->ConvertString(filePath);
	HRESULT hr = DirectX::LoadFromWICFile(
		filePathW.c_str(),
		DirectX::WIC_FLAGS_FORCE_SRGB,
		nullptr,
		image);
	assert(SUCCEEDED(hr));

	DirectX::ScratchImage mipImages{};//MipMapの作成
	hr = DirectX::GenerateMipMaps(
		image.GetImages(),
		image.GetImageCount(),
		image.GetMetadata(),
		DirectX::TEX_FILTER_SRGB,
		0,
		mipImages);
	assert(SUCCEEDED(hr));

	TextureData& textureData = textureDatas[filePath];
	textureData.metadata = mipImages.GetMetadata();
	textureData.resource = CreateTextureResource(textureData.metadata);
	UploadTextureData(textureData.resource.Get(), mipImages, textureData.metadata);

	textureData.srvIndex = SRVManager::GetInstance()->Allocate();
	textureData.srvHandleCPU = SRVManager::GetInstance()->
		GetCPUDescriptorHandle(textureData.srvIndex);

	textureData.srvHandleGPU = SRVManager::GetInstance()->
		GetGPUDescriptorHandle(textureData.srvIndex);

	SRVManager::GetInstance()->CreateSRVforStructuredBuffer(
		textureData.srvIndex,
		textureData.resource.Get(),
		textureData.metadata.format,
		UINT(textureData.metadata.mipLevels));

	textureDatas[filePath] = textureData;
}

ComPtr<ID3D12Resource> TextureManager::CreateTextureResource(const DirectX::TexMetadata& metadata)
{
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(metadata.width);
	resourceDesc.Height = UINT(metadata.height);
	resourceDesc.MipLevels = UINT16(metadata.mipLevels);
	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize);
	resourceDesc.Format = metadata.format;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);

	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_CUSTOM;
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

	ComPtr<ID3D12Resource> resource = nullptr;
	HRESULT hr = DX12Common::GetInstance()->GetDevice()->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&resource));

	assert(SUCCEEDED(hr));

	return resource;
}

uint32_t TextureManager::GetSrvIndex(const std::string& filePath)
{
	TextureData& textureData = textureDatas[filePath];
	assert(SRVManager::GetInstance()->CheckNumTexture(textureData.srvIndex));
	return textureData.srvIndex;
}

D3D12_GPU_DESCRIPTOR_HANDLE TextureManager::GetSRVHandleGPU(const std::string& filePath)
{
	TextureData& textureData = textureDatas[filePath];
	assert(SRVManager::GetInstance()->CheckNumTexture(textureData.srvIndex));
	return textureData.srvHandleGPU;
}

void TextureManager::UploadTextureData(
	ID3D12Resource* texture,
	const DirectX::ScratchImage& mipImages,
	const DirectX::TexMetadata& metadata)
{
	for (size_t mipLevel = 0; mipLevel < metadata.mipLevels; mipLevel++)
	{
		const DirectX::Image* img = mipImages.GetImage(mipLevel, 0, 0);
		HRESULT hr = texture->WriteToSubresource(
			UINT(mipLevel),
			nullptr,
			img->pixels,
			UINT(img->rowPitch),
			UINT(img->slicePitch)
		);
		assert(SUCCEEDED(hr));
	}
}

const DirectX::TexMetadata& TextureManager::GetMetaData(const std::string& filePath)
{
	TextureData& textureData = textureDatas[filePath];
	assert(SRVManager::GetInstance()->CheckNumTexture(textureData.srvIndex));
	return textureData.metadata;
}
