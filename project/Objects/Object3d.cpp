#include "Object3d.h"
#include "Commons/Object3dCommon.h"

void Object3d::Initialize(Object3dCommon* object3dCommon, SRVManager* srvManager)
{
	this->object3dCommon_ = object3dCommon;
	this->srvManager = srvManager;

	transformationMatrixResource = CreateBufferResource(object3dCommon_,sizeof(TransformationMatrix));
	directionalLightResource = CreateBufferResource(object3dCommon_, sizeof(DirectionalLight));
	this->camera = object3dCommon_->GetDefaultCamera();
	cameraResource = CreateBufferResource(object3dCommon_, sizeof(CameraTransform));
	transformMatrix =
	{
	{1.0f, 1.0f, 1.0f},
	{0.0f, 0.0f, 0.0f},
	{0.0f, 0.0f, 0.0f}
	};

	transformationMatrixResource->Map(
		0, nullptr, reinterpret_cast<void**>(&transformationMatrixData));
    directionalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&DirectionalLightData));
	cameraResource->Map(0, nullptr, reinterpret_cast<void**>(&cameraData));
    DirectionalLightData->color = { 1.0f, 1.0f, 1.0f, 1.0f };
    DirectionalLightData->direction = { 0.0f, -1.0f, 0.0f };
    DirectionalLightData->intensity = 1.0f;
}

void Object3d::Update()
{
	transformMatrix.rotate.y -= 0.02f;
		Matrix4x4 worldMatrix = MakeAffineMatrix(
		transformMatrix.scale, transformMatrix.rotate, transformMatrix.translate);

	if (camera)
	{
		const Matrix4x4& viewProjectionMatrix = camera->GetViewProjectionMatrix();
		worldViewProjectionMatrix = Multiply(worldMatrix, viewProjectionMatrix);
	}

	cameraData->worldPosition =
	{
		camera->GetWorldMatrix().m[3][0],
	    camera->GetWorldMatrix().m[3][1],
	    camera->GetWorldMatrix().m[3][2]
	};
	transformationMatrixData->WVP = worldViewProjectionMatrix;
	transformationMatrixData->World = worldMatrix;
}

void Object3d::Draw(Object3dCommon* object3dCommon, ModelCommon* modelCommon)
{
	this->object3dCommon_ = object3dCommon;
	this->modelCommon_ = modelCommon;

	object3dCommon_->GetDx12Common()->GetCommandList().Get()->
		SetPipelineState(object3dCommon_->GetGraphicsPipelineState().Get());

	object3dCommon_->GetDx12Common()->GetCommandList().Get()->
		SetGraphicsRootSignature(object3dCommon_->GetRootSignature().Get());

	object3dCommon_->GetDx12Common()->GetCommandList().Get()->
		IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3D12_CPU_DESCRIPTOR_HANDLE rtv = object3dCommon_->GetDx12Common()->GetRtvHandles(
		srvManager->GetBackBufferIndex());

	D3D12_CPU_DESCRIPTOR_HANDLE dsv = object3dCommon_->GetDx12Common()->GetDsvHandle();
	object3dCommon_->GetDx12Common()->GetCommandList().Get()->
		OMSetRenderTargets(1, &rtv, false, &dsv);

	object3dCommon_->GetDx12Common()->GetCommandList().Get()->
		SetGraphicsRootConstantBufferView(
		1, transformationMatrixResource->GetGPUVirtualAddress());

	object3dCommon_->GetDx12Common()->GetCommandList().Get()->
		SetGraphicsRootConstantBufferView(
		3, directionalLightResource->GetGPUVirtualAddress());

	object3dCommon_->GetDx12Common()->GetCommandList()->
		SetGraphicsRootConstantBufferView(
		4, cameraResource->GetGPUVirtualAddress());

	if (model_)
	{
		model_->Draw(modelCommon_,srvManager);
	}
}

ComPtr<ID3D12Resource> Object3d::CreateBufferResource(Object3dCommon* object3dCommon, size_t sizeInBytes)
{
	this->object3dCommon_ = object3dCommon;
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};

	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
	D3D12_RESOURCE_DESC ResourceDesc{};

	ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;

	ResourceDesc.Width = sizeInBytes;

	ResourceDesc.Height = 1;
	ResourceDesc.DepthOrArraySize = 1;
	ResourceDesc.MipLevels = 1;
	ResourceDesc.SampleDesc.Count = 1;

	ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	ComPtr<ID3D12Resource> Resource = nullptr;

	hr = object3dCommon_->GetDx12Common()->GetDevice().Get()->CreateCommittedResource(
		&uploadHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&ResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&Resource));
	assert(SUCCEEDED(hr));
	return Resource;
}

void Object3d::SetModel(const std::string& filePath)
{
	model_ = ModelManager::GetInstance()->FindModel(filePath);
}
