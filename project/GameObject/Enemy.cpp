#include "Enemy.h"

void Enemy::Initialize(Player* player, Whale* whale)
{
	player_ = player;
	whale_ = whale;
	object3d = new Object3d;
	object3d->Initialize(Object3dCommon::GetInstance(), SRVManager::GetInstance());
	ModelManager::GetInstance()->LoadModel(enemyModel, enemySkin);
	object3d->SetModel(enemyModel);
	Model* model = ModelManager::GetInstance()->FindModel(enemyModel);
	Model::ModelData* modelData = model->GetModelData();
	for (Model::VertexData& vertex : modelData->vertices)
	{
		vertex.normal.x = vertex.position.x;
		vertex.normal.y = vertex.position.y;
		vertex.normal.z = vertex.position.z;
	}
	model->Memcpy();
	object3d->SetTranslate({ 0.0f,0.0f,10.0f });
}

void Enemy::Update()
{
	eBullets.remove_if([](EnemyBullet* bullet)
		{
			if (bullet->IsDead())
			{
				delete bullet;
				return true;
			}
			return false;
		});

	if (object3d->GetTranslate().z > player_->GetTranslate().z)
	{
		Shot();
	}
	if (shotInterval == 1 && object3d->GetTranslate().z > whale_->GetTranslate().z)
	{
		SetEnemyVector(whale_->GetTranslate());
	}
	else if (shotInterval == 1 && enemyVector.z > 0.0f)
	{
		SetEnemyVector({ enemyVector.x,enemyVector.y,-1.0f });
	}
	object3d->SetTranslate(Add(object3d->GetTranslate(), Multiply(0.05f, enemyVector)));
	if (object3d->GetTranslate().z < Camera::GetInstance()->GetTranslate().z)
	{
		isDead = true;
	}
	object3d->Update(Camera::GetInstance());
	for (EnemyBullet* bullet : eBullets)
	{
		bullet->Update();
	}
}

void Enemy::Draw()
{
	object3d->Draw(ModelManager::GetInstance()->GetModelCommon());
	for (EnemyBullet* bullet : eBullets)
	{
		bullet->Draw();
	}
}

void Enemy::Shot()
{
	shotInterval++;
	if (shotInterval == 1)
	{
		EnemyBullet* newBullet = new EnemyBullet;
		newBullet->Initialize(object3d->GetTranslate());
			newBullet->SetEnemyBulletVector(player_->GetTranslate());
		eBullets.push_back(newBullet);
	}
	if (shotInterval == 60)
	{
		shotInterval = 0;
	}
}

void Enemy::OnCollision()
{
	isDead = true;
}

void Enemy::SetTranslate(Vector3 translate)
{
	object3d->SetTranslate(translate);
}

void Enemy::SetEnemyVector(Vector3 position)
{
	enemyVector = Normalize(Subtract(position, object3d->GetTranslate()));
}
