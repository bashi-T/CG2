#include "ClearScene.h"

void ClearScene::Init()
{
	sprite = new Sprite;
	sprite->Initialize(SpriteCommon::GetInstance(), SRVManager::GetInstance(), "Resource/clearExample.png");
	sprite->SetPositoin({ float(WinAPP::clientWidth_ / 2) - (sprite->GetSize().x / 2),0.0f });
	Camera::GetInstance()->SetTranslate({ 0.0f,7.0f,-20.0f });
}

void ClearScene::Update()
{
	XINPUT_STATE joyState;
	sprite->Update();
	if (Input::GetInstance()->GetJoystickState(0, joyState))
	{
		if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER)
		{
			sceneNo = TITLE;
		}
	}
}

void ClearScene::Draw()
{
	sprite->Draw();
}

void ClearScene::Finalize()
{
}
