// PacDrive.h header file.
#ifdef PACDRIVE_EXPORTS
#define PACDRIVE_API __declspec(dllexport)
#else
#define PACDRIVE_API __declspec(dllimport)
#endif

#define VENDOR_ID		0xd209	// Ultimarc Vendor Id
#define PAC_UHID_ID		0x1500	// Both PacDrive and U-HID have a PID of 0x15xx
#define PAC64_ID		0x1400	// Pac64 has a PID of 0x14xx
#define SERVOSTIK_ID	0x1700	// ServoStik has a PID of 0x17xx
#define PID_PACDRIVE	0x1500	// PacDrive PID (PacDrive uses it's Version Number to id it's different devices)
#define PID_PACD64_LO	0x1401	// Start of Pac64 PID
#define PID_PACD64_HI	0x1408	// Start of Pac64 PID
#define PID_UHID_LO		0x1501	// Start of U-HID PID
#define PID_UHID_HI		0x1508  // End of U-HID PID
#define PID_SERVOSTIK	0x1700  // ServoStik PID

#define WINDOW_NAME		"PacDrive.dll Event Window"
#define WINDOW_CLASS	"PacDrive.dll Event Class"

static GUID GUID_DEVINTERFACE_HID =
{ 0x4D1E55B2L, 0xF16F, 0x11CF, { 0x88, 0xCB, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30 } };

typedef struct
{
	UCHAR ReportID;
	UCHAR ReportBuffer[4];
} REPORT_BUF;

enum DeviceType
{
	DEVICETYPE_UNKNOWN,
	DEVICETYPE_PACDRIVE,
	DEVICETYPE_UHID,
	DEVICETYPE_PAC64,
	DEVICETYPE_SERVOSTIK
};

typedef struct
{
	INT Type;
	HANDLE hDevice;
	USHORT VendorID;
	USHORT ProductID;
	USHORT VersionNumber;
	CHAR VendorName[256];
	CHAR ProductName[256];
	CHAR SerialNumber[256];
	CHAR DevicePath[256];
} HID_DEVICE_DATA;

typedef VOID (__stdcall *PAC_ATTACHED_CALLBACK)(INT id);
typedef VOID (__stdcall *PAC_REMOVED_CALLBACK)(INT id);

PACDRIVE_API VOID __stdcall PacSetCallbacks(PAC_ATTACHED_CALLBACK pacAttachedCallback, PAC_REMOVED_CALLBACK pacRemovedCallback);

PACDRIVE_API INT __stdcall PacInitialize();
PACDRIVE_API VOID __stdcall PacShutdown();

PACDRIVE_API BOOL __stdcall PacSetLEDStates(INT id, USHORT data);
PACDRIVE_API BOOL __stdcall PacSetLEDState(INT id, INT port, BOOL state);

PACDRIVE_API BOOL __stdcall Pac64SetLEDStates(INT id, INT group, BYTE data);
PACDRIVE_API BOOL __stdcall Pac64SetLEDState(INT id, INT group, INT port, BOOL state);
PACDRIVE_API BOOL __stdcall Pac64SetLEDStatesRandom(INT id);
PACDRIVE_API BOOL __stdcall Pac64SetLEDIntensities(INT id, PBYTE data);
PACDRIVE_API BOOL __stdcall Pac64SetLEDIntensity(INT id, INT port, BYTE intensity);
PACDRIVE_API BOOL __stdcall Pac64SetLEDFadeTime(INT id, BYTE fadeTime);
PACDRIVE_API BOOL __stdcall Pac64SetLEDFlashSpeeds(INT id, BYTE flashSpeed);
PACDRIVE_API BOOL __stdcall Pac64SetLEDFlashSpeed(INT id, INT port, BYTE flashSpeed);
PACDRIVE_API BOOL __stdcall Pac64StartScriptRecording(INT id);
PACDRIVE_API BOOL __stdcall Pac64StopScriptRecording(INT id);
PACDRIVE_API BOOL __stdcall Pac64SetScriptStepDelay(INT id, BYTE stepDelay);
PACDRIVE_API BOOL __stdcall Pac64RunScript(INT id);
PACDRIVE_API BOOL __stdcall Pac64ClearFlash(INT id);
PACDRIVE_API BOOL __stdcall Pac64SetDeviceId(INT id, INT newId);
PACDRIVE_API BOOL __stdcall Pac64UpdateFirmware(INT id);

PACDRIVE_API INT __stdcall PacGetDeviceType(INT id);
PACDRIVE_API INT __stdcall PacGetVendorId(INT id);
PACDRIVE_API INT __stdcall PacGetProductId(INT id);
PACDRIVE_API INT __stdcall PacGetVersionNumber(INT id);
PACDRIVE_API VOID __stdcall PacGetVendorName(INT id, PCHAR sVendorName);
PACDRIVE_API VOID __stdcall PacGetProductName(INT id, PCHAR sProductName);
PACDRIVE_API VOID __stdcall PacGetSerialNumber(INT id, PCHAR sSerialNumber);
PACDRIVE_API VOID __stdcall PacGetDevicePath(INT id, PCHAR sDevicePath);

PACDRIVE_API BOOL __stdcall PacProgramUHid(INT id, PCHAR sFilePath);

PACDRIVE_API BOOL __stdcall PacSetServoStik4Way();
PACDRIVE_API BOOL __stdcall PacSetServoStik8Way();

BOOL PacOpen(LPCSTR devicePath, HID_DEVICE_DATA *pDeviceData);
BOOL PacRead(INT id, REPORT_BUF *pInputReport, INT size);
BOOL PacWrite(INT id, REPORT_BUF *pOutputReport, INT size);

DWORD WINAPI EventWindowThread(LPVOID lpParam);
BOOL RegisterDeviceInterface(HWND hWnd);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

void strlow(char *s);
