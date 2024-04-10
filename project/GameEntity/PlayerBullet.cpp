#include "PlayerBullet.h"

void PlayerBullet::Initialize(const Vector3 position)
{
	object3d = new Object3d;
	object3d->Initialize();
	modelManager->LoadModel("ball.obj", "Resource/ground.png");
	object3d->SetModel("ball.obj");
	Model* model = modelManager->FindModel("ball.obj");
	Model::ModelData* modelData = model->GetModelData();
	for (Model::VertexData& vertex : modelData->vertices)
	{
		vertex.normal.x = vertex.position.x;
		vertex.normal.y = vertex.position.y;
		vertex.normal.z = vertex.position.z;
	}
	model->Memcpy();
	object3d->SetScale({ 0.0005f,0.0005f,0.0005f });
	object3d->SetTranslate({ position });
}

void PlayerBullet::Update()
{
	object3d->SetTranslate({ object3d->GetTranslate().x, object3d->GetTranslate().y, object3d->GetTranslate().z + 5.0f });
	if (--deathTimer_ <= 0) {
		isDead = true;
	}
	object3d->Update(camera);
}

void PlayerBullet::Draw()
{
	object3d->Draw();
}