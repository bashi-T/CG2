#include "DX12.h"

DX12::DX12()
{
}

DX12::~DX12()
{
}

void DX12::MakeDXGIFactory()
{
	hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));
	assert(SUCCEEDED(hr));
}

void DX12::ChoseUseAdapter()
{
	for (UINT i = 0; dxgiFactory->EnumAdapterByGpuPreference(
		i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
		IID_PPV_ARGS(&useAdapter)) != DXGI_ERROR_NOT_FOUND; i++)
	{
		DXGI_ADAPTER_DESC3 adapterDesc{};
		hr = useAdapter->GetDesc3(&adapterDesc);
		assert(SUCCEEDED(hr));

		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE))
		{
			debug_->Log(debug_->ConvertString(
				std::format(L"Use Adapter:{}\n",
					adapterDesc.Description)));
			break;
		}
		useAdapter = nullptr;
	}
}

void DX12::MakeD3D12Device()
{
	D3D_FEATURE_LEVEL featureLevels[] =
	{
	D3D_FEATURE_LEVEL_12_2,
	D3D_FEATURE_LEVEL_12_1,
	D3D_FEATURE_LEVEL_12_0
	};
	const char* featureLevelStrings[] =
	{
		"12.2",
		"12.1",
		"12.0"
	};
	for (size_t i = 0; i < _countof(featureLevels); ++i)
	{
		hr = D3D12CreateDevice(useAdapter, featureLevels[i],
			IID_PPV_ARGS(&device));
		if (SUCCEEDED(hr))
		{
			debug_->Log(std::format("featureLevel {}\n", featureLevelStrings[i]));
			break;
	    }
	}
	assert(device != nullptr);
	debug_->Log("Complete create D3D12Device\n");
}
