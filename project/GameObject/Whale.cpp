#include "Whale.h"

void Whale::Initialize()
{
	object3d = new Object3d; 
	player = new Player;
	object3d->Initialize(Object3dCommon::GetInstance(), SRVManager::GetInstance());
	ModelManager::GetInstance()->LoadSkeltonAnimation(whaleModel, whaleSkin, SRVManager::GetInstance());
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
	life = 4;
}

void Whale::Update()
{
	XINPUT_STATE joyState;
	if(player->GetIsHit()==false)
	{
		if (Input::GetInstance()->GetJoystickState(0, joyState))
		{
			if ((float)joyState.Gamepad.sThumbLX > 0.0f)
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
					accSpeed.x += 0.01f;
					if (accSpeed.x >= 1.0f)
					{
						accSpeed.x = 1.0f;
					}
				}
			}
			else if ((float)joyState.Gamepad.sThumbLX < 0.0f)
			{
				if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER)
				{
					accSpeed.x += 0.01f;
					if (accSpeed.x >= 0.0f)
					{
						accSpeed.x = 0.0f;
					}
				}
				else
				{
					accSpeed.x -= 0.01f;
					if (accSpeed.x <= -1.0f)
					{
						accSpeed.x = -1.0f;
					}
				}
			}
			else
			{
				if (accSpeed.x > 0.0f)
				{
					accSpeed.x -= 0.01f;
				}
				else if (accSpeed.x < 0.0f)
				{
					accSpeed.x += 0.01f;
				}
				else if (accSpeed.x == 0.0f)
				{
					accSpeed.x = 0.0f;
				}
			}

			if ((float)joyState.Gamepad.sThumbLY > 0.0f)
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
					accSpeed.z += 0.01f;
					if (accSpeed.z >= 1.0f)
					{
						accSpeed.z = 1.0f;
					}
				}
			}
			else if ((float)joyState.Gamepad.sThumbLY < 0.0f)
			{
				if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER)
				{
					accSpeed.z += 0.01f;
					if (accSpeed.z >= 0.0f)
					{
						accSpeed.z = 0.0f;
					}
				}
				else
				{
					accSpeed.z -= 0.01f;
					if (accSpeed.z <= -1.0f)
					{
						accSpeed.z = -1.0f;
					}
				}
			}
			else
			{
				if (accSpeed.z > 0.0f)
				{
					accSpeed.z -= 0.01f;
				}
				else if (accSpeed.z < 0.0f)
				{
					accSpeed.z += 0.01f;
				}
				else if (accSpeed.z == 0.0f)
				{
					accSpeed.z = 0.0f;
				}
			}
		}
	}
    else
    {
		int x = 1;
		x++;
	}

	if (isHit == true)
	{
		object3d->SetRotate({ object3d->GetRotate().x,object3d->GetRotate().y + 1.0f,object3d->GetRotate().z });
		coolTimer++;
		if (coolTimer == 120)
		{
			isHit = false;
			coolTimer = 0;
			object3d->SetRotate({ 0.0f,0.0f,0.0f });
		}
	}

	nowWhaleSpeed = { (whaleSpeed.x * accSpeed.x) ,0.0f,(whaleSpeed.z * accSpeed.z) };
	object3d->SetTranslate(Add(object3d->GetTranslate(), nowWhaleSpeed));
	object3d->SetIsAnimation(true);
	object3d->SkeltonUpdate(Camera::GetInstance());
#ifdef _DEBUG

	ImGui::Begin("whale");
	ImGui::DragFloat3("whale.translate", (float*)&object3d->GetTranslate(), 0.01f);
	ImGui::Text("life:%d", life);
	ImGui::End();
#endif
}

void Whale::Draw()
{
	object3d->SkeltonDraw(ModelManager::GetInstance()->GetModelCommon());
}

void Whale::OnCollision()
{
	life -= 1;
	isHit = true;
}

void Whale::SetTranslate(Vector3 translate)
{
	object3d->SetTranslate(translate);
}
