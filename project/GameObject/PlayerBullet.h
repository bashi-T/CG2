#pragma once
#include"Input/Input.h"
#include"Objects/Object3d.h"
#include"Commons/Object3dCommon.h"
#include"Objects/Particle.h"

class PlayerBullet
{
public:
	void Initialize(Vector3 firstPos);
	void Update();
	void Draw();
	void OnCollision();
	bool IsDead()const { return isDead; }

	void SetTranslate(Vector3 translate);

	Vector3 GetTranslate() { return object3d->GetTranslate(); }
	OBB GetCollision() { return pbCollision; }

private:
	Object3d* object3d;

	const std::string bulletModel = "AnimatedCube/AnimatedCube.gltf";
	const std::string bulletSkin = "Resource/AnimatedCube/AnimatedCube_BaseColor.png";
	Vector3 bulletSpeed = { 0.0f,0.0f,1.0f };
	static const int32_t kLifeTime = 60 * 2;
	int32_t deathTimer = kLifeTime;
	bool isDead = false;
	OBB pbCollision;

};

