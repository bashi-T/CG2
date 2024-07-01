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
	void Initialize(DX12Common* dxcommon);
	ComPtr<IDxcBlob> CompileShader(
		const std::wstring& filePath,
		const wchar_t* profile,
		IDxcUtils* dxcUtils,
		IDxcCompiler3* dxcCompiler,
		IDxcIncludeHandler* includeHandler);
	void ResetDXC();
	void MakePSO(DX12Common* dxcommon);
	void MakeSkeltonPSO(DX12Common* dxcommon);
	void SetDefaultCamera(Camera* camera) { this->defaultCamera = camera; }
	static Object3dCommon* GetInstance();
	ComPtr<ID3D12Resource> GetVertexResource() { return vertexResource; }
	ComPtr<ID3D12PipelineState> GetGraphicsPipelineStates(int t) { return graphicsPipelineStates[t]; }
	ComPtr<ID3D12RootSignature> GetRootSignatures(int t) { return rootSignatures[t]; }
	DX12Common* GetDx12Common() { return DX12Common::GetInstance(); }
	Camera* GetDefaultCamera()const { return defaultCamera; }

private:
	Debug* debug_;
	WinAPP* sWinApp;
	MyImGui* imgui_;
	HRESULT hr = NULL;
	DX12Common* dx12Common_;
	EulerTransform transformMatrix;
	Camera* defaultCamera = nullptr;
	ComPtr<ID3D12Resource> transformationMatrixResource;
	static inline Object3dCommon* instance;

	ComPtr<IDxcUtils> dxcUtils = nullptr;
	ComPtr<IDxcCompiler3> dxcCompiler = nullptr;
	ComPtr<IDxcIncludeHandler> includeHandler = nullptr;
	std::array<ComPtr<ID3D12RootSignature>, 2> rootSignatures;
	std::array < ComPtr<ID3D12PipelineState>, 2>  graphicsPipelineStates;
	ComPtr<ID3DBlob> signatureBlob = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;

	ComPtr<ID3D12Resource> vertexResource = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};

	ComPtr<ID3D12Resource> materialResource;

	ComPtr<ID3D12Resource> directionalLightResource;

};
