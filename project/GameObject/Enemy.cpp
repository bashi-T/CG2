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
	object3d->SetIsAnimation(false);
	object3d->Update(Camera::GetInstance());
}

void Enemy::Draw()
{
	object3d->Draw(ModelManager::GetInstance()->GetModelCommon());
}
