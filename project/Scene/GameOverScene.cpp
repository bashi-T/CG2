#include "GameOverScene.h"

void GameOverScene::Init()
{
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
