#include "GameScene.h"

void GameScene::Init()
{
	//SPCommon = new SpriteCommon; 
	//srvManager = new SRVManager; 
	//input = new Input;
	//Vector2 posSprite = { 0.0f,0.0f };
	//std::string textureFilePath[10] =
	//{
	//	"Resource/civ6.png",
	//	"Resource/uvChecker.png",
	//	"Resource/monsterBall.png",
	//	"Resource/worldMap.png",
	//	"Resource/world.png",
	//	"Resource/circle.png",
	//	"Resource/ganban.png",
	//	"Resource/uvChecker.png",
	//	"Resource/uvChecker.png",
	//	"Resource/cursor.png"
	//};
	//for (uint32_t i = 0; i < 9; i++)
	//{
	//	Sprite* sprite = new Sprite();
	//	sprite->Initialize(WinAPP::clientWidth_, WinAPP::clientHeight_, SpriteCommon::GetInstance(), SRVManager::GetInstance(), textureFilePath[i+1]);
	//	posSprite.x = 100.0f * i;
	//	posSprite.y = 50.0f * i;
	//	sprite->SetPositoin(posSprite);
	//	sprites.push_back(sprite);
	//}
	player_ = new Player;
	whale_ = new Whale;
	enemy_ = new Enemy;
	player_->Initialize();
	whale_->Initialize();
	enemy_->Initialize();
}

void GameScene::Update()
{
	//for (Sprite* sprite : sprites)
	//{
	//	sprite->Update(WinAPP::clientWidth_, WinAPP::clientHeight_);
	//}
	if (Input::GetInstance()->TriggerKey(DIK_SPACE))
	{
		sceneNo = TITLE;
	}
	player_->Update();
	whale_->Update();
	enemy_->Update();
}

void GameScene::Draw()
{
	//for (Sprite* sprite : sprites)
	//{
	//	sprite->Draw(SpriteCommon::GetInstance());
	//}
	player_->Draw();
	whale_->Draw();
	enemy_->Draw();
}

void GameScene::Finalize()
{
	for (Sprite* sprite : sprites)
	{
		delete sprite;
	}
	delete player_;
	delete whale_;
	delete enemy_;
}

void GameScene::CheckAllCollisions()
{
	Vector3 posA, posB;
	const std::list<PlayerBullet*>& playerBullets = player_->GetBullets();
	const std::list<EnemyBullet*>& enemyBullets = enemy_->GetBullets();
#pragma region 自機と敵弾の当たり判定
	posA = player_->GetTranslate();
	for (EnemyBullet* bullet : enemyBullets)
	{
		posB = bullet->GetTranslate();

		if()
	}
#pragma endregion
#pragma region 自機と敵の当たり判定

#pragma endregion
#pragma region 敵と自弾の当たり判定

#pragma endregion
#pragma region 自弾と敵弾の当たり判定

#pragma endregion
#pragma region 敵とクジラの当たり判定

#pragma endregion
#pragma region 敵弾とクジラの当たり判定

#pragma endregion
}
