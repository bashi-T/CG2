#include "Camera.h"

Camera::Camera()
:transformMatrix({ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} })
, fovY_(0.45f)
, aspectRetio_((float(WinAPP::clientWidth_) / float(WinAPP::clientHeight_)))
, nearClip_(0.1f)
, farClip_(100.0f)
, worldMatrix(MakeAffineMatrix(transformMatrix.scale, transformMatrix.rotate, transformMatrix.translate))
, viewMatrix(Inverse(worldMatrix))
, projectionMatrix(MakePerspectiveFovMatrix(fovY_, aspectRetio_, nearClip_, farClip_))
, viewProjectionMatrix(Multiply(viewMatrix, projectionMatrix))
{}

void Camera::Initialize()
{
	//transformMatrix={ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	//fovY=0.45f;
	//aspectRetio = (float(width) / float(height));
	//nearClip = 0.1f;
	//farClip = 100.0f;
	//worldMatrix = MakeAffineMatrix(transformMatrix.scale, transformMatrix.rotate, transformMatrix.translate);
	//viewMatrix = Inverse(worldMatrix);
	//projectionMatrix =
	//	MakePerspectiveFovMatrix(fovY, aspectRetio, nearClip, farClip);
	//viewProjectionMatrix = Multiply(viewMatrix, projectionMatrix);
}

void Camera::Update()
{
	worldMatrix = MakeAffineMatrix(transformMatrix.scale, transformMatrix.rotate, transformMatrix.translate);
	viewMatrix = Inverse(worldMatrix);
	projectionMatrix =
		MakePerspectiveFovMatrix(fovY_, aspectRetio_, nearClip_, farClip_);
	viewProjectionMatrix = Multiply(viewMatrix, projectionMatrix);
}

Camera* Camera::GetInstance()
{
	if (instance == NULL)
	{
		instance = new Camera;
	}
	return instance;
}
