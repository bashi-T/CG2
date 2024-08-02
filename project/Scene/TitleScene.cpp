#include "titleScene.h"

void TitleScene::Init()
{
	std::string textureFilePath[100] =
	{
		"Resource/white.png",
		"Resource/ganban.png",
		"Resource/worldMap.png",
		"Resource/monsterBall.png",
		"Resource/AnimatedCube/AnimatedCube_BaseColor.png",
		"Resource/world.png",
		"Resource/uvChecker.png",
		"Resource/cursor.png",
		"Resource/circle.png",
		"Resource/particle.png",
	};
	TextureManager::GetInstance()->LoadTexture(textureFilePath[0]);
	std::string objFilePath[100] =
	{
		"AnimatedCube/AnimatedCube.gltf",
		"human/sneakWalk.gltf",
		"simpleSkin/simpleSkin.gltf",
		"plane/plane.gltf",
	};
	for (uint32_t i = 0; i < 1; i++)
	{
		Object3d* object3d = new Object3d;
		Particle* particle = new Particle;
		//uint32_t j = i + 1;
		if (i == 0)
		{
			object3d->Initialize(Object3dCommon::GetInstance(), SRVManager::GetInstance());
			ModelManager::GetInstance()->LoadAnimationModel(objFilePath[i], textureFilePath[i]);
			object3d->SetModel(objFilePath[i]);
		}
		else if (i == 1 || i == 2)
		{
			object3d->InitializeSkeleton(Object3dCommon::GetInstance(), SRVManager::GetInstance());
			ModelManager::GetInstance()->LoadSkeltonAnimation(objFilePath[i], textureFilePath[i], SRVManager::GetInstance());
			object3d->SetModel(objFilePath[i]);
		}
		else
		{
			object3d->Initialize(Object3dCommon::GetInstance(), SRVManager::GetInstance());
			ModelManager::GetInstance()->LoadModel(objFilePath[i], textureFilePath[i]);
			object3d->SetModel(objFilePath[i]);
		}
		//object3d->Initialize(Object3dCommon::GetInstance(), SRVManager::GetInstance());
		//ModelManager::GetInstance()->LoadSkeltonAnimation(objFilePath[1], textureFilePath[1], SRVManager::GetInstance());
		//object3d->SetModel(objFilePath[1]);
		//Model* model = ModelManager::GetInstance()->FindModel(objFilePath[1]);

		Model* model = ModelManager::GetInstance()->FindModel(objFilePath[i]);
		for (Model::VertexData& vertex : model->GetModelData()->vertices)
		{
			vertex.normal.x = vertex.position.x;
			vertex.normal.y = vertex.position.y;
			vertex.normal.z = vertex.position.z;
		}
		model->Memcpy();
		object3d->SetTranslate({ 3.0f - (2.0f * i), float(pow(-1.0,i)), 1.0f });
		objects3d.push_back(object3d);
		//object3d->SetRotate({ 0.0f, 3.3f, 0.0f });
		particle->Initialize("Resource/particle.png", Object3dCommon::GetInstance());
		particles.push_back(particle);
	}

	Object3dCommon::GetInstance()->SetDefaultCamera(Camera::GetInstance());
}

void TitleScene::Update()
{
	//XINPUT_STATE joyState;
	for (Object3d* object3d : objects3d)
	{
		object3d->SetIsAnimation(true);
		if (Input::GetInstance()->PushKey(DIK_D))
		{
			object3d->SetTranslate({ object3d->GetTranslate().x + 0.01f ,object3d->GetTranslate().y ,object3d->GetTranslate().z });
		}
		if (Input::GetInstance()->PushKey(DIK_A))
		{
			object3d->SetTranslate({ object3d->GetTranslate().x - 0.01f ,object3d->GetTranslate().y ,object3d->GetTranslate().z });
		}
		//if (Input::GetInstance()->GetJoystickState(0, joyState))
		//{
		//	object3d->SetTranslate({ object3d->GetTranslate().x + (float)joyState.Gamepad.sThumbLX / (SHRT_MAX * 10.0f) ,0.0f ,object3d->GetTranslate().z + (float)joyState.Gamepad.sThumbLY / (SHRT_MAX * 10.0f) });
		//	
		//}
		//if ((float)joyState.Gamepad.sThumbLX != 0.0f || (float)joyState.Gamepad.sThumbLY != 0.0f)
		//{
		//	object3d->SetIsAnimation(true);
		//}
		//else
		//{
		//	object3d->SetIsAnimation(false);
		//}
		//
		//if ((float)joyState.Gamepad.sThumbLY > 0)
		//{
		//	object3d->SetRotate({ 0.0f,0.0f,0.0f });
		//}
		//if ((float)joyState.Gamepad.sThumbLY < 0)
		//{
		//	object3d->SetRotate({ 0.0f,3.0f,0.0f });
		//}
		//if ((float)joyState.Gamepad.sThumbLX > 0)
		//{
		//	object3d->SetRotate({ 0.0f,1.5f,0.0f });
		//}
		//if ((float)joyState.Gamepad.sThumbLX < 0)
		//{
		//	object3d->SetRotate({ 0.0f,4.5f,0.0f });
		//}
	}
	objects3d[0]->AnimationUpdate(Camera::GetInstance());
	//objects3d[1]->SkeltonUpdate(Camera::GetInstance());
	//objects3d[2]->SkeltonUpdate(Camera::GetInstance());
	//objects3d[3]->Update(Camera::GetInstance());

	for (Particle* particle : particles)
	{
		particle->Update();
	}
	if (Input::GetInstance()->TriggerKey(DIK_SPACE))
	{
		sceneNo = INGAME;
	}
}

void TitleScene::Draw()
{
	objects3d[0]->Draw(ModelManager::GetInstance()->GetModelCommon());
	//objects3d[1]->SkeltonDraw(ModelManager::GetInstance()->GetModelCommon());
	//objects3d[2]->SkeltonDraw(ModelManager::GetInstance()->GetModelCommon());
	//objects3d[3]->Draw(ModelManager::GetInstance()->GetModelCommon());

	//for (Particle* particle : particles)
	//{
	//	particle->Draw();
	//}
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
