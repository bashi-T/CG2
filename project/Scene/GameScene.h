#pragma once
#include"Input/Input.h"
#include"SceneManager.h"
#include"Sprites/Sprite.h"
#include"Commons/SpriteCommon.h"
#include"Objects/Object3d.h"
#include"Commons/Object3dCommon.h"
#include"Objects/Particle.h"
#include"GameEntity/Player.h"

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
	std::vector<Object3d*> objects3d;
	std::vector <Particle*> particles;
	Player* player = new Player;
};

