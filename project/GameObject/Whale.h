#pragma once
#include"Input/Input.h"
#include"Objects/Object3d.h"
#include"Commons/Object3dCommon.h"
#include"Objects/Particle.h"
#include"Player.h"

class Whale
{
public:
	void Initialize();
	void Update();
	void Draw();

	void OnCollision();
	void SetTranslate(Vector3 translate);
	Vector3 GetTranslate() { return object3d->GetTranslate(); }
	OBB GetCollision() { return wCollision; }
	int GetLife() { return life; }
	bool GetIsHit() { return isHit; }
	float GetMaxDistance() { return maxDistance; }

private:
	Object3d* object3d;
	Player* player;
	const std::string whaleModel = "simpleSkin/simpleSkin.gltf";
	const std::string whaleSkin = "Resource/monsterBall.png";
	Vector3 whaleSpeed = { 0.1f,0.1f,0.1f };
	Vector3 nowWhaleSpeed = { 0.0f,0.0f,0.0f };
	Vector3 accSpeed = { 0.0f,0.0f,0.0f };

	bool isDead = false;
	bool isHit = false;
	OBB wCollision;
	int life = 4;
	uint32_t coolTimer;
	float maxDistance = 8.0f;
};

