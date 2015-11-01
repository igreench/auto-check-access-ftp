#pragma once

#include <windows.h>
#include <string>
#include <vector>
using namespace std;

class FTPUserData {
public:
	FTPUserData();
	FTPUserData(string host, string login, string password);
	FTPUserData(string str);
	static vector<string> &split(const string &s, char delim, vector<string> &elems);
	static vector<string> split(const string &s, char delim);
	static wstring s2ws(const string& s);
	void print();
	string getHost();
	string getLogin();
	string getPassword();
	string getString();
private:
	string host;
	string login;
	string password;
};