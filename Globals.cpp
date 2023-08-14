#include "stdafx.h"
#include "Globals.h"

INT g_U360JoyCount = 0;
INT g_ServoJoyCount = 0;
INT g_PacDriveCount = 0;

BOOL g_Exit = FALSE;
BOOL g_FirstRun = TRUE;

HANDLE g_hMainThread = NULL;

BOOL g_LaunchOnStartup = FALSE;
BOOL g_MouseDrag = FALSE;
BOOL g_ReverseMouse = FALSE;
CHAR g_U360MapFile[256] = { 0 };
CHAR g_ServoMapFile[256] = { 0 };

BOOL g_MouseDown = FALSE;
