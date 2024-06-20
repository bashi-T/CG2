#include "Model.h"

void Model::ModelInitialize(ModelCommon* modelCommon, std::string objFilePath, std::string TextureFilePath)
{
	this->modelCommon_ = modelCommon;

	modelData = LoadModelFile("Resource", objFilePath);
	vertexResource = CreateBufferResource(modelCommon_, sizeof(VertexData) * modelData.vertices.size());
	materialResource = CreateBufferResource(modelCommon_, sizeof(Material));
	indexResource = CreateBufferResource(modelCommon_, sizeof(uint32_t) * modelData.indices.size());

	MakeBufferView();

	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	indexResource->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
	modelData.material.textureFilePath = TextureFilePath;
	TextureManager::GetInstance()->LoadTexture(TextureFilePath);
	modelData.material.textureIndex = TextureManager::GetInstance()->GetSrvIndex(TextureFilePath);

	materialData[0].color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialData[0].enableLighting = true;
	materialData[0].uvTransform = MakeIdentity4x4();
	materialData[0].shininess = 50.0f; 

	std::memcpy(vertexData, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size());
	std::memcpy(indexData, modelData.indices.data(), sizeof(uint32_t) * modelData.indices.size());
}

void Model::AnimationInitialize(ModelCommon* modelCommon, std::string objFilePath, std::string TextureFilePath)
{
	this->modelCommon_ = modelCommon;

	modelData = LoadModelFile("Resource", objFilePath);
	animation = LoadAnimationFile("Resource", objFilePath);
	vertexResource = CreateBufferResource(modelCommon_, sizeof(VertexData) * modelData.vertices.size());
	materialResource = CreateBufferResource(modelCommon_, sizeof(Material));
	indexResource = CreateBufferResource(modelCommon_, sizeof(uint32_t) * modelData.indices.size());

	MakeBufferView();

	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	indexResource->Map(0, nullptr, reinterpret_cast<void**>(&indexData));

	modelData.material.textureFilePath = TextureFilePath;
	TextureManager::GetInstance()->LoadTexture(TextureFilePath);
	modelData.material.textureIndex = TextureManager::GetInstance()->GetSrvIndex(TextureFilePath);

	materialData[0].color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialData[0].enableLighting = true;
	materialData[0].uvTransform = MakeIdentity4x4();
	materialData[0].shininess = 50.0f;

	std::memcpy(vertexData, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size());
	std::memcpy(indexData, modelData.indices.data(), sizeof(uint32_t) * modelData.indices.size());
}

void Model::SkeltonInitialize(ModelCommon* modelCommon, std::string objFilePath, std::string TextureFilePath, SRVManager* srvManager)
{
	this->modelCommon_ = modelCommon;
	this->srvManager_ = srvManager;

	modelData = LoadModelFile("Resource", objFilePath);
	animation = LoadAnimationFile("Resource", objFilePath);
	skelton = CreateSkelton(modelData.rootNode);
	vertexResource = CreateBufferResource(modelCommon_, sizeof(VertexData) * modelData.vertices.size());
	materialResource = CreateBufferResource(modelCommon_, sizeof(Material));
	indexResource = CreateBufferResource(modelCommon_, sizeof(uint32_t) * modelData.indices.size());

	MakeBufferView();

	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	indexResource->Map(0, nullptr, reinterpret_cast<void**>(&indexData));

	modelData.material.textureFilePath = TextureFilePath;
	TextureManager::GetInstance()->LoadTexture(TextureFilePath);
	modelData.material.textureIndex = TextureManager::GetInstance()->GetSrvIndex(TextureFilePath);

	materialData[0].color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialData[0].enableLighting = true;
	materialData[0].uvTransform = MakeIdentity4x4();
	materialData[0].shininess = 50.0f;

	std::memcpy(vertexData, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size());
	std::memcpy(indexData, modelData.indices.data(), sizeof(uint32_t) * modelData.indices.size());

	CreateSkinCluster(skelton, modelData, srvManager_->GetSrvDescriptorHeap(),
		modelCommon_->GetDx12Common()->GetDevice()->
		GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
}

void Model::Draw(ModelCommon* modelCommon, SRVManager* srvManager)
{
	this->modelCommon_ = modelCommon;
	this->srvManager_ = srvManager;
	Matrix4x4 uvTransformMatrix = MakeScaleMatrix(uvTransform.scale);
	uvTransformMatrix = Multiply(uvTransformMatrix, MakerotateZMatrix(uvTransform.rotate.z));
	uvTransformMatrix = Multiply(uvTransformMatrix, MakeTranslateMatrix(uvTransform.translate));
	materialData[0].uvTransform = uvTransformMatrix;

	modelCommon_->GetDx12Common()->GetCommandList().Get()->
		SetGraphicsRootConstantBufferView(
			0, materialResource->GetGPUVirtualAddress());
	modelCommon_->GetDx12Common()->GetCommandList().Get()->
		IASetVertexBuffers(0, 1, &vertexBufferView);
	modelCommon_->GetDx12Common()->GetCommandList().Get()->
		IASetIndexBuffer(&indexBufferView);
	srvManager_->SetGraphicsRootDescriptorTable(
		2, modelData.material.textureIndex);

	modelCommon_->GetDx12Common()->GetCommandList().Get()->DrawIndexedInstanced(
		UINT(modelData.indices.size()), 1, 0, 0, 0);
}

void Model::SkeltonDraw(ModelCommon* modelCommon, SRVManager* srvManager)
{
	this->modelCommon_ = modelCommon;
	this->srvManager_ = srvManager;
	Matrix4x4 uvTransformMatrix = MakeScaleMatrix(uvTransform.scale);
	uvTransformMatrix = Multiply(uvTransformMatrix, MakerotateZMatrix(uvTransform.rotate.z));
	uvTransformMatrix = Multiply(uvTransformMatrix, MakeTranslateMatrix(uvTransform.translate));
	materialData[0].uvTransform = uvTransformMatrix;

	modelCommon_->GetDx12Common()->GetCommandList().Get()->
		SetGraphicsRootConstantBufferView(
			0, materialResource->GetGPUVirtualAddress());
	D3D12_VERTEX_BUFFER_VIEW vbvs[2]
	{
		vertexBufferView,
		skinCluster.influenceBufferView
	};
	modelCommon_->GetDx12Common()->GetCommandList().Get()->
		IASetVertexBuffers(0, 2, vbvs);//開始スロット番号、使用スロット数、vbv配列へのポインタ
	modelCommon_->GetDx12Common()->GetCommandList().Get()->
		IASetIndexBuffer(&indexBufferView);
	DX12Common::GetInstance()->GetCommandList().Get()->
		SetGraphicsRootDescriptorTable(
			5, skinCluster.paletteSrvHandle.second);
	srvManager_->SetGraphicsRootDescriptorTable(
		2, modelData.material.textureIndex); 

	modelCommon_->GetDx12Common()->GetCommandList().Get()->DrawIndexedInstanced(
		UINT(modelData.indices.size()), 1, 0, 0, 0);
}

void Model::Memcpy()
{
	std::memcpy(vertexData, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size());
}

ComPtr<ID3D12Resource> Model::CreateBufferResource(ModelCommon* modelCommon, size_t sizeInBytes)
{
	this->modelCommon_ = modelCommon;
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

	hr = modelCommon_->GetDx12Common()->GetDevice().Get()->CreateCommittedResource(
		&uploadHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&ResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&Resource));
	assert(SUCCEEDED(hr));
	return Resource;
}

Model::ModelData Model::LoadModelFile(const std::string& directryPath, const std::string& filename)
{
	ModelData modelData;
	Assimp::Importer importer;
	std::string file = directryPath + "/" + filename;
	const aiScene* scene = importer.ReadFile(file.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs);
	assert(scene->HasMeshes());

	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex)
	{
		aiMesh* mesh = scene->mMeshes[meshIndex];
		assert(mesh->HasNormals());
		assert(mesh->HasTextureCoords(0));
		modelData.vertices.resize(mesh->mNumVertices);
		for (uint32_t vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex)
		{
			aiVector3D& position = mesh->mVertices[vertexIndex];
			aiVector3D& normal = mesh->mNormals[vertexIndex];
			aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];

			modelData.vertices[vertexIndex].position = {-position.x,position.y,position.z,1.0f};
			modelData.vertices[vertexIndex].normal = { -normal.x,normal.y,normal.z };
			modelData.vertices[vertexIndex].texcoord = { texcoord.x,texcoord.y };
		}

		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex)
		{
			aiFace& face = mesh->mFaces[faceIndex];
			assert(face.mNumIndices == 3);

			for (uint32_t element = 0; element < face.mNumIndices; ++element)
			{
				uint32_t vertexIndex = face.mIndices[element];
				modelData.indices.push_back(vertexIndex);
			}
		}
		for (uint32_t boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
		{
			aiBone* bone = mesh->mBones[boneIndex];//meshに関連したjointから情報を得る
			std::string jointName = bone->mName.C_Str();
			JointWeightData& jointWeightData = modelData.skinClusterData[jointName];

			aiMatrix4x4 bindPoseMatrixAssimp = bone->mOffsetMatrix.Inverse();//BindPoseMatrixにもどす
			aiVector3D scale, translate;
			aiQuaternion rotate;
			bindPoseMatrixAssimp.Decompose(scale, rotate, translate);//成分抽出
			Matrix4x4 bindPoseMatrix = MakeAffineMatrix(//左手系BindPoseMatrix
				{ scale.x,scale.y,scale.z },
				{ rotate.x,-rotate.y,-rotate.z,rotate.w },
				{ -translate.x,translate.y,translate.z });
			jointWeightData.inverseBindPoseMatrix = Inverse(bindPoseMatrix);//InverseBindPoseMatrix

			for (uint32_t weightIndex = 0; weightIndex < bone->mNumWeights; ++weightIndex)//jointに関連した頂点のweightとindexを抽出・格納
			{
				jointWeightData.vertexWeights.push_back(
					{ bone->mWeights[weightIndex].mWeight,bone->mWeights[weightIndex].mVertexId });
			}
		}

	}

	for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex)
	{
		aiMaterial* material = scene->mMaterials[materialIndex];
		if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0)
		{
			aiString textureFilePath;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
			modelData.material.textureFilePath = directryPath + "/" + textureFilePath.C_Str();
		}
	}

	modelData.rootNode = ReadNode(scene->mRootNode);
	return modelData;
}

//Model::MaterialData Model::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename)
//{
//	MaterialData materialData;
//	std::string line;
//
//	std::ifstream file(directoryPath + "/" + filename);
//	assert(file.is_open());
//
//	while (std::getline(file, line)) {
//		std::string identifier;
//		std::stringstream s(line);
//		s >> identifier;
//
//		if (identifier == "map_Kd") {
//			std::string textureFilename;
//			s >> textureFilename;
//			materialData.textureFilePath = directoryPath + "/" + textureFilename;
//		}
//	}
//	return materialData;
//}

void Model::MakeBufferView()
{
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * modelData.vertices.size());
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	indexBufferView.BufferLocation = indexResource->GetGPUVirtualAddress();
	indexBufferView.SizeInBytes = sizeof(uint32_t) * modelData.indices.size();
	indexBufferView.Format = DXGI_FORMAT_R32_UINT;
}

Model::Node Model::ReadNode(aiNode* node)
{
	Node result;
	aiVector3D scale, translate;
	aiQuaternion rotate;
	node->mTransformation.Decompose(scale, rotate, translate);
	result.qTransform.scale = { scale.x,scale.y,scale.z };
	result.qTransform.rotate = { rotate.x,-rotate.y,-rotate.z,rotate.w };
	result.qTransform.translate = { -translate.x,translate.y,translate.z };
	result.localMatrix = 
		MakeAffineMatrix(result.qTransform.scale, result.qTransform.rotate, result.qTransform.translate);

	result.name = node->mName.C_Str();
	result.children.resize(node->mNumChildren);
	for (uint32_t childIndex = 0; childIndex < node->mNumChildren; ++childIndex)
	{
		result.children[childIndex] = ReadNode(node->mChildren[childIndex]);
	}
	return result;
}

Model::Animation Model::LoadAnimationFile(const std::string& directoryPath, const std::string& filename)
{
	Animation animation;
	Assimp::Importer importer;
	std::string filePath = directoryPath + "/" + filename;
	const aiScene* scene = importer.ReadFile(filePath.c_str(), 0);
	assert(scene->mNumAnimations != 0);//animationがないと止まる
	aiAnimation* animationAssimp = scene->mAnimations[0];
	animation.duration = float(animationAssimp->mDuration / animationAssimp->mTicksPerSecond);

	for (uint32_t channelIndex = 0; channelIndex < animationAssimp->mNumChannels; ++channelIndex)
	{
		aiNodeAnim* nodeAnimationAssimp = animationAssimp->mChannels[channelIndex];
		NodeAnimation& nodeAnimation = animation.nodeAnimations[nodeAnimationAssimp->mNodeName.C_Str()];
	
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumPositionKeys; ++keyIndex)
		{
			aiVectorKey& keyAssimp = nodeAnimationAssimp->mPositionKeys[keyIndex];
			KeyFrameVector3 keyframe;
			keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);
			keyframe.value = { -keyAssimp.mValue.x,keyAssimp.mValue.y,keyAssimp.mValue.z };
			nodeAnimation.translate.keyframes.push_back(keyframe);
		}

		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumRotationKeys; ++keyIndex)
		{
			aiQuatKey& keyAssimp = nodeAnimationAssimp->mRotationKeys[keyIndex];
			KeyFrameQuaternion keyframe;
			keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);
			keyframe.value = { keyAssimp.mValue.x,-keyAssimp.mValue.y,-keyAssimp.mValue.z,keyAssimp.mValue.w };
			nodeAnimation.rotate.keyframes.push_back(keyframe);
		}

		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumScalingKeys; ++keyIndex)
		{
			aiVectorKey& keyAssimp = nodeAnimationAssimp->mScalingKeys[keyIndex];
			KeyFrameVector3 keyframe;
			keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);
			keyframe.value = { keyAssimp.mValue.x,keyAssimp.mValue.y,keyAssimp.mValue.z };
			nodeAnimation.scale.keyframes.push_back(keyframe);
		}
	}
	return animation;
}

Model::Skelton Model::CreateSkelton(const Node& rootNode)
{
	Skelton skelton;
	skelton.root = CreateJoint(rootNode, {}, skelton.joints);
	for (const Joint& joint : skelton.joints)
	{
		skelton.jointMap.emplace(joint.name, joint.index);
	}
	return skelton;
}

int32_t Model::CreateJoint(const Node& node, const std::optional<int32_t>parent, std::vector<Joint>& joints)
{
	 Joint joint;
	 joint.name = node.name;
	 joint.localMatrix = node.localMatrix;
	 joint.skeltonSpaceMatrix = MakeIdentity4x4();
	 joint.transform = node.qTransform;
	 joint.index = int32_t(joints.size());
	 joint.parent = parent;
	 joints.push_back(joint);
	 for (const Node& child : node.children)
	 {
		 int32_t childIndex = CreateJoint(child, joint.index, joints);
	 }
	 return joint.index;
}

Model::SkinCluster Model::CreateSkinCluster(const Skelton& skelton, const ModelData& modelData,
	const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize)
{
	WellForGPU* mappedParette = nullptr;
	//parette用resource作成
	skinCluster.paletteResource = CreateBufferResource(modelCommon_, sizeof(WellForGPU) * skelton.joints.size());
	skinCluster.paletteResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedParette));
	skinCluster.mappedPalette = { mappedParette,skelton.joints.size() };
	uint32_t index = srvManager_->Allocate();

	//skinCluster.paletteSrvHandle.first = modelCommon_->GetDx12Common()->GetCPUDescriptorHandle(descriptorHeap.Get(), descriptorSize, 0);
	//skinCluster.paletteSrvHandle.second = modelCommon_->GetDx12Common()->GetGPUDescriptorHandle(descriptorHeap.Get(), descriptorSize, 0);
	skinCluster.paletteSrvHandle.first = srvManager_->GetCPUDescriptorHandle(index);
	skinCluster.paletteSrvHandle.second = srvManager_->GetGPUDescriptorHandle(index);

	//parette用srv作成　structuredBufferでアクセスできるように
	D3D12_SHADER_RESOURCE_VIEW_DESC paletteSrvDesc{};
	paletteSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
	paletteSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	paletteSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	paletteSrvDesc.Buffer.FirstElement = 0;
	paletteSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	paletteSrvDesc.Buffer.NumElements = UINT(skelton.joints.size());
	paletteSrvDesc.Buffer.StructureByteStride = sizeof(WellForGPU);

	modelCommon_->GetDx12Common()->GetDevice().Get()->CreateShaderResourceView(
		skinCluster.paletteResource.Get(), &paletteSrvDesc, skinCluster.paletteSrvHandle.first);

	VertexInfluence* mappedInfluence = nullptr;
	skinCluster.influenceResource = CreateBufferResource(modelCommon_, sizeof(VertexInfluence) * modelData.vertices.size());
	skinCluster.influenceResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedInfluence));
	std::memset(mappedInfluence, 0, sizeof(VertexInfluence) * modelData.vertices.size());
	skinCluster.mappedInfluence = { mappedInfluence,modelData.vertices.size() };

	skinCluster.influenceBufferView.BufferLocation = skinCluster.influenceResource->GetGPUVirtualAddress();
	skinCluster.influenceBufferView.SizeInBytes = UINT(sizeof(VertexInfluence) * modelData.vertices.size());
	skinCluster.influenceBufferView.StrideInBytes = sizeof(VertexInfluence);

	skinCluster.inverseBindPoseMatrices.resize(skelton.joints.size());
	for (Matrix4x4& inverseBindPoseMatrix : skinCluster.inverseBindPoseMatrices)
	{
		inverseBindPoseMatrix = MakeIdentity4x4();
	}

	for (const auto& jointWeight : modelData.skinClusterData)
	{
		auto it = skelton.jointMap.find(jointWeight.first);
		if (it == skelton.jointMap.end())
		{
			continue;
		}
		skinCluster.inverseBindPoseMatrices[(*it).second] = jointWeight.second.inverseBindPoseMatrix;
		for (const auto& vertexWeight : jointWeight.second.vertexWeights)
		{
			auto& currentInfluence = skinCluster.mappedInfluence[vertexWeight.vertexIndex];
			for (uint32_t index = 0; index < kNumMaxInfluence; ++index)
			{
				if (currentInfluence.weights[index] == 0.0f)
				{
					currentInfluence.weights[index] = vertexWeight.weight;
					currentInfluence.jointIndices[index] = (*it).second;
					break;
				}
			}
		}
	}

	return skinCluster;
}
