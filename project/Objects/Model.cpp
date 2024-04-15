#include "Model.h"

void Model::Initialize(ModelCommon* modelCommon,std::string objFilePath, std::string TextureFilePath)
{
	this->modelCommon_ = modelCommon;

	modelData = LoadObjFile("Resource", objFilePath);

	vertexResource = CreateBufferResource(modelCommon_, sizeof(VertexData) * modelData.vertices.size());
	materialResource = CreateBufferResource(modelCommon_, sizeof(Material));

	MakeBufferView();

	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));

	modelData.material.textureFilePath = TextureFilePath;
	TextureManager::GetInstance()->LoadTexture(TextureFilePath);
	modelData.material.textureIndex = TextureManager::GetInstance()->GetSrvIndex(TextureFilePath);

	materialData[0].color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialData[0].enableLighting = true;
	materialData[0].uvTransform = MakeIdentity4x4();
	materialData[0].shininess = 50.0f;

	std::memcpy(vertexData, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size());
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
	modelCommon_->GetDx12Common()->GetCommandList().Get()->IASetVertexBuffers(
		0, 1, &vertexBufferView);
	srvManager_->SetGraphicsRootDescriptorTable(
		2, modelData.material.textureIndex);

	modelCommon_->GetDx12Common()->GetCommandList().Get()->DrawInstanced(
		UINT(modelData.vertices.size()), 1, 0, 0);
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

Model::ModelData Model::LoadObjFile(const std::string& directryPath, const std::string& filename)
{
	ModelData modelData;
	std::vector<Vector4> positions;
	std::vector<Vector3> normals;
	std::vector<Vector2> texcoords;
	std::string line;

	Assimp::Importer importer;
	std::string file(directryPath + "/" + filename);
	const aiScene* scene = importer.ReadFile(file.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs);
	assert(scene->HasMeshes());

	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex)
	{
		aiMesh* mesh = scene->mMeshes[meshIndex];
		assert(mesh->HasNormals());
		assert(mesh->HasTextureCoords(0));

		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex)
		{
			aiFace& face = mesh->mFaces[faceIndex];
			assert(face.mNumIndices == 3);
			for (uint32_t element = 0; element < face.mNumIndices; ++element)
			{
				uint32_t vertexIndex = face.mIndices[element];
				aiVector3D& position = mesh->mVertices[vertexIndex];
				aiVector3D& normal = mesh->mNormals[vertexIndex];
				aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];
				VertexData vertex;
				vertex.position = { position.x,position.y,position.z,1.0f };
				vertex.normal = { normal.x,normal.y,normal.z };
				vertex.texcoord = { texcoord.x,texcoord.y };
				vertex.position.x *= -1.0f;
				vertex.normal.x *= -1.0f;
				modelData.vertices.push_back(vertex);
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
	return modelData;
}

Model::MaterialData Model::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename)
{
	MaterialData materialData;
	std::string line;

	std::ifstream file(directoryPath + "/" + filename);
	assert(file.is_open());

	while (std::getline(file, line)) {
		std::string identifier;
		std::stringstream s(line);
		s >> identifier;

		if (identifier == "map_Kd") {
			std::string textureFilename;
			s >> textureFilename;
			materialData.textureFilePath = directoryPath + "/" + textureFilename;
		}
	}
	return materialData;
}

void Model::MakeBufferView()
{
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * modelData.vertices.size());
	vertexBufferView.StrideInBytes = sizeof(VertexData);
}
