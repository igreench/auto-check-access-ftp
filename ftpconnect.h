#pragma once

#include <windows.h>

#include <vector>

#include "ftpuserdata.h"

class FTPConnect
{
public:
	FTPConnect(vector<FTPUserData> *vData);
    bool connect();
	void setVData(vector<FTPUserData> *vData);
	bool isEmpty();
	string getGood();
private:
	vector<FTPUserData> *vData;
	string good;
};