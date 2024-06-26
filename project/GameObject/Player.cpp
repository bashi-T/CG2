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
	if (Input::GetInstance()->GetJoystickState(0, joyState) && !joyState.Gamepad.bLeftTrigger)
	{
		object3d->SetTranslate(
			{ object3d->GetTranslate().x + (float)joyState.Gamepad.sThumbLX / (SHRT_MAX * 10.0f),
			0.0f, object3d->GetTranslate().z + (float)joyState.Gamepad.sThumbLY / (SHRT_MAX * 10.0f) });

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

	if (joyState.Gamepad.wButtons && XINPUT_GAMEPAD_RIGHT_SHOULDER)
	{
		isShot = true;
	}
	else
	{
		isShot = false;
	}

	object3d->SkeltonUpdate(Camera::GetInstance());
}

void Player::Draw()
{
	object3d->SkeltonDraw(ModelManager::GetInstance()->GetModelCommon());
}
