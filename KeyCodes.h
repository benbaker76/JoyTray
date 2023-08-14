#include "StdAfx.h"
#include <dinput.h>

struct KeyCode
{
	PCHAR Name;
	PCHAR NameCode;
	int DIKCode;
	int VKCode;
	int ASCIICode;
};

extern const KeyCode key_codes[121];

int VK2DIK(int vk);
PCHAR VK2Name(int vk);
PCHAR VK2NameCode(int vk);
int DIK2VK(int dik);
int NameCode2VK(PCHAR name);
int NameCode2DIK(PCHAR name);
int NameCode2Joy(PCHAR name);
