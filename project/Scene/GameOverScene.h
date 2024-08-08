#pragma once
#include"Input/Input.h"
#include"SceneManager.h"
#include"Objects/Object3d.h"
#include"Commons/Object3dCommon.h"
#include"Objects/Particle.h"
#include"Sprites/Sprite.h"

class GameOverScene :public SceneManager
{
public:
	void Init() override;
	void Update() override;
	void Draw() override;
	void Finalize() override;

private:
	Sprite* sprite;
};

