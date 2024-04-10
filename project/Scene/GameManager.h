#pragma once
#include<memory>
#include"SceneManager.h"
#include"TitleScene.h"
#include"GameScene.h"

#include"Systems/WindowApp.h"
#include"Systems/Debug.h"
#include"Input/Input.h"
#include"Commons/SpriteCommon.h"
#include"Commons/Object3dCommon.h"
#include"Managers/ModelManager.h"
#include"Objects/model.h"
#include"Managers/TextureManager.h"
#include"Managers/SRVManager.h"

class GameManager
{
private:
	std::unique_ptr<SceneManager> sceneArr_[2];
	int currentSceneNo_ = 0;
	int prevSceneNo_ = 0;

	WinAPP* winAPP = WinAPP::GetInstance();
	DX12Common* dx12Common = DX12Common::GetInstance();
	SRVManager* srvManager = SRVManager::GetInstance();
	TextureManager* textureManager = TextureManager::GetInstance();
	Input* input = Input::GetInstance();
	Camera* camera = Camera::GetInstance();

	SpriteCommon* SPCommon = SpriteCommon::GetInstance();

	Object3dCommon* object3dCommon = Object3dCommon::GetInstance();
	ModelManager* modelManager = ModelManager::GetInstance();
	ModelCommon* modelCommon = ModelCommon::GetInstance();
	

	MyImGui* imgui;
	Particle* particle;
public: 
	GameManager();
	~GameManager();
	int Run();
};

