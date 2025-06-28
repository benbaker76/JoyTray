int InitializeMouse(HINSTANCE hInstance, HWND hWnd);
void ShutdownMouse();
void ReverseMouse();
DWORD WINAPI MouseThread(LPVOID lpParam);
void StartMouseThread();
void KillMouseThread();
