#include "titleScene.h"

void TitleScene::Init()
{
	//object3dCommon = new Object3dCommon; 
	//input = new Input;
	//srvManager = new SRVManager;
	//camera = new Camera();
	std::string textureFilePath[100] =
	{
		"Resource/civ6.png",//一番最初のテクスチャがうまく読み込まれない
		"Resource/worldMap.png",
		"Resource/world.png",
		"Resource/monsterBall.png",
		"Resource/uvChecker.png",
		"Resource/ganban.png",
		"Resource/uvChecker.png",
		"Resource/uvChecker.png",
		"Resource/cursor.png",
		"Resource/particle.png",
	};
	TextureManager::GetInstance()->LoadTexture(textureFilePath[0]);
	std::string objFilePath[10] =
	{
		"sphere.obj",
		"world.obj",
		"ball.obj",
		"axis.obj",
		"plane.obj",
	};
	for (uint32_t i = 0; i < 2; i++)
	{
		Object3d* object3d = new Object3d;
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
		object3d->SetTranslate({ 1.0f - (2.0f * i), 0.0f, 0.0f });
		objects3d.push_back(object3d);

		Particle* particle = new Particle;
		particle->Initialize(textureFilePath[9-i]);
		particles.push_back(particle);
	};
	object3dCommon->SetDefaultCamera();
}

void TitleScene::Update()
{
	objects3d[0]->SetRotate({ objects3d[0]->GetRotate().x, objects3d[0]->GetRotate().y - 0.005f ,objects3d[0]->GetRotate().z });
	for (Object3d* object3d : objects3d)
	{
		//if (input->PushKey(DIK_D))
		//{
		//	object3d->SetTranslate({ object3d->GetTranslate().x + 0.1f ,object3d->GetTranslate().y ,object3d->GetTranslate().z });
		//	camera->SetTranslate({ camera->GetTranslate().x + 0.1f, camera->GetTranslate().y,camera->GetTranslate().z });
		//}
		//if (input->PushKey(DIK_A))
		//{
		//	object3d->SetTranslate({ object3d->GetTranslate().x - 0.1f ,object3d->GetTranslate().y ,object3d->GetTranslate().z });
		//	camera->SetTranslate({ camera->GetTranslate().x - 0.1f, camera->GetTranslate().y,camera->GetTranslate().z });
		//}
		//if (input->PushKey(DIK_S))
		//{
		//	object3d->SetTranslate({ object3d->GetTranslate().x ,object3d->GetTranslate().y - 0.1f ,object3d->GetTranslate().z });
		//	camera->SetTranslate({ camera->GetTranslate().x, camera->GetTranslate().y - 0.1f,camera->GetTranslate().z });
		//}
		//if (input->PushKey(DIK_W))
		//{
		//	object3d->SetTranslate({ object3d->GetTranslate().x ,object3d->GetTranslate().y + 0.1f ,object3d->GetTranslate().z });
		//	camera->SetTranslate({ camera->GetTranslate().x, camera->GetTranslate().y + 0.1f,camera->GetTranslate().z });
		//}
		object3d->Update(camera);
	}
	for (Particle* particle : particles)
	{
		particle->Update();
	}

	if (input->TriggerKey(DIK_SPACE))
	{
		sceneNo = INGAME;
	}
}

void TitleScene::Draw()
{
	for (Object3d* object3d : objects3d)
	{
		object3d->Draw();
	}
	for (Particle* particle : particles)
	{
		particle->Draw();
	}
}

void TitleScene::Finalize()
{
	for (Object3d* object3d : objects3d)
	{
		delete object3d;
	}
	for (Particle* particle : particles)
	{
		delete particle;
	}

}
