#include "PlayerBullet.h"

void PlayerBullet::Initialize(Vector3 firstPos)
{
	object3d = new Object3d;
	object3d->Initialize(Object3dCommon::GetInstance(), SRVManager::GetInstance());
	ModelManager::GetInstance()->LoadAnimationModel(bulletModel, bulletSkin);
	object3d->SetModel(bulletModel);
	Model* model = ModelManager::GetInstance()->FindModel(bulletModel);
	Model::ModelData* modelData = model->GetModelData();
	for (Model::VertexData& vertex : modelData->vertices)
	{
		vertex.normal.x = vertex.position.x;
		vertex.normal.y = vertex.position.y;
		vertex.normal.z = vertex.position.z;
	}
	model->Memcpy();
	object3d->SetTranslate({ firstPos.x + 0.01f, firstPos.y,firstPos.z });
	object3d->SetScale({ 0.5f,0.5f,0.5f });
	object3d->SetIsAnimation(true);
}

void PlayerBullet::Update()
{
	if (--deathTimer <= 0)
	{
		isDead = true;
	}
	object3d->SetTranslate(Add(object3d->GetTranslate(), bulletSpeed));
	object3d->AnimationUpdate(Camera::GetInstance());
}

void PlayerBullet::Draw()
{
	object3d->Draw(ModelManager::GetInstance()->GetModelCommon());
}

void PlayerBullet::OnCollision()
{
	isDead = true;
}

void PlayerBullet::SetTranslate(Vector3 translate)
{
	object3d->SetTranslate(translate);
}
