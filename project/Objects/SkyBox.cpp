#include "SkyBox.h"

void SkyBox::Initialize(const std::string& filename)
{
	projectionMatrix = MakePerspectiveFovMatrix(0.65f, float(WinAPP::clientWidth_) / float(WinAPP::clientHeight_), 0.1f, 100.0f);
	vertexResource = CreateBufferResource(sizeof(VertexData) * 6);
	colorResource = CreateBufferResource(sizeof(Material));
	indexResource = CreateBufferResource(sizeof(uint32_t) * 6);
	cameraResource = CreateBufferResource(sizeof(CameraTransform));

	MakeBufferView();

	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	colorResource->Map(0, nullptr, reinterpret_cast<void**>(&colorData));
	cameraResource->Map(0, nullptr, reinterpret_cast<void**>(&cameraData));
	InputData();
}

void SkyBox::Update()
{
}

void SkyBox::Draw()
{
}

ComPtr<ID3D12Resource> SkyBox::CreateBufferResource(size_t sizeInBytes)
{
	return ComPtr<ID3D12Resource>();
}

void SkyBox::MakeBufferView()
{
}

void SkyBox::InputData()
{
	//右面 [0,1,2][2,1,3]
	vertexData[0].position = { 1.0f,1.0f,1.0f,1.0f };
	vertexData[1].position = { 1.0f,1.0f,-1.0f,1.0f };
	vertexData[2].position = { 1.0f,-1.0f,1.0f,1.0f };
	vertexData[3].position = { 1.0f,-1.0f,-1.0f,1.0f };
	//左面　[4,5,6][6,5,7]
	vertexData[4].position = { -1.0f,1.0f,-1.0f,1.0f };
	vertexData[5].position = { -1.0f,1.0f,1.0f,1.0f };
	vertexData[6].position = { -1.0f,-1.0f,-1.0f,1.0f };
	vertexData[7].position = { -1.0f,-1.0f,1.0f,1.0f };
	//前面 [8,9,10][10,9,11]
	vertexData[8].position = { -1.0f,1.0f,1.0f,1.0f };
	vertexData[9].position = { 1.0f,1.0f,1.0f,1.0f };
	vertexData[10].position = { -1.0f,-1.0f,1.0f,1.0f };
	vertexData[11].position = { 1.0f,-1.0f,1.0f,1.0f };
	//後面 [12,13,14][14,13,15]
	vertexData[12].position = { 1.0f,1.0f,-1.0f,1.0f };
	vertexData[13].position = { 1.0f,1.0f,-1.0f,1.0f };
	vertexData[14].position = { 1.0f,1.0f,-1.0f,1.0f };
	vertexData[15].position = { 1.0f,1.0f,-1.0f,1.0f };
	//上面 [16,17,18][18,17,19]
	vertexData[16].position = { 1.0f,1.0f,1.0f,1.0f };
	vertexData[17].position = { 1.0f,1.0f,1.0f,1.0f };
	vertexData[18].position = { 1.0f,1.0f,1.0f,1.0f };
	vertexData[19].position = { 1.0f,1.0f,1.0f,1.0f };
	//下面 [20,21,22][22,21,23]
	vertexData[20].position = { 1.0f,-1.0f,1.0f,1.0f };
	vertexData[21].position = { 1.0f,-1.0f,1.0f,1.0f };
	vertexData[22].position = { 1.0f,-1.0f,1.0f,1.0f };
	vertexData[23].position = { 1.0f,-1.0f,1.0f,1.0f };
}

void SkyBox::MakeShaderResourceViewInstance()
{
}
