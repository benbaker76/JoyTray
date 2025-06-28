// JoyTray.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include <sys/stat.h>
#include <Commdlg.h>
#include <shlobj.h> // For SHGetFolderPath
#include "resource.h"
#include "SystemTraySDK.h"
#include "InterComm.h"
#include "Globals.h"
#include "KeyCodes.h"
#include "Hook.h"
#include "Ini.h"
#include "CmdLine.h"
#include "UltraStik.h"
#include "PacDrive.h"
#include "Mouse.h"

struct U360JoyData
{
	UINT id;
	const char *name;
};

#define DEMO	0

#define ARRAY_LENGTH(x)		(sizeof(x) / sizeof(x[0]))

#define	WM_ICON_NOTIFY WM_APP+10

#define TIMER_HALFHOUR			1800000

#define APP_DIR    "\\JoyTray"
#define INI_FILE   "\\JoyTray.ini"

//#define VK_4		0x34
//#define VK_8		0x38

int m_joystickIndex = 0;
HINSTANCE m_hInstance;
HWND m_hWnd;
CSystemTray m_TrayIcon;
CInterComm m_InterComm;
CCmdLine m_CmdLine;
CHAR m_appDirectory[MAX_PATH+1] = { 0 };
CHAR m_iniFileName[MAX_PATH+1] = { 0 };

U360JoyData m_U360JoyData[] =
{
	{ IDM_U360_MOUSE,		"mouse" },
	{ IDM_U360_ANALOG,		"analog" },
	{ IDM_U360_JOY8WAY,		"joy8way" },
	{ IDM_U360_EASYJOY8WAY,	"easyjoy8way" },
	{ IDM_U360_JOY4WAY,		"joy4way" },
	{ IDM_U360_DJOY4WAY,	"djoy4way" },
	{ IDM_U360_RDJOY4WAY,	"rdjoy4way" },
	{ IDM_U360_JOY2WAY,		"joy2way" },
	{ IDM_U360_VJOY2WAY,	"vjoy2way" }
};

const char *m_commandHelp = 
	"JoyTray v1.0 - By Ben Baker\n\n\
USAGE: JoyTray.exe -options\n\n\
options:\n\
  -exit             Exit program\n\
  -help             Displays this Message Box\n\
  -u360 [mouse|analog|joy8way|easyjoy8way|joy4way\n\
         djoy4way|rdjoy4way|joy2way|vjoy2way]\n\
  -servo [joy4way|joy8way]";

PCHAR* CommandLineToArgvA(PCHAR CmdLine, int* _argc);
VOID GetApplicationDir(PCHAR path);
BOOL FileExists(PCHAR fileName);
UINT MessageLoop(HWND hWnd);
DWORD WINAPI MainThread(LPVOID lpParam);
void StartMainThread();
void KillMainThread();
BOOL CALLBACK MainProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK AboutProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK InputProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void FillComboBox(HWND hWnd, int nIDDlgItem);
void SelectComboBoxItem(HWND hWnd, int nIDDlgItem, PCHAR item);
void SelectComboBoxIndex(HWND hWnd, int nIDDlgItem, int index);
void OpenFile();
void SaveFile();
BOOL ProcessCmdLine(LPSTR lpCmdLine);
void AddToStartup();
void RemoveFromStartup();
void LoadMap(CONST CHAR *mapFile);
BOOL ProcessU360(int id);
BOOL ProcessServo(int id);

PCHAR* CommandLineToArgvA(PCHAR CmdLine, int* _argc)
{
    PCHAR* argv;
    PCHAR _argv;
    ULONG len;
    ULONG argc;
    CHAR a;
    ULONG i, j;

    BOOLEAN in_QM;
    BOOLEAN in_TEXT;
    BOOLEAN in_SPACE;

    len = strlen(CmdLine);
    i = ((len+2)/2)*sizeof(PVOID) + sizeof(PVOID);

    argv = (PCHAR*)GlobalAlloc(GMEM_FIXED, i + (len+2)*sizeof(CHAR));
    _argv = (PCHAR)(((PUCHAR)argv)+i);

    argc = 0;
    argv[argc] = _argv;
    in_QM = FALSE;
    in_TEXT = FALSE;
    in_SPACE = TRUE;
    i = 0;
    j = 0;

    while(a = CmdLine[i]) {
        if(in_QM) {
            if(a == '\"') {
                in_QM = FALSE;
            } else {
                _argv[j] = a;
                j++;
            }
        } else {
            switch(a) {
            case '\"':
                in_QM = TRUE;
                in_TEXT = TRUE;
                if(in_SPACE) {
                    argv[argc] = _argv+j;
                    argc++;
                }
                in_SPACE = FALSE;
                break;
            case ' ':
            case '\t':
            case '\n':
            case '\r':
                if(in_TEXT) {
                    _argv[j] = '\0';
                    j++;
                }
                in_TEXT = FALSE;
                in_SPACE = TRUE;
                break;
            default:
                in_TEXT = TRUE;
                if(in_SPACE) {
                    argv[argc] = _argv+j;
                    argc++;
                }
                _argv[j] = a;
                j++;
                in_SPACE = FALSE;
                break;
            }
        }
        i++;
    }
    _argv[j] = '\0';
    argv[argc] = NULL;

    (*_argc) = argc;
    return argv;
}


VOID GetApplicationDir(PCHAR path)
{
   CHAR szPath[MAX_PATH+1];
   SHGetFolderPath(nullptr, CSIDL_LOCAL_APPDATA, nullptr, SHGFP_TYPE_CURRENT, szPath);
   strncat(szPath, APP_DIR, MAX_PATH-1);
   CreateDirectory(szPath, NULL);
   strncpy(path, szPath, MAX_PATH-1);
}

BOOL FileExists(PCHAR fileName)
{ 
	struct stat stFileInfo;

	if(stat(fileName ,&stFileInfo) == 0)
		return TRUE;

	return FALSE;
}

int _stdcall MessageReceived(DataMode id, PCHAR data)
{
    switch (id)
    {
    case MODE_CMDLINE:
		ProcessCmdLine(data);
        break;
    }

	return 1;
}

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	m_hInstance = hInstance;

	m_InterComm.Initialize(hInstance, MessageReceived);

    if (m_InterComm.CommMode() == MODE_CLIENT)
    {
		m_InterComm.SendMessage(MODE_CMDLINE, lpCmdLine);

		return 1;
    }

	GetApplicationDir(m_appDirectory);
	strncpy(m_iniFileName, m_appDirectory, MAX_PATH-1);
	strncat(m_iniFileName, INI_FILE, MAX_PATH-1);

	m_hWnd = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_MAIN), NULL, MainProc);

#if DEMO
	SetTimer(m_hWnd, 1, TIMER_HALFHOUR, NULL);
#endif

	SendMessage(m_hWnd, WM_SETICON, ICON_BIG, (LPARAM) LoadIcon(hInstance, MAKEINTRESOURCE(IDI_JOYTRAY)));
	SendMessage(m_hWnd, WM_SETICON, ICON_SMALL, (LPARAM) LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SMALL)));

	ReadSettings(m_iniFileName);
	ProcessCmdLine(lpCmdLine);

	g_U360JoyCount = UltraStik_Initialize();

	if(g_U360JoyCount == 0)
		m_TrayIcon.GrayMenuPosition(0, TRUE);

	g_PacDriveCount = PacInitialize();
	g_ServoJoyCount = 0;

	for(int i=0; i < g_PacDriveCount; i++)
		if(PacGetDeviceType(i) == DEVICETYPE_SERVOSTIK)
			g_ServoJoyCount++;

	if(g_ServoJoyCount == 0)
		m_TrayIcon.GrayMenuPosition(1, TRUE);

	SendDlgItemMessage(m_hWnd, IDC_LAUNCH, BM_SETCHECK, g_LaunchOnStartup, 0);
	SendDlgItemMessage(m_hWnd, IDC_MOUSEDRAG, BM_SETCHECK, g_MouseDrag, 0);
	SendDlgItemMessage(m_hWnd, IDC_REVERSEMOUSE, BM_SETCHECK, g_ReverseMouse, 0);

	//RegisterHotKey(m_hWnd, 0, MOD_ALT, VK_4);
	//RegisterHotKey(m_hWnd, 1, MOD_ALT, VK_8);

	RegisterHotKey(m_hWnd, 0, 0, VK_F4);
	RegisterHotKey(m_hWnd, 1, 0, VK_F8);

	if (!m_TrayIcon.Create(hInstance, m_hWnd, WM_ICON_NOTIFY, _T("JoyTray"), ::LoadIcon(hInstance, (LPCTSTR)IDI_SMALL), IDC_JOYTRAY))
        return FALSE;

	//StartHookThread();

	//InitializeMouse(hInstance, NULL);
	//StartMouseThread();

	for(int i=0; i<ARRAY_LENGTH(m_U360JoyData); i++)
	{
		if(strcmp(m_U360JoyData[i].name, g_U360MapFile) == 0)
		{
			ProcessU360(m_U360JoyData[i].id);

			break;
		}
	}

	ProcessServo(strcmp("joy4way", g_ServoMapFile) == 0 ? IDM_SERVO_JOY4WAY : IDM_SERVO_JOY8WAY);

	int retVal = MessageLoop(m_hWnd);

	//KillHookThread();

	//ShutdownMouse();
	//KillMouseThread();

	UltraStik_Shutdown();

	PacShutdown();

	WriteSettings(m_iniFileName);

	return retVal;
}

UINT MessageLoop(HWND hWnd)
{
	MSG msg;

	while(GetMessage(&msg, NULL, 0, 0))
    {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
    }

	DestroyWindow(hWnd);

	return msg.wParam;
}

BOOL CALLBACK MainProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	HWND hWndDialog;

	switch (message)
	{
    case WM_ICON_NOTIFY:
        return m_TrayIcon.OnTrayNotification(wParam, lParam);
	
	case WM_TIMER:
		PostQuitMessage(0);
		return TRUE;

	case WM_CLOSE:
		PostQuitMessage(0);
		return TRUE;

	case WM_QUERYENDSESSION:
		WriteSettings(m_iniFileName);
		return FALSE;

	case WM_SYSCOMMAND:
		switch(wParam)
		{
		case SC_CLOSE:
			if(MessageBox(hWnd, "Are You Sure?", "Exit JoyTray", MB_OKCANCEL) == IDOK)
				return FALSE;
			else
				return TRUE;
		case SC_MINIMIZE:
			ShowWindow(hWnd, SW_HIDE);
			return TRUE;
		}
		break;
		
	case WM_HOTKEY:
		switch(wParam)
		{
		case 0: // 4 Way
			ProcessServo(IDM_SERVO_JOY4WAY);
			return TRUE;
		case 1: // 8 Way
			ProcessServo(IDM_SERVO_JOY8WAY);
			return TRUE;

		}
		return TRUE;

	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		hWndDialog = (HWND) lParam;

		if(ProcessU360(wmId))
			return TRUE;

		if(ProcessServo(wmId))
			return TRUE;

		switch (wmId)
		{
		case IDC_LAUNCH:
			if(wmEvent == BN_CLICKED)
			{
				g_LaunchOnStartup = SendMessage(hWndDialog, BM_GETCHECK, 0, 0);

				if(g_LaunchOnStartup)
					AddToStartup();
				else
					RemoveFromStartup();
			}
			return TRUE;
		case IDC_MOUSEDRAG:
			if(wmEvent == BN_CLICKED)
			{
				g_MouseDrag = SendMessage(hWndDialog, BM_GETCHECK, 0, 0);
			}
			return TRUE;
		case IDC_REVERSEMOUSE:
			if(wmEvent == BN_CLICKED)
			{
				g_ReverseMouse = SendMessage(hWndDialog, BM_GETCHECK, 0, 0);
			}
			return TRUE;
		case IDB_OK:
		case IDB_CLOSE:
			if(wmEvent == BN_CLICKED)
				ShowWindow(hWnd, SW_HIDE);
			return TRUE;
		case IDM_SETTINGS:
			ShowWindow(hWnd, SW_SHOW);
			UpdateWindow(hWnd);
			return TRUE;
		case IDM_ABOUT:
			DialogBox(m_hInstance, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)AboutProc);
			return TRUE;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			return TRUE;
		default:
			return FALSE;
		}

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return FALSE;
	}

	return FALSE;
}


LRESULT CALLBACK AboutProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}

	return FALSE;
}

BOOL CALLBACK InputProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static int buttonIndex = 0;

	switch (message)
	{
	case WM_INITDIALOG:
		buttonIndex = lParam;
		return TRUE;

	case WM_HOTKEY:
		return TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}

	return FALSE;
}

void FillComboBox(HWND hWnd, int nIDDlgItem)
{
	HWND hWndDialog = GetDlgItem(hWnd, nIDDlgItem);

	//SendMessage(hWndDialog, CB_SETDROPPEDWIDTH, 64, 0);
	//SendMessage(hWndDialog, CB_ADDSTRING, 0, (LPARAM)"");

	for (int i = 0; i < ARRAY_LENGTH(key_codes); i++)
		SendMessage(hWndDialog, CB_ADDSTRING, 0, (LPARAM)key_codes[i].Name);
}

void SelectComboBoxItem(HWND hWnd, int nIDDlgItem, PCHAR item)
{
	HWND hWndDialog = GetDlgItem(hWnd, nIDDlgItem);

	SelectComboBoxIndex(hWnd, nIDDlgItem, SendMessage(hWndDialog, CB_FINDSTRINGEXACT, -1, (LPARAM)item));
}

void SelectComboBoxIndex(HWND hWnd, int nIDDlgItem, int index)
{
	HWND hWndDialog = GetDlgItem(hWnd, nIDDlgItem);

	SendMessage(hWndDialog, CB_SETCURSEL, index, 0);
}

void OpenFile()
{
	OPENFILENAME ofn;

	memset(&ofn, 0, sizeof(ofn));

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = m_hWnd;
	ofn.hInstance = m_hInstance;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	ofn.lpstrInitialDir = m_appDirectory;
	ofn.lpstrFile = m_iniFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.nFilterIndex = 2;
	ofn.lpstrFilter = "All Files\0*.*\0Ini Files\0*.ini\0";

	if(GetOpenFileName(&ofn))
		ReadSettings(m_iniFileName);
}

void SaveFile()
{
	OPENFILENAME ofn;

	memset(&ofn, 0, sizeof(ofn));

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = m_hWnd;
	ofn.hInstance = m_hInstance;
	ofn.Flags = 0;
	ofn.lpstrInitialDir = m_appDirectory;
	ofn.lpstrFile = m_iniFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.nFilterIndex = 2;
	ofn.lpstrFilter = "All Files\0*.*\0Ini Files\0*.ini\0";

	if(GetSaveFileName(&ofn))
		WriteSettings(m_iniFileName);
}

BOOL ProcessCmdLine(LPSTR lpCmdLine)
{
	int argc;
	PCHAR *argv;

	argv = CommandLineToArgvA(lpCmdLine, &argc);

	if(m_CmdLine.SplitLine(argc, argv) < 1)
		goto exit;

	if (m_CmdLine.HasSwitch("-help"))
	{
		MessageBox(NULL, m_commandHelp, "Help", MB_OK);
		goto exit;
	}

	if(m_CmdLine.HasSwitch("-u360"))
	{
		string arg = m_CmdLine.GetSafeArgument("-u360", 0, "mouse");

		LoadMap(arg.c_str());

		for(int i=0; i<ARRAY_LENGTH(m_U360JoyData); i++)
		{
			m_TrayIcon.CheckMenuItem(m_U360JoyData[i].id, FALSE);

			if(strcmp(m_U360JoyData[i].name, arg.c_str()) == 0)
				m_TrayIcon.CheckMenuItem(m_U360JoyData[i].id, TRUE);
		}
	}

	if(m_CmdLine.HasSwitch("-servo"))
	{
		string arg = m_CmdLine.GetSafeArgument("-servo", 0, "joy4way");

		ProcessServo(strcmp("joy4way", arg.c_str()) == 0 ? IDM_SERVO_JOY4WAY : IDM_SERVO_JOY8WAY);
	}

	if(m_CmdLine.HasSwitch("-exit"))
	{
		SendMessage(m_hWnd, WM_CLOSE, 0, 0);
	}

exit:

	for(int i=0; i<argc; i++)
		GlobalFree(argv[i]);

	return TRUE;
}

void AddToStartup()
{
	HKEY hKey;
	CHAR szPath[MAX_PATH+1];
	GetModuleFileName(NULL, szPath, MAX_PATH);
	DWORD cbData = strlen(szPath) + 1;

	if(RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS)
	{
		RegSetValueEx(hKey, "JoyTray", 0, REG_SZ, (PUCHAR) szPath, cbData);
		RegCloseKey(hKey);
	}
}

void RemoveFromStartup()
{
	HKEY hKey;

    if(RegOpenKey(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", &hKey) == ERROR_SUCCESS)
	{
		RegDeleteValue(hKey, "JoyTray");
	    RegCloseKey(hKey);
	}
}

void LoadMap(CONST CHAR *mapFile)
{
	int i;

	strcpy(g_U360MapFile, mapFile);

	for(i=0; i<g_U360JoyCount; i++)
		UltraStik_LoadMap(i, g_U360MapFile);

	if(strcmp(g_U360MapFile, "mouse") == 0 && g_U360JoyCount > 0)
		m_TrayIcon.SetIcon(::LoadIcon(m_hInstance, (LPCTSTR)IDI_MOUSE));
	else
		m_TrayIcon.SetIcon(::LoadIcon(m_hInstance, (LPCTSTR)IDI_JOYSTICK));
}

BOOL ProcessU360(int id)
{
	BOOL itemFound = FALSE;

	for(int i=0; i<ARRAY_LENGTH(m_U360JoyData); i++)
		if(m_U360JoyData[i].id == id)
			itemFound = TRUE;

	if(!itemFound)
		return FALSE;

	for(int i=0; i<ARRAY_LENGTH(m_U360JoyData); i++)
	{
		m_TrayIcon.CheckMenuItem(m_U360JoyData[i].id, FALSE);

		if(m_U360JoyData[i].id == id)
		{
			LoadMap(m_U360JoyData[i].name);
			m_TrayIcon.CheckMenuItem(m_U360JoyData[i].id, TRUE);
		}
	}

	return TRUE;
}

BOOL ProcessServo(int id)
{
	switch(id)
	{
	case IDM_SERVO_JOY4WAY:
		{
			strcpy(g_ServoMapFile, "joy4way");
			m_TrayIcon.CheckMenuItem(IDM_SERVO_JOY4WAY, TRUE);
			m_TrayIcon.CheckMenuItem(IDM_SERVO_JOY8WAY, FALSE);

			PacSetServoStik4Way();
			return TRUE;
		}
	case IDM_SERVO_JOY8WAY:
		{
			strcpy(g_ServoMapFile, "joy8way");
			m_TrayIcon.CheckMenuItem(IDM_SERVO_JOY4WAY, FALSE);
			m_TrayIcon.CheckMenuItem(IDM_SERVO_JOY8WAY, TRUE);

			PacSetServoStik8Way();
			return TRUE;
		}
	}

	return FALSE;
}

