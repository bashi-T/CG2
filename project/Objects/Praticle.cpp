#include "Particle.h"

Particle::~Particle() {
}

void Particle::Initialize(const std::string& textureFilePath,SRVManager* srvManager, Object3dCommon* object3dCommon, DX12Common* dxCommon)
{
	this->dxCommon = dxCommon;
	this->object3dCommon_ = object3dCommon;
	this->srvManager = srvManager;
	this->camera_ = object3dCommon_->GetDefaultCamera();
	kNumMaxInstance = 10;
	kSubdivision = 16;
	std::random_device seedGenerator;
	std::mt19937 randomEngine(seedGenerator());
	std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);
	ResetDXC();
	MakePSO();

	LeftTop[0] = { -0.5f, 0.5f, 0.0f, 1.0f };
	RightTop[0] = { 0.5f, 0.5f, 0.0f, 1.0f };
	RightBottom[0] = { 0.5f, -0.5f, 0.0f, 1.0f };
	LeftBottom[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
	ColorPlane[0] = { 1.0f, 1.0f, 1.0f, 1.0f };
	
	texcoordLeftTop[0] = { 0.0f,0.0f };
	texcoordRightTop[0] = { 0.0f,1.0f };
	texcoordRightBottom[0] = { 1.0f,1.0f };
	texcoordLeftBottom[0] = { 1.0f,0.0f };

	for (uint32_t index = 0; index < kNumMaxInstance; ++index)
	{
		particlesPlane[index] = MakeNewParticle(randomEngine);
	}
	projectionMatrixPlane = MakePerspectiveFovMatrix(0.65f, float(WinAPP::clientWidth_) / float(WinAPP::clientHeight_), 0.1f, 100.0f);
	vertexResourcePlane = CreateBufferResource(sizeof(VertexData) * 6);
	colorResourcePlane = CreateBufferResource(sizeof(Material));
	indexResourcePlane = CreateBufferResource(sizeof(uint32_t) * 6);
	instancingResourcePlane = CreateBufferResource(sizeof(ParticleForGPU) * kNumMaxInstance);
	cameraResource = CreateBufferResource(sizeof(CameraTransform));

	MakeBufferView();

	vertexResourcePlane->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataPlane));
	colorResourcePlane->Map(0, nullptr, reinterpret_cast<void**>(&colorDataPlane));
	instancingResourcePlane->Map(0, nullptr, reinterpret_cast<void**>(&instancingDataPlane));
	cameraResource->Map(0, nullptr, reinterpret_cast<void**>(&cameraData));

	InputDataPlane(LeftTop[0], RightTop[0], RightBottom[0], LeftBottom[0], ColorPlane[0],
		texcoordLeftTop[0], texcoordRightTop[0], texcoordRightBottom[0], texcoordLeftBottom[0]);

	for (uint32_t index = 0; index < kNumMaxInstance; ++index)
	{
		instancingDataPlane[index].WVP = MakeIdentity4x4();
		instancingDataPlane[index].World = MakeIdentity4x4();
		instancingDataPlane[index].color = particlesPlane[index].color;
	}
	MakeShaderResourceViewInstance(instancingResourcePlane.Get());

	materialDataPlane.textureFilePath = textureFilePath;
	TextureManager::GetInstance()->LoadTexture(textureFilePath);
	materialDataPlane.textureIndex = TextureManager::GetInstance()->GetSrvIndex(textureFilePath);
}

void Particle::ResetDXC()
{
	hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
	assert(SUCCEEDED(hr));
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler));
	assert(SUCCEEDED(hr));
	hr = dxcUtils->CreateDefaultIncludeHandler(&includeHandler);
	assert(SUCCEEDED(hr));
}

ComPtr<IDxcBlob> Particle::CompileShader(
	const std::wstring& filePath,
	const wchar_t* profile,
	IDxcUtils* dxcUtils_,
	IDxcCompiler3* dxcCompiler_,
	IDxcIncludeHandler* includeHandler_)
{
	debug_->Log(
		debug_->ConvertString(std::format(L"Begin CompileShader,path{},\n", filePath, profile)));
	ComPtr<IDxcBlobEncoding> shaderSource = nullptr;
	hr = dxcUtils_->LoadFile(filePath.c_str(), nullptr, shaderSource.GetAddressOf());
	assert(SUCCEEDED(hr));

	DxcBuffer shaderSourceBuffer;
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8;

	LPCWSTR arguments[]{
		filePath.c_str(), L"-E", L"main", L"-T", profile, L"-Zi", L"-Qembed_debug", L"-Od", L"-Zpr",
	};
	ComPtr<IDxcResult> shaderResult = nullptr;
	hr = dxcCompiler_->Compile(
		&shaderSourceBuffer, arguments, _countof(arguments), includeHandler_,
		IID_PPV_ARGS(&shaderResult));
	assert(SUCCEEDED(hr));

	ComPtr<IDxcBlobUtf8> shaderError = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
		debug_->Log(shaderError->GetStringPointer());
		assert(SUCCEEDED(hr));
	}
	ComPtr<IDxcBlob> shaderBlob = nullptr;
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(hr));
	debug_->Log(
		debug_->ConvertString(std::format(L"Compile Succeded,path:{}\n", filePath, profile)));
	shaderSource->Release();
	shaderResult->Release();
	return shaderBlob;
}

void Particle::MakePSO()
{
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature_{};
	descriptionRootSignature_.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;	//0から始まる
	descriptorRange[0].NumDescriptors = 1;
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_DESCRIPTOR_RANGE descriptorRangeForInstancing[1] = {};
	descriptorRangeForInstancing[0].BaseShaderRegister = 0;	//0から始まる
	descriptorRangeForInstancing[0].NumDescriptors = 1;
	descriptorRangeForInstancing[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRangeForInstancing[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_PARAMETER rootParameters[5] = {};

	//色
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[0].Descriptor.ShaderRegister = 0;

	//WVP
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[1].Descriptor.ShaderRegister = 0;
	rootParameters[1].DescriptorTable.pDescriptorRanges = descriptorRangeForInstancing;
	rootParameters[1].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeForInstancing);

	//テクスチャ
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);

	//カメラ
	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[3].Descriptor.ShaderRegister = 1;
	
	//ライト
	rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[4].Descriptor.ShaderRegister = 1;

	descriptionRootSignature_.pParameters = rootParameters;
	descriptionRootSignature_.NumParameters = _countof(rootParameters);

	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
	staticSamplers[0].ShaderRegister = 0;
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	descriptionRootSignature_.pStaticSamplers = staticSamplers;
	descriptionRootSignature_.NumStaticSamplers = _countof(staticSamplers);

	hr = D3D12SerializeRootSignature(
		&descriptionRootSignature_, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);

	if (FAILED(hr))
	{
		debug_->Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}

	hr = dxCommon->GetDevice().Get()->CreateRootSignature(
		0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature));

	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);

	D3D12_BLEND_DESC blendDesc{};
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

	D3D12_RASTERIZER_DESC rasterizerDesc{};
	rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	ComPtr<IDxcBlob> vertexShaderBlob = nullptr;
	ComPtr<IDxcBlob> pixelShaderBlob = nullptr;
	vertexShaderBlob =
		CompileShader(L"HLSL/Particle.VS.hlsl", L"vs_6_0", dxcUtils.Get(), dxcCompiler.Get(), includeHandler.Get());
	assert(vertexShaderBlob != nullptr);

	pixelShaderBlob =
		CompileShader(L"HLSL/Particle.PS.hlsl", L"ps_6_0", dxcUtils.Get(), dxcCompiler.Get(), includeHandler.Get());
	assert(pixelShaderBlob != nullptr);

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignature.Get();
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;
	graphicsPipelineStateDesc.VS = {
		vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize() };
	graphicsPipelineStateDesc.PS = {
		pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize() };
	graphicsPipelineStateDesc.BlendState = blendDesc;
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;

	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	hr = dxCommon->GetDevice()->CreateGraphicsPipelineState(
		&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState));
	assert(SUCCEEDED(hr));
}

void Particle::Update()
{
	InputDataPlane(LeftTop[0], RightTop[0], RightBottom[0], LeftBottom[0], ColorPlane[0],
		texcoordLeftTop[0], texcoordRightTop[0], texcoordRightBottom[0], texcoordLeftBottom[0]);
}

void Particle::Draw()
{
	DrawPlane();
}

void Particle::DrawPlane()
{
	dxCommon->GetCommandList().Get()->SetPipelineState(graphicsPipelineState.Get());
	dxCommon->GetCommandList().Get()->SetGraphicsRootSignature(rootSignature.Get());
	dxCommon->GetCommandList().Get()->IASetPrimitiveTopology(
		D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	dxCommon->GetCommandList().Get()->IASetVertexBuffers(0, 1, &vertexBufferViewPlane);
	dxCommon->GetCommandList().Get()->
		IASetIndexBuffer(&indexBufferViewPlane);

	dxCommon->GetCommandList().Get()->SetGraphicsRootConstantBufferView(
		0, colorResourcePlane->GetGPUVirtualAddress());
	dxCommon->GetCommandList().Get()->
		SetGraphicsRootDescriptorTable(
			1, instancingSrvHandleGPU);
	srvManager->SetGraphicsRootDescriptorTable(
		2, materialDataPlane.textureIndex);
	dxCommon->GetCommandList().Get()->SetGraphicsRootConstantBufferView(
		3, cameraResource->GetGPUVirtualAddress());

	D3D12_CPU_DESCRIPTOR_HANDLE rtv =
		dxCommon->GetRtvHandles(srvManager->GetBackBufferIndex());
	D3D12_CPU_DESCRIPTOR_HANDLE dsv = dxCommon->GetDsvHandle();
	dxCommon->GetCommandList().Get()->OMSetRenderTargets(1, &rtv, false, &dsv);

	dxCommon->GetCommandList().Get()->DrawIndexedInstanced(6, kNumInstance, 0, 0, 0);
}

ComPtr<ID3D12Resource> Particle::CreateBufferResource(size_t sizeInBytes)
{
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
	D3D12_RESOURCE_DESC ResourceDesc{};

	ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;

	ResourceDesc.Width = sizeInBytes * 3;

	ResourceDesc.Height = 1;
	ResourceDesc.DepthOrArraySize = 1;
	ResourceDesc.MipLevels = 1;
	ResourceDesc.SampleDesc.Count = 1;

	ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	ComPtr<ID3D12Resource> Resource = nullptr;

	hr = dxCommon->GetDevice().Get()->CreateCommittedResource(
		&uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&Resource));
	assert(SUCCEEDED(hr));
	return Resource;
}

void Particle::MakeBufferView()
{
	vertexBufferViewPlane.BufferLocation = vertexResourcePlane->GetGPUVirtualAddress();
	vertexBufferViewPlane.SizeInBytes = UINT(sizeof(VertexData)) * 4;
	vertexBufferViewPlane.StrideInBytes = sizeof(VertexData);

	indexBufferViewPlane.BufferLocation = indexResourcePlane->GetGPUVirtualAddress();
	indexBufferViewPlane.SizeInBytes = sizeof(uint32_t) * 6;
	indexBufferViewPlane.Format = DXGI_FORMAT_R32_UINT;
}

void Particle::InputDataPlane(
	Vector4 TopLeft, Vector4 TopRight, Vector4 BottomRight, Vector4 BottomLeft, Vector4 color,
	Vector2 coordTopLeft, Vector2 coordTopRight, Vector2 coordBottomRight, Vector2 coordBottomLeft)
{
	uint32_t* indexDataPlane = nullptr;
	indexResourcePlane->Map(0, nullptr, reinterpret_cast<void**>(&indexDataPlane));

	colorDataPlane[0].color = { 1.0f,1.0f,1.0f,1.0f };

	kNumInstance = 0;
	for (uint32_t index = 0; index <kNumMaxInstance; ++index)
	{
		float alpha = 1.0f - (particlesPlane[index].currentTime / particlesPlane[index].lifeTime);
		Matrix4x4 worldMatrix =
			MakeAffineMatrix(
				particlesPlane[index].transform.scale,
				particlesPlane[index].transform.rotate,
				particlesPlane[index].transform.translate);
		if (particlesPlane[index].lifeTime <= particlesPlane[index].currentTime)
		{
			continue;
		}
		particlesPlane[index].transform.translate.x += particlesPlane[index].velocity.x * kDeltaTime;
		particlesPlane[index].transform.translate.y += particlesPlane[index].velocity.y * kDeltaTime;
		particlesPlane[index].transform.translate.z += particlesPlane[index].velocity.z * kDeltaTime;
		particlesPlane[index].currentTime += kDeltaTime;
		if (camera_)
		{
			const Matrix4x4& viewProjectionMatrix = camera_->GetViewProjectionMatrix();
			worldViewProjectionMatrixPlane = Multiply(worldMatrix, viewProjectionMatrix);
		}
		instancingDataPlane[kNumInstance].WVP = worldViewProjectionMatrixPlane;
		instancingDataPlane[kNumInstance].World = worldMatrix;
		particlesPlane[index].color.a = alpha;
		instancingDataPlane[kNumInstance].color = particlesPlane[index].color;
		++kNumInstance;
	}
	cameraData->worldPosition =
	{
		camera_->GetWorldMatrix().m[3][0],
		camera_->GetWorldMatrix().m[3][1],
		camera_->GetWorldMatrix().m[3][2]
	};


	vertexDataPlane[0].position = TopLeft;
	vertexDataPlane[0].texcoord = coordTopLeft;
	vertexDataPlane[0].normal.x = vertexDataPlane[0].position.x;
	vertexDataPlane[0].normal.y = vertexDataPlane[0].position.y;
	vertexDataPlane[0].normal.z = vertexDataPlane[0].position.z;

	vertexDataPlane[1].position = TopRight;
	vertexDataPlane[1].texcoord = coordTopRight;
	vertexDataPlane[1].normal.x = vertexDataPlane[1].position.x;
	vertexDataPlane[1].normal.y = vertexDataPlane[1].position.y;
	vertexDataPlane[1].normal.z = vertexDataPlane[1].position.z;

	vertexDataPlane[2].position = BottomRight;
	vertexDataPlane[2].texcoord = coordBottomRight;
	vertexDataPlane[2].normal.x = vertexDataPlane[2].position.x;
	vertexDataPlane[2].normal.y = vertexDataPlane[2].position.y;
	vertexDataPlane[2].normal.z = vertexDataPlane[2].position.z;

	vertexDataPlane[3].position = BottomLeft;
	vertexDataPlane[3].texcoord = coordBottomLeft;
	vertexDataPlane[3].normal.x = vertexDataPlane[3].position.x;
	vertexDataPlane[3].normal.y = vertexDataPlane[3].position.y;
	vertexDataPlane[3].normal.z = vertexDataPlane[3].position.z;

	indexDataPlane[0] = 0;
	indexDataPlane[1] = 1;
	indexDataPlane[2] = 2;

	indexDataPlane[3] = 0;
	indexDataPlane[4] = 2;
	indexDataPlane[5] = 3;

}

void Particle::MakeShaderResourceViewInstance(ID3D12Resource* instancingResource)
{
	uint32_t index = srvManager->Allocate();
	instancingSrvHandleCPU = srvManager->GetCPUDescriptorHandle(index);
	instancingSrvHandleGPU = srvManager->GetGPUDescriptorHandle(index);
	srvManager->CreateSRVforStructuredBuffer(index, instancingResourcePlane.Get(), kNumInstance, sizeof(ParticleForGPU));
}

Particle::Particles Particle::MakeNewParticle(std::mt19937& randomEngine)
{
	std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);
	std::uniform_real_distribution<float> distColor(0.0f, 1.0f);
	std::uniform_real_distribution<float> distTime(1.0f, 3.0f);
	Particles particle;
	particle.transform.scale = { 1.0f,1.0f,1.0f };
	particle.transform.rotate = { 0.0f,0.0f,0.0f };
	particle.transform.translate = { distribution(randomEngine),distribution(randomEngine),distribution(randomEngine) };
	particle.velocity = { distribution(randomEngine) ,distribution(randomEngine) ,distribution(randomEngine) };
	particle.color = { distColor(randomEngine) ,distColor(randomEngine) ,distColor(randomEngine) ,1.0f };
	particle.lifeTime = distTime(randomEngine);
	particle.currentTime = 0;
	return particle;
}