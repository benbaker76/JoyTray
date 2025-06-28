#include "stdafx.h"
#include <stdio.h>
#include "Globals.h"
#include "KeyCodes.h"

#define ARRAY_LENGTH(x)		(sizeof(x) / sizeof(x[0]))

BOOL IniOptionGet(PCHAR fileName, PCHAR Buffer, PCHAR Section, PCHAR Key, PCHAR Default, PCHAR Value)
{
	if(GetPrivateProfileString(Section, Key, Default, Buffer, MAX_PATH, fileName) != 0)
		if(Value == NULL || stricmp(Buffer, Value) == 0)
			return TRUE;

	return FALSE;
}

BOOL IniOptionSet(PCHAR fileName, PCHAR Section, PCHAR Key, PCHAR Value)
{
	if(WritePrivateProfileString(Section, Key, Value, fileName) != 0)
		return TRUE;

	return FALSE;
}

void ReadSettings(PCHAR szPath)
{
	CHAR buf[MAX_PATH+1];

	g_LaunchOnStartup = IniOptionGet(szPath, buf, "General", "LaunchOnStartup", "False", "True");
	g_MouseDrag = IniOptionGet(szPath, buf, "General", "MouseDrag", "False", "True");
	g_ReverseMouse = IniOptionGet(szPath, buf, "General", "ReverseMouse", "False", "True");
	IniOptionGet(szPath, g_U360MapFile, "General", "U360MapFile", "mouse", NULL);
	IniOptionGet(szPath, g_ServoMapFile, "General", "ServoMapFile", "joy4way", NULL);
}

void WriteSettings(PCHAR szPath)
{
	IniOptionSet(szPath, "General", "LaunchOnStartup", g_LaunchOnStartup ? "True" : "False");
	IniOptionSet(szPath, "General", "MouseDrag", g_MouseDrag ? "True" : "False");
	IniOptionSet(szPath, "General", "ReverseMouse", g_ReverseMouse ? "True" : "False");
	IniOptionSet(szPath, "General", "U360MapFile", g_U360MapFile);
	IniOptionSet(szPath, "General", "ServoMapFile", g_ServoMapFile);
}
