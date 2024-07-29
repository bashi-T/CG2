#include "SkyBox.h"
#include "Commons/SkyBoxCommon.h"

void SkyBox::Initialize(SkyBoxCommon* skyBoxCommon, std::string textureFilePath)
{
	this->skyBoxCommon_ = skyBoxCommon;
	projectionMatrix = MakePerspectiveFovMatrix(0.65f, float(WinAPP::clientWidth_) / float(WinAPP::clientHeight_), 0.1f, 100.0f);
	vertexResource = CreateBufferResource(sizeof(VertexData) * 24);
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
	indexResource->Map(0, nullptr, reinterpret_cast<void**>(&indexData));

	materialData[0].color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialData[0].enableLighting = false;
	materialData[0].uvTransform = MakeIdentity4x4();

	materialData->material.textureFilePath = textureFilePath;
	TextureManager::GetInstance()->LoadTexture(textureFilePath);
	materialData->material.textureIndex = TextureManager::GetInstance()->GetSrvIndex(textureFilePath);
	//std::memcpy(vertexData, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size());
	transformMatrix.scale = { 100.0f,100.0f,100.0f };
	InputData();
}

void SkyBox::Update()
{
	InputData();
}

void SkyBox::Draw(SkyBoxCommon* skyboxCommon)
{
	this->skyBoxCommon_ = skyboxCommon;

	skyBoxCommon_->GetDx12Common()->GetCommandList().Get()->
		SetPipelineState(skyBoxCommon_->GetGraphicsPipelineState().Get());
	skyBoxCommon_->GetDx12Common()->GetCommandList().Get()->
		SetGraphicsRootSignature(skyBoxCommon_->GetRootSignature().Get());
	skyBoxCommon_->GetDx12Common()->GetCommandList().Get()->
		IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	skyBoxCommon_->GetDx12Common()->GetCommandList().Get()->
		IASetVertexBuffers(0, 1, &vertexBufferView);
	skyBoxCommon_->GetDx12Common()->GetCommandList().Get()->
		IASetIndexBuffer(&indexBufferView);

	skyBoxCommon_->GetDx12Common()->GetCommandList().Get()->SetGraphicsRootConstantBufferView(
		0, materialResource->GetGPUVirtualAddress());
	skyBoxCommon_->GetDx12Common()->GetCommandList().Get()->SetGraphicsRootConstantBufferView(
		1, transformationMatrixResource->GetGPUVirtualAddress());
	skyBoxCommon_->GetDx12Common()->GetCommandList().Get()->SetGraphicsRootConstantBufferView(
		3, cameraResource->GetGPUVirtualAddress());
	SRVManager::GetInstance()->SetGraphicsRootDescriptorTable(
		2, materialData->material.textureIndex);

	skyBoxCommon_->GetDx12Common()->GetCommandList().Get()->
		DrawIndexedInstanced(indexBufferView.SizeInBytes, 1, 0, 0, 0);
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
	//右面 [0,1,2]→↙↑[2,1,3]↗↓←
	vertexData[0].position = { +1.0f,+1.0f,+1.0f,1.0f };
	vertexData[1].position = { +1.0f,+1.0f,-1.0f,1.0f };
	vertexData[2].position = { +1.0f,-1.0f,+1.0f,1.0f };
	vertexData[3].position = { +1.0f,-1.0f,-1.0f,1.0f };
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
	vertexData[16].position = { -1.0f,+1.0f,-1.0f,+1.0f };
	vertexData[17].position = { +1.0f,+1.0f,-1.0f,+1.0f };
	vertexData[18].position = { -1.0f,+1.0f,+1.0f,+1.0f };
	vertexData[19].position = { +1.0f,+1.0f,+1.0f,+1.0f };
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

	indexData[ 9] = 6;
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


	//vertexData[0].texcoord = { vertexData[0].position.x,vertexData[0].position.y,vertexData[0].position.z };
	//vertexData[1].texcoord = { vertexData[1].position.x,vertexData[1].position.y,vertexData[1].position.z };
	//vertexData[2].texcoord = { vertexData[2].position.x,vertexData[2].position.y,vertexData[2].position.z };
	//vertexData[3].texcoord = { vertexData[3].position.x,vertexData[3].position.y,vertexData[3].position.z };
	//vertexData[4].texcoord = { vertexData[4].position.x,vertexData[4].position.y,vertexData[4].position.z };
	//vertexData[5].texcoord = { vertexData[5].position.x,vertexData[5].position.y,vertexData[5].position.z };
	//vertexData[6].texcoord = { vertexData[6].position.x,vertexData[6].position.y,vertexData[6].position.z };
	//vertexData[7].texcoord = { vertexData[7].position.x,vertexData[7].position.y,vertexData[7].position.z };
	//vertexData[8].texcoord = { vertexData[8].position.x,vertexData[8].position.y,vertexData[8].position.z };
	//vertexData[9].texcoord = { vertexData[9].position.x,vertexData[9].position.y,vertexData[9].position.z };
	//vertexData[10].texcoord = { vertexData[10].position.x,vertexData[10].position.y,vertexData[10].position.z };
	//vertexData[11].texcoord = { vertexData[11].position.x,vertexData[11].position.y,vertexData[11].position.z };
	//vertexData[12].texcoord = { vertexData[12].position.x,vertexData[12].position.y,vertexData[12].position.z };
	//vertexData[13].texcoord = { vertexData[13].position.x,vertexData[13].position.y,vertexData[13].position.z };
	//vertexData[14].texcoord = { vertexData[14].position.x,vertexData[14].position.y,vertexData[14].position.z };
	//vertexData[15].texcoord = { vertexData[15].position.x,vertexData[15].position.y,vertexData[15].position.z };
	//vertexData[16].texcoord = { vertexData[16].position.x,vertexData[16].position.y,vertexData[16].position.z };
	//vertexData[17].texcoord = { vertexData[17].position.x,vertexData[17].position.y,vertexData[17].position.z };
	//vertexData[18].texcoord = { vertexData[18].position.x,vertexData[18].position.y,vertexData[18].position.z };
	//vertexData[19].texcoord = { vertexData[19].position.x,vertexData[19].position.y,vertexData[19].position.z };
	//vertexData[20].texcoord = { vertexData[20].position.x,vertexData[20].position.y,vertexData[20].position.z };
	//vertexData[21].texcoord = { vertexData[21].position.x,vertexData[21].position.y,vertexData[21].position.z };
	//vertexData[22].texcoord = { vertexData[22].position.x,vertexData[22].position.y,vertexData[22].position.z };
	//vertexData[23].texcoord = { vertexData[23].position.x,vertexData[23].position.y,vertexData[23].position.z };
}
