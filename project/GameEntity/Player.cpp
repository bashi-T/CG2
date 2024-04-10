#include "Player.h"

void Player::Initialize()
{
	object3d = new Object3d;
	object3d->Initialize();
	ModelManager::GetInstance()->LoadModel("sphere.obj", "Resource/worldMap.png");
	object3d->SetModel("sphere.obj");
	Model* model = ModelManager::GetInstance()->FindModel("sphere.obj");
	Model::ModelData* modelData = model->GetModelData();
	for (Model::VertexData& vertex : modelData->vertices)
	{
		vertex.normal.x = vertex.position.x;
		vertex.normal.y = vertex.position.y;
		vertex.normal.z = vertex.position.z;
	}
	model->Memcpy();
	object3d->SetScale({ 0.0025f,0.0025f,0.0025f });

}

void Player::Update()
{
	if (Input::GetInstance()->PushKey(DIK_D)) {
		object3d->SetTranslate({ object3d->GetTranslate().x + 0.01f ,object3d->GetTranslate().y ,object3d->GetTranslate().z });
	}
	if (Input::GetInstance()->PushKey(DIK_A)) {
		object3d->SetTranslate({ object3d->GetTranslate().x - 0.01f ,object3d->GetTranslate().y ,object3d->GetTranslate().z });
	}
	if (Input::GetInstance()->PushKey(DIK_S)) {
		object3d->SetTranslate({ object3d->GetTranslate().x ,object3d->GetTranslate().y - 0.01f ,object3d->GetTranslate().z });
	}
	if (Input::GetInstance()->PushKey(DIK_W)) {
		object3d->SetTranslate({ object3d->GetTranslate().x ,object3d->GetTranslate().y + 0.01f ,object3d->GetTranslate().z });
	}
	object3d->Update(Camera::GetInstance());
}

void Player::Draw()
{
	object3d->Draw();
}
