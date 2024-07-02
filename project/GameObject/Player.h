#pragma once
#include"Input/Input.h"
#include"Objects/Object3d.h"
#include"Commons/Object3dCommon.h"
#include"Objects/Particle.h"
#include"PlayerBullet.h"

class Player
{
public:
	void Initialize();
	void Update();
	void Draw();
	void Shot();

private:
	Object3d* object3d;
	std::list<PlayerBullet*> pBullets;
	const std::string playerModel = "human/walk.gltf";
	const std::string playerSkin = "Resource/ganban.png";

	uint32_t shotInterval = 0;
	bool isShot = false;
};

