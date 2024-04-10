#include "Input.h"

void Input::Initialize()
{
	hr = DirectInput8Create(
		WinAPP::GetInstance()->GetHInstance(),
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&directInput,
		nullptr);
	assert(SUCCEEDED(hr));

	hr = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	assert(SUCCEEDED(hr));

	hr = keyboard->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(hr));

	hr = keyboard->SetCooperativeLevel(
		WinAPP::GetInstance()->GetHWND(),
		DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(hr));
}

void Input::Update()
{
	memcpy(keyPre, key, sizeof(key));
	keyboard->Acquire();
	keyboard->GetDeviceState(sizeof(key), key);
}

bool Input::PushKey(BYTE keyNumber)
{
	if (key[keyNumber])
	{
		return true;
	}else
	{
		return false;
	}
}

bool Input::TriggerKey(BYTE keyNumber)
{
	if (keyPre[keyNumber] == 0 && key[keyNumber])
	{
		return true;
	}else
	{
		return false;
	}
}

Input* Input::GetInstance()
{
	if (instance == NULL)
	{
		instance = new Input;
	}
	return instance;
}
