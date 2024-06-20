#include "Input.h"

void Input::Initialize(WinAPP* winApp)
{
	this->winApp_ = winApp;
	hr = DirectInput8Create(
		winApp_->GetHInstance(),
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
		winApp_->GetHWND(),
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
	if (keyPre[keyNumber] != 0 && key[keyNumber] == 0)
	{
		return true;
	}
	else
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

bool Input::GetJoystickState(int32_t stickNo, XINPUT_STATE& state)
{
	DWORD Result = XInputGetState(stickNo, &state);

	// XInputGetStateが成功した場合、resultの値はERROR_SUCCESS
	if (Result == ERROR_SUCCESS) {
		// デッドゾーンを適用
		state.Gamepad.sThumbLX = ApplyDeadzone(state.Gamepad.sThumbLX, DEADZONE_THRESHOLD);
		state.Gamepad.sThumbLY = ApplyDeadzone(state.Gamepad.sThumbLY, DEADZONE_THRESHOLD);
		// 他にも必要ならデッドゾーンの適用を追加

		return true;  // 成功した場合はtrueを返す
	}

	return Result == ERROR_SUCCESS;
}

// ジョイスティックのデッドゾーンを適用する関数
SHORT Input::ApplyDeadzone(SHORT value, SHORT deadzone) {
	if (value < -deadzone || value > deadzone) {
		// デッドゾーン外の場合、そのままの値を返す
		return value;
	}
	else {
		// デッドゾーン内の場合、0を返す
		return 0;
	}
}

