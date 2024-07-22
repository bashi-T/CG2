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
	player_->Initialize();
	whale_->Initialize();
	for (uint32_t i = 0; i < 9; i++)
	{
		Enemy* enemy_ = new Enemy;
		enemy_ = new Enemy;
		enemy_->Initialize(player_,whale_);
		enemys_.push_back(enemy_);
		enemy_->SetTranslate({10.0f - 2.0f * i,0.0f,30.0f });
		enemy_->SetEnemyVector(whale_->GetTranslate());
	};
}

void GameScene::Update()
{
	//for (Sprite* sprite : sprites)
	//{
	//	sprite->Update(WinAPP::clientWidth_, WinAPP::clientHeight_);
	//}
	enemys_.remove_if([](Enemy* enemy)
		{
			if (enemy->IsDead())
			{
				delete enemy;
				return true;
			}
			return false;
		});
	if (whale_->GetLife() <0)
	{
		sceneNo = TITLE;
	}
	player_->Update();
	whale_->Update();
	for(Enemy*enemy_:enemys_)
	{
		enemy_->Update();
	}
	CheckAllCollisions();
}

void GameScene::Draw()
{
	//for (Sprite* sprite : sprites)
	//{
	//	sprite->Draw(SpriteCommon::GetInstance());
	//}
	player_->Draw();
	whale_->Draw();
	for (Enemy* enemy_ : enemys_)
	{
		enemy_->Draw();
	}
}

void GameScene::Finalize()
{
	for (Sprite* sprite : sprites)
	{
		delete sprite;
	}
	delete player_;
	delete whale_;
	for (Enemy* enemy_ : enemys_)
	{
		delete enemy_;
	}
}

void GameScene::CheckAllCollisions()
{
	Vector3 posA, posB;
	const std::list<PlayerBullet*>& playerBullets = player_->GetBullets();
	if(player_->GetIsHit()==false&&player_->GetIsHitTimer()==0)
	{
#pragma region 自機と敵弾の当たり判定
		for (Enemy* enemy_ : enemys_)
		{
			const std::list<EnemyBullet*>& enemyBullets = enemy_->GetBullets();

			posA = player_->GetTranslate();
			for (EnemyBullet* bullet : enemyBullets)
			{
				posB = bullet->GetTranslate();
				Vector3 distance = Subtract(posA, posB);
				if ((distance.x * distance.x) + (distance.y * distance.y) +
					(distance.z * distance.z) <= 4)
				{
					player_->OnCollision();
					bullet->OnCollision();
				}
			}
		}
#pragma endregion
#pragma region 自機と敵の当たり判定
		for (Enemy* enemy_ : enemys_)
		{
			const std::list<EnemyBullet*>& enemyBullets = enemy_->GetBullets();

			posA = player_->GetTranslate();
			posB = enemy_->GetTranslate();
			Vector3 distance = Subtract(posA, posB);
			if ((distance.x * distance.x) + (distance.y * distance.y) +
				(distance.z * distance.z) <= 4)
			{
				player_->OnCollision();
				//enemy_->OnCollision();
			}
		}
#pragma endregion
	}
#pragma region 敵と自弾の当たり判定
	for (Enemy* enemy_ : enemys_)
	{
		const std::list<EnemyBullet*>& enemyBullets = enemy_->GetBullets();

		posA = enemy_->GetTranslate();
		for (PlayerBullet* bullet : playerBullets)
		{
			posB = bullet->GetTranslate();
			Vector3 distance = Subtract(posA, posB);
			if ((distance.x * distance.x) + (distance.y * distance.y) +
				(distance.z * distance.z) <= 4)
			{
				enemy_->OnCollision();
				bullet->OnCollision();
			}
		}
	}
#pragma endregion
#pragma region 自弾と敵弾の当たり判定
	for (Enemy* enemy_ : enemys_)
	{
		const std::list<EnemyBullet*>& enemyBullets = enemy_->GetBullets();

		for (PlayerBullet* pBullet : playerBullets)
		{
			for (EnemyBullet* eBullet : enemyBullets)
			{
				posA = pBullet->GetTranslate();
				posB = eBullet->GetTranslate();
				Vector3 distance = Subtract(posA, posB);
				if ((distance.x * distance.x) + (distance.y * distance.y) +
					(distance.z * distance.z) <= 4)
				{
					pBullet->OnCollision();
					eBullet->OnCollision();
				}
			}
		}
	}
#pragma endregion
	if (whale_->GetIsHit() == false)
	{
#pragma region 敵とクジラの当たり判定
		for (Enemy* enemy_ : enemys_)
		{
			posA = whale_->GetTranslate();
			posB = enemy_->GetTranslate();
			Vector3 distance = Subtract(posA, posB);
			if ((distance.x * distance.x) + (distance.y * distance.y) +
				(distance.z * distance.z) <= 4)
			{
				whale_->OnCollision();
				enemy_->OnCollision();
				break;
			}
		}
#pragma endregion
#pragma region 敵弾とクジラの当たり判定
		for (Enemy* enemy_ : enemys_)
		{
			const std::list<EnemyBullet*>& enemyBullets = enemy_->GetBullets();

			posA = whale_->GetTranslate();
			for (EnemyBullet* bullet : enemyBullets)
			{
				posB = bullet->GetTranslate();
				Vector3 distance = Subtract(posA, posB);
				if ((distance.x * distance.x) + (distance.y * distance.y) +
					(distance.z * distance.z) <= 4)
				{
					whale_->OnCollision();
					bullet->OnCollision();
					break;
				}
			}
		}
#pragma endregion
	}
}

