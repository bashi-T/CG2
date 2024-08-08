#include "titleScene.h"

void TitleScene::Init()
{
	//object3dCommon = new Object3dCommon; 
	//input = new Input;
	//srvManager = new SRVManager;
	//camera = new Camera();
	std::string textureFilePath[100] =//textureの左右が反転してる
	{
		"Resource/civ6.png",//一番最初のテクスチャがうまく読み込まれない
		"Resource/ganban.png",
		"Resource/white.png",
		"Resource/worldMap.png",
		"Resource/monsterBall.png",
		"Resource/AnimatedCube/AnimatedCube_BaseColor.png",
		"Resource/world.png",
		"Resource/uvChecker.png",
		"Resource/cursor.png",
		"Resource/circle.png",
		"Resource/particle.png",
		"Resource/colorbabble.png",
		
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
		//if(i==0)
		//{
		//	object3d->Initialize(Object3dCommon::GetInstance(), SRVManager::GetInstance());
		//	ModelManager::GetInstance()->LoadAnimationModel(objFilePath[i], textureFilePath[i + 1]);
		//	object3d->SetModel(objFilePath[i]);
		//}
		//else if (i == 1||i==2)
		//{
		//	object3d->Initialize(Object3dCommon::GetInstance(), SRVManager::GetInstance());
		//	ModelManager::GetInstance()->LoadSkeltonAnimation(objFilePath[i], textureFilePath[i + 1], SRVManager::GetInstance());
		//	object3d->SetModel(objFilePath[i]);
		//}else
		//{
		//	object3d->Initialize(Object3dCommon::GetInstance(), SRVManager::GetInstance());
		//	ModelManager::GetInstance()->LoadModel(objFilePath[i], textureFilePath[i + 1]);
		//	object3d->SetModel(objFilePath[i]);
		//}
	object3d->Initialize(Object3dCommon::GetInstance(), SRVManager::GetInstance());
	ModelManager::GetInstance()->LoadSkeltonAnimation(objFilePath[1], textureFilePath[/*2 + */1], SRVManager::GetInstance());
	object3d->SetModel(objFilePath[1]);
	Model* model = ModelManager::GetInstance()->FindModel(objFilePath[1]);
	Model::ModelData* modelData = model->GetModelData();
	for (Model::VertexData& vertex : modelData->vertices)
	{
		vertex.normal.x = vertex.position.x;
		vertex.normal.y = vertex.position.y;
		vertex.normal.z = vertex.position.z;
	}
	model->Memcpy();
	//object3d->SetTranslate({ 3.0f - (2.0f * i), float(pow(-1.0,i)), 1.0f });
	object3d->SetRotate({ 0.0f, 3.3f, 0.0f });
	objects3d.push_back(object3d);
	particle->Initialize(textureFilePath[11], SRVManager::GetInstance(), Object3dCommon::GetInstance());
	particles.push_back(particle);
};
    //objects3d[1]->SetScale({ 0.005f,0.005f ,0.005f });
	sprite = new Sprite;
	sprite->Initialize(SpriteCommon::GetInstance(), SRVManager::GetInstance(), "Resource/Titleexample.png");
	sprite->SetPositoin({ float(WinAPP::clientWidth_ / 2) - (sprite->GetSize().x / 2),0.0f });
	Object3dCommon::GetInstance()->SetDefaultCamera(Camera::GetInstance());
}

void TitleScene::Update()
{
	XINPUT_STATE joyState;
	for (Object3d* object3d : objects3d)
	{
		//if (Input::GetInstance()->PushKey(DIK_D))
		//{
		//	object3d->SetTranslate({ object3d->GetTranslate().x + 0.01f ,object3d->GetTranslate().y ,object3d->GetTranslate().z });
		//}
		//if (Input::GetInstance()->PushKey(DIK_A))
		//{
		//	object3d->SetTranslate({ object3d->GetTranslate().x - 0.01f ,object3d->GetTranslate().y ,object3d->GetTranslate().z });
		//}
		if (Input::GetInstance()->GetJoystickState(0, joyState))
		{
			object3d->SetTranslate({ object3d->GetTranslate().x + (float)joyState.Gamepad.sThumbLX / (SHRT_MAX * 10.0f) ,0.0f ,object3d->GetTranslate().z + (float)joyState.Gamepad.sThumbLY / (SHRT_MAX * 10.0f) });
			
		}
		if ((float)joyState.Gamepad.sThumbLX != 0.0f || (float)joyState.Gamepad.sThumbLY != 0.0f)
		{
			object3d->SetIsAnimation(true);
		}
		else
		{
			object3d->SetIsAnimation(false);
		}

		if ((float)joyState.Gamepad.sThumbLY > 0)
		{
			object3d->SetRotate({ 0.0f,0.0f,0.0f });
		}
		if ((float)joyState.Gamepad.sThumbLY < 0)
		{
			object3d->SetRotate({ 0.0f,3.0f,0.0f });
		}
		if ((float)joyState.Gamepad.sThumbLX > 0)
		{
			object3d->SetRotate({ 0.0f,1.5f,0.0f });
		}
		if ((float)joyState.Gamepad.sThumbLX < 0)
		{
			object3d->SetRotate({ 0.0f,4.5f,0.0f });
		}
	}
	//objects3d[0]->AnimationUpdate(Camera::GetInstance());
	//objects3d[1]->SkeltonUpdate(Camera::GetInstance());
	//objects3d[2]->SkeltonUpdate(Camera::GetInstance());
	//objects3d[3]->Update(Camera::GetInstance());
	objects3d[0]->SkeltonUpdate(Camera::GetInstance());
	for (Particle* particle : particles)
	{
		particle->Update();
	}
	sprite->Update();
	if (Input::GetInstance()->GetJoystickState(0, joyState))
	{
		if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER)
		{
			sceneNo = INGAME;
			GameManager::stageNumber = 0;
		}
	}

}

void TitleScene::Draw()
{
	//for (Object3d* object3d : objects3d)
	//{
	//	object3d->Draw(Object3dCommon::GetInstance(), ModelManager::GetInstance()->GetModelCommon());
	//}
	//objects3d[1]->SkeltonDraw(ModelManager::GetInstance()->GetModelCommon());
	//objects3d[3]->Draw(ModelManager::GetInstance()->GetModelCommon());

	//objects3d[0]->SkeltonDraw(ModelManager::GetInstance()->GetModelCommon());
	for (Particle* particle : particles)
	{
		particle->Draw();
	}
	sprite->Draw();
}

void TitleScene::Finalize()
{
	for (Object3d* object3d : objects3d)
	{
		delete object3d;
	}
	delete sprite;
}
