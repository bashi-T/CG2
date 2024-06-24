#include "ModelManager.h"

ModelManager* ModelManager::instance = nullptr;

ModelManager* ModelManager::GetInstance()
{
	if (instance == nullptr)
	{
		instance = new ModelManager;
	}
	return instance;
}

void ModelManager::Finalize()
{
	delete instance;
	instance = nullptr;
}

void ModelManager::Initialize(DX12Common* dxCommon)
{
	modelCommon_ = new ModelCommon;
	modelCommon_->Initialize(dxCommon);
}

void ModelManager::LoadModel(const std::string& filePath, const std::string& TextureFilePath)
{
	//if (models.contains(filePath))
	//{
	//	return;
	//}

	std::unique_ptr<Model> model = std::make_unique<Model>();
	model->ModelInitialize(modelCommon_, filePath, TextureFilePath);
	models.insert(std::make_pair(filePath, std::move(model)));
}

void ModelManager::LoadAnimationModel(const std::string& filePath, const std::string& TextureFilePath)
{
	//if (models.contains(filePath))
	//{
	//	return;
	//}

	std::unique_ptr<Model> model = std::make_unique<Model>();
	model->AnimationInitialize(modelCommon_, filePath, TextureFilePath);
	models.insert(std::make_pair(filePath, std::move(model)));
}

void ModelManager::LoadSkeltonAnimation(const std::string& filePath, const std::string& TextureFilePath, SRVManager* srvManager)
{
	//if (models.contains(filePath))
	//{
	//	return;
	//}

	std::unique_ptr<Model> model = std::make_unique<Model>();
	model->SkeltonInitialize(modelCommon_, filePath, TextureFilePath,srvManager);
	models.insert(std::make_pair(filePath, std::move(model)));
}

Model* ModelManager::FindModel(const std::string& filePath)
{
	if (models.contains(filePath))
	{
		return models.at(filePath).get();
	}
	return nullptr;
}

