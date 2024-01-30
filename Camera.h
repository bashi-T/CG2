#pragma once
#include"CGVector.h"

class Camera
{
private:
	TransformMatrix cameraTransform =
	{
		{1.0f, 1.0f, 1.0f},
		{ 0.3f, 0.0f, 0.0f },
		{ 0.0f, 4.0f, -10.0f }
	};
	Matrix4x4 cameraMatrix;
public:
	void Initialize();
	void Update();
	TransformMatrix GetCameraTransform() { return cameraTransform; }
	Matrix4x4 GetCameraMatrix() { return cameraMatrix; }
	void SetCameraTranslate(Vector3 Translate) { cameraTransform.translate = Translate; }
};

