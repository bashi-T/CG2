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
	(void)CoInitializeEx(0, COINIT_MULTITHREADED);
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
	object3d = new Object3d;
	modelCommon = new ModelCommon;
	camera = new Camera();
	particle = new Particle;
	skyboxCommon = new SkyBoxCommon;
	skybox = new SkyBox;
	std::vector<Model*> models;
	//bool useWorldMap = true;

	std::string textureFilePath[100] =//textureの左右が反転してる
	{
		"Resource/civ6.png",
		"Resource/ganban.png",
		"Resource/white.png",
		"Resource/worldMap.png",
		"Resource/monsterBall.png",
		"Resource/AnimatedCube/AnimatedCube_BaseColor.png",
		"Resource/world.png",
		"Resource/uvChecker.png",
		"Resource/cursor.png",
		"Resource/circle.png",
		"Resource/particle.png",
		"Resource/rostock_laage_airport_4k.dds",
	};

	winAPP->Initialize(WinAPP::clientWidth_, WinAPP::clientHeight_, L"GE3");
	dx12Common->Initialize(WinAPP::clientWidth_, WinAPP::clientHeight_, winAPP);
	srvManager->Initialize();
	input->Initialize(winAPP);
	imgui->Initialize(
		winAPP->GetHWND(),
		dx12Common->GetDevice().Get(),
		dx12Common->GetSwapChainDesc(),
		dx12Common->GetRtvDesc(),
		srvManager->GetSrvDescriptorHeap().Get());
	TextureManager::GetInstance()->Initialize();

	object3dCommon->Initialize(dx12Common);
	ModelManager::GetInstance()->Initialize(dx12Common);
	camera->GetInstance()->SetRotate({ 0.26f,0.0f,0.0f });
	camera->GetInstance()->SetTranslate({ 0.0f,7.0f,-20.0f });

	object3dCommon->SetDefaultCamera(camera->GetInstance());
	SPCommon->Initialize();

	skyboxCommon->Initialize(dx12Common);
	skybox->Initialize(skyboxCommon, "Resource/rostock_laage_airport_4k.dds");
	sceneArr_[TITLE]->Init();
	sceneArr_[INGAME]->Init();

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
		skybox->Update();
		sceneArr_[currentSceneNo_]->Update();
#ifdef _DEBUG
		ImGui::Begin("camera");

		//ImGui::DragFloat3("object.rotate", (float*)&object3d->GetRotate(), 0.01f);
			//ImGui::DragFloat3("object.translate", (float*)&object3d->GetTranslate(), 0.01f);
		ImGui::DragFloat3("camera.rotate", (float*)&camera->GetInstance()->GetRotate(), 0.01f);
		ImGui::DragFloat3("camera.translate", (float*)&camera->GetInstance()->GetTranslate(), 0.01f);
		//ImGui::DragFloat4("light.color", (float*)&object3d->GetDirectionalLightData()->color, 0.01f);
			//ImGui::DragFloat("light.intensity", (float*)&object3d->GetDirectionalLightData()->intensity, 0.01f);
		ImGui::DragFloat3("light.direction", (float*)&object3d->GetDirectionalLight().direction, 0.01f, -1.0f, 1.0f);
		//object3d->GetDirectionalLightData()->direction = Normalize(directionlLight);
		//ImGui::DragFloat4("particles.color", (float*)&particle->GetInstancingDataPlane()->color, 0.01f);
		//ImGui::ColorEdit4("particles.color", (float*)&particle->GetParticlesPlane()->color, 0.01f);
		ImGui::End();
#endif
		if (winAPP->ProcessMessage())
		{
			ImGui::Render();
			break;
		}
		dx12Common->PreDraw();
		skybox->Draw(skyboxCommon);
		sceneArr_[currentSceneNo_]->Draw();

		imgui->Endframe(dx12Common->GetCommandList().Get());

		dx12Common->PostDraw();
	}

	CloseHandle(dx12Common->GetFenceEvent());
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
