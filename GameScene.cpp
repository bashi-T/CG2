#include "GameScene.h"

void GameScene::Initialize(int32_t width, int32_t height, WinAPP* winAPP)
{
	std::string textureFilePath[10] =
	{
		"Resource/Player.png",
		"Resource/sight.png",
		"Resource/sight.png",
		"Resource/sight.png",
		"Resource/sight.png",
		"Resource/worldMap.png",
	};

	std::string objFilePath[10] =
	{
		"cube.obj",
		"plane.obj",
		"plane.obj",
		"plane.obj",
		"plane.obj",
		"world.obj"
	};
	std::string ScreenFilePath[2] =
	{
		"Resource/title.png",
		"Resource/clear.png",
	};
	numShot = 0;
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

	object3dCommon->Initialize(dx12Common);
	ModelManager::GetInstance()->Initialize(dx12Common);
	//player->Initialize(width, height);
	for (uint32_t i = 0; i < 6; i++)
	{
		Model* model = new Model;
		Object3d* object3d = new Object3d;
		object3d->Initialize(object3dCommon, width, height);
		ModelManager::GetInstance()->LoadModel(objFilePath[i], textureFilePath[i]);
		object3d->SetModel(model);
		object3d->SetModel(objFilePath[i]);
		//object3d->SetTranslate({ 0.2f * i, 0.2f * i, 0.2f * i });
		object3d->SetScale({ 0.5f, 0.5f, 0.5f });
		objects3d.push_back(object3d);
	};

	SPCommon->Initialize(dx12Common);
	for (uint32_t i = 0; i < 2; i++)
	{
		Sprite* sprite = new Sprite();
		sprite->Initialize(width, height, SPCommon, ScreenFilePath[i]);
			//posSprite.x = 0.0f;
			//posSprite.y = 0.0f;
			//sprite->SetPositoin(posSprite);
		sprite->SetSize({ float(width),float(height) });
			sprites.push_back(sprite);
	}


}

void GameScene::Update(int32_t width, int32_t height)
{
	dx12Common->update();
	input_->Update();
	imgui->Update();

	switch (numScene)
	{
	case 1:
		for (Object3d* object3d : objects3d)
		{
			objects3d.at(5)->SetScale({ 0.001f,0.001f,0.001f });
			object3d->Update();

			if (input_->PushKey(DIK_D))
			{
				objects3d.at(0)->SetTranslate({ objects3d.at(0)->GetTranslate().x + 0.05f,objects3d.at(0)->GetTranslate().y,objects3d.at(0)->GetTranslate().z });
			}
			if (input_->PushKey(DIK_A))
			{
				objects3d.at(0)->SetTranslate({ objects3d.at(0)->GetTranslate().x - 0.05f,objects3d.at(0)->GetTranslate().y,objects3d.at(0)->GetTranslate().z });
			}
			if (input_->PushKey(DIK_S))
			{
				objects3d.at(0)->SetTranslate({ objects3d.at(0)->GetTranslate().x,objects3d.at(0)->GetTranslate().y - 0.05f,objects3d.at(0)->GetTranslate().z });
			}
			if (input_->PushKey(DIK_W))
			{
				objects3d.at(0)->SetTranslate({ objects3d.at(0)->GetTranslate().x,objects3d.at(0)->GetTranslate().y + 0.05f,objects3d.at(0)->GetTranslate().z });
			}

			if (objects3d.at(1)->GetTranslate().z <= 10.0f)
			{
				v[1] = 0.05f;
				if (objects3d.at(1)->GetTranslate().z <= -30.0f)
				{
					objects3d.at(1)->SetTranslate({ 0.0f,0.0f,-40.0f });
					v[1] = 0.0f;
				}
			}
			if (objects3d.at(1)->GetTranslate().z >= 30.0f)
			{
				v[1] = -0.05f;
			}
			objects3d.at(1)->SetTranslate({ objects3d.at(1)->GetTranslate().x,objects3d.at(1)->GetTranslate().y,objects3d.at(1)->GetTranslate().z + v[1] });

			if (objects3d.at(2)->GetTranslate().y <= -5.0f)
			{
				v[2] = 0.05f;
			}
			if (objects3d.at(2)->GetTranslate().y >= 5.0f)
			{
				v[2] = -0.05f;
			}
			objects3d.at(2)->SetTranslate({ objects3d.at(2)->GetTranslate().x,objects3d.at(2)->GetTranslate().y + v[2],objects3d.at(2)->GetTranslate().z });

			if (objects3d.at(3)->GetTranslate().y <= -5.0f)
			{
				v[3] = 0.05f;
			}
			if (objects3d.at(3)->GetTranslate().y >= 5.0f)
			{
				v[3] = -0.05f;
			}
			objects3d.at(3)->SetTranslate({ objects3d.at(3)->GetTranslate().x,objects3d.at(3)->GetTranslate().y + v[3],objects3d.at(3)->GetTranslate().z });

			if (objects3d.at(4)->GetTranslate().x <= -8.0f)
			{
				v[4] = 0.05f;
			}
			if (objects3d.at(4)->GetTranslate().x >= 8.0f)
			{
				v[4] = -0.05f;
			}
			objects3d.at(4)->SetTranslate({ objects3d.at(4)->GetTranslate().x + v[4], objects3d.at(4)->GetTranslate().y,objects3d.at(4)->GetTranslate().z });

			if (input_->PushKey(DIK_SPACE) && numShot == 0)
			{
				numShot = 1;
				objects3d.at(5)->SetTranslate(
					{
					objects3d.at(0)->GetTranslate().x,
					objects3d.at(0)->GetTranslate().y,
					objects3d.at(0)->GetTranslate().z
					});
			}
			if (numShot == 1)
			{
				objects3d.at(5)->SetTranslate({ objects3d.at(5)->GetTranslate().x,objects3d.at(5)->GetTranslate().y,objects3d.at(5)->GetTranslate().z + 0.2f });
			}
			if (numShot == 1 && objects3d.at(5)->GetTranslate().z >= 30.0f)
			{
				numShot = 0;
			}
			if (numShot == 0)
			{
				objects3d.at(5)->SetTranslate({ -20.0f,-20.0f,-20.0f });
			}

			if (objects3d.at(5)->GetTranslate().x >= objects3d.at(1)->GetTranslate().x - 0.7f && objects3d.at(5)->GetTranslate().x <= objects3d.at(1)->GetTranslate().x + 0.7f)
			{
				if (objects3d.at(5)->GetTranslate().y >= objects3d.at(1)->GetTranslate().y - 0.7f && objects3d.at(5)->GetTranslate().y <= objects3d.at(1)->GetTranslate().y + 0.7f)
				{
					if (objects3d.at(5)->GetTranslate().z >= objects3d.at(1)->GetTranslate().z - 0.1f && objects3d.at(5)->GetTranslate().z <= objects3d.at(1)->GetTranslate().z + 0.1f)
					{
						objects3d.at(1)->SetTranslate({ 0.0f,0.0f,-40.0f });
						hitCount++;
					}
				}
			}
			if (objects3d.at(5)->GetTranslate().x >= objects3d.at(2)->GetTranslate().x - 0.7f && objects3d.at(5)->GetTranslate().x <= objects3d.at(2)->GetTranslate().x + 0.7f)
			{
				if (objects3d.at(5)->GetTranslate().y >= objects3d.at(2)->GetTranslate().y - 0.7f && objects3d.at(5)->GetTranslate().y <= objects3d.at(2)->GetTranslate().y + 0.7f)
				{
					if (objects3d.at(5)->GetTranslate().z >= objects3d.at(2)->GetTranslate().z - 0.1f && objects3d.at(5)->GetTranslate().z <= objects3d.at(2)->GetTranslate().z + 0.1f)
					{
						objects3d.at(2)->SetTranslate({ 0.0f,0.0f,-40.0f });
						hitCount++;
					}
				}
			}
			if (objects3d.at(5)->GetTranslate().x >= objects3d.at(3)->GetTranslate().x - 0.7f && objects3d.at(5)->GetTranslate().x <= objects3d.at(3)->GetTranslate().x + 0.7f)
			{
				if (objects3d.at(5)->GetTranslate().y >= objects3d.at(3)->GetTranslate().y - 0.7f && objects3d.at(5)->GetTranslate().y <= objects3d.at(3)->GetTranslate().y + 0.7f)
				{
					if (objects3d.at(5)->GetTranslate().z >= objects3d.at(3)->GetTranslate().z - 0.1f && objects3d.at(5)->GetTranslate().z <= objects3d.at(3)->GetTranslate().z + 0.1f)
					{
						objects3d.at(3)->SetTranslate({ 0.0f,0.0f,-40.0f });
						hitCount++;
					}
				}
			}
			if (objects3d.at(5)->GetTranslate().x >= objects3d.at(4)->GetTranslate().x - 0.7f && objects3d.at(5)->GetTranslate().x <= objects3d.at(4)->GetTranslate().x + 0.7f)
			{
				if (objects3d.at(5)->GetTranslate().y >= objects3d.at(4)->GetTranslate().y - 0.7f && objects3d.at(5)->GetTranslate().y <= objects3d.at(4)->GetTranslate().y + 0.7f)
				{
					if (objects3d.at(5)->GetTranslate().z >= objects3d.at(4)->GetTranslate().z - 0.1f && objects3d.at(5)->GetTranslate().z <= objects3d.at(4)->GetTranslate().z + 0.1f)
					{
						objects3d.at(4)->SetTranslate({ 0.0f,0.0f,-40.0f });
						hitCount++;
					}
				}
			}
		}
		if (hitCount == 4)
		{
			numScene = 2;
			hitCount = 0;
		}
		break;
	case 0:
		sprites.at(0)->Update(width, height);
		if (input_->PushKey(DIK_SPACE))
		{
			objects3d.at(0)->SetTranslate({ 0.0f,2.0f,0.0f });
			objects3d.at(1)->SetTranslate({ 0.0f,0.0f,objects3d.at(1)->GetTranslate().z + v[1] });
			objects3d.at(2)->SetTranslate({ 4.0f,objects3d.at(2)->GetTranslate().y + v[2],15.0f });
			objects3d.at(3)->SetTranslate({ -4.0f,objects3d.at(3)->GetTranslate().y + v[3],15.0f });
			objects3d.at(4)->SetTranslate({ objects3d.at(4)->GetTranslate().x + v[4], 1.0f,15.0f });
			numScene++;
		}
		break;
	case 2:
		sprites.at(1)->Update(width, height);
		if (input_->PushKey(DIK_SPACE))
		{
			numScene = 0;
		}
		break;
	}
}

void GameScene::Draw()
{
	dx12Common->PreDraw();
	if (numScene == 1)
	{
		for (Object3d* object3d : objects3d)
		{
			object3d->Draw(object3dCommon, true, ModelManager::GetInstance()->GetModelCommon());
		}
	}
	if (numScene == 0)
	{
		sprites.at(0)->Draw(SPCommon);

	}
	if (numScene == 2)
	{
		sprites.at(1)->Draw(SPCommon);

	}
	imgui->Endframe(dx12Common->GetCommandList().Get());

	dx12Common->PostDraw();

}

void GameScene::Finalize(WinAPP* winAPP)
{
	CloseHandle(dx12Common->GetFenceEvent());
	//dx12Common->DX12Release();
	//delete imgui;
	for (Sprite* sprite : sprites)
	{
		delete sprite;
	}
	for (Model* model : models)
	{
		delete model;
	}
	ModelManager::GetInstance()->Finalize();
	for (Object3d* object3d : objects3d)
	{
		delete object3d;
	}
	delete object3dCommon;
	TextureManager::GetInstance()->Finalize();
	imgui->Finalize();
	delete input_;
	dx12Common->DeleteInstance();
	winAPP->Finalize();
	CoUninitialize();
}
