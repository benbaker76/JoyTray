#define WH_KEYBOARD_LL 13

#define MOUSE_NONE			0
#define MOUSE_LBUTTONDOWN	1
#define MOUSE_LBUTTONUP		2
#define MOUSE_RBUTTONDOWN	3
#define MOUSE_RBUTTONUP		4
#define MOUSE_MBUTTONDOWN	5
#define MOUSE_MBUTTONUP		6

typedef struct
{
	BYTE Keys[256];
	UINT Key;
	BOOL Alt;
	BOOL Control;
	BOOL Shift;
	BOOL CapsLock;
	CHAR Char;
	BOOL KeyDown;
	BOOL Handled;
	BOOL ReMap;
} KEYEVENTARGS, *PKEYEVENTARGS;

typedef struct {
	INT Button;
	INT ClickCount;
	INT X;
	INT Y;
	SHORT MouseDelta;
	BOOL Handled;
} MOUSEEVENTARGS, *PMOUSEEVENTARGS;

void ProcessMouseClickLock(PMOUSEEVENTARGS e);
