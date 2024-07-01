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
    directionalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));
	cameraResource->Map(0, nullptr, reinterpret_cast<void**>(&cameraData));
	directionalLightData->color = directionalLight.color;
	directionalLightData->direction = directionalLight.direction;
	directionalLightData->intensity = directionalLight.intensity;
}

void Object3d::Update(Camera* camera)
{
	//transformMatrix.rotate.y -= 0.02f;
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
	directionalLightData->color = directionalLight.color;
	directionalLightData->direction = directionalLight.direction;
	directionalLightData->intensity = directionalLight.intensity;

	transformationMatrixData->WVP = Multiply(model_->GetModelData()->rootNode.localMatrix, worldViewProjectionMatrix);
	transformationMatrixData->World = Multiply(model_->GetModelData()->rootNode.localMatrix, worldMatrix);
	transformationMatrixData->WorldInverseTranspose = Transpose(Inverse(transformationMatrixData->World));
}

void Object3d::AnimationUpdate(Camera* camera)
{
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

	if (isAnimation == true)
	{
		animationTime += 1.0f / 60.0f;
	}
	animationTime = std::fmod(animationTime, model_->GetAnimation().duration);
	Model::NodeAnimation& rootNodeAnimation = model_->GetAnimation().nodeAnimations[model_->GetModelData()->rootNode.name];
	Vector3 translate = CalculatevalueV(rootNodeAnimation.translate.keyframes, animationTime);
	Quaternion rotate = CalculatevalueQ(rootNodeAnimation.rotate.keyframes, animationTime);
	Vector3 scale = CalculatevalueV(rootNodeAnimation.scale.keyframes, animationTime);
	Matrix4x4 localMatrix = MakeAffineMatrix(scale, rotate, translate);

	transformationMatrixData->WVP = Multiply(localMatrix, worldViewProjectionMatrix);
	transformationMatrixData->World = Multiply(localMatrix, worldMatrix);
	transformationMatrixData->WorldInverseTranspose = Transpose(Inverse(transformationMatrixData->World));
}

void Object3d::SkeltonUpdate(Camera* camera)
{
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
	if(isAnimation==true)
	{
		skeltonAnimationTime += 1.0f / 60.0f;
	}
	//skeltonAnimationTime = 0.5f;
	skeltonAnimationTime = std::fmod(skeltonAnimationTime, model_->GetAnimation().duration);
	ApplyAnimation(model_->GetSkelton(), model_->GetAnimation(), skeltonAnimationTime);

	int i = 0;
	for (Model::Joint& joint : model_->GetSkelton().joints)
	{
		joint.localMatrix = MakeAffineMatrix(joint.transform.scale, joint.transform.rotate, joint.transform.translate);

		i++;
		if (joint.parent)
		{
			joint.skeltonSpaceMatrix = Multiply(joint.localMatrix, model_->GetSkelton().joints[*joint.parent].skeltonSpaceMatrix);
		}
		else
		{
			joint.skeltonSpaceMatrix = joint.localMatrix;
		}
		joint.worldMatrix = Multiply(joint.skeltonSpaceMatrix, worldMatrix);
	}
	transformationMatrixData->WVP = worldViewProjectionMatrix;
	//transformationMatrixData->World = worldViewProjectionMatrix;
	transformationMatrixData->WorldInverseTranspose = Transpose(Inverse(worldViewProjectionMatrix));

	for (size_t jointIndex = 0; jointIndex < model_->GetSkelton().joints.size(); jointIndex++)
	{
		assert(jointIndex < model_->GetSkinCluster().inverseBindPoseMatrices.size());

		model_->GetSkinCluster().mappedPalette[jointIndex].skeltonSpaceMatrix = 
			Multiply(model_->GetSkinCluster().inverseBindPoseMatrices[jointIndex],
				model_->GetSkelton().joints[jointIndex].skeltonSpaceMatrix);
		
		model_->GetSkinCluster().mappedPalette[jointIndex].skeltonSpaceInverseTransposeMatrix =
			Transpose(Inverse(model_->GetSkinCluster().mappedPalette[jointIndex].skeltonSpaceMatrix));
	}

	auto&& test = model_->GetSkinCluster();
	(void)test;
}

void Object3d::Draw(ModelCommon* modelCommon)
{
	this->modelCommon_ = modelCommon;

	object3dCommon_->GetDx12Common()->GetCommandList().Get()->
		SetPipelineState(object3dCommon_->GetGraphicsPipelineStates(0).Get());

	object3dCommon_->GetDx12Common()->GetCommandList().Get()->
		SetGraphicsRootSignature(object3dCommon_->GetRootSignatures(0).Get());

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

void Object3d::SkeltonDraw(ModelCommon* modelCommon)
{
	this->modelCommon_ = modelCommon;

	object3dCommon_->GetDx12Common()->GetCommandList().Get()->
		SetPipelineState(object3dCommon_->GetGraphicsPipelineStates(1).Get());

	object3dCommon_->GetDx12Common()->GetCommandList().Get()->
		SetGraphicsRootSignature(object3dCommon_->GetRootSignatures(1).Get());

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
		model_->SkeltonDraw(modelCommon_, srvManager);
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

Vector3 Object3d::CalculatevalueV(const std::vector<Model::KeyFrameVector3>& keyframes, float time)
{
	assert(!keyframes.empty());//キーがないと止まる
	if (keyframes.size() == 1 || time <= keyframes[0].time)
	{
		return keyframes[0].value;
	}
	for (size_t index = 0; index < keyframes.size() - 1; ++index)
	{
		size_t nextIndex = index + 1;
		if (keyframes[index].time <= time && time <= keyframes[nextIndex].time)
		{
			float t = (time - keyframes[index].time) / (keyframes[nextIndex].time - keyframes[index].time);
			return Lerp(keyframes[index].value, keyframes[nextIndex].value, t);
		}
		return (*keyframes.rbegin()).value;
	}
}

Quaternion Object3d::CalculatevalueQ(const std::vector<Model::KeyFrameQuaternion>& keyframes, float time)
{
	assert(!keyframes.empty());//キーがないと止まる
	if (keyframes.size() == 1 || time <= keyframes[0].time)
	{
		return keyframes[0].value;
	}
	for (size_t index = 0; index < keyframes.size() - 1; ++index)
	{
		size_t nextIndex = index + 1;
		if (keyframes[index].time <= time && time <= keyframes[nextIndex].time)
		{
			float t = (time - keyframes[index].time) / (keyframes[nextIndex].time - keyframes[index].time);
			return Slerp(keyframes[index].value, keyframes[nextIndex].value, t);
		}
	}
	return (*keyframes.rbegin()).value;

}

void Object3d::ApplyAnimation(Model::Skelton& skelton, const Model::Animation& animation, float animationTime)
{
	for (Model::Joint& joint : skelton.joints)
	{
		if (auto it = animation.nodeAnimations.find(joint.name); it != animation.nodeAnimations.end())
		{
			const Model::NodeAnimation& rootNodeAnimation = (*it).second;
			joint.transform.translate = CalculatevalueV(rootNodeAnimation.translate.keyframes, animationTime);
			joint.transform.rotate = CalculatevalueQ(rootNodeAnimation.rotate.keyframes, animationTime);
			joint.transform.scale = CalculatevalueV(rootNodeAnimation.scale.keyframes, animationTime);
		}
	}
}

void Object3d::SetModel(const std::string& filePath)
{
	model_ = ModelManager::GetInstance()->FindModel(filePath);
}
