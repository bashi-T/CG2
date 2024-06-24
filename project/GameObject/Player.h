#pragma once
#include"Input/Input.h"
#include"Objects/Object3d.h"
#include"Commons/Object3dCommon.h"
#include"Objects/Particle.h"

class Player
{
public:
	void Initialize();
	void Update();
	void Draw();

private:
	Object3d* object3d;
	const std::string playerModel = "human/walk.gltf";
	const std::string playerSkin = "Resource/ganban.png";
};

