#pragma once
#include"Input/Input.h"
#include"Objects/Object3d.h"
#include"Commons/Object3dCommon.h"
#include"Objects/Particle.h"
#include"EnemyBullet.h"
#include"Player.h"
#include"Whale.h"

class Enemy
{
public:
	void Initialize(Player*player, Whale* whale);
	void Update();
	void Draw();
	void Shot();
	void OnCollision();
	void SetTranslate(Vector3 translate);
	void SetEnemyVector(Vector3 translate);

	bool IsDead()const { return isDead; }
	const std::list<EnemyBullet*>& GetBullets()const { return eBullets; }
	Vector3 GetTranslate() { return object3d->GetTranslate(); }
	OBB GetCollision() { return eCollision; }

private:
	Object3d* object3d;
	std::list<EnemyBullet*> eBullets;
	Player* player_;
	Whale* whale_;
	const std::string enemyModel = "plane/plane.gltf";
	const std::string enemySkin = "Resource/uvChecker.png";

	uint32_t shotInterval = 0;
	bool isShot = false;
	bool isDead = false;
	OBB eCollision;
	Vector3 enemyVector;
};

