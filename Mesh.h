#pragma once
#include "CGVector.h"
#include "DX12Common.h"
#include "Debug.h"
#include "MyImGui.h"
#include "list"
#include <cassert>
#include <d3d12.h>
#include <dxcapi.h>
#include <dxgi1_6.h>
#include <fstream>
#include <sstream>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxcompiler.lib")

class Mesh
{
public:
	~Mesh();
	void Initialize(const std::string& filename, int32_t width, int32_t height);
	void ResetDXC();
	void Update();
	Microsoft::WRL::ComPtr<IDxcBlob> CompileShader(
	    const std::wstring& filePath,
		const wchar_t* profile,
		IDxcUtils* dxcUtils,
		IDxcCompiler3* dxcCompiler,
		IDxcIncludeHandler* includeHandler);
	void DrawTriangle(
	    Vector4 Top, Vector4 Right, Vector4 Left, Vector4 color, Vector2 coordTop,
	    Vector2 coordRight, Vector2 coordLeft, bool useWorldMap);
	void DrawSprite(
		Vector4 LeftTop, Vector4 RightTop, Vector4 RightBottom, Vector4 LeftBottom, Vector4 color,
		Vector2 coordLeftTop, Vector2 coordRightTop, Vector2 coordRightBottom,
		Vector2 coordLeftBottom, int32_t width, int32_t height);
	void DrawPlane(
		Vector4 LeftTop, Vector4 RightTop, Vector4 RightBottom, Vector4 LeftBottom, Vector4 color,
		Vector2 coordLeftTop, Vector2 coordRightTop, Vector2 coordRightBottom,
		Vector2 coordLeftBottom, int32_t width, int32_t height, bool useWorldMap);
	void DrawSphere(
		const Sphere& sphere_, Vector4 color, bool useWorldMap, int32_t width, int32_t height);
	void DrawOBJ(
		Vector4 color, bool useWorldMap, int32_t width, int32_t height);

	void MakePSO();
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(size_t sizeInBytes);
	void MakeBufferView();
	void InputDataTriangle(
	    Vector4 Top, Vector4 Right, Vector4 Left, Vector4 color, Vector2 coordTop,
	    Vector2 coordRight, Vector2 coordLeft);
	void InputDataSprite(
		Vector4 LeftTop, Vector4 RightTop, Vector4 RightBottom, Vector4 LeftBottom, Vector4 color,
		Vector2 coordLeftTop, Vector2 coordRightTop, Vector2 coordRightBottom,
		Vector2 coordLeftBottom, int32_t width, int32_t height);
	void InputDataPlane(
		Vector4 LeftTop, Vector4 RightTop, Vector4 RightBottom, Vector4 LeftBottom, Vector4 color,
		Vector2 coordLeftTop, Vector2 coordRightTop, Vector2 coordRightBottom,
		Vector2 coordLeftBottom, int32_t width, int32_t height);
	void InputDataSphere(
		Vector4 LeftTop, Vector4 RightTop, Vector4 RightBottom, Vector4 LeftBottom, Vector4 color,
		Vector2 coordLeftTop, Vector2 coordRightTop, Vector2 coordRightBottom,
		Vector2 coordLeftBottom, uint32_t count, int32_t width, int32_t height);

	void MakeShaderResourceView(const DirectX::TexMetadata& metadata, const DirectX::TexMetadata& metadata2);
	
	void MakeShaderResourceViewStructuredBuffer();
	
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
	MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);
	
	struct ModelData
	{
		std::vector<VertexData> vertices;
		MaterialData material;
	};
	ModelData LoadObjFile(const std::string& directryPath, const std::string& filename);

	DirectX::ScratchImage LoadTexture(const std::string& filePath);
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(
		ID3D12Device* device,
		const DirectX::TexMetadata& metadata);
	void UploadTextureData(
		ID3D12Resource* texture,
		const DirectX::ScratchImage& mipImages,
		const DirectX::TexMetadata& metadata);

	//void MeshRelease();

	Microsoft::WRL::ComPtr<ID3D12Resource> GetVertexResource() { return vertexResource; }
	TransformMatrix GetCameraTransform() { return cameraTransform; }
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
	Debug* debug_;
	WinAPP* sWinApp;
	MyImGui* imgui_;
	HRESULT hr = NULL;
	TransformMatrix transformMatrix;
	TransformMatrix transformMatrixPlane;
	TransformMatrix transformMatrixSprite;
	TransformMatrix transformMatrixSphere;
	TransformMatrix transformMatrixObj;

	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResourcePlane;
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResourceSprite;
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResourceSphere;
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResourceObj;

	Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils = nullptr;
	Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler = nullptr;
	Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler = nullptr;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState = NULL;
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature_{};
	Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob = nullptr;
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResourcePlane = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewPlane{};

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResourceSprite = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSprite{};

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResourceSphere = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSphere{};

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResourceObj = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewObj{};

	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource = nullptr;
	D3D12_INDEX_BUFFER_VIEW indexBufferView{};

	Microsoft::WRL::ComPtr<ID3D12Resource> indexResourceSprite = nullptr;
	D3D12_INDEX_BUFFER_VIEW indexBufferViewSprite{};

	Microsoft::WRL::ComPtr<ID3D12Resource> indexResourcePlane = nullptr;
	D3D12_INDEX_BUFFER_VIEW indexBufferViewPlane{};

	Microsoft::WRL::ComPtr<ID3D12Resource> indexResourceSphere = nullptr;
	D3D12_INDEX_BUFFER_VIEW indexBufferViewSphere{};

	Microsoft::WRL::ComPtr<ID3D12Resource> indexResourceObj = nullptr;
	D3D12_INDEX_BUFFER_VIEW indexBufferViewObj{};

	D3D12_VIEWPORT viewport{};
	D3D12_RECT scissorRect{};
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	D3D12_BLEND_DESC blendDesc{};
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResourcePlane;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResourceSprite;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResourceSphere;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResourceObj;

	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource;
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
	TransformMatrix uvTransform{
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f},
	};
	TransformMatrix uvTransformSprite{
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f},
	};
	TransformMatrix uvTransformPlane{
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f},
	};
	TransformMatrix uvTransformSphere{
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f},
	};
	TransformMatrix uvTransformObj{
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f},
	};
	TransformMatrix cameraTransform;
	DirectionalLight* DirectionalLightData = nullptr;
	TransformationMatrix* transformationMatrixDataPlane = nullptr;
	TransformationMatrix* transformationMatrixDataSprite = nullptr;
	TransformationMatrix* transformationMatrixDataSphere = nullptr;
	TransformationMatrix* transformationMatrixDataObj = nullptr;

	Matrix4x4 cameraMatrix;
	Matrix4x4 viewMatrix;
	Matrix4x4 viewMatrixPlane;
	Matrix4x4 viewMatrixSprite;
	Matrix4x4 viewMatrixSphere;
	Matrix4x4 viewMatrixObj;

	Matrix4x4 projectionMatrix;
	Matrix4x4 projectionMatrixPlane;
	Matrix4x4 projectionMatrixSprite;
	Matrix4x4 projectionMatrixSphere;
	Matrix4x4 projectionMatrixObj;

	Matrix4x4 worldViewProjectionMatrix;
	Matrix4x4 worldViewProjectionMatrixPlane;
	Matrix4x4 worldViewProjectionMatrixSprite;
	Matrix4x4 worldViewProjectionMatrixSphere;
	Matrix4x4 worldViewProjectionMatrixObj;

	const uint32_t kSubdivision = 16;
	ModelData modelData;

	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource2;

	DirectX::ScratchImage mipImages;
	DirectX::ScratchImage mipImages2;
	DirectX::ScratchImage texmipImages;
	DirectX::ScratchImage texmipImages2;
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU;
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU;
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU2;
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU2;

	const uint32_t kNumInstance = 10;
	D3D12_CPU_DESCRIPTOR_HANDLE instancingSrvHandleCPU;
	D3D12_GPU_DESCRIPTOR_HANDLE instancingSrvHandleGPU;
	TransformMatrix transformsInstancing[10];
	TransformationMatrix* instancingData = nullptr;
	ComPtr<ID3D12Resource> instancingResource;
};
