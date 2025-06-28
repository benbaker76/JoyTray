// UltraStik.h header file.
#ifdef ULTRASTIK_EXPORTS
#define ULTRASTIK_API __declspec(dllexport)
#else
#define ULTRASTIK_API __declspec(dllimport)
#endif

ULTRASTIK_API int __stdcall UltraStik_Initialize();
ULTRASTIK_API void __stdcall UltraStik_Shutdown();
ULTRASTIK_API int __stdcall UltraStik_GetVendorId(int id);
ULTRASTIK_API int __stdcall UltraStik_GetProductId(int id);
ULTRASTIK_API void __stdcall UltraStik_GetManufacturer(int id, PCHAR sManufacturer);
ULTRASTIK_API void __stdcall UltraStik_GetProduct(int id, PCHAR sProduct);
ULTRASTIK_API void __stdcall UltraStik_GetSerialNumber(int id, PCHAR sSerialNumber);
ULTRASTIK_API int __stdcall UltraStik_GetFirmwareVersion(int id);
ULTRASTIK_API void __stdcall UltraStik_SetRestrictor(int id, BYTE value);
ULTRASTIK_API void __stdcall UltraStik_SetFlash(int id, bool value);
ULTRASTIK_API int __stdcall UltraStik_GetUltraStikId(int id);
ULTRASTIK_API void __stdcall UltraStik_SetUltraStikId(int id, int value);
ULTRASTIK_API int __stdcall UltraStik_LoadMap(int id, PCHAR map);
ULTRASTIK_API int __stdcall UltraStik_LoadMapFile(int id, PCHAR fileName);
ULTRASTIK_API int __stdcall UltraStik_LoadConfigFile(PCHAR mapPath, PCHAR fileName);
