#pragma once
#include<memory>
#include<iostream>
#include"SceneManager.h"
#include"TitleScene.h"
#include"GameScene.h"
#include"GameOverScene.h"
#include"ClearScene.h"

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
	std::unique_ptr<SceneManager> sceneArr_[4];
	int currentSceneNo_ = 0;
	int prevSceneNo_ = 0;
	WinAPP* winAPP;
	DX12Common* dx12Common;
	SRVManager* srvManager;
	Input* input;
	MyImGui* imgui;
	SpriteCommon* SPCommon;
	Object3dCommon* object3dCommon;
	Object3d* object3d;
	ModelCommon* modelCommon;
	Camera* camera;
	Particle* particle;

public: 
	GameManager();
	~GameManager();
	int Run();
	static int stageNumber;
};

