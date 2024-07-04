#pragma once
#include "Math/CGVector.h"
#include "Commons/DX12Common.h"
#include "Systems/Debug.h"
#include "Systems/MyImGui.h"
#include "list"
#include <cassert>
#include <d3d12.h>
#include <dxcapi.h>
#include <dxgi1_6.h>
#include <fstream>
#include <sstream>
#include"Sprites/Sprite.h"
#include"Commons/SpriteCommon.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxcompiler.lib")

class SkyBox
{
public:
	void Initialize(const std::string& filename);
	void Update();
	void Draw();

	ComPtr<ID3D12Resource> CreateBufferResource(size_t sizeInBytes);
	void MakeBufferView();
	void InputData();
	void MakeShaderResourceViewInstance();

	struct VertexData
	{
		Vector4 position;
		Vector2 texcoord;
		Vector3 normal;
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
	ModelData modelData;
	struct Material
	{
		Vector4 color;
	};
	struct TransformationMatrix
	{
		Matrix4x4 WVP;
		Matrix4x4 World;
	};
	struct Particles
	{
		EulerTransform transform;
		Vector3 velocity;
		Vector4 color;
		float lifeTime;
		float currentTime;
	};
	struct CameraTransform {
		Vector3 worldPosition;
	};

	CameraTransform* cameraData = nullptr;

private:
	VertexData* vertexData = nullptr;
	MaterialData materialData;
	Material* colorData = nullptr;
	TransformationMatrix* transformationMatrixData = nullptr;
	ComPtr<ID3D12Resource> vertexResource = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	ComPtr<ID3D12Resource> indexResource = nullptr;
	ComPtr<ID3D12Resource> colorResource;
	ComPtr<ID3D12Resource> cameraResource;
	Matrix4x4 projectionMatrix;

};

