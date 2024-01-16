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

class SpriteCommon;
class Sprite
{
public:
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	~Sprite();
	void Initialize(int32_t width, int32_t height, SpriteCommon* spriteCommon);
	void Update(int32_t width, int32_t height);
	void Draw(SpriteCommon* spriteCommon);

	ComPtr<IDxcBlob> CompileShader(
		const std::wstring& filePath,
		const wchar_t* profile,
		IDxcUtils* dxcUtils,
		IDxcCompiler3* dxcCompiler,
		IDxcIncludeHandler* includeHandler);


	ComPtr<ID3D12Resource> CreateBufferResource(SpriteCommon* spriteCommon, size_t sizeInBytes);
	void MakeBufferView();
	void InputData(Vector4 color);

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

	DirectX::ScratchImage LoadTexture(const std::string& filePath);
	ComPtr<ID3D12Resource> CreateTextureResource(
		ID3D12Device* device,
		const DirectX::TexMetadata& metadata);
	void UploadTextureData(
		ID3D12Resource* texture,
		const DirectX::ScratchImage& mipImages,
		const DirectX::TexMetadata& metadata);
	void SetPositoin(const Vector2& position) { this->position = position; }
	void SetRotation(float rotation) { this->rotation = rotation; }
	void SetColor(const Vector4& color) { materialData->color = color; }
	void SetSize(const Vector2& size) { this->size = size; }

	ComPtr<ID3D12Resource> GetVertexResource() { return vertexResource; }
	TransformMatrix GetCameraTransform() { return cameraTransform; }
	Matrix4x4 GetCameraMatrix() { return cameraMatrix; }
	Vector4 GetLeftTop() { return LeftTop; }
	Vector4 GetRightTop() { return RightTop; }
	Vector4 GetRightBottom(){ return RightBottom; }
	Vector4 GetLeftBottom(){ return LeftBottom; }
	Vector4 GetColor(){ return LeftTop; }
	Vector2 GetTexcoordLeftTop(){ return coordLeftTop; }
	Vector2 GetTexcoordRightTop(){ return coordRightTop; }
	Vector2 GetTexcoordRightBottom(){ return coordRightBottom; }
	Vector2 GetTexcoordLeftBottom() { return coordLeftBottom; }
	const Vector2& GetPosition()const { return position; }
	float GetRotation()const { return rotation; }
	const Vector4& GetColor()const { return materialData->color; }
	const Vector2& GetSize()const { return size; }

	struct DirectionalLight {
		Vector4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
		Vector3 direction = { 0.0f, -1.0f, 0.0f };
		float intensity = 1.0f;
	};
	DirectionalLight* DrawDirectionalLightData() { return DirectionalLightData; }

private:
	SpriteCommon* spriteCommon_;
	Debug* debug_;
	WinAPP* sWinApp;
	MyImGui* imgui_;
	HRESULT hr = NULL;

	TransformMatrix transformMatrix;
	Vector2 position = { 0.0f,0.0f };
	float rotation = 0.0f;
	Vector2 size = { 100.0f,100.0f };

	VertexData* vertexData = nullptr;
	ComPtr<ID3D12Resource> vertexResource = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};

	uint32_t* indexData = nullptr;
	ComPtr<ID3D12Resource> indexResource = nullptr;
	D3D12_INDEX_BUFFER_VIEW indexBufferView{};

	struct Material
	{
		Vector4 color;
		int32_t enableLighting;
		float padding[3];
		Matrix4x4 uvTransform;
	};
	Material* materialData = nullptr;
	ComPtr<ID3D12Resource> materialResource = nullptr;

	struct TransformationMatrix {
		Matrix4x4 WVP;
		Matrix4x4 World;
	};
	TransformMatrix uvTransform{
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f},
	};
	ComPtr<ID3D12Resource> transformationMatrixResource;
	TransformationMatrix* transformationMatrixData = nullptr;

	Vector4 LeftTop;
	Vector4 RightTop;
	Vector4 RightBottom;
	Vector4 LeftBottom;
	Vector4 Color;
	Vector2 coordLeftTop;
	Vector2 coordRightTop;
	Vector2 coordRightBottom;
	Vector2 coordLeftBottom;

	TransformMatrix cameraTransform;
	DirectionalLight* DirectionalLightData = nullptr;

	Matrix4x4 cameraMatrix;

	Matrix4x4 viewMatrix;

	Matrix4x4 projectionMatrix;

	Matrix4x4 ViewProjectionMatrix;

	Matrix4x4 worldViewProjectionMatrix;

	ComPtr<ID3D12Resource> textureResource;

	DirectX::ScratchImage mipImages;
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU;
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU;


};

