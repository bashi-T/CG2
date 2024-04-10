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

	MSG NewMSG = winAPP->GetMSG();
	imgui = new MyImGui;
	particle = new Particle;
	std::vector<Model*> models;

	winAPP->Initialize(L"GE3");
	dx12Common->Initialize();
	srvManager->Initialize();
	input->Initialize();
	imgui->Initialize(
		winAPP->GetHWND(),
		dx12Common->GetDevice().Get(),
		dx12Common->GetSwapChainDesc(),
		dx12Common->GetRtvDesc(),
		srvManager->GetSrvDescriptorHeap().Get());
	textureManager->Initialize();

	object3dCommon->Initialize();
	modelManager->Initialize();
	camera->SetRotate({0.0f,0.0f,0.0f});
	camera->SetTranslate({ 0.0f,0.0f,-10.0f });

	object3dCommon->SetDefaultCamera();
	SPCommon->Initialize();

	sceneArr_[TITLE]->Init();
	sceneArr_[INGAME]->Init();

	Vector3 directionlLight = { 0.0f,-1.0f,0.0f };
	while (NewMSG.message != WM_QUIT)
	{
		dx12Common->update();
		input->Update();
		if (input->PushKey(DIK_RIGHT))
		{
			camera->SetTranslate({ camera->GetTranslate().x + 0.2f, camera->GetTranslate().y, camera->GetTranslate().z });
		}
		if (input->PushKey(DIK_LEFT))
		{
			camera->SetTranslate({ camera->GetTranslate().x - 0.2f, camera->GetTranslate().y, camera->GetTranslate().z});
		}
		camera->Update();
		imgui->Update();

		prevSceneNo_ = currentSceneNo_;
		currentSceneNo_ = sceneArr_[currentSceneNo_]->GetSceneNo();
		//if (prevSceneNo_ != currentSceneNo_) {
		//	sceneArr_[currentSceneNo_]->Init();
		//}
		sceneArr_[currentSceneNo_]->Update();

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
		srvManager->PreDraw();
		sceneArr_[currentSceneNo_]->Draw();

		imgui->Endframe(dx12Common->GetCommandList().Get());

		srvManager->PostDraw();
	}

	CloseHandle(srvManager->GetFenceEvent());
	delete particle;
	sceneArr_[TITLE]->Finalize();
	for (Model* model : models)
	{
		delete model;
	}
	sceneArr_[INGAME]->Finalize();
	modelManager->Finalize();
	delete object3dCommon;
	textureManager->Finalize();
	imgui->Finalize();
	delete srvManager;
	dx12Common->DeleteInstance();
	winAPP->Finalize();
	CoUninitialize();
	delete leakCheck;
	return 0;

}
