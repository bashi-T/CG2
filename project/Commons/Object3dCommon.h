#pragma once
#include "Math/CGVector.h"
#include "DX12Common.h"
#include "Systems/MyImGui.h"
#include"Camera/Camera.h"
#include <cassert>
#include <dxcapi.h>
#include <fstream>
#include <sstream>

#pragma comment(lib, "dxcompiler.lib")

class Object3dCommon
{
public:
	void Initialize();
	ComPtr<IDxcBlob> CompileShader(
		const std::wstring& filePath,
		const wchar_t* profile,
		IDxcUtils* dxcUtils,
		IDxcCompiler3* dxcCompiler,
		IDxcIncludeHandler* includeHandler);
	void ResetDXC();
	void MakePSO();
	void SetDefaultCamera() { this->defaultCamera = Camera::GetInstance(); }
	static Object3dCommon* GetInstance();
	ComPtr<ID3D12Resource> GetVertexResource() { return vertexResource; }
	ComPtr<ID3D12PipelineState> GetGraphicsPipelineState() { return graphicsPipelineState; }
	ComPtr<ID3D12RootSignature> GetRootSignature() { return rootSignature; }
	DX12Common* GetDx12Common() { return DX12Common::GetInstance(); }
	Camera* GetDefaultCamera()const { return defaultCamera; }

private:
	Debug* debug_;
	WinAPP* sWinApp;
	MyImGui* imgui_;
	HRESULT hr = NULL;
	DX12Common* dx12Common_;
	TransformMatrix transformMatrix;
	Camera* defaultCamera = nullptr;
	ComPtr<ID3D12Resource> transformationMatrixResource;
	static inline Object3dCommon* instance;

	ComPtr<IDxcUtils> dxcUtils = nullptr;
	ComPtr<IDxcCompiler3> dxcCompiler = nullptr;
	ComPtr<IDxcIncludeHandler> includeHandler = nullptr;
	ComPtr<ID3D12RootSignature> rootSignature = nullptr;
	ComPtr<ID3D12PipelineState> graphicsPipelineState = NULL;
	ComPtr<ID3DBlob> signatureBlob = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;

	ComPtr<ID3D12Resource> vertexResource = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};

	ComPtr<ID3D12Resource> materialResource;

	ComPtr<ID3D12Resource> directionalLightResource;

};
