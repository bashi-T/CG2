#pragma once
#include<windows.h>
#include<cstdint>
#include"Systems/MyImGui.h"

class WinAPP
{
public:
	static void Initialize(const wchar_t* title);
	static LRESULT CALLBACK WindowProc(HWND hwnd,
		UINT msg, WPARAM wparam, LPARAM lparam);
	bool ProcessMessage();
	static void Finalize();
	const wchar_t* title = L"CG2";
	static int32_t GetClientWidth() { return clientWidth_; }
	static int32_t GetClientHeight() { return clientHeight_; }
	static HWND GetHWND() { return hwnd_; }
	static inline MSG GetMSG() { return msg; }
	static void ProcessMessage(MSG NewMSG);
	static inline MSG msg{};
	static WinAPP* GetInstance();
	static inline WNDCLASS wc_{};
	static HINSTANCE GetHInstance() { return wc_.hInstance; }

	static int32_t clientWidth_;
	static int32_t clientHeight_;
private:
	WinAPP() = default;
	~WinAPP() = default;
	WinAPP(const WinAPP& obj) = delete;
	WinAPP& oparator(const WinAPP& obj) = delete;

	static HWND hwnd_;
	static HINSTANCE hInstance;
};



