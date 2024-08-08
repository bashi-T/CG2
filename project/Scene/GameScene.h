#pragma once
#include"Input/Input.h"
#include"SceneManager.h"
#include"Sprites/Sprite.h"
#include"Commons/SpriteCommon.h"
#include"GameObject/Player.h"
#include"GameObject/Whale.h"
#include"GameObject/Enemy.h"
#include "GameManager.h"

class GameScene:public SceneManager
{
public:
	void Init() override;
	void Update() override;
	void Draw() override;
	void Finalize() override;

	void LoadEnemyPopData(std::string filePath, int fileNum);
	void UpdateEnemyPopCommands(int fileNum);


	void CheckAllCollisions();
private:
	std::vector<Sprite*> sprites;
	Player* player_;
	Whale* whale_;
	std::list<Enemy*> enemys_;
	Object3d* object3d;
	std::stringstream enemyPopCommands[10];
	std::string enemyPopFile[10];
	bool isWait = false;
	int32_t WaitTimer = 1;
	bool gameEnd = false;
};

