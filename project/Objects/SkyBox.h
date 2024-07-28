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
#include"Commons/SkyBoxCommon.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxcompiler.lib")

class SkyBoxCommon;
class SkyBox
{
public:
	void Initialize(SkyBoxCommon* skyBoxCommon, std::string textureFilePath);
	void Update();
	void Draw(SkyBoxCommon*skyboxCommon);

	ComPtr<ID3D12Resource> CreateBufferResource(size_t sizeInBytes);
	void MakeBufferView();
	void InputData();

	struct VertexData
	{
		Vector4 position;
		Vector3 texcoord;
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
		int32_t enableLighting;
		float padding[3];
		Matrix4x4 uvTransform;
		MaterialData material;
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
	SkyBoxCommon* skyBoxCommon_=nullptr;
	HRESULT hr = NULL;

	VertexData* vertexData = nullptr;
	Material* materialData = nullptr;
	Material* colorData = nullptr;
	ComPtr<ID3D12Resource> materialResource = nullptr;

	TransformationMatrix* transformationMatrixData = nullptr;
	ComPtr<ID3D12Resource> vertexResource = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};

	uint32_t* indexData = nullptr;
	ComPtr<ID3D12Resource> indexResource = nullptr;
	D3D12_INDEX_BUFFER_VIEW indexBufferView{};

	ComPtr<ID3D12Resource> colorResource;
	ComPtr<ID3D12Resource> cameraResource;
	ComPtr<ID3D12Resource> transformationMatrixResource;
	Matrix4x4 projectionMatrix;
	EulerTransform transformMatrix = {};
	Matrix4x4 worldViewProjectionMatrix = {};
	Matrix4x4 viewMatrix;

};

