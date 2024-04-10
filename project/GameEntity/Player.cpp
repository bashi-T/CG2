#include "Player.h"

Player::~Player()
{
	for (PlayerBullet* bullet : bullets_)
	{
		delete bullet;
	}
}

void Player::Initialize()
{
	object3d->Initialize();
	modelManager->LoadModel("sphere.obj", "Resource/worldMap.png");
	object3d->SetModel("sphere.obj");
	Model* model = modelManager->FindModel("sphere.obj");
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
	bullets_.remove_if([](PlayerBullet* bullet) {
		if (bullet->GetIsDead()) {
			delete bullet;
			return true;
		}
		return false;
		});

	if (input->PushKey(DIK_D)) {
		object3d->SetTranslate({ object3d->GetTranslate().x + 0.1f ,object3d->GetTranslate().y ,object3d->GetTranslate().z });
	}
	if (input->PushKey(DIK_A)) {
		object3d->SetTranslate({ object3d->GetTranslate().x - 0.1f ,object3d->GetTranslate().y ,object3d->GetTranslate().z });
	}
	if (input->PushKey(DIK_S)) {
		object3d->SetTranslate({ object3d->GetTranslate().x ,object3d->GetTranslate().y - 0.1f ,object3d->GetTranslate().z });
	}
	if (input->PushKey(DIK_W)) {
		object3d->SetTranslate({ object3d->GetTranslate().x ,object3d->GetTranslate().y + 0.1f ,object3d->GetTranslate().z });
	}
	Attack();

	object3d->Update(camera);
	for (PlayerBullet* bullet : bullets_) {
		bullet->Update();
	}

}

void Player::Draw()
{
	object3d->Draw();
	for (PlayerBullet* bullet : bullets_)
	{
		bullet->Draw();
	}
}

void Player::Attack()
{
	if (Input::GetInstance()->PushKey(DIK_RETURN))
	{
		FireTimer--;
		if (FireTimer == 0)
		{
			FireTimer = 10;
			//const float kBulletSpeedZ = 1.0f;
			//Vector3 velocity(0, 0, kBulletSpeedZ);
			//velocity.x =
			//	Subtract(worldTransform3Dreticle_.matWorld_, worldTransform_.matWorld_).m[3][0];
			//velocity.y =
			//	Subtract(worldTransform3Dreticle_.matWorld_, worldTransform_.matWorld_).m[3][1];
			//velocity.z =
			//	Subtract(worldTransform3Dreticle_.matWorld_, worldTransform_.matWorld_).m[3][2];
			//velocity = Multiply(kBulletSpeedZ, Normalize(velocity));

			PlayerBullet* newBullet = new PlayerBullet();
			newBullet->Initialize(object3d->GetTranslate());
			bullets_.push_back(newBullet);
		}
	}
	else
	{
		FireTimer = 1;
	}

}

const Vector3& Player::GetTranslate()
{
	return object3d->GetTranslate();
}
