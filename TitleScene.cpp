#include "TitleScene.h"

void TitleScene::Initialize(int32_t width, int32_t height, WinAPP* winAPP)
{
	std::string textureFilePath[10] =
	{
		"Resource/cube.png",
		"Resource/monsterBall.png",
		"Resource/cursor.png"
		"Resource/axis.png",
	};
	
	winAPP->Initialize(width, height, L"GE3");
	dx12Common->Initialize(width, height, winAPP);
	input_->Initialize(winAPP);
	imgui->Initialize(
		winAPP->GetHWND(),
		dx12Common->GetDevice().Get(),
		dx12Common->GetSwapChainDesc(),
		dx12Common->GetRtvDesc(),
		dx12Common->GetSrvDescriptorHeap().Get());
	camera->Initialize();
	TextureManager::GetInstance()->Initialize();

	//object3dCommon->Initialize(dx12Common);
	//ModelManager::GetInstance()->Initialize(dx12Common);
	//player->Initialize(width, height);
	//for (uint32_t i = 0; i < 3; i++)
	//{
	//	Model* model = new Model;
	//	Object3d* object3d = new Object3d;
	//	object3d->Initialize(object3dCommon, width, height);
	//	ModelManager::GetInstance()->LoadModel(objFilePath[i], textureFilePath[i]);
	//	object3d->SetModel(model);
	//	object3d->SetModel(objFilePath[i]);
	//	//object3d->SetTranslate({ 0.2f * i, 0.2f * i, 0.2f * i });
	//	object3d->SetScale({ 0.5f, 0.5f, 0.5f });
	//	objects3d.push_back(object3d);
	//};
	//SPCommon->Initialize(dx12Common);
	for (uint32_t i = 0; i < 10; i++)
	{
		Sprite* sprite = new Sprite();
		sprite->Initialize(width, height, SPCommon, textureFilePath[i]);
		if (i == 0) {
			posSprite.x = -50.0f;
			posSprite.y = -50.0f;
		}else
		{
			posSprite.x = 100.0f * i;
			posSprite.y = 50.0f * i;
		}
		sprite->SetPositoin(posSprite);
		sprites.push_back(sprite);
	}
}

void TitleScene::Update(int32_t width, int32_t height)
{
	for (Sprite* sprite : sprites)
	{
		sprite->Update(width, height);
	}
}

void TitleScene::Draw()
{
	for (Sprite* sprite : sprites)
	{
		sprite->Draw(SPCommon);
	}
}

void TitleScene::Finalize(WinAPP* winAPP)
{
	CloseHandle(dx12Common->GetFenceEvent());
	for (Sprite* sprite : sprites)
	{
		delete sprite;
	}
	//for (Model* model : models)
	//{
	//	delete model;
	//}
	//ModelManager::GetInstance()->Finalize();
	//for (Object3d* object3d : objects3d)
	//{
	//	delete object3d;
	//}
	//delete object3dCommon;
	TextureManager::GetInstance()->Finalize();
	imgui->Finalize();
	delete input_;
	dx12Common->DeleteInstance();
	winAPP->Finalize();
	CoUninitialize();
}
