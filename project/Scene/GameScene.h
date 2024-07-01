#pragma once
#include"Input/Input.h"
#include"SceneManager.h"
#include"Sprites/Sprite.h"
#include"Commons/SpriteCommon.h"
#include"GameObject/Player.h"
#include"GameObject/Whale.h"
#include"GameObject/Enemy.h"

class GameScene:public SceneManager
{
public:
	void Init() override;
	void Update() override;
	void Draw() override;
	void Finalize() override;
private:
	//Input* input;
	//SpriteCommon* SPCommon;
	//SRVManager* srvManager;
	std::vector<Sprite*> sprites;
	Player* player_;
	Whale* whale_;
	Enemy* enemy_;
	Object3d* object3d;
};

