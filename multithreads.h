#include <windows.h>
#include <vector>

#define THREADS_NUMBER 10
#define ITERATIONS_NUMBER 2 //100
#define PAUSE 10 /* ms */

class MultiThreads
{
public:
    static int initThreads();
	static DWORD ThreadProc(CONST LPVOID lpParam);
	static VOID Error(CONST HANDLE hStdOut, CONST LPCWSTR szMessage);
};