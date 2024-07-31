#include "ClearScene.h"

void ClearScene::Init()
{
	Camera::GetInstance()->GetInstance()->SetTranslate({ 0.0f,7.0f,-20.0f });
}

void ClearScene::Update()
{
	if (Input::GetInstance()->TriggerKey(DIK_SPACE))
	{
		sceneNo = TITLE;
	}
}

void ClearScene::Draw()
{
}

void ClearScene::Finalize()
{
}
