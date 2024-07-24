#include "ClearScene.h"

void ClearScene::Init()
{
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
