#include "ftpuserdata.h"

#include <iostream>
#include <sstream>

wstring FTPUserData::s2ws(const string& s) {
    int len;
    int slength = (int)s.length() + 1;
    len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0); 
    wchar_t* buf = new wchar_t[len];
    MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
    std::wstring r(buf);
    delete[] buf;
    return r;
}

std::vector<std::string> &FTPUserData::split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


std::vector<std::string> FTPUserData::split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

FTPUserData::FTPUserData() {
	this->host = "";
	this->login = "";
	this->password = "";
}

FTPUserData::FTPUserData(string host, string login, string password) {
	this->host = host;
	this->login = login;
	this->password = password;
}

FTPUserData::FTPUserData(string str) {
	std::vector<std::string> splited;
	split(str, ';', splited);
	if (splited.size() > 0) {
	this->host = splited[0];
	} else { this->host = ""; }
	if (splited.size() > 1) {
	this->login = splited[1];
	} else { this->login = ""; }
	if (splited.size() > 2) {
	this->password = splited[2];
	} else { this->password = ""; }
}

void FTPUserData::print() {
	std::cout << host << " " << login << " " << password << std::endl;
}

string FTPUserData::getHost() {
	return host;
}

string FTPUserData::getLogin() {
	return login;
}

string FTPUserData::getPassword() {
	return password;
}

string FTPUserData::getString() {
	return host + ";" + login + ";" + password;
}