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

void Model::SkeltonInitialize(ModelCommon* modelCommon, std::string objFilePath, std::string TextureFilePath)
{
	this->modelCommon_ = modelCommon;

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
		UINT(modelData.vertices.size()), 1, 0, 0, 0);
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
			keyframe.value = { -keyAssimp.mValue.x,keyAssimp.mValue.y,keyAssimp.mValue.z };
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
		 joints[joint.index].children.push_back(childIndex);
	 }
	 return joint.index;
}
