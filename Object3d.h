#pragma once
#include "CGVector.h"
#include "DX12Common.h"
#include "MyImGui.h"
#include <cassert>
#include <dxcapi.h>
#include <fstream>
#include <sstream>
#include"TextureManager.h"
#include"Model.h"
#include"ModelManager.h"
#include"Camera.h"

#pragma comment(lib, "dxcompiler.lib")

class Object3dCommon;
class Object3d
{
private:
	Object3dCommon* object3dCommon_ = nullptr;
	HRESULT hr = NULL;
	Model* model_ = nullptr;
	ModelCommon* modelCommon_ = nullptr;
	Camera* camera = nullptr;

	struct DirectionalLight
	{
		Vector4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
		Vector3 direction = { 0.0f, -1.0f, 0.0f };
		float intensity = 1.0f;
	};
	DirectionalLight* DirectionalLightData = nullptr;

	ComPtr<ID3D12Resource> transformationMatrixResource;

	struct TransformationMatrix
	{
		Matrix4x4 WVP;
		Matrix4x4 World;
	};

	TransformationMatrix* transformationMatrixData = nullptr;

	TransformMatrix transformMatrix;
	Matrix4x4 worldViewProjectionMatrix;

	ComPtr<ID3D12Resource> directionalLightResource;
public:
	void Initialize(Object3dCommon* object3dCommon, int32_t width, int32_t height);
	void Update();
	void Draw(Object3dCommon* object3dCommon,bool useWorldMap, ModelCommon* modelCommon);
	
	ComPtr<ID3D12Resource> CreateBufferResource(Object3dCommon* object3dCommon, size_t sizeInBytes);

	DirectionalLight* DrawDirectionalLightData() { return DirectionalLightData; }

	void SetModel(const std::string& filePath);
	void SetModel(Model* model) { this->model_ = model; }
	void SetScale(const Vector3& scale) { transformMatrix.scale = scale; }
	void SetRotate(const Vector3& rotate) { transformMatrix.rotate = rotate; }
	void SetTranslate(const Vector3& translate) { transformMatrix.translate = translate; }
	void SetCamera(Camera* camera) { this->camera = camera; }
	const Vector3& GetScale()const { return transformMatrix.scale; }
	const Vector3& GetRotate()const { return transformMatrix.rotate; }
	const Vector3& GetTranslate()const { return transformMatrix.translate; }

};

