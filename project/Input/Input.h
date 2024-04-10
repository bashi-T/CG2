#pragma once
#define DIRECTINPUT_VERSION 0x0800
#include<dinput.h>
#include<WRL.h>
#include"Systems/WindowApp.h"

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

class Input
{
public:
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	void Initialize();
	void Update();
	bool PushKey(BYTE keyNumber);
	bool TriggerKey(BYTE keyNumber);
	static Input* GetInstance();

private:
	WinAPP* winApp_ = nullptr;
	HRESULT hr;
	ComPtr<IDirectInput8>directInput = nullptr;
	ComPtr<IDirectInputDevice8> keyboard;
	BYTE key[256] = {};
	BYTE keyPre[256] = {};
	static inline Input* instance;
};

