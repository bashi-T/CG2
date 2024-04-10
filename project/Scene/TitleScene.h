#pragma once
#include"Input/Input.h"
#include"SceneManager.h"
#include"Sprites/Sprite.h"
#include"Commons/SpriteCommon.h"
#include"Objects/Object3d.h"
#include"Commons/Object3dCommon.h"
#include"Objects/Particle.h"

class TitleScene:public SceneManager
{
public:
	void Init() override;
	void Update() override;
	void Draw() override;
	void Finalize() override;
private:
	//Input* input;
	//Object3dCommon* object3dCommon;
	//SRVManager* srvManager;
	//Camera* camera;
	std::vector<Object3d*> objects3d;
	std::vector <Particle*> particles;
	std::vector<Sprite*> sprites;

};

