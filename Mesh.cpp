#include "Mesh.h"

Mesh::~Mesh() {
}

void Mesh::Initialize(const std::string& filename, int32_t width, int32_t height) {
	kSubdivision == 16;

	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = width;
	viewport.Height = height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	scissorRect.left = 0.0f;
	scissorRect.right = width;
	scissorRect.top = 0.0f;
	scissorRect.bottom = height;

	Mesh::ResetDXC();

	Mesh::MakePSO();

	transformMatrix = {
			{1.0f, 1.0f, 1.0f},
			{0.0f, 0.0f, 0.0f},
			{0.0f, 0.0f, 0.0f}
	};
	cameraTransform = {
		{1.0f, 1.0f, 1.0f  },
		{0.0f, 0.0f, 0.0f  },
		{0.0f, 0.0f, -15.0f}
	};
	projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(width) / float(height), 0.1f, 100.0f);
	cameraMatrix =
		MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);

	transformMatrixSprite = {
		{1.0f, 1.0f, 1.0f},
		{0.0f, 0.0f, 0.0f},
		{0.0f, 0.0f, 0.0f}
	};
	transformMatrixSphere = {
		{1.0f, 1.0f, 1.0f},
		{0.0f, 0.0f, 0.0f},
		{0.0f, 0.0f, 0.0f}
	};
	transformMatrixObj = {
		{1.0f, 1.0f, 1.0f},
		{0.0f, 0.0f, 0.0f},
		{0.0f, 0.0f, 0.0f}
	};
	modelData = LoadObjFile("Resource", filename);
	vertexResource = CreateBufferResource(sizeof(VertexData) * 3);
	vertexResourceSprite = CreateBufferResource(sizeof(VertexData) * 6);
	vertexResourceSphere = CreateBufferResource(sizeof(VertexData) * 6 * kSubdivision * kSubdivision);
	vertexResourceObj = CreateBufferResource(sizeof(VertexData) * modelData.vertices.size());

	materialResource = CreateBufferResource(sizeof(Material));
	materialResourceSprite = CreateBufferResource(sizeof(Material));
	materialResourceSphere = CreateBufferResource(sizeof(Material));
	materialResourceObj = CreateBufferResource(sizeof(Material));

	transformationMatrixResource = CreateBufferResource(sizeof(TransformationMatrix));
	transformationMatrixResourceSprite = CreateBufferResource(sizeof(TransformationMatrix));
	transformationMatrixResourceSphere = CreateBufferResource(sizeof(TransformationMatrix));
	transformationMatrixResourceObj = CreateBufferResource(sizeof(TransformationMatrix));

	directionalLightResource = CreateBufferResource(sizeof(DirectionalLight));
	directionalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&DirectionalLightData));

	indexResourceSprite = CreateBufferResource(sizeof(uint32_t) * 6);
	indexResourceSphere = CreateBufferResource(sizeof(uint32_t) * 6 * kSubdivision * kSubdivision);

	DirectionalLightData->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	DirectionalLightData->direction = { 0.0f, -1.0f, 0.0f };
	DirectionalLightData->intensity = 1.0f;

	mipImages = LoadTexture(modelData.material.textureFilePath);
	mipImages2 = LoadTexture(modelData.material.textureFilePath);
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	const DirectX::TexMetadata& metadata2 = mipImages2.GetMetadata();
	textureResource = CreateTextureResource(DX12Common::GetInstance()->GetDevice().Get(), metadata);
	textureResource2 = CreateTextureResource(DX12Common::GetInstance()->GetDevice().Get(), metadata2);
	UploadTextureData(textureResource.Get(), mipImages, metadata);
	UploadTextureData(textureResource2.Get(), mipImages2, metadata2);
	MakeShaderResourceView(metadata, metadata2);

	MakeBufferView();

}

void Mesh::ResetDXC() {
	hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
	assert(SUCCEEDED(hr));
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler));
	assert(SUCCEEDED(hr));
	hr = dxcUtils->CreateDefaultIncludeHandler(&includeHandler);
	assert(SUCCEEDED(hr));
}

Microsoft::WRL::ComPtr<IDxcBlob> Mesh::CompileShader(
	const std::wstring& filePath,
	const wchar_t* profile,
	IDxcUtils* dxcUtils_,
	IDxcCompiler3* dxcCompiler_,
	IDxcIncludeHandler* includeHandler_)
{
	debug_->Log(
		debug_->ConvertString(std::format(L"Begin CompileShader,path{},\n", filePath, profile)));
	Microsoft::WRL::ComPtr<IDxcBlobEncoding> shaderSource = nullptr;
	hr = dxcUtils_->LoadFile(filePath.c_str(), nullptr, shaderSource.GetAddressOf());
	assert(SUCCEEDED(hr));

	DxcBuffer shaderSourceBuffer;
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8;

	LPCWSTR arguments[]{
		filePath.c_str(), L"-E", L"main", L"-T", profile, L"-Zi", L"-Qembed_debug", L"-Od", L"-Zpr",
	};
	Microsoft::WRL::ComPtr<IDxcResult> shaderResult = nullptr;
	hr = dxcCompiler_->Compile(
		&shaderSourceBuffer, arguments, _countof(arguments), includeHandler_,
		IID_PPV_ARGS(&shaderResult));
	assert(SUCCEEDED(hr));

	Microsoft::WRL::ComPtr<IDxcBlobUtf8> shaderError = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
		debug_->Log(shaderError->GetStringPointer());
		assert(SUCCEEDED(hr));
	}
	Microsoft::WRL::ComPtr<IDxcBlob> shaderBlob = nullptr;
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(hr));
	debug_->Log(
		debug_->ConvertString(std::format(L"Compile Succeded,path:{}\n", filePath, profile)));
	shaderSource->Release();
	shaderResult->Release();
	return shaderBlob;
}

void Mesh::MakePSO()
{
	descriptionRootSignature_.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	D3D12_ROOT_PARAMETER rootParameters[4] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[0].Descriptor.ShaderRegister = 0;
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[1].Descriptor.ShaderRegister = 0;

	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;
	descriptorRange[0].NumDescriptors = 1;
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);

	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[3].Descriptor.ShaderRegister = 1;

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

	if (FAILED(hr)) {
		debug_->Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}

	hr = DX12Common::GetInstance()->GetDevice().Get()->CreateRootSignature(
		0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature));

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

	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	vertexShaderBlob =
		CompileShader(L"Object3d.VS.hlsl", L"vs_6_0", dxcUtils.Get(), dxcCompiler.Get(), includeHandler.Get());
	assert(vertexShaderBlob != nullptr);

	pixelShaderBlob =
		CompileShader(L"Object3d.PS.hlsl", L"ps_6_0", dxcUtils.Get(), dxcCompiler.Get(), includeHandler.Get());
	assert(pixelShaderBlob != nullptr);

	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

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

	hr = DX12Common::GetInstance()->GetDevice().Get()->CreateGraphicsPipelineState(
		&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState));
	assert(SUCCEEDED(hr));
}

void Mesh::Update()
{

	DirectionalLightData->direction = Normalize(DirectionalLightData->direction);

	ImGui::Begin("Light");
	ImGui::ColorEdit3("LightColor", (float*)&DirectionalLightData->color, 0.01f);
	ImGui::DragFloat3("LightDirection", (float*)&DirectionalLightData->direction, 0.01f);
	ImGui::DragFloat3("CameraRotate", (float*)&cameraTransform.rotate.x, 0.01f);
	ImGui::DragFloat3("CameraTranslate", (float*)&cameraTransform.translate.x, 0.01f);
	ImGui::DragFloat("Intensity", (float*)&DirectionalLightData->intensity, 0.01f);
	ImGui::DragFloat2("UVTranslate", &uvTransformSprite.translate.x, 0.01f, -10.0f, 10.0f);
	ImGui::DragFloat2("UVScale", &uvTransformSprite.scale.x, 0.01f, -10.0f, 10.0f);
	ImGui::SliderAngle("UVRotate", &uvTransformSprite.rotate.z);
	ImGui::DragFloat3("Sphere.rotate", (float*)&transformMatrixSphere.rotate, 0.01f);

	ImGui::End();
	cameraMatrix =
		MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);

}

Microsoft::WRL::ComPtr<ID3D12Resource> Mesh::CreateBufferResource(size_t sizeInBytes)
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

	Microsoft::WRL::ComPtr<ID3D12Resource> Resource = nullptr;

	hr = DX12Common::GetInstance()->GetDevice().Get()->CreateCommittedResource(
		&uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &ResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&Resource));
	assert(SUCCEEDED(hr));
	return Resource;
}

void Mesh::MakeBufferView()
{
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = UINT(sizeof(VertexData)) * 3;
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	vertexBufferViewSprite.BufferLocation = vertexResourceSprite->GetGPUVirtualAddress();
	vertexBufferViewSprite.SizeInBytes = sizeof(VertexData) * 6;
	vertexBufferViewSprite.StrideInBytes = sizeof(VertexData);

	indexBufferViewSprite.BufferLocation = indexResourceSprite->GetGPUVirtualAddress();
	indexBufferViewSprite.SizeInBytes = sizeof(uint32_t) * 6;
	indexBufferViewSprite.Format = DXGI_FORMAT_R32_UINT;

	vertexBufferViewSphere.BufferLocation = vertexResourceSphere->GetGPUVirtualAddress();
	vertexBufferViewSphere.SizeInBytes = sizeof(VertexData) * 6 * kSubdivision * kSubdivision;
	vertexBufferViewSphere.StrideInBytes = sizeof(VertexData);

	indexBufferViewSphere.BufferLocation = indexResourceSphere->GetGPUVirtualAddress();
	indexBufferViewSphere.SizeInBytes = sizeof(uint32_t) * 6 * kSubdivision * kSubdivision;
	indexBufferViewSphere.Format = DXGI_FORMAT_R32_UINT;

	vertexBufferViewObj.BufferLocation = vertexResourceObj->GetGPUVirtualAddress();
	vertexBufferViewObj.SizeInBytes = UINT(sizeof(VertexData) * modelData.vertices.size());
	vertexBufferViewObj.StrideInBytes = sizeof(VertexData);
}

void Mesh::InputDataTriangle(
	Vector4 Top, Vector4 Right, Vector4 Left, Vector4 color, Vector2 coordTop, Vector2 coordRight,
	Vector2 coordLeft) {
	VertexData* vertexData = nullptr;
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	std::memcpy(vertexData, modelData.vertices.data(), sizeof(VertexData) * 3);
	Material* materialData = nullptr;
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	TransformationMatrix* transformationMatrixData = nullptr;
	transformationMatrixResource->Map(
		0, nullptr, reinterpret_cast<void**>(&transformationMatrixData));

	materialData[0].color = color;
	materialData[0].enableLighting = true;

	transformationMatrixData->WVP = MakeIdentity4x4();

	transformMatrix.rotate.y += 0.02f;
	Matrix4x4 worldMatrix =
		MakeAffineMatrix(transformMatrix.scale, transformMatrix.rotate, transformMatrix.translate);
	viewMatrix = Inverse(cameraMatrix);
	worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
	transformationMatrixData->WVP = worldViewProjectionMatrix;
	transformationMatrixData->World = worldMatrix;

	vertexData[0].position = Top;
	vertexData[1].position = Right;
	vertexData[2].position = Left;
	vertexData[0].texcoord = coordTop;
	vertexData[1].texcoord = coordRight;
	vertexData[2].texcoord = coordLeft;
	vertexData[0].normal.x = vertexData[0].position.x;
	vertexData[0].normal.y = vertexData[0].position.y;
	vertexData[0].normal.z = vertexData[0].position.z;
	vertexData[1].normal.x = vertexData[1].position.x;
	vertexData[1].normal.y = vertexData[1].position.y;
	vertexData[1].normal.z = vertexData[1].position.z;
	vertexData[2].normal.x = vertexData[2].position.x;
	vertexData[2].normal.y = vertexData[2].position.y;
	vertexData[2].normal.z = vertexData[2].position.z;
}

void Mesh::InputDataSprite(
	Vector4 LeftTop, Vector4 RightTop, Vector4 RightBottom, Vector4 LeftBottom, Vector4 color,
	Vector2 coordLeftTop, Vector2 coordRightTop, Vector2 coordRightBottom, Vector2 coordLeftBottom,
	int32_t width, int32_t height) {
	VertexData* vertexDataSprite = nullptr;
	vertexResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataSprite));
	Material* materialDataSprite = nullptr;
	materialResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&materialDataSprite));
	uint32_t* indexDataSprite = nullptr;
	indexResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&indexDataSprite));
	transformationMatrixResourceSprite->Map(
		0, nullptr, reinterpret_cast<void**>(&transformationMatrixDataSprite));

	transformationMatrixDataSprite->WVP = MakeIdentity4x4();
	projectionMatrixSprite =
		MakeOrthographicMatrix(0.0f, 0.0f, float(width), float(height), 0.0f, 100.0f);

	Matrix4x4 worldMatrix = MakeAffineMatrix(
		transformMatrixSprite.scale, transformMatrixSprite.rotate, transformMatrixSprite.translate);
	viewMatrixSprite = MakeIdentity4x4();
	worldViewProjectionMatrixSprite =
		Multiply(worldMatrix, Multiply(viewMatrixSprite, projectionMatrixSprite));
	transformationMatrixDataSprite->WVP = worldViewProjectionMatrixSprite;
	transformationMatrixDataSprite->World = worldMatrix;

	materialDataSprite[0].color = color;
	materialDataSprite[0].enableLighting = false;
	materialDataSprite[0].uvTransform = MakeIdentity4x4();
	Matrix4x4 uvTransformMatrix = MakeScaleMatrix(uvTransformSprite.scale);
	uvTransformMatrix = Multiply(uvTransformMatrix, MakerotateZMatrix(uvTransformSprite.rotate.z));
	uvTransformMatrix = Multiply(uvTransformMatrix, MakeTranslateMatrix(uvTransformSprite.translate));
	materialDataSprite[0].uvTransform = uvTransformMatrix;

	vertexDataSprite[0].position = LeftTop;
	vertexDataSprite[1].position = RightTop;
	vertexDataSprite[2].position = RightBottom;
	vertexDataSprite[0].texcoord = coordLeftTop;
	vertexDataSprite[1].texcoord = coordRightTop;
	vertexDataSprite[2].texcoord = coordRightBottom;
	vertexDataSprite[3].position = LeftBottom;
	vertexDataSprite[3].texcoord = coordLeftBottom;

	vertexDataSprite[0].normal = { 0.0f, 0.0f, -1.0f };

	indexDataSprite[0] = 0;
	indexDataSprite[1] = 1;
	indexDataSprite[2] = 2;

	indexDataSprite[3] = 0;
	indexDataSprite[4] = 2;
	indexDataSprite[5] = 3;
}

void Mesh::InputDataSphere(
	Vector4 LeftTop, Vector4 RightTop, Vector4 RightBottom, Vector4 LeftBottom, Vector4 color,
	Vector2 coordLeftTop, Vector2 coordRightTop, Vector2 coordRightBottom, Vector2 coordLeftBottom,
	uint32_t count, int32_t width, int32_t height)
{
	VertexData* vertexDataSphere = nullptr;

	vertexResourceSphere->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataSphere));

	Material* materialDataSphere = nullptr;
	materialResourceSphere->Map(0, nullptr, reinterpret_cast<void**>(&materialDataSphere));

	uint32_t* indexDataSphere = nullptr;
	indexResourceSphere->Map(0, nullptr, reinterpret_cast<void**>(&indexDataSphere));

	transformationMatrixResourceSphere->Map(
		0, nullptr, reinterpret_cast<void**>(&transformationMatrixDataSphere));
	projectionMatrixSphere =
		MakePerspectiveFovMatrix(0.45f, float(width) / float(height), 0.1f, 100.0f);
	transformationMatrixDataSphere->WVP = MakeIdentity4x4();

	Matrix4x4 worldMatrix = MakeAffineMatrix(
		transformMatrixSphere.scale, transformMatrixSphere.rotate, transformMatrixSphere.translate);
	viewMatrixSphere = Inverse(cameraMatrix);
	worldViewProjectionMatrixSphere =
		Multiply(worldMatrix, Multiply(viewMatrixSphere, projectionMatrixSphere));
	transformationMatrixDataSphere->WVP = worldViewProjectionMatrixSphere;
	transformationMatrixDataSphere->World = worldMatrix;

	materialDataSphere[0].color = color;
	materialDataSphere[0].enableLighting = true;
	materialDataSphere[0].uvTransform = MakeIdentity4x4();
	Matrix4x4 uvTransformMatrix = MakeScaleMatrix(uvTransformSphere.scale);
	uvTransformMatrix = Multiply(uvTransformMatrix, MakerotateZMatrix(uvTransformSphere.rotate.z));
	uvTransformMatrix = Multiply(uvTransformMatrix, MakeTranslateMatrix(uvTransformSphere.translate));
	materialDataSphere[0].uvTransform = uvTransformMatrix;

	vertexDataSphere[count * 4].position = LeftTop;
	vertexDataSphere[count * 4].texcoord = coordLeftTop;
	vertexDataSphere[count * 4].normal.x = vertexDataSphere[count * 4].position.x;
	vertexDataSphere[count * 4].normal.y = vertexDataSphere[count * 4].position.y;
	vertexDataSphere[count * 4].normal.z = vertexDataSphere[count * 4].position.z;

	vertexDataSphere[count * 4 + 1].position = RightTop;
	vertexDataSphere[count * 4 + 1].texcoord = coordRightTop;
	vertexDataSphere[count * 4 + 1].normal.x = vertexDataSphere[count * 4 + 1].position.x;
	vertexDataSphere[count * 4 + 1].normal.y = vertexDataSphere[count * 4 + 1].position.y;
	vertexDataSphere[count * 4 + 1].normal.z = vertexDataSphere[count * 4 + 1].position.z;

	vertexDataSphere[count * 4 + 2].position = RightBottom;
	vertexDataSphere[count * 4 + 2].texcoord = coordRightBottom;
	vertexDataSphere[count * 4 + 2].normal.x = vertexDataSphere[count * 4 + 2].position.x;
	vertexDataSphere[count * 4 + 2].normal.y = vertexDataSphere[count * 4 + 2].position.y;
	vertexDataSphere[count * 4 + 2].normal.z = vertexDataSphere[count * 4 + 2].position.z;

	vertexDataSphere[count * 4 + 3].position = LeftBottom;
	vertexDataSphere[count * 4 + 3].texcoord = coordLeftBottom;
	vertexDataSphere[count * 4 + 3].normal.x = vertexDataSphere[count * 4 + 3].position.x;
	vertexDataSphere[count * 4 + 3].normal.y = vertexDataSphere[count * 4 + 3].position.y;
	vertexDataSphere[count * 4 + 3].normal.z = vertexDataSphere[count * 4 + 3].position.z;

	indexDataSphere[count * 6 + 0] = count * 4 + 0;
	indexDataSphere[count * 6 + 1] = count * 4 + 1;
	indexDataSphere[count * 6 + 2] = count * 4 + 2;

	indexDataSphere[count * 6 + 3] = count * 4 + 0;
	indexDataSphere[count * 6 + 4] = count * 4 + 2;
	indexDataSphere[count * 6 + 5] = count * 4 + 3;
}

void Mesh::DrawSprite(
	Vector4 LeftTop, Vector4 RightTop, Vector4 RightBottom, Vector4 LeftBottom, Vector4 color,
	Vector2 coordLeftTop, Vector2 coordRightTop, Vector2 coordRightBottom, Vector2 coordLeftBottom,
	int32_t width, int32_t height) {
	InputDataSprite(
		LeftTop, RightTop, RightBottom, LeftBottom, color, coordLeftTop, coordRightTop,
		coordRightBottom, coordLeftBottom, width, height);
	DX12Common::GetInstance()->GetCommandList().Get()->RSSetViewports(1, &viewport);
	DX12Common::GetInstance()->GetCommandList().Get()->RSSetScissorRects(1, &scissorRect);
	DX12Common::GetInstance()->GetCommandList().Get()->SetPipelineState(graphicsPipelineState.Get());
	DX12Common::GetInstance()->GetCommandList().Get()->SetGraphicsRootSignature(rootSignature.Get());
	DX12Common::GetInstance()->GetCommandList().Get()->IASetPrimitiveTopology(
		D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DX12Common::GetInstance()->GetCommandList().Get()->SetGraphicsRootConstantBufferView(
		0, materialResourceSprite->GetGPUVirtualAddress());
	DX12Common::GetInstance()->GetCommandList().Get()->SetGraphicsRootConstantBufferView(
		1, transformationMatrixResourceSprite->GetGPUVirtualAddress());
	D3D12_CPU_DESCRIPTOR_HANDLE rtv =
		DX12Common::GetInstance()->GetRtvHandles(DX12Common::GetInstance()->GetBackBufferIndex());
	D3D12_CPU_DESCRIPTOR_HANDLE dsv = DX12Common::GetInstance()->GetDsvHandle();

	DX12Common::GetInstance()->GetCommandList().Get()->OMSetRenderTargets(1, &rtv, false, &dsv);
	DX12Common::GetInstance()->GetCommandList().Get()->SetGraphicsRootDescriptorTable(
		2, GetTextureSrvHandleGPU());
	DX12Common::GetInstance()->GetCommandList().Get()->SetGraphicsRootConstantBufferView(
		3, directionalLightResource->GetGPUVirtualAddress());
	DX12Common::GetInstance()->GetCommandList().Get()->
		IASetVertexBuffers(0, 1, &vertexBufferViewSprite);
	DX12Common::GetInstance()->GetCommandList().Get()->
		IASetIndexBuffer(&indexBufferViewSprite);
	DX12Common::GetInstance()->GetCommandList().Get()->
		DrawIndexedInstanced(6, 1, 0, 0, 0);
}

void Mesh::DrawTriangle(
	Vector4 Top, Vector4 Right, Vector4 Left, Vector4 color, Vector2 coordTop, Vector2 coordRight,
	Vector2 coordLeft, bool useWorldMap) {
	InputDataTriangle(Top, Right, Left, color, coordTop, coordRight, coordLeft);
	DX12Common::GetInstance()->GetCommandList().Get()->RSSetViewports(1, &viewport);
	DX12Common::GetInstance()->GetCommandList().Get()->RSSetScissorRects(1, &scissorRect);
	DX12Common::GetInstance()->GetCommandList().Get()->SetPipelineState(graphicsPipelineState.Get());
	DX12Common::GetInstance()->GetCommandList().Get()->SetGraphicsRootSignature(rootSignature.Get());
	DX12Common::GetInstance()->GetCommandList().Get()->IASetVertexBuffers(0, 1, &vertexBufferView);
	DX12Common::GetInstance()->GetCommandList().Get()->
		IASetIndexBuffer(&indexBufferViewSphere);
	DX12Common::GetInstance()->GetCommandList().Get()->IASetPrimitiveTopology(
		D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DX12Common::GetInstance()->GetCommandList().Get()->SetGraphicsRootConstantBufferView(
		0, materialResource->GetGPUVirtualAddress());
	DX12Common::GetInstance()->GetCommandList().Get()->SetGraphicsRootConstantBufferView(
		1, transformationMatrixResource->GetGPUVirtualAddress());
	D3D12_CPU_DESCRIPTOR_HANDLE rtv =
		DX12Common::GetInstance()->GetRtvHandles(DX12Common::GetInstance()->GetBackBufferIndex());
	D3D12_CPU_DESCRIPTOR_HANDLE dsv = DX12Common::GetInstance()->GetDsvHandle();

	DX12Common::GetInstance()->GetCommandList().Get()->OMSetRenderTargets(1, &rtv, false, &dsv);

	DX12Common::GetInstance()->GetCommandList().Get()->SetGraphicsRootDescriptorTable(
		2, useWorldMap ? GetTextureSrvHandleGPU2()
		: GetTextureSrvHandleGPU());

	DX12Common::GetInstance()->GetCommandList().Get()->SetGraphicsRootConstantBufferView(
		3, directionalLightResource->GetGPUVirtualAddress());

	DX12Common::GetInstance()->GetCommandList().Get()->DrawInstanced(3, 1, 0, 0);
}

void Mesh::DrawSphere(
	const Sphere& sphere_, Vector4 color, bool useWorldMap, int32_t width, int32_t height)
{
	float pi = 3.141592f;
	const float kLonevery = 2.0f / kSubdivision * pi;
	const float kLatevery = pi / kSubdivision;
	uint32_t sphereCount = 0;

	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex)
	{
		float lat = -pi / 2.0f + kLatevery * latIndex; // theta
		float latB = pi / kSubdivision;

		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex)
		{
			float lon = lonIndex * kLonevery; // phi
			float lonC = 2 * pi / kSubdivision;

			Vector4 a = {
				sphere_.center.x + sphere_.radius * cos(lat) * cos(lon),
				sphere_.center.y + sphere_.radius * sin(lat),
				sphere_.center.z + sphere_.radius * cos(lat) * sin(lon), 1.0f };
			Vector4 b = {
				sphere_.center.x + sphere_.radius * cos(lat + latB) * cos(lon),
				sphere_.center.y + sphere_.radius * sin(lat + latB),
				sphere_.center.z + sphere_.radius * cos(lat + latB) * sin(lon), 1.0f };
			Vector4 c = {
				sphere_.center.x + sphere_.radius * cos(lat) * cos(lon + lonC),
				sphere_.center.y + sphere_.radius * sin(lat),
				sphere_.center.z + sphere_.radius * cos(lat) * sin(lon + lonC), 1.0f };
			Vector4 d = {
				sphere_.center.x + sphere_.radius * cos(lat + latB) * cos(lon + lonC),
				sphere_.center.y + sphere_.radius * sin(lat + latB),
				sphere_.center.z + sphere_.radius * cos(lat + latB) * sin(lon + lonC), 1.0f };
			Vector2 texcoordA
			{
				float(lonIndex) / float(kSubdivision),
				1.0f - float(latIndex) / float(kSubdivision)
			};
			Vector2 texcoordB
			{
				float(lonIndex) / float(kSubdivision),
				1.0f - float(latIndex + 1) / float(kSubdivision)
			};
			Vector2 texcoordC
			{
				float(lonIndex + 1) / float(kSubdivision),
				1.0f - float(latIndex) / float(kSubdivision)
			};
			Vector2 texcoordD
			{
				float(lonIndex + 1) / float(kSubdivision),
				1.0f - float(latIndex + 1) / float(kSubdivision)
			};

			InputDataSphere(
				b, d, c, a, color, texcoordB, texcoordD, texcoordC, texcoordA, sphereCount, width, height);

			DX12Common::GetInstance()->GetCommandList().Get()->RSSetViewports(1, &viewport);
			DX12Common::GetInstance()->GetCommandList().Get()->RSSetScissorRects(1, &scissorRect);
			DX12Common::GetInstance()->GetCommandList().Get()->SetPipelineState(graphicsPipelineState.Get());
			DX12Common::GetInstance()->GetCommandList().Get()->SetGraphicsRootSignature(rootSignature.Get());
			DX12Common::GetInstance()->GetCommandList().Get()->IASetPrimitiveTopology(
				D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			DX12Common::GetInstance()->GetCommandList().Get()->SetGraphicsRootConstantBufferView(
				0, materialResourceSphere->GetGPUVirtualAddress());
			DX12Common::GetInstance()->GetCommandList().Get()->SetGraphicsRootConstantBufferView(
				1, transformationMatrixResourceSphere->GetGPUVirtualAddress());
			D3D12_CPU_DESCRIPTOR_HANDLE rtv = DX12Common::GetInstance()->GetRtvHandles(
				DX12Common::GetInstance()->GetBackBufferIndex());
			D3D12_CPU_DESCRIPTOR_HANDLE dsv = DX12Common::GetInstance()->GetDsvHandle();
			DX12Common::GetInstance()->GetCommandList().Get()->OMSetRenderTargets(1, &rtv, false, &dsv);
			DX12Common::GetInstance()->GetCommandList().Get()->SetGraphicsRootDescriptorTable(
				2, useWorldMap ? GetTextureSrvHandleGPU2()
				: GetTextureSrvHandleGPU());
			DX12Common::GetInstance()->GetCommandList().Get()->SetGraphicsRootConstantBufferView(
				3, directionalLightResource->GetGPUVirtualAddress());
			DX12Common::GetInstance()->GetCommandList().Get()->IASetVertexBuffers(
				0, 1, &vertexBufferViewSphere);
			DX12Common::GetInstance()->GetCommandList().Get()->
				IASetIndexBuffer(&indexBufferViewSphere);

			sphereCount++;
		}
	}
	DX12Common::GetInstance()->GetCommandList().Get()->DrawIndexedInstanced(
		6 * kSubdivision * kSubdivision, 1, 0, 0, 0);
}

void Mesh::DrawOBJ(Vector4 color, bool useWorldMap, int32_t width, int32_t height)
{
	VertexData* vertexDataObj = nullptr;
	vertexResourceObj->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataObj));
	std::memcpy(vertexDataObj, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size());

	Material* materialDataObj = nullptr;
	materialResourceObj->Map(0, nullptr, reinterpret_cast<void**>(&materialDataObj));

	transformationMatrixResourceObj->Map(
		0, nullptr, reinterpret_cast<void**>(&transformationMatrixDataObj));
	projectionMatrixObj =
		MakePerspectiveFovMatrix(0.45f, float(width) / float(height), 0.1f, 100.0f);
	transformationMatrixDataObj->WVP = MakeIdentity4x4();

	Matrix4x4 worldMatrix = MakeAffineMatrix(
		transformMatrixObj.scale, transformMatrixObj.rotate, transformMatrixObj.translate);
	viewMatrixObj = Inverse(cameraMatrix);
	worldViewProjectionMatrixObj =
		Multiply(worldMatrix, Multiply(viewMatrixObj, projectionMatrixObj));
	transformationMatrixDataObj->WVP = worldViewProjectionMatrixObj;
	transformationMatrixDataObj->World = worldMatrix;

	materialDataObj[0].color = color;
	materialDataObj[0].enableLighting = true;
	materialDataObj[0].uvTransform = MakeIdentity4x4();
	Matrix4x4 uvTransformMatrix = MakeScaleMatrix(uvTransformObj.scale);
	uvTransformMatrix = Multiply(uvTransformMatrix, MakerotateZMatrix(uvTransformObj.rotate.z));
	uvTransformMatrix = Multiply(uvTransformMatrix, MakeTranslateMatrix(uvTransformObj.translate));
	materialDataObj[0].uvTransform = uvTransformMatrix;

	DX12Common::GetInstance()->GetCommandList().Get()->RSSetViewports(1, &viewport);
	DX12Common::GetInstance()->GetCommandList().Get()->RSSetScissorRects(1, &scissorRect);
	DX12Common::GetInstance()->GetCommandList().Get()->SetPipelineState(graphicsPipelineState.Get());
	DX12Common::GetInstance()->GetCommandList().Get()->SetGraphicsRootSignature(rootSignature.Get());
	DX12Common::GetInstance()->GetCommandList().Get()->IASetPrimitiveTopology(
		D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DX12Common::GetInstance()->GetCommandList().Get()->SetGraphicsRootConstantBufferView(
		0, materialResourceObj->GetGPUVirtualAddress());
	DX12Common::GetInstance()->GetCommandList().Get()->SetGraphicsRootConstantBufferView(
		1, transformationMatrixResourceObj->GetGPUVirtualAddress());
	D3D12_CPU_DESCRIPTOR_HANDLE rtv = DX12Common::GetInstance()->GetRtvHandles(
		DX12Common::GetInstance()->GetBackBufferIndex());
	D3D12_CPU_DESCRIPTOR_HANDLE dsv = DX12Common::GetInstance()->GetDsvHandle();
	DX12Common::GetInstance()->GetCommandList().Get()->OMSetRenderTargets(1, &rtv, false, &dsv);
	DX12Common::GetInstance()->GetCommandList().Get()->SetGraphicsRootDescriptorTable(
		2, useWorldMap ? GetTextureSrvHandleGPU2()
		: GetTextureSrvHandleGPU());
	DX12Common::GetInstance()->GetCommandList().Get()->SetGraphicsRootConstantBufferView(
		3, directionalLightResource->GetGPUVirtualAddress());
	DX12Common::GetInstance()->GetCommandList().Get()->IASetVertexBuffers(
		0, 1, &vertexBufferViewObj);
	DX12Common::GetInstance()->GetCommandList().Get()->
		IASetIndexBuffer(&indexBufferViewObj);
	DX12Common::GetInstance()->GetCommandList().Get()->DrawInstanced(
		UINT(modelData.vertices.size()), 1, 0, 0);
}

Mesh::ModelData Mesh::LoadObjFile(const std::string& directoryPath, const std::string& filename)
{
	ModelData modelData;
	std::vector<Vector4> positions;
	std::vector<Vector3> normals;
	std::vector<Vector2> texcoords;
	std::string line;

	std::ifstream file(directoryPath + "/" + filename);
	assert(file.is_open());

	while (std::getline(file, line))
	{
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		if (identifier == "v")
		{
			Vector4 position;
			position.w = 1.0f;
			s >> position.x >> position.y >> position.z;
			position.w = 1.0f;
			positions.push_back(position);
		}
		else if (identifier == "vt")
		{
			Vector2 texcoord;
			s >> texcoord.x >> texcoord.y;
			texcoord.y = 1.0f - texcoord.y;
			texcoords.push_back(texcoord);
		}
		else if (identifier == "vn")
		{
			Vector3 normal;
			s >> normal.x >> normal.y >> normal.z;
			normals.push_back(normal);
		}
		else if (identifier == "mtllib")
		{
			std::string materialFilemane;
			s >> materialFilemane;
			modelData.material = LoadMaterialTemplateFile(directoryPath, materialFilemane);
		}
		else if (identifier == "f")
		{
			VertexData triangle[3];

			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
				std::string vertexDefinition;
				s >> vertexDefinition;

				std::istringstream v(vertexDefinition);
				uint32_t elementIndices[3];
				for (int32_t element = 0; element < 3; ++element) {
					std::string index;
					std::getline(v, index, '/');
					elementIndices[element] = std::stoi(index);
				}

				Vector4 position = positions[elementIndices[0] - 1];
				Vector2 texcoord = texcoords[elementIndices[1] - 1];
				Vector3 normal = normals[elementIndices[2] - 1];
				VertexData vertex = { position, texcoord, normal };
				modelData.vertices.push_back(vertex);
				triangle[faceVertex] = { position, texcoord, normal };
			}
			modelData.vertices.push_back(triangle[0]);
			modelData.vertices.push_back(triangle[1]);
			modelData.vertices.push_back(triangle[2]);
		}
	}
	return modelData;
}


Mesh::MaterialData Mesh::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename)
{
	MaterialData materialData;
	std::string line;

	std::ifstream file(directoryPath + "/" + filename);
	assert(file.is_open());

	while (std::getline(file, line)) {
		std::string identifier;
		std::stringstream s(line);
		s >> identifier;

		if (identifier == "map_Kd") {
			std::string textureFilename;
			s >> textureFilename;
			materialData.textureFilePath = directoryPath + "/" + textureFilename;
		}
	}
	return materialData;
}

//void Mesh::MeshRelease()
//{
//	signatureBlob->Release();
//	if (errorBlob) {
//		errorBlob->Release();
//	}
//	pixelShaderBlob->Release();
//	vertexShaderBlob->Release();
//}

void Mesh::MakeShaderResourceView(const DirectX::TexMetadata& metadata, const DirectX::TexMetadata& metadata2)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc2{};
	srvDesc2.Format = metadata2.format;
	srvDesc2.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc2.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc2.Texture2D.MipLevels = UINT(metadata2.mipLevels);

	const uint32_t descriptorSizeSRV = DX12Common::GetInstance()->GetDevice().Get()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	textureSrvHandleCPU = DX12Common::GetInstance()->GetCPUDescriptorHandle(DX12Common::GetInstance()->GetSrvDescriptorHeap().Get(), descriptorSizeSRV, 1);
	textureSrvHandleGPU = DX12Common::GetInstance()->GetGPUDescriptorHandle(DX12Common::GetInstance()->GetSrvDescriptorHeap().Get(), descriptorSizeSRV, 1);

	textureSrvHandleCPU2 = DX12Common::GetInstance()->GetCPUDescriptorHandle(DX12Common::GetInstance()->GetSrvDescriptorHeap().Get(), descriptorSizeSRV, 2);
	textureSrvHandleGPU2 = DX12Common::GetInstance()->GetGPUDescriptorHandle(DX12Common::GetInstance()->GetSrvDescriptorHeap().Get(), descriptorSizeSRV, 2);

	DX12Common::GetInstance()->GetDevice().Get()->CreateShaderResourceView(textureResource.Get(), &srvDesc, textureSrvHandleCPU);
	DX12Common::GetInstance()->GetDevice().Get()->CreateShaderResourceView(textureResource2.Get(), &srvDesc2, textureSrvHandleCPU2);
}

DirectX::ScratchImage Mesh::LoadTexture(const std::string& filePath)
{
	DirectX::ScratchImage image{};
	std::wstring filePathW = debug_->ConvertString(filePath);
	HRESULT hr = DirectX::LoadFromWICFile(
		filePathW.c_str(),
		DirectX::WIC_FLAGS_FORCE_SRGB,
		nullptr,
		image);
	assert(SUCCEEDED(hr));

	DirectX::ScratchImage mipImages{};
	hr = DirectX::GenerateMipMaps(
		image.GetImages(),
		image.GetImageCount(),
		image.GetMetadata(),
		DirectX::TEX_FILTER_SRGB,
		0,
		mipImages);

	return mipImages;
}

Microsoft::WRL::ComPtr<ID3D12Resource> Mesh::CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata)
{
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(metadata.width);
	resourceDesc.Height = UINT(metadata.height);
	resourceDesc.MipLevels = UINT16(metadata.mipLevels);
	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize);
	resourceDesc.Format = metadata.format;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);

	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_CUSTOM;
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

	Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&resource));

	assert(SUCCEEDED(hr));

	return resource;
}

void Mesh::UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages, const DirectX::TexMetadata& metadata)
{
	for (size_t mipLevel = 0; mipLevel < metadata.mipLevels; mipLevel++)
	{
		const DirectX::Image* img = mipImages.GetImage(mipLevel, 0, 0);
		HRESULT hr = texture->WriteToSubresource(
			UINT(mipLevel),
			nullptr,
			img->pixels,
			UINT(img->rowPitch),
			UINT(img->slicePitch)
		);
		assert(SUCCEEDED(hr));
	}
}