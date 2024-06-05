#pragma once
#include "Math/CGVector.h"
#include "DX12Common.h"
#include "Systems/MyImGui.h"
#include <cassert>
#include <dxcapi.h>
#include <fstream>
#include <sstream>
#include"Managers/TextureManager.h"

#pragma comment(lib, "dxcompiler.lib")

class ModelCommon
{
private:
	DX12Common* dxCommon_;
	HRESULT hr = NULL;
	Debug* debug_;

	ComPtr<IDxcUtils> dxcUtils = nullptr;
	ComPtr<IDxcCompiler3> dxcCompiler = nullptr;
	ComPtr<IDxcIncludeHandler> includeHandler = nullptr;
	ComPtr<ID3DBlob> signatureBlob = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	ComPtr<ID3D12RootSignature> rootSignature = nullptr;
	ComPtr<ID3D12PipelineState> graphicsPipelineState = NULL;

public:
	void Initialize(DX12Common* dxCommon);
	ComPtr<IDxcBlob> CompileShader(
		const std::wstring& filePath,
		const wchar_t* profile,
		IDxcUtils* dxcUtils,
		IDxcCompiler3* dxcCompiler,
		IDxcIncludeHandler* includeHandler);
	void ResetDXC();
	void MakePSO(DX12Common* dxcommon);
	void MakeSkeltonPSO(DX12Common* dxcommon);

	DX12Common* GetDx12Common()const { return dxCommon_; }
};

