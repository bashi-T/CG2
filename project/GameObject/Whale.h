#pragma once
#include"Input/Input.h"
#include"Objects/Object3d.h"
#include"Commons/Object3dCommon.h"
#include"Objects/Particle.h"

class Whale
{
public:
	void Initialize();
	void Update();
	void Draw();

private:
	Object3d* object3d = new Object3d;
	const std::string whaleModel = "simpleSkin/simpleSkin.gltf";
	const std::string whaleSkin = "Resource/monsterBall.png";
	Vector3 whaleSpeed = { 0.0f,0.0f,0.0f };
	Vector3 nowWhaleSpeed = { 0.0f,0.0f,0.0f };
	Vector3 accSpeed = { 0.0f,0.0f,0.0f };
};

