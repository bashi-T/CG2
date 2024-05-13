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

class Mesh
{
public:
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	~Mesh();
	void Initialize(const std::string& filename, int32_t width, int32_t height);
	void Update();
	void Draw(int32_t width, int32_t height);
	ComPtr<IDxcBlob> CompileShader(
	    const std::wstring& filePath,
		const wchar_t* profile,
		IDxcUtils* dxcUtils,
		IDxcCompiler3* dxcCompiler,
		IDxcIncludeHandler* includeHandler);

	void DrawTriangle(
	    Vector4 Top, Vector4 Right, Vector4 Left, Vector4 color, Vector2 coordTop,
	    Vector2 coordRight, Vector2 coordLeft, bool useWorldMap);
	void DrawSphere(
		const Sphere& sphere_, Vector4 color, bool useWorldMap, int32_t width, int32_t height);
	void ResetDXC();
	void MakePSO();
	ComPtr<ID3D12Resource> CreateBufferResource(size_t sizeInBytes);
	void MakeBufferView();
	//void InputDataTriangle(
	//    Vector4 Top, Vector4 Right, Vector4 Left, Vector4 color, Vector2 coordTop,
	//    Vector2 coordRight, Vector2 coordLeft);
	void InputDataSphere(
		Vector4 LeftTop, Vector4 RightTop, Vector4 RightBottom, Vector4 LeftBottom, Vector4 color,
		Vector2 coordLeftTop, Vector2 coordRightTop, Vector2 coordRightBottom,
		Vector2 coordLeftBottom, uint32_t count, int32_t width, int32_t height);

	void MakeShaderResourceView(const DirectX::TexMetadata& metadata, const DirectX::TexMetadata& metadata2);

	struct VertexData
	{
		Vector4 position;
		Vector2 texcoord;
		Vector3 normal;
	};
	struct MaterialData
	{
		std::string textureFilePath;
	};
	
	struct ModelData
	{
		std::vector<VertexData> vertices;
		MaterialData material;
	};
	ModelData modelData;

	DirectX::ScratchImage LoadTexture(const std::string& filePath);
	ComPtr<ID3D12Resource> CreateTextureResource(
		ID3D12Device* device,
		const DirectX::TexMetadata& metadata);
	void UploadTextureData(
		ID3D12Resource* texture,
		const DirectX::ScratchImage& mipImages,
		const DirectX::TexMetadata& metadata);

	//void MeshRelease();

	ComPtr<ID3D12Resource> GetVertexResource() { return vertexResource; }
	EulerTransform GetCameraTransform() { return cameraTransform; }
	Matrix4x4 GetCameraMatrix() { return cameraMatrix; }
	D3D12_CPU_DESCRIPTOR_HANDLE GetTextureSrvHandleCPU() { return textureSrvHandleCPU; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetTextureSrvHandleGPU() { return textureSrvHandleGPU; }
	D3D12_CPU_DESCRIPTOR_HANDLE GetTextureSrvHandleCPU2() { return textureSrvHandleCPU2; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetTextureSrvHandleGPU2() { return textureSrvHandleGPU2; }

	struct DirectionalLight {
		Vector4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
		Vector3 direction = { 0.0f, -1.0f, 0.0f };
		float intensity = 1.0f;
	};
	DirectionalLight* DrawDirectionalLightData() { return DirectionalLightData; }

private:
	Mesh* mesh_;
	Debug* debug_;
	WinAPP* sWinApp;
	MyImGui* imgui_;
	SpriteCommon* spriteCom_;
	HRESULT hr = NULL;
	EulerTransform transformMatrix;
	EulerTransform transformMatrixSphere;

	ComPtr<ID3D12Resource> transformationMatrixResource;
	ComPtr<ID3D12Resource> transformationMatrixResourceSphere;

	ComPtr<IDxcUtils> dxcUtils = nullptr;
	ComPtr<IDxcCompiler3> dxcCompiler = nullptr;
	ComPtr<IDxcIncludeHandler> includeHandler = nullptr;
	ComPtr<ID3D12RootSignature> rootSignature = nullptr;
	ComPtr<ID3D12PipelineState> graphicsPipelineState = NULL;
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature_{};
	ComPtr<ID3DBlob> signatureBlob = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	ComPtr<IDxcBlob> pixelShaderBlob = nullptr;
	ComPtr<IDxcBlob> vertexShaderBlob = nullptr;

	ComPtr<ID3D12Resource> vertexResource = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};

	ComPtr<ID3D12Resource> vertexResourceSphere = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSphere{};


	ComPtr<ID3D12Resource> indexResourceSphere = nullptr;
	D3D12_INDEX_BUFFER_VIEW indexBufferViewSphere{};


	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	D3D12_BLEND_DESC blendDesc{};
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	ComPtr<ID3D12Resource> materialResource;
	ComPtr<ID3D12Resource> materialResourceSphere;

	ComPtr<ID3D12Resource> directionalLightResource;
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};

	struct Material {
		Vector4 color;
		int32_t enableLighting;
		float padding[3];
		Matrix4x4 uvTransform;
	};
	struct TransformationMatrix {
		Matrix4x4 WVP;
		Matrix4x4 World;
	};
	EulerTransform uvTransformSphere{
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f},
	};

	EulerTransform cameraTransform;
	DirectionalLight* DirectionalLightData = nullptr;
	TransformationMatrix* transformationMatrixDataSphere = nullptr;

	Matrix4x4 cameraMatrix;
	Matrix4x4 viewMatrix;
	Matrix4x4 viewMatrixSphere;

	Matrix4x4 projectionMatrix;
	Matrix4x4 projectionMatrixSphere;

	Matrix4x4 ViewProjectionMatrix;

	Matrix4x4 worldViewProjectionMatrix;
	Matrix4x4 worldViewProjectionMatrixSphere;

	uint32_t kSubdivision = 16;

	ComPtr<ID3D12Resource> textureResource;
	ComPtr<ID3D12Resource> textureResource2;

	DirectX::ScratchImage mipImages;
	DirectX::ScratchImage mipImages2;
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU;
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU;
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU2;
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU2;

	const int32_t kNumTriangle = 1;
	Vector4 ColorSphere[1];

	Sphere sphere = { { 0.0f,0.0f,0.0f },1.0f };

	Vector4 Top[1];
	Vector4 Left[1];
	Vector4 Right[1];
	Vector4 Color[1];
	Vector2 texcoordTop[1];
	Vector2 texcoordLeft[1];
	Vector2 texcoordRight[1];


};
