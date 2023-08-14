#include "stdafx.h"
#include "Mouse.h"
#include "Globals.h"
#include <dinput.h>

LPDIRECTINPUT8 m_lpdi = NULL;
LPDIRECTINPUTDEVICE8 m_lpdimouse = NULL;
DIMOUSESTATE m_mouseState;

HANDLE m_hMouseThread = NULL;
DWORD m_tidMouse = 0;

POINT m_mousePoint = { 0, 0 };

#define CLAMP(X, A, B) ((X < A) ? A : ((X > B) ? B : X))

int InitializeMouse(HINSTANCE hInstance, HWND hWnd)
{
	if(FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void **)&m_lpdi,NULL)))
		return 0;

	if(FAILED(m_lpdi->CreateDevice(GUID_SysMouse, &m_lpdimouse, NULL)))
		return 0;

	if(FAILED(m_lpdimouse->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND)))
		return 0;

	if(FAILED(m_lpdimouse->SetDataFormat(&c_dfDIMouse)))
		return 0;

	DIPROPDWORD dipdw;
	dipdw.diph.dwSize = sizeof(DIPROPDWORD);
	dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	dipdw.diph.dwObj = 0;
	dipdw.diph.dwHow = DIPH_DEVICE;
	dipdw.dwData = DIPROPAXISMODE_REL;

	if(FAILED(m_lpdimouse->SetProperty(DIPROP_AXISMODE, &dipdw.diph)))
		return 0;

	if(FAILED(m_lpdimouse->Acquire()))
		return 0;

	return 1;
}

void ShutdownMouse()
{
	m_lpdimouse->Unacquire();
}

void ReverseMouse()
{
	m_lpdimouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_mouseState);

	int nX = GetSystemMetrics(SM_CXSCREEN);
	int nY = GetSystemMetrics(SM_CYSCREEN);

	m_mousePoint.x = CLAMP(m_mousePoint.x - m_mouseState.lX, 0, nX);
	m_mousePoint.y = CLAMP(m_mousePoint.y - m_mouseState.lY, 0, nY);
	
	SetCursorPos(m_mousePoint.x, m_mousePoint.y);
}

DWORD WINAPI MouseThread(LPVOID lpParam)
{
	MSG msg;

	BOOL fDone = FALSE;

	while (!fDone) 
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
		{ 
			switch(msg.message) 
			{
			case WM_QUIT:
			case WM_LBUTTONDOWN: 
			case WM_RBUTTONDOWN: 
			case WM_KEYDOWN: 
				fDone = TRUE;
				break;
			}
		};

		if(g_ReverseMouse)
			ReverseMouse();

		Sleep(1);
	}

	return (int) msg.wParam;
}

void StartMouseThread()
{
	GetCursorPos(&m_mousePoint);

	DWORD dwThrdParam = 1;
	m_hMouseThread = CreateThread(NULL, 0, MouseThread, &dwThrdParam, 0, &m_tidMouse);
}

void KillMouseThread()
{
	PostThreadMessage(m_tidMouse, WM_QUIT, 0, 0);

	if(m_hMouseThread != NULL)
	{
		CloseHandle(m_hMouseThread);
		m_hMouseThread = NULL;
	}
}

