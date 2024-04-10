#pragma once
#include"objects/object3d.h"
#include"Commons/Object3dCommon.h"
#include"Managers/SRVManager.h"
#include"Input/Input.h"
#include"Player.h"

class PlayerBullet
{
public:
	void Initialize(const Vector3 position);
	void Update();
	void Draw();
	void SetTranslate(const Vector3& translate) { object3d->SetTranslate(translate); }
	void SetisShot(bool isShot) { this->isShot = isShot; }
	void SetisDead(bool isDead) { this->isDead = isDead; }

	const Vector3& GetTranslate()const { return object3d->GetTranslate(); }
	bool GetIsShot() { return isShot; }
	bool GetIsDead() { return isDead; }
private:
	Object3d* object3d = new Object3d;

	TextureManager* textureManager = TextureManager::GetInstance();
	Input* input = Input::GetInstance();
	Camera* camera = Camera::GetInstance();

	Object3dCommon* object3dCommon = Object3dCommon::GetInstance();
	ModelManager* modelManager = ModelManager::GetInstance();
	ModelCommon* modelCommon = ModelCommon::GetInstance();

	bool isShot = false;
	bool isDead = false;
	int32_t deathTimer_ = kLifeTime;
	static const int32_t kLifeTime = 60 * 3;

};

