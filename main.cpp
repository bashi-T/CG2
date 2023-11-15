#include"WindowApp.h"
#include"Debug.h"
#include"Mesh.h"

const int32_t kWindowWidth = 1280;
const int32_t kWindowHeight = 720;
const int32_t kNumTriangle = 2;
Vector4* vertexData = nullptr;
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	Debug::D3DResourceLeakChecker* leakCheck = new Debug::D3DResourceLeakChecker;	CoInitializeEx(0, COINIT_MULTITHREADED);
	Mesh* mesh[kNumTriangle];
	for (int i = 0; i < kNumTriangle; i++)
	{
		mesh[i] = new Mesh;
	}

	DX12Common* dx12Common = DX12Common::GetInstance();
	Debug* debug = new Debug;
	WinAPP* winAPP = WinAPP::GetInstance();
	MSG NewMSG = winAPP->GetMSG();
	MyImGui* imgui = new MyImGui;
	bool useWorldMap = true;

	winAPP->Initialize(kWindowWidth, kWindowHeight);
	winAPP->CreateWindowView(L"CG2");

	dx12Common->Init( kWindowWidth, kWindowHeight);
	imgui->Initialize(
	    winAPP->GetHWND(),
		dx12Common->GetInstance()->GetDevice().Get(),
	    dx12Common->GetInstance()->GetSwapChainDesc(),
	    dx12Common->GetInstance()->GetRtvDesc(),
	    dx12Common->GetInstance()->GetSrvDescriptorHeap().Get());

	Vector4 Top[kNumTriangle];
	Vector4 Left[kNumTriangle];
	Vector4 Right[kNumTriangle];
	Vector4 Color[kNumTriangle];
	Vector2 texcoordTop[kNumTriangle];
	Vector2 texcoordLeft[kNumTriangle];
	Vector2 texcoordRight[kNumTriangle];

	Top[0] = { 0.0f, 1.75f, 0.0f, 1.0f };
	Right[0] = { 0.5f, -0.5f, 0.0f, 1.0f };
	Left[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
	Color[0] = { 0.5f, 0.5f, 0.5f, 1.0f };
	texcoordTop[0] = { 0.5f,0.0f };
	texcoordRight[0] = { 1.0f,1.0f };
	texcoordLeft[0] = { 0.0f,1.0f };

	Vector4 LeftTop[kNumTriangle];
	Vector4 RightTop[kNumTriangle];
	Vector4 RightBottom[kNumTriangle];
	Vector4 LeftBottom[kNumTriangle];
	Vector4 ColorSprite[kNumTriangle];
	Vector2 texcoordLeftTop[kNumTriangle];
	Vector2 texcoordRightTop[kNumTriangle];
	Vector2 texcoordRightBottom[kNumTriangle];
	Vector2 texcoordLeftBottom[kNumTriangle];

	Vector4 ColorSphere[kNumTriangle];
	
	LeftTop[0] = {0.0f, 0.0f, 0.0f, 1.0f};
	RightTop[0] = { kWindowWidth/3, 0.0f, 0.0f, 1.0f};
	RightBottom[0] = { kWindowWidth/3, kWindowHeight/3, 0.0f, 1.0f};
	LeftBottom[0] = { 0.0f, kWindowHeight/3, 0.0f, 1.0f};
	ColorSprite[0] = { 1.0f, 1.0f, 1.0f, 1.0f };
	texcoordLeftTop[0] = {0.0f, 0.0f};
	texcoordRightTop[0] = {1.0f, 0.0f};
	texcoordRightBottom[0] = {1.0f, 1.0f};
	texcoordLeftBottom[0] = {0.0f, 1.0f};

	
	Sphere sphere = { { 0.0f,0.0f,0.0f },1.0f };
	ColorSphere[0] = { 1.0f, 1.0f, 1.0f, 1.0f };


	for (int i = 0; i < kNumTriangle; i++)
	{
		mesh[i]->Initialize("axis.obj", kWindowWidth, kWindowHeight);
	}

	while (NewMSG.message != WM_QUIT)
	{
		
		imgui->Update();

		for (int i = 0; i < kNumTriangle; i++) {
			mesh[i]->Update();
		}
		//ImGui::Begin("sphereEdit");
		//ImGui::ColorEdit3("Sphere", (float*)&ColorSphere[0]);
		//ImGui::DragFloat3("sphere.center", (float*)&sphere.center, 0.01f);
		//ImGui::DragFloat("sphere.radius", (float*)&sphere.radius, 0.01f);
		//ImGui::Checkbox("useWorldMap", &useWorldMap);
		//ImGui::End();

				if (PeekMessage(&NewMSG, NULL, 0, 0, PM_REMOVE)) {
			winAPP->ProcessMessage(NewMSG);
			ImGui::Render();
		} else{
			dx12Common->DrawScreen();

			Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeaps[] =
			{
			    dx12Common->GetInstance()->GetSrvDescriptorHeap().Get()
			};
			dx12Common->GetInstance()->GetCommandList()->
				SetDescriptorHeaps(1, descriptorHeaps->GetAddressOf());

			//for (int i = 0; i < kNumTriangle; i++) {
			//	mesh[i]->DrawTriangle(
			//		Top[i],
			//		Right[i],
			//		Left[i],
			//		Color[i],
			//		texcoordTop[i],
			//		texcoordRight[i],
			//		texcoordLeft[i],
			//		useWorldMap);
			//}
				//mesh[0]->DrawSprite(
				//	LeftTop[0],
				//	RightTop[0],
				//	RightBottom[0],
				//	LeftBottom[0],
				//	ColorSprite[0],
				//	texcoordLeftTop[0],
				//	texcoordRightTop[0],
				//	texcoordRightBottom[0],
				//	texcoordLeftBottom[0],
				//	kWindowWidth,
				//	kWindowHeight);

				//mesh[0]->DrawSphere(
			 //       sphere, ColorSphere[0], useWorldMap, kWindowWidth, kWindowHeight);
			mesh[0]->DrawOBJ(ColorSphere[0], useWorldMap, kWindowWidth, kWindowHeight);

				imgui->Endframe(dx12Common->GetInstance()->GetCommandList().Get());
			dx12Common->ClearScreen();
		}
	}

	CloseHandle(dx12Common->GetFenceEvent());
	//for (int i = 0; i < kNumTriangle; i++)
	//{
	//	mesh[i]->MeshRelease();
	//}
	CoUninitialize();
	delete leakCheck;
	return 0;
}