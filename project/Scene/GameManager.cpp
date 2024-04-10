#include "GameManager.h"
Vector4* vertexData = nullptr;

GameManager::GameManager()
{
	sceneArr_[TITLE] = std::make_unique<TitleScene>();
	sceneArr_[INGAME] = std::make_unique<GameScene>();
	currentSceneNo_ = TITLE;
}

GameManager::~GameManager() {}

int GameManager::Run()
{
	CoInitializeEx(0, COINIT_MULTITHREADED);
	Debug::D3DResourceLeakChecker* leakCheck = new Debug::D3DResourceLeakChecker;
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	MSG NewMSG = WinAPP::GetInstance()->GetMSG();
	imgui = new MyImGui;
	modelCommon = new ModelCommon;
	particle = new Particle;
	std::vector<Model*> models;
	//bool useWorldMap = true;



	WinAPP::GetInstance()->Initialize(L"GE3");
	DX12Common::GetInstance()->Initialize();
	SRVManager::GetInstance()->Initialize();
	Input::GetInstance()->Initialize();
	imgui->Initialize(
		WinAPP::GetInstance()->GetHWND(),
		DX12Common::GetInstance()->GetDevice().Get(),
		DX12Common::GetInstance()->GetSwapChainDesc(),
		DX12Common::GetInstance()->GetRtvDesc(),
		SRVManager::GetInstance()->GetSrvDescriptorHeap().Get());
	TextureManager::GetInstance()->Initialize();

	Object3dCommon::GetInstance()->Initialize();
	ModelManager::GetInstance()->Initialize();
	Camera::GetInstance()->SetRotate({0.0f,0.0f,0.0f});
	Camera::GetInstance()->SetTranslate({ 0.0f,0.0f,-10.0f });

	Object3dCommon::GetInstance()->SetDefaultCamera();
	SpriteCommon::GetInstance()->Initialize();

	sceneArr_[TITLE]->Init();
	sceneArr_[INGAME]->Init();

	Vector3 directionlLight = { 0.0f,-1.0f,0.0f };
	while (NewMSG.message != WM_QUIT)
	{
		DX12Common::GetInstance()->update();
		Input::GetInstance()->Update();
		if (Input::GetInstance()->PushKey(DIK_RIGHT))
		{
			Camera::GetInstance()->SetTranslate({ Camera::GetInstance()->GetTranslate().x + 0.2f, Camera::GetInstance()->GetTranslate().y, Camera::GetInstance()->GetTranslate().z });
		}
		if (Input::GetInstance()->PushKey(DIK_LEFT))
		{
			Camera::GetInstance()->SetTranslate({ Camera::GetInstance()->GetTranslate().x - 0.2f, Camera::GetInstance()->GetTranslate().y, Camera::GetInstance()->GetTranslate().z});
		}
		Camera::GetInstance()->Update();
		prevSceneNo_ = currentSceneNo_;
		currentSceneNo_ = sceneArr_[currentSceneNo_]->GetSceneNo();
		//if (prevSceneNo_ != currentSceneNo_) {
		//	sceneArr_[currentSceneNo_]->Init();
		//}
		sceneArr_[currentSceneNo_]->Update();
		imgui->Update();

		//ImGui::Begin("sphereEdit");
		//	ImGui::DragFloat3("object.rotate", (float*)&object3d->GetRotate(), 0.01f);
		//	ImGui::DragFloat3("object.translate", (float*)&object3d->GetTranslate(), 0.01f);
		//	ImGui::DragFloat3("Camera.rotate", (float*)&Camera::GetRotate(), 0.01f);
		//	ImGui::DragFloat3("Camera.translate", (float*)&Camera::GetTranslate(), 0.01f);
		//	ImGui::DragFloat4("light.color", (float*)&object3d->GetDirectionalLightData()->color, 0.01f);
		//	ImGui::DragFloat("light.intensity", (float*)&object3d->GetDirectionalLightData()->intensity, 0.01f);
		//	ImGui::DragFloat3("light.direction", (float*)&directionlLight, 0.01f,-1.0f,1.0f);
		//	object3d->GetDirectionalLightData()->direction = Normalize(directionlLight);
		//
		//ImGui::DragFloat4("particles.color", (float*)&particle->GetInstancingDataPlane()->color, 0.01f);
		//ImGui::ColorEdit4("particles.color", (float*)&particle->GetParticlesPlane()->color, 0.01f);
		//ImGui::End();

		if (WinAPP::GetInstance()->ProcessMessage())
		{
			ImGui::Render();
			break;
		}
		SRVManager::GetInstance()->PreDraw();
		sceneArr_[currentSceneNo_]->Draw();

		imgui->Endframe(DX12Common::GetInstance()->GetCommandList().Get());

		SRVManager::GetInstance()->PostDraw();
	}

	CloseHandle(SRVManager::GetInstance()->GetFenceEvent());
	delete particle;
	sceneArr_[TITLE]->Finalize();
	for (Model* model : models)
	{
		delete model;
	}
	sceneArr_[INGAME]->Finalize();
	ModelManager::GetInstance()->Finalize();
	delete Object3dCommon::GetInstance();
	TextureManager::GetInstance()->Finalize();
	imgui->Finalize();
	delete SRVManager::GetInstance();
	DX12Common::GetInstance()->DeleteInstance();
	WinAPP::GetInstance()->Finalize();
	CoUninitialize();
	delete leakCheck;
	return 0;

}
