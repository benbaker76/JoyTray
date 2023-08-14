#include "stdafx.h"
#include <stdio.h>
#include "Input.h"
#include "Globals.h"
#include "Mouse.h"

void ProcessMouseClickLock(PMOUSEEVENTARGS e)
{
	switch(e->Button)
	{
	case MOUSE_LBUTTONDOWN:
	case MOUSE_LBUTTONUP:
        break;
	case MOUSE_RBUTTONDOWN:
	case MOUSE_RBUTTONUP:
        break;
	case MOUSE_MBUTTONDOWN:
		e->Handled = TRUE;
		break;
	case MOUSE_MBUTTONUP:
		g_MouseDown = !g_MouseDown;

		mouse_event((g_MouseDown ? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_LEFTUP), 0, 0, 0, 0);

		e->Handled = TRUE;
		break;
	}
}
