#include "Enemy.h"

void Enemy::Initialize()
{
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

	Shot();
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
		eBullets.push_back(newBullet);
	}
	if (shotInterval == 60)
	{
		shotInterval = 0;

	}
}
