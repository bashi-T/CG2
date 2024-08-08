#include "Sprite.h"
#include"Commons/SpriteCommon.h"


Sprite::~Sprite()
{
}

void Sprite::Initialize(SpriteCommon* spriteCommon,SRVManager* srvManager, std::string textureFilePath)
{
	this->spriteCommon_ = spriteCommon;
	this->srvManager = srvManager;
	vertexResource = CreateBufferResource(spriteCommon_, sizeof(VertexData) * 6);
	indexResource = CreateBufferResource(spriteCommon_, sizeof(uint32_t) * 6);
	materialResource = CreateBufferResource(spriteCommon_, sizeof(Material));
	transformationMatrixResource = CreateBufferResource(spriteCommon_, sizeof(TransformationMatrix));

	uvTransform =
	{
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f},
	};
	transformMatrix =
	{
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f},
	};
	cameraTransform = {
		{1.0f, 1.0f, 1.0f},
		{0.0f, 0.0f, 0.0f},
		{0.0f, 0.0f, -15.0f}
	};
	cameraMatrix =
		MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);
	MakeBufferView();

	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	indexResource->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	transformationMatrixResource->Map(
		0, nullptr, reinterpret_cast<void**>(&transformationMatrixData));


	LeftTop = { 0.0f, 0.0f, 0.0f, 1.0f };
	RightTop = { float(WinAPP::clientWidth_) / 3, 0.0f, 0.0f, 1.0f };
	RightBottom = { float(WinAPP::clientWidth_) / 3, float(WinAPP::clientHeight_) / 3, 0.0f, 1.0f };
	LeftBottom = { 0.0f, float(WinAPP::clientHeight_) / 3, 0.0f, 1.0f };
	Color = { 1.0f, 1.0f, 1.0f, 1.0f };
	coordLeftTop = { 0.0f, 0.0f };
	coordRightTop = { 1.0f, 0.0f };
	coordRightBottom = { 1.0f, 1.0f };
	coordLeftBottom = { 0.0f, 1.0f };
	
	InputData(Color);
	materialData->material.textureFilePath = textureFilePath;
	TextureManager::GetInstance()->LoadTexture(textureFilePath);
	materialData->material.textureIndex = TextureManager::GetInstance()->GetSrvIndex(textureFilePath);
	AdjestTextureSize();
}

void Sprite::Update()
{

	cameraMatrix =
		MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);
	projectionMatrix =
		MakeOrthographicMatrix(0.0f, 0.0f, float(WinAPP::clientWidth_), float(WinAPP::clientHeight_), 0.0f, 100.0f);
	transformMatrix.translate = { position.x,position.y,0.0f };
	transformMatrix.rotate = { 0.0f,0.0f,rotation };
	transformMatrix.scale = { size.x,size.y,1.0f };
	InputData(Color);

	//ImGui::Begin("spriteEdit");
	//int num = textureIndex;
	//ImGui::InputInt("texture", &num);
	//ImGui::DragFloat2("position", &position.x, 0.1f);
	//ImGui::DragFloat("rotate", &rotation, 0.1f);
	//ImGui::DragFloat2("size", &size.x);
	//ImGui::DragFloat2("anchor", &anchorPoint.x, 0.1f);
	//ImGui::DragFloat2("textureLeftTop", &textureLeftTop.x);
	//ImGui::DragFloat2("textureSize", &textureSize.x);
	//ImGui::Checkbox("flipX", &isFlipX_);
	//ImGui::Checkbox("flipY", &isFlipY_);
	//ImGui::End();
}

void Sprite::MakeBufferView()
{
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = sizeof(VertexData) * 6;
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	indexBufferView.BufferLocation = indexResource->GetGPUVirtualAddress();
	indexBufferView.SizeInBytes = sizeof(uint32_t) * 6;
	indexBufferView.Format = DXGI_FORMAT_R32_UINT;
}

void Sprite::InputData(Vector4 color)
{
	float left = 0.0f - anchorPoint.x;
	float right = 1.0f - anchorPoint.x;
	float top = 0.0f - anchorPoint.y;
	float bottom = 1.0f - anchorPoint.y;

	if (isFlipX_)
	{
		left = -left;
		right = -right;
	}
	if (isFlipY_)
	{
		top = -top;
		bottom = -bottom;
	}

	const DirectX::TexMetadata& metadata =
		TextureManager::GetInstance()->GetMetaData(materialData->material.textureFilePath);
	float tex_left = textureLeftTop.x / metadata.width;
	float tex_right = (textureLeftTop.x + textureSize.x) / metadata.width;
	float tex_top = textureLeftTop.y / metadata.width;
	float tex_bottom = (textureLeftTop.y + textureSize.y) / metadata.width;

	vertexData[0].position = { left,bottom,0.0f,1.0f };//左下
	vertexData[1].position = { left,top,0.0f,1.0f };//左上
	vertexData[2].position = { right,bottom,0.0f,1.0f };//右下
	vertexData[3].position = { right,top,0.0f,1.0f };//右上

	vertexData[0].texcoord = { tex_left,tex_bottom };
	vertexData[1].texcoord = { tex_left,tex_top };
	vertexData[2].texcoord = { tex_right,tex_bottom };
	vertexData[3].texcoord = { tex_right,tex_top };

	vertexData[0].normal = { 0.0f, 0.0f, -1.0f };
	vertexData[1].normal = { 0.0f, 0.0f, -1.0f };
	vertexData[2].normal = { 0.0f, 0.0f, -1.0f };
	vertexData[3].normal = { 0.0f, 0.0f, -1.0f };

	indexData[0] = 0;
	indexData[1] = 1;
	indexData[2] = 2;

	indexData[3] = 1;
	indexData[4] = 3;
	indexData[5] = 2;

	materialData[0].color = color;
	materialData[0].uvTransform = MakeIdentity4x4();
	Matrix4x4 uvTransformMatrix = MakeScaleMatrix(uvTransform.scale);
	uvTransformMatrix = Multiply(uvTransformMatrix, MakerotateZMatrix(uvTransform.rotate.z));
	uvTransformMatrix = Multiply(uvTransformMatrix, MakeTranslateMatrix(uvTransform.translate));
	materialData[0].uvTransform = uvTransformMatrix;

	Matrix4x4 worldMatrix = MakeAffineMatrix(
		transformMatrix.scale, transformMatrix.rotate, transformMatrix.translate);
	viewMatrix = MakeIdentity4x4();
	worldViewProjectionMatrix =
		Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
	transformationMatrixData->WVP = worldViewProjectionMatrix;
	transformationMatrixData->World = worldMatrix;
}

void Sprite::Draw()
{
	spriteCommon_->GetDx12Common()->GetCommandList().Get()->
		SetPipelineState(spriteCommon_->GetGraphicsPipelineState().Get());
	spriteCommon_->GetDx12Common()->GetCommandList().Get()->
		SetGraphicsRootSignature(spriteCommon_->GetRootSignature().Get());
	spriteCommon_->GetDx12Common()->GetCommandList().Get()->
		IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	spriteCommon_->GetDx12Common()->GetCommandList().Get()->
		IASetVertexBuffers(0, 1, &vertexBufferView);
	spriteCommon_->GetDx12Common()->GetCommandList().Get()->
		IASetIndexBuffer(&indexBufferView);
	spriteCommon_->GetDx12Common()->GetCommandList().Get()->
		SetGraphicsRootConstantBufferView(
		0, materialResource->GetGPUVirtualAddress());
	spriteCommon_->GetDx12Common()->GetCommandList().Get()->
		SetGraphicsRootConstantBufferView(
		1, transformationMatrixResource->GetGPUVirtualAddress());

	D3D12_CPU_DESCRIPTOR_HANDLE rtv =
		spriteCommon_->GetDx12Common()->
		GetRtvHandles(srvManager->GetBackBufferIndex());
	D3D12_CPU_DESCRIPTOR_HANDLE dsv = spriteCommon_->GetDx12Common()->GetDsvHandle();
	spriteCommon_->GetDx12Common()->GetCommandList().Get()->
		OMSetRenderTargets(1, &rtv, false, &dsv);

	srvManager->SetGraphicsRootDescriptorTable(
		2, materialData->material.textureIndex);

	spriteCommon_->GetDx12Common()->GetCommandList().Get()->
		DrawIndexedInstanced(6, 1, 0, 0, 0);
}

ComPtr<ID3D12Resource> Sprite::CreateBufferResource(SpriteCommon* spriteCommon, size_t sizeInBytes)
{
	this->spriteCommon_ = spriteCommon;
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

	hr = spriteCommon_->GetDx12Common()->GetDevice().Get()->CreateCommittedResource(
		&uploadHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&ResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&Resource));
	assert(SUCCEEDED(hr));
	return Resource;
}

ComPtr<ID3D12Resource> Sprite::CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata)
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
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&resource));

	assert(SUCCEEDED(hr));

	return resource;
}

void Sprite::AdjestTextureSize()
{
	const DirectX::TexMetadata& metadata = TextureManager::GetInstance()->
		GetMetaData(materialData->material.textureFilePath);
	textureSize.x = static_cast<float>(metadata.width);
	textureSize.y = static_cast<float>(metadata.height);
	size = textureSize;
}


