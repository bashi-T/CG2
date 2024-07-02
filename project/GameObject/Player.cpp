#include "Player.h"

void Player::Initialize()
{
	object3d = new Object3d; 
	object3d->Initialize(Object3dCommon::GetInstance(), SRVManager::GetInstance());
	ModelManager::GetInstance()->LoadSkeltonAnimation(playerModel, playerSkin, SRVManager::GetInstance());
	object3d->SetModel(playerModel);
	Model* model = ModelManager::GetInstance()->FindModel(playerModel);
	Model::ModelData* modelData = model->GetModelData();
	for (Model::VertexData& vertex : modelData->vertices)
	{
		vertex.normal.x = vertex.position.x;
		vertex.normal.y = vertex.position.y;
		vertex.normal.z = vertex.position.z;
	}
	model->Memcpy();

}

void Player::Update()
{
	XINPUT_STATE joyState;
	if (Input::GetInstance()->GetJoystickState(0, joyState))
	{
		if(joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER)
		{
		}else
		{
			object3d->SetTranslate(
				{ object3d->GetTranslate().x + (float)joyState.Gamepad.sThumbLX / (SHRT_MAX * 10.0f),
				0.0f, object3d->GetTranslate().z + (float)joyState.Gamepad.sThumbLY / (SHRT_MAX * 10.0f) });
		}
	}
	if ((float)joyState.Gamepad.sThumbLX != 0.0f || (float)joyState.Gamepad.sThumbLY != 0.0f)
	{
		object3d->SetIsAnimation(true);
	}
	else
	{
		object3d->SetIsAnimation(false);
	}

	if ((float)joyState.Gamepad.sThumbLY > 0)
	{
		object3d->SetRotate({ 0.0f,0.0f,0.0f });
	}
	if ((float)joyState.Gamepad.sThumbLY < 0)
	{
		object3d->SetRotate({ 0.0f,3.0f,0.0f });
	}
	if ((float)joyState.Gamepad.sThumbLX > 0)
	{
		object3d->SetRotate({ 0.0f,1.5f,0.0f });
	}
	if ((float)joyState.Gamepad.sThumbLX < 0)
	{
		object3d->SetRotate({ 0.0f,4.5f,0.0f });
	}

	if (Input::GetInstance()->PushKey(DIK_D))
	{
		object3d->SetTranslate({ object3d->GetTranslate().x + 0.05f, object3d->GetTranslate().y, object3d->GetTranslate().z });
	}
	if (Input::GetInstance()->PushKey(DIK_A))
	{
		object3d->SetTranslate({ object3d->GetTranslate().x - 0.05f, object3d->GetTranslate().y, object3d->GetTranslate().z });
	}

	if (/*joyState.Gamepad.bRightTrigger|| */Input::GetInstance()->PushKey(DIK_C))
	{
		isShot = true;
	}
	else
	{
		isShot = false;
	}
	Shot();

	object3d->SkeltonUpdate(Camera::GetInstance());
	for (PlayerBullet* bullet : pBullets)
	{
			bullet->Update();

	}
}

void Player::Draw()
{
	object3d->SkeltonDraw(ModelManager::GetInstance()->GetModelCommon());
	for (PlayerBullet* bullet : pBullets)
	{
		bullet->Draw();
	}
}

void Player::Shot()
{
	if (isShot == true)
	{
		shotInterval++;
		if (shotInterval == 1)
		{
			PlayerBullet* newBullet = new PlayerBullet;
			newBullet->Initialize(object3d->GetTranslate());
			pBullets.push_back(newBullet);
		}
		if (shotInterval == 30)
		{
			shotInterval = 0;

		}
	}
	else
	{
		shotInterval = 0;
	}
}
