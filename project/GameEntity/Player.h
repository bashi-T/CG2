#pragma once
#include"objects/object3d.h"
#include"Commons/Object3dCommon.h"
#include"Managers/SRVManager.h"
#include"Input/Input.h"
#include"PlayerBullet.h"
#include<list>

class PlayerBullet;

class Player
{
public:
	~Player();
	void Initialize();
	void Update();
	void Draw();
	void Attack();

	const Vector3& GetTranslate();
	std::list<PlayerBullet*>bullets_;

private:
	Object3d* object3d = new Object3d;

	TextureManager* textureManager = TextureManager::GetInstance();
	Input* input = Input::GetInstance();
	Camera* camera = Camera::GetInstance();
	
	Object3dCommon* object3dCommon = Object3dCommon::GetInstance();
	ModelManager* modelManager = ModelManager::GetInstance();
	ModelCommon* modelCommon = ModelCommon::GetInstance();

	int32_t FireTimer = 1;

};

