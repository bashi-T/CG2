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

	void Oncollision();
	Vector3 GetTranslate() { return object3d->GetTranslate(); }
	OBB GetCollision() { return wCollision; }

private:
	Object3d* object3d = new Object3d;
	const std::string whaleModel = "simpleSkin/simpleSkin.gltf";
	const std::string whaleSkin = "Resource/monsterBall.png";
	Vector3 whaleSpeed = { 0.1f,0.1f,0.1f };
	Vector3 nowWhaleSpeed = { 0.0f,0.0f,0.0f };
	Vector3 accSpeed = { 0.0f,0.0f,0.0f };

	bool isDead = false;
	OBB wCollision;

};

