#pragma once

#include <windows.h>
#include <string>
#include <vector>

#include <iostream>
using namespace std;

#include "ftpuserdata.h"

class HttpRequest
{
public:    
    std::wstring responseHeader;
    std::vector<BYTE> responseBody;
	HttpRequest(const std::wstring&, const std::wstring&, const std::wstring&);
    bool SendRequest(vector<FTPUserData> *vData, const std::wstring &url, const std::wstring &req);	
	bool SendPostRequest(string good, const std::wstring &url, const std::wstring &req);
private:
    std::wstring _userAgent;
    //std::wstring _proxyIp;
    //std::wstring _proxyPort;
};