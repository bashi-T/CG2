#pragma once
#include<string>
#include<map>
#include<memory>
#include"Objects/Model.h"
#include "Commons/DX12Common.h"

class ModelManager
{
private:
	static ModelManager* instance;
	ModelCommon* modelCommon_ = nullptr;

	std::map<std::string, std::unique_ptr<Model>>models;

	ModelManager() = default;
	~ModelManager() = default;
	ModelManager(ModelManager&) = delete;
	ModelManager& operator=(ModelManager&) = delete;

public:
	static ModelManager* GetInstance();

	void Finalize();
	void Initialize();
	void LoadModel(const std::string& filePath, const std::string& TextureFilePath);
	Model* FindModel(const std::string& filePath);

	ModelCommon* GetModelCommon() { return modelCommon_; }

};

