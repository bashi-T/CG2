#pragma once
#include"ModelCommon.h"

class Model
{
private:
	ModelCommon* modelCommon_;
	HRESULT hr = NULL;


	struct VertexData
	{
		Vector4 position;
		Vector2 texcoord;
		Vector3 normal;
	};
	VertexData* vertexData = nullptr;
	ComPtr<ID3D12Resource> vertexResource = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};

	struct Material {
		Vector4 color;
		int32_t enableLighting;
		float padding[3];
		Matrix4x4 uvTransform;
	};
	Material* materialData = nullptr;
	ComPtr<ID3D12Resource> materialResource = nullptr;

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
	ModelData modelData;

	TransformMatrix uvTransform
	{
			{1.0f,1.0f,1.0f},
			{0.0f,0.0f,0.0f},
			{0.0f,0.0f,0.0f},
	};

public:
	void Initialize(ModelCommon* modelCommon, std::string objFilePath, std::string textureFilePath);
	void Draw(ModelCommon* modelCommon);
	ComPtr<ID3D12Resource> CreateBufferResource(ModelCommon* modelCommon, size_t sizeInBytes);
	ModelData LoadObjFile(const std::string& directryPath, const std::string& filename);
	MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);
	void MakeBufferView();


};

