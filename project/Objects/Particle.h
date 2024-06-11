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
#include"Camera/Camera.h"
#include"Managers/TextureManager.h"
#include "Commons/Object3dCommon.h"
#include <random>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxcompiler.lib")

class Particle
{
public:
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	~Particle();
	void Initialize(const std::string& filename, SRVManager* srvManager, Object3dCommon* object3dCommon);
	void Update();
	ComPtr<IDxcBlob> CompileShader(
	    const std::wstring& filePath,
		const wchar_t* profile,
		IDxcUtils* dxcUtils,
		IDxcCompiler3* dxcCompiler,
		IDxcIncludeHandler* includeHandler);

	void Draw();
	void ResetDXC();
	void MakePSO();
	ComPtr<ID3D12Resource> CreateBufferResource(size_t sizeInBytes);
	void MakeBufferView();
	void InputData(
		Vector4 TopLeft, Vector4 TopRight, Vector4 BottomRight, Vector4 BottomLeft, Vector4 color,
		Vector2 coordTopLeft, Vector2 coordTopRight, Vector2 coordBottomRight, Vector2 coordBottomLeft);

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
	struct ParticleForGPU
	{
		Matrix4x4 WVP;
		Matrix4x4 World;
		Vector4 color;
	};
	struct CameraTransform {
		Vector3 worldPosition;
	};

	CameraTransform* cameraData = nullptr;

	Particles MakeNewParticle(std::mt19937& randomEngine);

	//void ParticleRelease();

	//ComPtr<ID3D12Resource> GetVertexResource() { return vertexResource; }
	Matrix4x4 GetCameraMatrix() { return cameraMatrix; }
	D3D12_CPU_DESCRIPTOR_HANDLE GetTextureSrvHandleCPU() { return textureSrvHandleCPU; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetTextureSrvHandleGPU() { return textureSrvHandleGPU; }
	D3D12_CPU_DESCRIPTOR_HANDLE GetTextureSrvHandleCPU2() { return textureSrvHandleCPU2; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetTextureSrvHandleGPU2() { return textureSrvHandleGPU2; }
	ParticleForGPU* GetInstancingData() { return instancingData; }
	Particles* GetParticles() { return particles; }
private:
	Debug* debug_;
	WinAPP* sWinApp;
	SRVManager* srvManager = nullptr;
	MyImGui* imgui_;
	SpriteCommon* spriteCom_;
	Object3dCommon* object3dCommon_;
	Camera* camera_;
	DX12Common* dxCommon;
	HRESULT hr = NULL;

	uint32_t kNumInstance = 10;
	uint32_t kNumMaxInstance = 10;
	ComPtr<ID3D12Resource> cameraResource;

	Particles particles[10];

	ComPtr<ID3D12Resource> transformationMatrixResource;

	ComPtr<ID3D12Resource> instancingResource;

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
	ComPtr<ID3D12Resource> indexResource = nullptr;
	D3D12_INDEX_BUFFER_VIEW indexBufferView{};
	ComPtr<ID3D12Resource> colorResource;

	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	D3D12_BLEND_DESC blendDesc{};
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};

	ComPtr<ID3D12Resource> directionalLightResource;
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};

	Matrix4x4 cameraMatrix;

	Matrix4x4 viewMatrix;
	Matrix4x4 projectionMatrix;
	Matrix4x4 ViewProjectionMatrix;
	Matrix4x4 worldViewProjectionMatrix;

	uint32_t kSubdivision = 16;

	ComPtr<ID3D12Resource> textureResource;
	ComPtr<ID3D12Resource> textureResource2;

	uint32_t textureIndex;

	VertexData* vertexData = nullptr;
	MaterialData materialData;
	Material* colorData = nullptr;
	TransformationMatrix* transformationMatrixData = nullptr;
	ParticleForGPU* instancingData = nullptr;

	DirectX::ScratchImage mipImages;
	DirectX::ScratchImage mipImages2;
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU;
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU;
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU2;
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU2;

	D3D12_CPU_DESCRIPTOR_HANDLE instancingSrvHandleCPU;
	D3D12_GPU_DESCRIPTOR_HANDLE instancingSrvHandleGPU;

	Vector4 ColorSphere[1];

	Sphere sphere = { { 0.0f,0.0f,0.0f },1.0f };

	Vector4 LeftTop[1];
	Vector4 RightTop[1];
	Vector4 RightBottom[1];
	Vector4 LeftBottom[1];
	Vector4 Color[1];
	Vector2 texcoordLeftTop[1];
	Vector2 texcoordLeftBottom[1];
	Vector2 texcoordRightTop[1];
	Vector2 texcoordRightBottom[1];

	const float kDeltaTime = 1.0f / 60.0f;
	Vector2 anchorPoint = { 0.0f,0.0f };
	Vector2 textureSize = { 100.0f,100.0f };
	bool isFlipX_ = false;
	bool isFlipY_ = false;

};
