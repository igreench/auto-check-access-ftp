#include "multithreads.h"
#include "ftpconnect.h"

DWORD dwCounter = 0;

int WINAPI MultiThreads::initThreads() {
	TCHAR szMessage[256];
	DWORD dwTemp, i;
	HANDLE hThreads[THREADS_NUMBER];
	CONST HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	CONST HANDLE hMutex = CreateMutex(NULL, FALSE, NULL);
	if(NULL == hMutex) {
		Error(hStdOut, TEXT("Failed to create mutex.\r\n"));
	}

	for(i = 0; i < THREADS_NUMBER; i++) {
		hThreads[i] = CreateThread(NULL, 0, &ThreadProc, hMutex, 0, NULL);
		if(NULL == hThreads[i]) {
			Error(hStdOut, TEXT("Failed to create thread.\r\n"));
		}
	}

	WaitForMultipleObjects(THREADS_NUMBER, hThreads, TRUE, INFINITE);
	wsprintf(szMessage, TEXT("Counter = %d\r\n"), dwCounter);
	WriteConsole(hStdOut, szMessage, lstrlen(szMessage), &dwTemp, NULL);

	for(i = 0; i < THREADS_NUMBER; i++) {
		CloseHandle(hThreads[i]);
	}
	CloseHandle(hMutex);
}

DWORD WINAPI MultiThreads::ThreadProc(CONST LPVOID lpParam) {
	CONST HANDLE hMutex = (CONST HANDLE)lpParam;
	DWORD i;
	for(i = 0; i < ITERATIONS_NUMBER; i++) {
		FTPConnect::connect();
		WaitForSingleObject(hMutex, INFINITE);
		dwCounter++;
		ReleaseMutex(hMutex);
		Sleep(PAUSE);
	}
	ExitThread(0);
}

VOID MultiThreads::Error(CONST HANDLE hStdOut, CONST LPCWSTR szMessage) {
	DWORD dwTemp;
	TCHAR szError[256];
	WriteConsole(hStdOut, szMessage, lstrlen(szMessage), &dwTemp, NULL);
	wsprintf(szError, TEXT("LastError = %d\r\n"), GetLastError());
	WriteConsole(hStdOut, szError, lstrlen(szError), &dwTemp, NULL);
	ExitProcess(0);
}