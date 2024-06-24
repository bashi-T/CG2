#pragma once
#include"Input/Input.h"
#include"Objects/Object3d.h"
#include"Commons/Object3dCommon.h"
#include"Objects/Particle.h"

class Whale
{
public:
	void Initialize();
	void Update();
	void Draw();

private:
	Object3d* object3d = new Object3d;
	const std::string whaleModel = "AnimatedCube/AnimatedCube.gltf";
	const std::string whaleSkin = "Resource/AnimatedCube/AnimatedCube_BaseColor.png";
	Vector3 whaleSpeed = { 0.0f,0.0f,0.0f };
	Vector3 accSpeed = { 0.0f,0.0f,0.0f };
};

