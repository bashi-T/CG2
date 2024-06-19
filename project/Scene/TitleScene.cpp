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
		"Resource/monsterBall.png",
		"Resource/white.png",
		"Resource/worldMap.png",
		"Resource/AnimatedCube/AnimatedCube_BaseColor.png",
		"Resource/world.png",
		"Resource/uvChecker.png",
		"Resource/ganban.png",
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
	objects3d.push_back(object3d);
	//particle->Initialize(textureFilePath[9-i], SRVManager::GetInstance(), Object3dCommon::GetInstance());
	//particles.push_back(particle);
};
//objects3d[1]->SetScale({ 0.005f,0.005f ,0.005f });

	Object3dCommon::GetInstance()->SetDefaultCamera(Camera::GetInstance());
}

void TitleScene::Update()
{
	for (Object3d* object3d : objects3d)
	{
		if (Input::GetInstance()->PushKey(DIK_D))
		{
			object3d->SetTranslate({ object3d->GetTranslate().x + 0.01f ,object3d->GetTranslate().y ,object3d->GetTranslate().z });
		}
		if (Input::GetInstance()->PushKey(DIK_A))
		{
			object3d->SetTranslate({ object3d->GetTranslate().x - 0.01f ,object3d->GetTranslate().y ,object3d->GetTranslate().z });
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
	if (Input::GetInstance()->TriggerKey(DIK_SPACE))
	{
		sceneNo = INGAME;
	}
}

void TitleScene::Draw()
{
	//for (Object3d* object3d : objects3d)
	//{
	//	object3d->Draw(Object3dCommon::GetInstance(), ModelManager::GetInstance()->GetModelCommon());
	//}
	//objects3d[1]->SkeltonDraw(ModelManager::GetInstance()->GetModelCommon());
	objects3d[0]->SkeltonDraw(ModelManager::GetInstance()->GetModelCommon());
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
}
