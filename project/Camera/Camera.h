#pragma once
#include"Math/CGVector.h"
#include"Systems/WindowApp.h"

class Camera
{
private:
	EulerTransform transformMatrix;
	Matrix4x4 worldMatrix;
	Matrix4x4 viewMatrix;
	Matrix4x4 projectionMatrix;
	Matrix4x4 viewProjectionMatrix;
	float fovY;
	float aspectRetio;
	float nearClip;
	float farClip;
	static inline Camera* instance;

public:
	Camera();
	void Initialize();
	void Update();
	void SetRotate(const Vector3& rotate) { transformMatrix.rotate = rotate; }
	void SetTranslate(const Vector3& translate) { transformMatrix.translate = translate; }
	void SetFovY(const float& fovY) { this->fovY = fovY; }
	void SetAspectretio(const float& aspectRetio) { this->aspectRetio = aspectRetio; }
	void SetNearClip(const float& nearClip) { this->nearClip = nearClip; }
	void SetFarClip(const float& farClip) { this->farClip = farClip; }
	const Vector3& GetRotate()const { return transformMatrix.rotate; }
	const Vector3& GetTranslate()const { return transformMatrix.translate; }
	const Matrix4x4& GetWorldMatrix() { return worldMatrix; }
	const Matrix4x4& GetViewMatrix() { return viewMatrix; }
	const Matrix4x4& GetProjectionMatrix() { return projectionMatrix; }
	const Matrix4x4& GetViewProjectionMatrix() { return viewProjectionMatrix; }
	static Camera* GetInstance();
};

