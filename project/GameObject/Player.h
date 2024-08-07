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
	void OnCollision();

	const std::list<PlayerBullet*>& GetBullets()const { return pBullets; }
	Vector3 GetTranslate() { return object3d->GetTranslate(); }
	OBB GetCollision() { return pCollision; }
	bool GetIsHit() { return isHit; }
	uint32_t GetIsHitTimer() { return hitTimer; }
	void SetTranslate(Vector3 translate) { object3d->SetTranslate(translate); }
private:
	Object3d* object3d;
	std::list<PlayerBullet*> pBullets;
	const std::string playerModel = "human/walk.gltf";
	const std::string playerSkin = "Resource/ganban.png";
	OBB pCollision;

	uint32_t shotInterval = 0;
	bool isShot = false;
	bool isHit = false;
	uint32_t hitTimer = 0;
};

