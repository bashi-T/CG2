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

		"Resource/worldMap.png",
		"Resource/world.png",
		"Resource/ground.png",
		"Resource/glass.png",
		"Resource/cursor.png",
		"Resource/ganban.png",

		"Resource/monsterBall.png",
		"Resource/uvChecker.png",
		"Resource/particle.png",
	};
	std::string objFilePath[10] =
	{
		"sphere.obj",
		"world.obj",
		"ball.obj",
		"cube.obj",
		"plane.obj",
		"axis.obj",
	};

	for (uint32_t i = 0; i < 9; i++)
	{
		Sprite* sprite = new Sprite();
		sprite->Initialize(textureFilePath[i + 1]);
		posSprite.x = 100.0f * i;
		posSprite.y = 50.0f * i;
		sprite->SetPositoin(posSprite);
		sprites.push_back(sprite);
	}
	for (uint32_t i = 0; i < 1; i++)
	{
		Object3d* object3d = new Object3d;
		Particle* particle = new Particle;
		object3d->Initialize();
		modelManager->LoadModel(objFilePath[i], textureFilePath[i + 1]);
		object3d->SetModel(objFilePath[i]);
		Model* model = modelManager->FindModel(objFilePath[i]);
		Model::ModelData* modelData = model->GetModelData();
		for (Model::VertexData& vertex : modelData->vertices)
		{
			vertex.normal.x = vertex.position.x;
			vertex.normal.y = vertex.position.y;
			vertex.normal.z = vertex.position.z;
		}
		model->Memcpy();
		//switch (i % 2)
		//{
		//case 0:
		//	object3d->SetTranslate({ -4.0f + 1.0f * i,-1.0f,0.0f });
		//	break;
		//case 1:
		//	object3d->SetTranslate({ -4.0f + 1.0f * i,1.0f,0.0f });
		//	break;
		//}
		objects3d.push_back(object3d);

		particle->Initialize(textureFilePath[i + 1]);
		particles.push_back(particle);
	};
	objects3d[0]->SetScale({ 10.0f,10.0f,10.0f });
	//objects3d[1]->SetScale({ 0.0025f,0.0025f,0.0025f });
	//objects3d[2]->SetScale({ 0.0025f,0.0025f,0.0025f });
	object3dCommon->SetDefaultCamera();
	player->Initialize();
}

void GameScene::Update()
{
	//if (input->PushKey(DIK_D))
	//{
	//	objects3d[1]->SetTranslate({ objects3d[1]->GetTranslate().x + 0.1f ,objects3d[1]->GetTranslate().y ,objects3d[1]->GetTranslate().z });
	//	camera->SetTranslate({ camera->GetTranslate().x + 0.1f, camera->GetTranslate().y,camera->GetTranslate().z });
	//}
	//if (input->PushKey(DIK_A))
	//{
	//	objects3d[1]->SetTranslate({ objects3d[1]->GetTranslate().x - 0.1f ,objects3d[1]->GetTranslate().y ,objects3d[1]->GetTranslate().z });
	//	camera->SetTranslate({ camera->GetTranslate().x - 0.1f, camera->GetTranslate().y,camera->GetTranslate().z });
	//}
	objects3d[0]->SetRotate({ objects3d[0]->GetRotate().x, objects3d[0]->GetRotate().y - 0.005f, objects3d[0]->GetRotate().z });
	
	for (Object3d* object3d : objects3d)
	{
		object3d->Update(camera);
	}
	for (Sprite* sprite : sprites)
	{
		sprite->Update();
	}
	player->Update();
	for (Particle* particle : particles)
	{
		particle->Update();
	}
	if (input->TriggerKey(DIK_SPACE))
	{
		sceneNo = TITLE;
	}
	ImGui::Begin("player");
	ImGui::DragFloat3("player.translate", (float*)&player->GetTranslate().x, 0.01f);
	ImGui::End();
}

void GameScene::Draw()
{
	for (Object3d* object3d : objects3d)
	{
		object3d->Draw();
	}
	player->Draw();
	//for (Particle* particle : particles)
	//{
	//	particle->Draw();
	//}
	for (Sprite* sprite : sprites)
	{
		//sprite->Draw(SPCommon);
	}
	//sprites[8]->Draw(SPCommon);
}

void GameScene::Finalize()
{
	for (Sprite* sprite : sprites)
	{
		delete sprite;
	}
}
