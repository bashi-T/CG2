#pragma once
#include<memory>
#include<iostream>
#include"SceneManager.h"
#include"TitleScene.h"
#include"GameScene.h"

#include"Systems/WindowApp.h"
#include"Systems/Debug.h"
#include"Input/Input.h"
#include"Commons/SpriteCommon.h"
#include"Commons/Object3dCommon.h"
#include"Commons/SkyBoxCommon.h"
#include"Managers/ModelManager.h"
#include"Objects/model.h"
#include"Managers/TextureManager.h"
#include"Managers/SRVManager.h"
#include"Objects/SkyBox.h"

class GameManager
{
private:
	std::unique_ptr<SceneManager> sceneArr_[2];
	int currentSceneNo_ = 0;
	int prevSceneNo_ = 0;
	WinAPP* winAPP = nullptr;
	DX12Common* dx12Common=nullptr;
	SRVManager* srvManager=nullptr;
	Input* input = nullptr;
	MyImGui* imgui = nullptr;
	SpriteCommon* SPCommon = nullptr;
	Object3dCommon* object3dCommon = nullptr;
	Object3d* object3d = nullptr;
	ModelCommon* modelCommon = nullptr;
	Camera* camera = nullptr;
	Particle* particle = nullptr;
	SkyBoxCommon* skyboxCommon = nullptr;
	SkyBox* skybox = nullptr;

public: 
	GameManager();
	~GameManager();
	int Run();
};

