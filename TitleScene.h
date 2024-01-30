#pragma once
#include"WindowApp.h"
#include"Debug.h"
#include"Mesh.h"
#include"Input.h"
#include"SpriteCommon.h"
#include"Sprite.h"
#include"Object3dCommon.h"
#include"Object3d.h"
#include"ModelManager.h"
#include"model.h"
#include"TextureManager.h"
#include"Input.h"
#include"Camera.h"

class TitleScene
{
private:
	DX12Common* dx12Common = DX12Common::GetInstance();
	MyImGui* imgui = new MyImGui;
	SpriteCommon* SPCommon = new SpriteCommon;
	Object3dCommon* object3dCommon = new Object3dCommon;
	ModelCommon* modelCommon = new ModelCommon;
	Input* input_ = new Input;
	Camera* camera = new Camera;

	std::vector<Object3d*> objects3d;
	std::vector<Model*> models;
	std::vector<Sprite*> sprites;
	Vector2 posSprite = { 0.0f,0.0f };
	uint32_t numShot = 0;

	float v;
public:
	void Initialize(int32_t width, int32_t height, WinAPP* winAPP);
	void Update(int32_t width, int32_t height);
	void Draw();
	void Finalize(WinAPP* winAPP);
};

