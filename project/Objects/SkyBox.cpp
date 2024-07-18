#include "SkyBox.h"
#include "Commons/SkyBoxCommon.h"

void SkyBox::Initialize(SkyBoxCommon* skyBoxCommon, std::string textureFilePath)
{
	this->skyBoxCommon_ = skyBoxCommon;
	projectionMatrix = MakePerspectiveFovMatrix(0.65f, float(WinAPP::clientWidth_) / float(WinAPP::clientHeight_), 0.1f, 100.0f);
	vertexResource = CreateBufferResource(sizeof(VertexData) * 6);
	colorResource = CreateBufferResource(sizeof(Material));
	indexResource = CreateBufferResource(sizeof(uint32_t) * 6);
	cameraResource = CreateBufferResource(sizeof(CameraTransform));
	materialResource = CreateBufferResource(sizeof(Material));
	transformationMatrixResource = CreateBufferResource(sizeof(TransformationMatrix));

	MakeBufferView();

	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	cameraResource->Map(0, nullptr, reinterpret_cast<void**>(&cameraData));
	transformationMatrixResource->Map(
		0, nullptr, reinterpret_cast<void**>(&transformationMatrixData));
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	cameraResource->Map(0, nullptr, reinterpret_cast<void**>(&cameraData));

	InputData();
	materialData->material.textureFilePath = textureFilePath;
	TextureManager::GetInstance()->LoadTexture(textureFilePath);
	materialData->material.textureIndex = TextureManager::GetInstance()->GetSrvIndex(textureFilePath);
	std::memcpy(vertexData, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size());
}

void SkyBox::Update()
{
}

void SkyBox::Draw(SkyBoxCommon* skyboxCommon)
{
	this->skyBoxCommon_ = skyboxCommon;

	skyBoxCommon_->GetDx12Common()->GetCommandList().Get()->SetPipelineState(skyBoxCommon_->GetGraphicsPipelineState().Get());
	skyBoxCommon_->GetDx12Common()->GetCommandList().Get()->SetGraphicsRootSignature(skyBoxCommon_->GetRootSignature().Get());
	skyBoxCommon_->GetDx12Common()->GetCommandList().Get()->IASetPrimitiveTopology(
		D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	skyBoxCommon_->GetDx12Common()->GetCommandList().Get()->IASetVertexBuffers(0, 1, &vertexBufferView);
	skyBoxCommon_->GetDx12Common()->GetCommandList().Get()->
		IASetIndexBuffer(&indexBufferView);

	skyBoxCommon_->GetDx12Common()->GetCommandList().Get()->SetGraphicsRootConstantBufferView(
		0, materialResource->GetGPUVirtualAddress());
	skyBoxCommon_->GetDx12Common()->GetCommandList().Get()->SetGraphicsRootConstantBufferView(
		1, transformationMatrixResource->GetGPUVirtualAddress());
	skyBoxCommon_->GetDx12Common()->GetCommandList().Get()->SetGraphicsRootConstantBufferView(
		4, cameraResource->GetGPUVirtualAddress());

	SRVManager::GetInstance()->SetGraphicsRootDescriptorTable(
		2, materialData->material.textureIndex);

	skyBoxCommon_->GetDx12Common()->GetCommandList().Get()->
		DrawIndexedInstanced(6, 1, 0, 0, 0);
}

ComPtr<ID3D12Resource> SkyBox::CreateBufferResource(size_t sizeInBytes)
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

	hr = skyBoxCommon_->GetDx12Common()->GetDevice().Get()->CreateCommittedResource(
		&uploadHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&ResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&Resource));
	assert(SUCCEEDED(hr));
	return Resource;
}

void SkyBox::MakeBufferView()
{
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = sizeof(VertexData) * 6;
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	indexBufferView.BufferLocation = indexResource->GetGPUVirtualAddress();
	indexBufferView.SizeInBytes = sizeof(uint32_t) * 6;
	indexBufferView.Format = DXGI_FORMAT_R32_UINT;
}

void SkyBox::InputData()
{
	//右面 [0,1,2][2,1,3]
	vertexData[0].position = { +1.0f,+1.0f,+1.0f,+1.0f };
	vertexData[1].position = { +1.0f,+1.0f,-1.0f,+1.0f };
	vertexData[2].position = { +1.0f,-1.0f,+1.0f,+1.0f };
	vertexData[3].position = { +1.0f,-1.0f,-1.0f,+1.0f };
	//左面　[4,5,6][6,5,7]
	vertexData[4].position = { -1.0f,+1.0f,-1.0f,+1.0f };
	vertexData[5].position = { -1.0f,+1.0f,+1.0f,+1.0f };
	vertexData[6].position = { -1.0f,-1.0f,-1.0f,+1.0f };
	vertexData[7].position = { -1.0f,-1.0f,+1.0f,+1.0f };
	//前面 [8,9,10][10,9,11]
	vertexData[ 8].position = { -1.0f,+1.0f,+1.0f,+1.0f };
	vertexData[ 9].position = { +1.0f,+1.0f,+1.0f,+1.0f };
	vertexData[10].position = { -1.0f,-1.0f,+1.0f,+1.0f };
	vertexData[11].position = { +1.0f,-1.0f,+1.0f,+1.0f };
	//後面 [12,13,14][14,13,15]
	vertexData[12].position = { +1.0f,+1.0f,-1.0f,+1.0f };
	vertexData[13].position = { -1.0f,+1.0f,-1.0f,+1.0f };
	vertexData[14].position = { +1.0f,-1.0f,-1.0f,+1.0f };
	vertexData[15].position = { -1.0f,-1.0f,-1.0f,+1.0f };
	//上面 [16,17,18][18,17,19]
	vertexData[16].position = { -1.0f,+1.0f,+1.0f,+1.0f };
	vertexData[17].position = { +1.0f,+1.0f,+1.0f,+1.0f };
	vertexData[18].position = { -1.0f,+1.0f,-1.0f,+1.0f };
	vertexData[19].position = { +1.0f,+1.0f,-1.0f,+1.0f };
	//下面 [20,21,22][22,21,23]
	vertexData[20].position = { +1.0f,-1.0f,+1.0f,+1.0f };
	vertexData[21].position = { -1.0f,-1.0f,+1.0f,+1.0f };
	vertexData[22].position = { +1.0f,-1.0f,-1.0f,+1.0f };
	vertexData[23].position = { -1.0f,-1.0f,-1.0f,+1.0f };
}

void SkyBox::MakeShaderResourceViewInstance()
{
}
