#include"Systems/WindowApp.h"
#include"Systems/Debug.h"
#include"Input/Input.h"
#include"Commons/SpriteCommon.h"
#include"Sprites/Sprite.h"
#include"Commons/Object3dCommon.h"
#include"Objects/Object3d.h"
#include"Managers/ModelManager.h"
#include"Objects/model.h"
#include"Managers/TextureManager.h"
#include"Objects/Particle.h"

const int32_t kWindowWidth = 1280;
const int32_t kWindowHeight = 720;
const int32_t kNumTriangle = 1;
Vector4* vertexData = nullptr;
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	CoInitializeEx(0, COINIT_MULTITHREADED);
	Debug::D3DResourceLeakChecker* leakCheck = new Debug::D3DResourceLeakChecker;
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	WinAPP* winAPP = WinAPP::GetInstance();
	DX12Common* dx12Common = DX12Common::GetInstance();
	Input* input = new Input;
	MSG NewMSG = winAPP->GetMSG();
	MyImGui* imgui = new MyImGui;
	SpriteCommon* SPCommon = new SpriteCommon;
	Object3dCommon* object3dCommon = nullptr;
	object3dCommon = new Object3dCommon;
	ModelCommon* modelCommon = new ModelCommon;
	Camera* camera = new Camera();
	Particle* particle = new Particle;
	std::vector<Object3d*> objects3d;
	std::vector<Model*> models;
	std::vector<Sprite*> sprites;
	Vector2 posSprite = { 0.0f,0.0f };
	//for (int i = 0; i < kNumTriangle; i++)
	//{
	//	mesh[i] = new Mesh;
	//}
	//bool useWorldMap = true;

	std::string textureFilePath[10] =
	{
		"Resource/civ6.png",
		"Resource/uvChecker.png",
		"Resource/monsterBall.png",
		"Resource/worldMap.png",
		"Resource/world.png",
		"Resource/circle.png",
		"Resource/uvChecker.png",
		"Resource/uvChecker.png",
		"Resource/uvChecker.png",
		"Resource/cursor.png"
	};

	std::string objFilePath[10] =
	{
		"plane.obj",
		"axis.obj",
		"world.obj",
	};

	winAPP->Initialize(WinAPP::clientWidth_,WinAPP::clientHeight_, L"GE3");
	dx12Common->Initialize(WinAPP::clientWidth_, WinAPP::clientHeight_, winAPP);
	input->Initialize(winAPP);
	imgui->Initialize(
		winAPP->GetHWND(),
		dx12Common->GetDevice().Get(),
		dx12Common->GetSwapChainDesc(),
		dx12Common->GetRtvDesc(),
		dx12Common->GetSrvDescriptorHeap().Get());
	TextureManager::GetInstance()->Initialize();

	object3dCommon->Initialize(dx12Common);
	ModelManager::GetInstance()->Initialize(dx12Common);
	camera->SetRotate({ 0.0f,0.0f,0.0f });
	camera->SetTranslate({ 0.0f,0.0f,-10.0f });

	object3dCommon->SetDefaultCamera(camera);
	for (uint32_t i = 0; i < 1; i++)
	{
		Object3d* object3d = new Object3d;
		object3d->Initialize(object3dCommon);
		ModelManager::GetInstance()->LoadModel(objFilePath[2]);
		object3d->SetModel(objFilePath[2]);
		Model* model = ModelManager::GetInstance()->FindModel(objFilePath[2]);
		Model::ModelData* modelData = model->GetModelData();
		for (Model::VertexData& vertex : modelData->vertices)
		{
			vertex.normal.x = vertex.position.x;
			vertex.normal.y = vertex.position.y;
			vertex.normal.z = vertex.position.z;
		}
		model->Memcpy();
		object3d->SetTranslate({ 0.2f * i, 0.2f * i, 0.2f * i });
		object3d->SetScale({ 0.02f, 0.02f, 0.02f  });
		objects3d.push_back(object3d);
	};

	SPCommon->Initialize(dx12Common);
	for (uint32_t i = 0; i < 10; i++)
	{
		Sprite* sprite = new Sprite();
		sprite->Initialize(kWindowWidth, kWindowHeight, SPCommon, textureFilePath[i]);
		posSprite.x = 100.0f * i;
		posSprite.y = 50.0f * i;
		sprite->SetPositoin(posSprite);
		sprites.push_back(sprite);
	}
	
	particle->Initialize(textureFilePath[1], WinAPP::clientWidth_, WinAPP::clientHeight_, object3dCommon);
	Vector3 directionlLight = { 0.0f,-1.0f,0.0f };

	while (NewMSG.message != WM_QUIT)
	{
		dx12Common->update();
		input->Update();
		camera->Update();
		imgui->Update();
		ImGui::Begin("sphereEdit");
		
		for (Object3d* object3d : objects3d)
		{
			if (input->PushKey(DIK_D))
			{
				object3d->SetTranslate({ object3d->GetTranslate().x + 0.01f ,object3d->GetTranslate().y ,object3d->GetTranslate().z });
			}
			if (input->PushKey(DIK_A))
			{
				object3d->SetTranslate({ object3d->GetTranslate().x - 0.01f ,object3d->GetTranslate().y ,object3d->GetTranslate().z });
			}
			object3d->Update();
			ImGui::DragFloat3("object.rotate", (float*)&object3d->GetRotate(), 0.01f);
			ImGui::DragFloat3("object.translate", (float*)&object3d->GetTranslate(), 0.01f);
			ImGui::DragFloat3("camera.rotate", (float*)&camera->GetRotate(), 0.01f);
			ImGui::DragFloat3("camera.translate", (float*)&camera->GetTranslate(), 0.01f);
			ImGui::DragFloat4("light.color", (float*)&object3d->GetDirectionalLightData()->color, 0.01f);
			ImGui::DragFloat("light.intensity", (float*)&object3d->GetDirectionalLightData()->intensity, 0.01f);
			ImGui::DragFloat3("light.direction", (float*)&directionlLight, 0.01f,-1.0f,1.0f);
			object3d->GetDirectionalLightData()->direction = Normalize(directionlLight);
		}
		ImGui::DragFloat4("particles.color", (float*)&particle->GetInstancingDataPlane()->color, 0.01f);
		ImGui::ColorEdit4("particles.color", (float*)&particle->GetParticlesPlane()->color, 0.01f);

		particle->Update();

		for (Sprite* sprite : sprites)
		{
			sprite->Update(kWindowWidth, kWindowHeight);
		}
		ImGui::End();

		if (winAPP->ProcessMessage())
		{
			ImGui::Render();
			break;
		}
		dx12Common->PreDraw();

		//for (Object3d* object3d : objects3d)
		//{
		//	object3d->Draw(object3dCommon, ModelManager::GetInstance()->GetModelCommon());
		//}
		
		particle->Draw();

		if (input->PushKey(DIK_SPACE)!=0) {
			for (Sprite* sprite : sprites)
			{
				sprite->Draw(SPCommon);
			}
		}
		imgui->Endframe(dx12Common->GetCommandList().Get());

		dx12Common->PostDraw();

	}

	CloseHandle(dx12Common->GetFenceEvent());
	delete particle;
	for (Sprite* sprite : sprites)
	{
		delete sprite;
	}
	for (Model* model : models)
	{
		delete model;
	}
	ModelManager::GetInstance()->Finalize();
for (Object3d* object3d : objects3d)
	{
		delete object3d;
	}
	delete object3dCommon;
	TextureManager::GetInstance()->Finalize();
	imgui->Finalize();
	dx12Common->DeleteInstance();
	winAPP->Finalize();
	CoUninitialize();
	delete leakCheck;
	return 0;
}