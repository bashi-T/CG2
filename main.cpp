#include"GameScene.h"

const int32_t kWindowWidth = 1280;
const int32_t kWindowHeight = 720;
const int32_t kNumTriangle = 1;
Vector4* vertexData = nullptr;
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	CoInitializeEx(0, COINIT_MULTITHREADED);
	Debug::D3DResourceLeakChecker* leakCheck = new Debug::D3DResourceLeakChecker;
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	GameScene* gameScene = new GameScene;
	WinAPP* winAPP = WinAPP::GetInstance();
	MSG NewMSG = winAPP->GetMSG();
	uint32_t sceneNumber = 0;

	gameScene->Initialize(kWindowWidth, kWindowHeight, winAPP);

	while (NewMSG.message != WM_QUIT)
	{
		gameScene->Update(kWindowWidth, kWindowHeight);

		if (winAPP->ProcessMessage())
		{
			ImGui::Render();
			break;
		}
		gameScene->Draw();
	}

	gameScene->Finalize(winAPP);
	delete leakCheck;
	return 0;
}