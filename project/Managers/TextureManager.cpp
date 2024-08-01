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
	//UploadTextureData(textureData.resource.Get(), mipImages, textureData.metadata);
	intermediateResources = TextureManager::GetInstance()->
		UploadTextureData(textureData.resource.Get(), mipImages);

	DX12Common::GetInstance()->ExecuteCommandList();
	DX12Common::GetInstance()->MakeFenceEvent();
	hr = DX12Common::GetInstance()->GetCommandAllocator()->Reset();
	assert(SUCCEEDED(hr));
	hr = DX12Common::GetInstance()->GetCommandList()->Reset(DX12Common::GetInstance()->GetCommandAllocator().Get(), nullptr);
	assert(SUCCEEDED(hr));
	
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
	hr = DirectX::LoadFromWICFile(
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
	//UploadTextureData(textureData.resource.Get(), mipImages, textureData.metadata);
	intermediateResources = TextureManager::GetInstance()->
		UploadTextureData(textureData.resource.Get(), mipImages);

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
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	//heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	//heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

	ComPtr<ID3D12Resource> resource = nullptr;
	hr = DX12Common::GetInstance()->GetDevice()->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
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

//void TextureManager::UploadTextureData(
//	ID3D12Resource* texture,
//	const DirectX::ScratchImage& mipImages,
//	const DirectX::TexMetadata& metadata)
//{
//	for (size_t mipLevel = 0; mipLevel < metadata.mipLevels; mipLevel++)
//	{
//		const DirectX::Image* img = mipImages.GetImage(mipLevel, 0, 0);
//		HRESULT hr = texture->WriteToSubresource(
//			UINT(mipLevel),
//			nullptr,
//			img->pixels,
//			UINT(img->rowPitch),
//			UINT(img->slicePitch)
//		);
//		assert(SUCCEEDED(hr));
//	}
//}

[[nodiscard]]
ComPtr<ID3D12Resource> TextureManager::UploadTextureData(
	ID3D12Resource* texture,
	const DirectX::ScratchImage& mipImages)
{
		std::vector<D3D12_SUBRESOURCE_DATA> subresources;
		
		DirectX::PrepareUpload(
			DX12Common::GetInstance()->GetDevice().Get(),
			mipImages.GetImages(),
			mipImages.GetImageCount(),
			mipImages.GetMetadata(),
			subresources);
		uint64_t intermediateSize = GetRequiredIntermediateSize(texture, 0, UINT(subresources.size()));
		ComPtr<ID3D12Resource> intermediateResource = CreateBufferResource(intermediateSize);
		UpdateSubresources(
			DX12Common::GetInstance()->GetCommandList().Get(),
			texture,
			intermediateResource.Get(),
			0,
			0,
			UINT(subresources.size()),
			subresources.data());

		D3D12_RESOURCE_BARRIER barrier{};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = texture;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
		DX12Common::GetInstance()->GetCommandList()->ResourceBarrier(1, &barrier);
		return intermediateResource;
}

const DirectX::TexMetadata& TextureManager::GetMetaData(const std::string& filePath)
{
	TextureData& textureData = textureDatas[filePath];
	assert(SRVManager::GetInstance()->CheckNumTexture(textureData.srvIndex));
	return textureData.metadata;
}

ComPtr<ID3D12Resource> TextureManager::CreateBufferResource(size_t sizeInBytes)
{
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};

	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
	D3D12_RESOURCE_DESC ResourceDesc{};

	ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;

	ResourceDesc.Width = sizeInBytes * 3;

	ResourceDesc.Height = 1;
	ResourceDesc.DepthOrArraySize = 1;
	ResourceDesc.MipLevels = 1;
	ResourceDesc.SampleDesc.Count = 1;

	ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	ComPtr<ID3D12Resource> Resource = nullptr;

	hr = DX12Common::GetInstance()->GetDevice().Get()->CreateCommittedResource(
		&uploadHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&ResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&Resource));
	assert(SUCCEEDED(hr));
	return Resource;
}
