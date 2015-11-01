#include "httprequest.h"

#include <winhttp.h>
#pragma comment(lib, "winhttp.lib")

HttpRequest::HttpRequest(const std::wstring &userAgent, const std::wstring &proxyIp, const std::wstring &proxyPort) :
    _userAgent(userAgent)
    //,_proxyIp(proxyIp)
    //,_proxyPort(proxyPort)
{
}

bool HttpRequest::SendRequest(vector<FTPUserData> *vData, const std::wstring &url, const std::wstring &req) {
  DWORD dwSize = 0;
  DWORD dwDownloaded = 0;
  LPSTR pszOutBuffer;
  BOOL  bResults = FALSE;
  HINTERNET  hSession = NULL, 
             hConnect = NULL,
             hRequest = NULL;

  std::wstring method = L"GET";
  void *body = NULL;
  DWORD bodySize = 0;
  std::string str = "";

  cout << "getting list" << endl;

  // Use WinHttpOpen to obtain a session handle.
  hSession = WinHttpOpen( _userAgent.c_str(),  
                          WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                          WINHTTP_NO_PROXY_NAME, 
                          WINHTTP_NO_PROXY_BYPASS, 0 );

  // Specify an HTTP server.
  if( hSession )
    hConnect = WinHttpConnect( hSession, url.c_str(),
                               INTERNET_DEFAULT_HTTP_PORT, 0 );

  // Create an HTTP request handle.
  if( hConnect )
      hRequest = WinHttpOpenRequest( hConnect, method.c_str(), req.c_str(),
                                   NULL, WINHTTP_NO_REFERER, 
                                   WINHTTP_DEFAULT_ACCEPT_TYPES, 
                                   0 );

  // Send a request.
  if( hRequest )
    bResults = WinHttpSendRequest( hRequest,
                                   WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                                   WINHTTP_NO_REQUEST_DATA, 0, 
                                   0, 0 );

  // End the request.
  if( bResults )
    bResults = WinHttpReceiveResponse( hRequest, NULL );

  // Keep checking for data until there is nothing left.
  if( bResults )
  {
    do 
    {
      // Check for available data.
      dwSize = 0;
      if( !WinHttpQueryDataAvailable( hRequest, &dwSize ) ) {
		  cout << "Error " << GetLastError( ) << " in WinHttpQueryDataAvailable." << endl;
	  }

      // Allocate space for the buffer.
      pszOutBuffer = new char[dwSize+1];
      if( !pszOutBuffer ) {
		cout << "Out of memory" << endl;
        dwSize=0;
      }
      else
      {
        // Read the data.
        ZeroMemory( pszOutBuffer, dwSize+1 );

        if( !WinHttpReadData( hRequest, (LPVOID)pszOutBuffer, 
                              dwSize, &dwDownloaded ) ) {
		  cout << "Error " << GetLastError( ) << " in WinHttpReadData." << endl;
		} else {
          //printf( "%s\n", pszOutBuffer );
		  str += pszOutBuffer;
		}

        // Free the memory allocated to the buffer.
        delete [] pszOutBuffer;
      }
    } while( dwSize > 0 );
  }


  // Report any errors.
  if( !bResults ) {
    cout << "Error " << GetLastError( ) << " has occurred." << endl;
  }

  // Close any open handles.
  if( hRequest ) WinHttpCloseHandle( hRequest );
  if( hConnect ) WinHttpCloseHandle( hConnect );
  if( hSession ) WinHttpCloseHandle( hSession );

  //cout << str << endl;
  vector<string> splited = FTPUserData::split(str, '\n');
	for (int i = 0; i < splited.size(); i++) {
		FTPUserData data(splited[i]);
		//data.print();
		vData->push_back(data);
	}
  
  return bResults;
}

bool HttpRequest::SendPostRequest(string good, const std::wstring &url, const std::wstring &req) {
  DWORD dwSize = 0;
  DWORD dwDownloaded = 0;
  LPSTR pszOutBuffer;
  BOOL  bResults = FALSE;
  HINTERNET  hSession = NULL, 
             hConnect = NULL,
             hRequest = NULL;

  std::wstring method = L"POST";
  void *body = NULL;
  DWORD bodySize = 0;
  std::string str = "";

  // Use WinHttpOpen to obtain a session handle.
  hSession = WinHttpOpen( _userAgent.c_str(),  
                          WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                          WINHTTP_NO_PROXY_NAME, 
                          WINHTTP_NO_PROXY_BYPASS, 0 );

  // Specify an HTTP server.
  if( hSession )
    hConnect = WinHttpConnect( hSession, url.c_str(),
                               INTERNET_DEFAULT_HTTP_PORT, 0 );

  // Create an HTTP request handle.
  if( hConnect )
      hRequest = WinHttpOpenRequest( hConnect, method.c_str(), req.c_str(),
                                   NULL, WINHTTP_NO_REFERER, 
                                   WINHTTP_DEFAULT_ACCEPT_TYPES, 
                                   0 );

  // Send a request.
  string poststr = "goods=" + good;
  LPSTR  post = const_cast<char *>(poststr.c_str());
  if( hRequest )
    bResults = WinHttpSendRequest( hRequest,
                                   L"content-type:application/x-www-form-urlencoded", -1,
                                   post, strlen(post), strlen(post), NULL );

  // End the request.
  if( bResults )
    bResults = WinHttpReceiveResponse( hRequest, NULL );

  // Keep checking for data until there is nothing left.
  if( bResults )
  {
    do 
    {
      // Check for available data.
      dwSize = 0;
      if( !WinHttpQueryDataAvailable( hRequest, &dwSize ) ) {
		  cout << "Error " << GetLastError( ) << " in WinHttpQueryDataAvailable." << endl;
	  }

      // Allocate space for the buffer.
      pszOutBuffer = new char[dwSize+1];
      if( !pszOutBuffer ) {
		cout << "Out of memory" << endl;
        dwSize=0;
      }
      else
      {
        // Read the data.
        ZeroMemory( pszOutBuffer, dwSize+1 );

        if( !WinHttpReadData( hRequest, (LPVOID)pszOutBuffer, 
                              dwSize, &dwDownloaded ) ) {
		  cout << "Error " << GetLastError( ) << " in WinHttpReadData." << endl;
		} else {
          //printf( "%s\n", pszOutBuffer );
		  str += pszOutBuffer;
		}

        // Free the memory allocated to the buffer.
        delete [] pszOutBuffer;
      }
    } while( dwSize > 0 );
  }


  // Report any errors.
  if( !bResults ) {
    cout << "Error " << GetLastError( ) << " has occurred." << endl;
  }

  // Close any open handles.
  if( hRequest ) WinHttpCloseHandle( hRequest );
  if( hConnect ) WinHttpCloseHandle( hConnect );
  if( hSession ) WinHttpCloseHandle( hSession );

  cout << str << endl;
  
  return bResults;
}