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

	winAPP = WinAPP::GetInstance();
	dx12Common = DX12Common::GetInstance();
	srvManager = SRVManager::GetInstance();
	input = Input::GetInstance();
	MSG NewMSG = winAPP->GetMSG();
	imgui = new MyImGui;
	SPCommon = SpriteCommon::GetInstance();
	object3dCommon = Object3dCommon::GetInstance();
	modelCommon = new ModelCommon;
	camera = new Camera();
	particle = new Particle;
	std::vector<Model*> models;
	//bool useWorldMap = true;



	winAPP->Initialize(WinAPP::clientWidth_, WinAPP::clientHeight_, L"GE3");
	dx12Common->Initialize(WinAPP::clientWidth_, WinAPP::clientHeight_, winAPP);
	srvManager->Initialize(dx12Common);
	input->Initialize(winAPP);
	imgui->Initialize(
		winAPP->GetHWND(),
		dx12Common->GetDevice().Get(),
		dx12Common->GetSwapChainDesc(),
		dx12Common->GetRtvDesc(),
		srvManager->GetSrvDescriptorHeap().Get());
	TextureManager::GetInstance()->Initialize(dx12Common, srvManager);

	object3dCommon->Initialize(dx12Common);
	ModelManager::GetInstance()->Initialize(dx12Common);
	camera->GetInstance()->SetRotate({0.0f,0.0f,0.0f});
	camera->GetInstance()->SetTranslate({ 0.0f,0.0f,-10.0f });

	object3dCommon->SetDefaultCamera(camera->GetInstance());
	SPCommon->Initialize(dx12Common);

	sceneArr_[TITLE]->Init();
	sceneArr_[INGAME]->Init();

	Vector3 directionlLight = { 0.0f,-1.0f,0.0f };
	while (NewMSG.message != WM_QUIT)
	{
		dx12Common->update();
		Input::GetInstance()->Update();
		if (Input::GetInstance()->PushKey(DIK_RIGHT))
		{
			camera->GetInstance()->SetTranslate({ camera->GetInstance()->GetTranslate().x + 0.2f, camera->GetInstance()->GetTranslate().y, camera->GetInstance()->GetTranslate().z });
		}
		if (Input::GetInstance()->PushKey(DIK_LEFT))
		{
			camera->GetInstance()->SetTranslate({ camera->GetInstance()->GetTranslate().x - 0.2f, camera->GetInstance()->GetTranslate().y, camera->GetInstance()->GetTranslate().z});
		}
		camera->GetInstance()->Update();
		prevSceneNo_ = currentSceneNo_;
		currentSceneNo_ = sceneArr_[currentSceneNo_]->GetSceneNo();
		//if (prevSceneNo_ != currentSceneNo_) {
		//	sceneArr_[currentSceneNo_]->Init();
		//}
		imgui->Update();

		ImGui::Begin("sphereEdit");
		sceneArr_[currentSceneNo_]->Update();
			//ImGui::DragFloat3("object.rotate", (float*)&object3d->GetRotate(), 0.01f);
			//ImGui::DragFloat3("object.translate", (float*)&object3d->GetTranslate(), 0.01f);
			ImGui::DragFloat3("camera.rotate", (float*)&camera->GetInstance()->GetRotate(), 0.01f);
			ImGui::DragFloat3("camera.translate", (float*)&camera->GetInstance()->GetTranslate(), 0.01f);
			//ImGui::DragFloat4("light.color", (float*)&object3d->GetDirectionalLightData()->color, 0.01f);
			//ImGui::DragFloat("light.intensity", (float*)&object3d->GetDirectionalLightData()->intensity, 0.01f);
			ImGui::DragFloat3("light.direction", (float*)&directionlLight, 0.01f,-1.0f,1.0f);
			//object3d->GetDirectionalLightData()->direction = Normalize(directionlLight);
		
		//ImGui::DragFloat4("particles.color", (float*)&particle->GetInstancingDataPlane()->color, 0.01f);
		//ImGui::ColorEdit4("particles.color", (float*)&particle->GetParticlesPlane()->color, 0.01f);
		ImGui::End();

		if (winAPP->ProcessMessage())
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
	ModelManager::GetInstance()->Finalize();
	delete object3dCommon;
	TextureManager::GetInstance()->Finalize();
	imgui->Finalize();
	delete srvManager;
	dx12Common->DeleteInstance();
	winAPP->Finalize();
	CoUninitialize();
	delete leakCheck;
	return 0;

}
