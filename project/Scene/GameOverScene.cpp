#include "GameOverScene.h"

void GameOverScene::Init()
{
	Camera::GetInstance()->GetInstance()->SetTranslate({ 0.0f,7.0f,-20.0f });
}

void GameOverScene::Update()
{
	if (Input::GetInstance()->TriggerKey(DIK_SPACE))
	{
		sceneNo = TITLE;
	}
}

void GameOverScene::Draw()
{
}

void GameOverScene::Finalize()
{
}
