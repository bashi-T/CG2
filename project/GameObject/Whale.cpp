#include "Whale.h"

void Whale::Initialize()
{
	object3d->Initialize(Object3dCommon::GetInstance(), SRVManager::GetInstance());
	ModelManager::GetInstance()->LoadAnimationModel(whaleModel, whaleSkin);
	object3d->SetModel(whaleModel);
	Model* model = ModelManager::GetInstance()->FindModel(whaleModel);
	Model::ModelData* modelData = model->GetModelData();
	for (Model::VertexData& vertex : modelData->vertices)
	{
		vertex.normal.x = vertex.position.x;
		vertex.normal.y = vertex.position.y;
		vertex.normal.z = vertex.position.z;
	}
	model->Memcpy();

}

void Whale::Update()
{
	XINPUT_STATE joyState;
	if (Input::GetInstance()->GetJoystickState(0, joyState))
	{

		if ((float)joyState.Gamepad.sThumbLX != 0.0f)
		{
			if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER)
			{
				accSpeed.x -= 0.01f;
				if (accSpeed.x <= 0.0f)
				{
					accSpeed.x = 0.0f;
				}
			}
			else
			{
				whaleSpeed.x = (float)joyState.Gamepad.sThumbLX / (SHRT_MAX * 10.0f);
				accSpeed.x += 0.01f;
				if (accSpeed.x >= 1.0f)
				{
					accSpeed.x = 1.0f;
				}
			}
		}
		else
		{
			accSpeed.x -= 0.01f;
			if (accSpeed.x <= 0.0f)
			{
				accSpeed.x = 0.0f;
			}
		}

		if ((float)joyState.Gamepad.sThumbLY != 0.0f)
		{
			if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER)
			{
				accSpeed.z -= 0.01f;
				if (accSpeed.z <= 0.0f)
				{
					accSpeed.z = 0.0f;
				}
			}
			else
			{
				whaleSpeed.z = (float)joyState.Gamepad.sThumbLY / (SHRT_MAX * 10.0f);
				accSpeed.z += 0.01f;
				if (accSpeed.z >= 1.0f)
				{
					accSpeed.z = 1.0f;
				}
			}
		}
		else
		{
			accSpeed.z -= 0.01f;
			if (accSpeed.z <= 0.0f)
			{
				accSpeed.z = 0.0f;
			}
		}
	}
	nowWhaleSpeed = { (nowWhaleSpeed.x + whaleSpeed.x) * accSpeed.x,0.0f, (nowWhaleSpeed.z + whaleSpeed.z) * accSpeed.z };
	nowWhaleSpeed.x = max(nowWhaleSpeed.x, -0.1f);
	nowWhaleSpeed.x = min(nowWhaleSpeed.x, 0.1f);
	nowWhaleSpeed.z = max(nowWhaleSpeed.z, -0.1f);
	nowWhaleSpeed.z = min(nowWhaleSpeed.z, 0.1f);
	object3d->SetTranslate(Add(object3d->GetTranslate(), nowWhaleSpeed));
	object3d->SetIsAnimation(false);
	object3d->AnimationUpdate(Camera::GetInstance());
	ImGui::Begin("whale");
	ImGui::DragFloat3("whale.translate", (float*)&object3d->GetTranslate(), 0.01f);
	ImGui::End();
}

void Whale::Draw()
{
	object3d->Draw(ModelManager::GetInstance()->GetModelCommon());
}
