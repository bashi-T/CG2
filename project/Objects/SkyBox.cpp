#include "SkyBox.h"
#include "Commons/SkyBoxCommon.h"

void SkyBox::Initialize(SkyBoxCommon* skyBoxCommon, std::string textureFilePath)
{
	this->skyBoxCommon_ = skyBoxCommon;
	projectionMatrix = MakePerspectiveFovMatrix(0.65f, float(WinAPP::clientWidth_) / float(WinAPP::clientHeight_), 0.1f, 100.0f);
	vertexResource = CreateBufferResource(sizeof(VertexData) * 24);
	colorResource = CreateBufferResource(sizeof(Material));
	indexResource = CreateBufferResource(sizeof(uint32_t) * 36);
	cameraResource = CreateBufferResource(sizeof(CameraTransform));
	materialResource = CreateBufferResource(sizeof(Material));
	transformationMatrixResource = CreateBufferResource(sizeof(TransformationMatrix));

	MakeBufferView();

	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	cameraResource->Map(0, nullptr, reinterpret_cast<void**>(&cameraData));
	transformationMatrixResource->Map(
		0, nullptr, reinterpret_cast<void**>(&transformationMatrixData));
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	colorResource->Map(0, nullptr, reinterpret_cast<void**>(&colorData));
	indexResource->Map(0, nullptr, reinterpret_cast<void**>(&indexData));

	materialData[0].color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialData[0].enableLighting = true;
	materialData[0].uvTransform = MakeIdentity4x4();
	colorData[0].color = { 1.0f,1.0f,1.0f,1.0f };

	InputData();
	materialData->material.textureFilePath = textureFilePath;
	TextureManager::GetInstance()->LoadTexture(textureFilePath);
	materialData->material.textureIndex = TextureManager::GetInstance()->GetSrvIndex(textureFilePath);
	std::memcpy(vertexData, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size());
	transformMatrix.scale = { 10.0f,10.0f,10.0f };
	//transformMatrix.translate = { 100.0f,100.0f,100.0f };
}

void SkyBox::Update()
{
	transformationMatrixData->WVP = MakeIdentity4x4();
	Matrix4x4 worldMatrix = MakeAffineMatrix(
		transformMatrix.scale, transformMatrix.rotate, transformMatrix.translate);

		const Matrix4x4& viewProjectionMatrix = Camera::GetInstance()->GetViewProjectionMatrix();
		worldViewProjectionMatrix = Multiply(worldMatrix, viewProjectionMatrix);

	cameraData->worldPosition =
	{
		Camera::GetInstance()->GetWorldMatrix().m[3][0],
		Camera::GetInstance()->GetWorldMatrix().m[3][1],
		Camera::GetInstance()->GetWorldMatrix().m[3][2]
	};
	//directionalLightData->color = directionalLight.color;
	//directionalLightData->direction = directionalLight.direction;
	//directionalLightData->intensity = directionalLight.intensity;
	//Matrix4x4 uvTransformMatrix = MakeScaleMatrix(uvTransform.scale);
	//uvTransformMatrix = Multiply(uvTransformMatrix, MakerotateZMatrix(uvTransform.rotate.z));
	//uvTransformMatrix = Multiply(uvTransformMatrix, MakeTranslateMatrix(uvTransform.translate));
	//materialData[0].uvTransform = uvTransformMatrix;

	transformationMatrixData->WVP = worldViewProjectionMatrix;
	transformationMatrixData->World = worldMatrix;

	InputData();
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
		3, cameraResource->GetGPUVirtualAddress());
	//skyBoxCommon_->GetDx12Common()->GetCommandList().Get()->
	//	SetGraphicsRootConstantBufferView(
	//		4, directionalLightResource->GetGPUVirtualAddress());
	SRVManager::GetInstance()->SetGraphicsRootDescriptorTable(
		2, materialData->material.textureIndex);

	skyBoxCommon_->GetDx12Common()->GetCommandList().Get()->
		DrawIndexedInstanced(24, 1, 0, 0, 0);
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
	vertexBufferView.SizeInBytes = sizeof(VertexData) * 24;
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	indexBufferView.BufferLocation = indexResource->GetGPUVirtualAddress();
	indexBufferView.SizeInBytes = sizeof(uint32_t) * 36;
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
	vertexData[16].position = { +1.0f,+1.0f,+1.0f,+1.0f };
	vertexData[17].position = { -1.0f,+1.0f,+1.0f,+1.0f };
	vertexData[18].position = { +1.0f,+1.0f,-1.0f,+1.0f };
	vertexData[19].position = { -1.0f,+1.0f,-1.0f,+1.0f };
	//下面 [20,21,22][22,21,23]
	vertexData[20].position = { -1.0f,-1.0f,+1.0f,+1.0f };
	vertexData[21].position = { +1.0f,-1.0f,+1.0f,+1.0f };
	vertexData[22].position = { -1.0f,-1.0f,-1.0f,+1.0f };
	vertexData[23].position = { +1.0f,-1.0f,-1.0f,+1.0f };

	indexData[0] = 0;
	indexData[1] = 1;
	indexData[2] = 2;

	indexData[3] = 2;
	indexData[4] = 1;
	indexData[5] = 3;

	indexData[6] = 4;
	indexData[7] = 5;
	indexData[8] = 6;

	indexData[9] = 6;
	indexData[10] = 5;
	indexData[11] = 7;

	indexData[12] = 8;
	indexData[13] = 9;
	indexData[14] = 10;

	indexData[15] = 10;
	indexData[16] = 9;
	indexData[17] = 11;

	indexData[18] = 12;
	indexData[19] = 13;
	indexData[20] = 14;

	indexData[21] = 14;
	indexData[22] = 13;
	indexData[23] = 15;

	indexData[24] = 16;
	indexData[25] = 17;
	indexData[26] = 18;

	indexData[27] = 18;
	indexData[28] = 17;
	indexData[29] = 19;

	indexData[30] = 20;
	indexData[31] = 21;
	indexData[32] = 22;

	indexData[33] = 22;
	indexData[34] = 21;
	indexData[35] = 23;


	//vertexData[0].texcoord = { 1.0f,1.0f };
	//vertexData[1].texcoord = { 1.0f,1.0f };
	//vertexData[2].texcoord = { 1.0f,1.0f };
	//vertexData[3].texcoord = { 1.0f,1.0f };
	//
	//vertexData[4].texcoord = { 1.0f,1.0f };
	//vertexData[5].texcoord = { 1.0f,1.0f };
	//vertexData[6].texcoord = { 1.0f,1.0f };
	//vertexData[7].texcoord = { 1.0f,1.0f };
	//
	//vertexData[8].texcoord = { 1.0f,1.0f };
	//vertexData[9].texcoord = { 1.0f,1.0f };
	//vertexData[10].texcoord = { 1.0f,1.0f };
	//vertexData[11].texcoord = { 1.0f,1.0f };
	//
	//vertexData[12].texcoord = { 1.0f,1.0f };
	//vertexData[13].texcoord = { 1.0f,1.0f };
	//vertexData[14].texcoord = { 1.0f,1.0f };
	//vertexData[15].texcoord = { 1.0f,1.0f };
	//
	//vertexData[16].texcoord = { 1.0f,1.0f };
	//vertexData[17].texcoord = { 1.0f,1.0f };
	//vertexData[18].texcoord = { 1.0f,1.0f };
	//vertexData[19].texcoord = { 1.0f,1.0f };
	//
	//vertexData[20].texcoord = { 1.0f,1.0f };
	//vertexData[21].texcoord = { 1.0f,1.0f };
	//vertexData[22].texcoord = { 1.0f,1.0f };
	//vertexData[23].texcoord = { 1.0f,1.0f };
	//
	//vertexData[0].normal = { 0.0f, 0.0f, -1.0f };
	//vertexData[1].normal = { 0.0f, 0.0f, -1.0f };
	//vertexData[2].normal = { 0.0f, 0.0f, -1.0f };
	//vertexData[3].normal = { 0.0f, 0.0f, -1.0f };
	//
	//vertexData[4].normal = { 0.0f, 0.0f, -1.0f };
	//vertexData[5].normal = { 0.0f, 0.0f, -1.0f };
	//vertexData[6].normal = { 0.0f, 0.0f, -1.0f };
	//vertexData[7].normal = { 0.0f, 0.0f, -1.0f };
	//
	//vertexData[8].normal = { 0.0f, 0.0f, -1.0f };
	//vertexData[9].normal = { 0.0f, 0.0f, -1.0f };
	//vertexData[10].normal = { 0.0f, 0.0f, -1.0f };
	//vertexData[11].normal = { 0.0f, 0.0f, -1.0f };
	//
	//vertexData[12].normal = { 0.0f, 0.0f, -1.0f };
	//vertexData[13].normal = { 0.0f, 0.0f, -1.0f };
	//vertexData[14].normal = { 0.0f, 0.0f, -1.0f };
	//vertexData[15].normal = { 0.0f, 0.0f, -1.0f };
	//
	//vertexData[16].normal = { 0.0f, 0.0f, -1.0f };
	//vertexData[17].normal = { 0.0f, 0.0f, -1.0f };
	//vertexData[18].normal = { 0.0f, 0.0f, -1.0f };
	//vertexData[19].normal = { 0.0f, 0.0f, -1.0f };
	//
	//vertexData[20].normal = { 0.0f, 0.0f, -1.0f };
	//vertexData[21].normal = { 0.0f, 0.0f, -1.0f };
	//vertexData[22].normal = { 0.0f, 0.0f, -1.0f };
	//vertexData[23].normal = { 0.0f, 0.0f, -1.0f };
}
