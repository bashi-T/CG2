#pragma once
#include"Commons/ModelCommon.h"
#include"Managers/SRVManager.h"

class Model
{
public:
	struct VertexData
	{
		Vector4 position;
		Vector2 texcoord;
		Vector3 normal;
		Vector3 worldPosition;
	};
	struct Material {
		Vector4 color;
		int32_t enableLighting;
		float padding[3];
		Matrix4x4 uvTransform;
		float shininess;
	};
	struct MaterialData
	{
		std::string textureFilePath;
		uint32_t textureIndex = 0;
	};
	struct ModelData
	{
		std::vector<VertexData> vertices;
		MaterialData material;
	};
	void Initialize(std::string objFilePath, std::string TextureFilePath);
	void Draw(ModelCommon* modelCommon, SRVManager* srvManager);
	void Memcpy();
	void InputTexture(std::string TextureFilePath);
	ComPtr<ID3D12Resource> CreateBufferResource(ModelCommon* modelCommon, size_t sizeInBytes);
	ModelData LoadObjFile(const std::string& directryPath, const std::string& filename);
	MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);
	void MakeBufferView();
	ModelData* GetModelData() { return &modelData; }

private:
	ModelData modelData;
	ModelCommon* modelCommon_;
	SRVManager* srvManager_ = nullptr;
	HRESULT hr = NULL;

	VertexData* vertexData = nullptr;
	ComPtr<ID3D12Resource> vertexResource = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};

	Material* materialData = nullptr;
	ComPtr<ID3D12Resource> materialResource = nullptr;

	TransformMatrix uvTransform
	{
			{1.0f,1.0f,1.0f},
			{0.0f,0.0f,0.0f},
			{0.0f,0.0f,0.0f},
	};
};

