#include "ftpconnect.h"

#include <wininet.h>
#pragma comment(lib, "wininet")

#include <iostream>
using namespace std;

FTPConnect::FTPConnect(vector<FTPUserData> *vData) {
	this->vData = vData;
	good = "";
}

bool FTPConnect::connect() {
	HINTERNET hInternet;
    HINTERNET hFtpSession;
    hInternet = InternetOpen(NULL,INTERNET_OPEN_TYPE_DIRECT,NULL,NULL,0);

	bool result = false;

	FTPUserData data = (*vData)[vData->size() - 1];

	wstring host = FTPUserData::s2ws(data.getHost());
	wstring login = FTPUserData::s2ws(data.getLogin());
	wstring password = FTPUserData::s2ws(data.getPassword());
	
	wcout << "host: " << host.c_str() << endl;
	wcout << "login: " << login.c_str() << endl;
	wcout << "password: " << password.c_str() << endl;

	vData->pop_back();
	//while (!isEmpty()) {
		//hFtpSession = InternetConnect(hInternet, TEXT("hostde1.fornex.org"), INTERNET_DEFAULT_FTP_PORT, TEXT("p24546"), TEXT("QFmSGukH"), INTERNET_SERVICE_FTP, 0, 0);
		hFtpSession = InternetConnect(hInternet, host.c_str(), INTERNET_DEFAULT_FTP_PORT, login.c_str(), password.c_str(), INTERNET_SERVICE_FTP, 0, 0);
		if (hFtpSession == NULL) {
			cout <<"FTP session: ERROR" << endl;
			cout << "Error: " << GetLastError() << endl;
		} else {
			result = true;
			good = data.getString();
			cout << "FTP session: SUCCESS" << endl;			
		}

		
	//}
 
    InternetCloseHandle(hFtpSession);
    InternetCloseHandle(hInternet);
	return result;
}

void FTPConnect::setVData(vector<FTPUserData> *vData) {
	this->vData = vData;
}

bool FTPConnect::isEmpty() {
	return (0 == vData->size());
}

string FTPConnect::getGood() {
	return good;
}