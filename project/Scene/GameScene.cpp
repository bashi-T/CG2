#include "GameScene.h"

void GameScene::Init()
{
	//SPCommon = new SpriteCommon; 
	//srvManager = new SRVManager; 
	//input = new Input;
	Vector2 posSprite = { 0.0f,0.0f };
	std::string textureFilePath[10] =
	{
		"Resource/civ6.png",
		"Resource/uvChecker.png",
		"Resource/monsterBall.png",
		"Resource/worldMap.png",
		"Resource/world.png",
		"Resource/circle.png",
		"Resource/ganban.png",
		"Resource/uvChecker.png",
		"Resource/uvChecker.png",
		"Resource/cursor.png"
	};

	for (uint32_t i = 0; i < 9; i++)
	{
		Sprite* sprite = new Sprite();
		sprite->Initialize( SpriteCommon::GetInstance(), textureFilePath[i+1]);
		posSprite.x = 100.0f * i;
		posSprite.y = 50.0f * i;
		sprite->SetPositoin(posSprite);
		sprites.push_back(sprite);
	}

}

void GameScene::Update()
{
	for (Sprite* sprite : sprites)
	{
		sprite->Update();
	}
	if (Input::GetInstance()->TriggerKey(DIK_SPACE))
	{
		sceneNo = TITLE;
	}
}

void GameScene::Draw()
{
	for (Sprite* sprite : sprites)
	{
		sprite->Draw(SpriteCommon::GetInstance());
	}
}

void GameScene::Finalize()
{
	for (Sprite* sprite : sprites)
	{
		delete sprite;
	}
}
