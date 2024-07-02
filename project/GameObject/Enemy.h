#pragma once
#include"Input/Input.h"
#include"Objects/Object3d.h"
#include"Commons/Object3dCommon.h"
#include"Objects/Particle.h"
#include"EnemyBullet.h"

class Enemy
{
public:
	void Initialize();
	void Update();
	void Draw();
	void Shot();

private:
	Object3d* object3d;
	std::list<EnemyBullet*> eBullets;
	const std::string enemyModel = "plane/plane.gltf";
	const std::string enemySkin = "Resource/uvChecker.png";

	uint32_t shotInterval = 0;
	bool isShot = false;
};

