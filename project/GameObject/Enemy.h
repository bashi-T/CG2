#pragma once
#include"Input/Input.h"
#include"Objects/Object3d.h"
#include"Commons/Object3dCommon.h"
#include"Objects/Particle.h"

class Enemy
{
public:
	void Initialize();
	void Update();
	void Draw();

private:
	Object3d* object3d;
	std::vector<Object3d*> bullets;
	const std::string enemyModel = "plane/plane.gltf";
	const std::string enemySkin = "Resource/uvChecker.png";

	bool isShot = false;
};

