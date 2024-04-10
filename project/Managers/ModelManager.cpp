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

void ModelManager::Initialize()
{
	ModelCommon::GetInstance()->Initialize(DX12Common::GetInstance());
}

void ModelManager::LoadModel(const std::string& filePath, const std::string& TextureFilePath)
{
	std::unique_ptr<Model> model = std::make_unique<Model>();
	if (models.contains(filePath))
	{
		return;
	}
	model->Initialize(filePath, TextureFilePath);
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

