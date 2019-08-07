#include "stdafx.h"

#include "Util.h"
#include "OperaBrowserFinder.h"

BOOL CUtil::InternetConnectTry()
{
	// Establish an asynchronous connection
	HANDLE hConnection = NULL;
	DWORD dwStatusCheck = 0;

	// Get the network information where we want to establish a
	// connection
	TCHAR tchRemoteUrl[256] = TEXT("");
	wsprintf(tchRemoteUrl, TEXT("http://www.naver.com"));
	GUID guidNetworkObject;
	DWORD dwIndex = 0;

	if( ConnMgrMapURL(tchRemoteUrl, &guidNetworkObject, &dwIndex) == E_FAIL) 
	{
		MessageBox(NULL, TEXT("Could not map the request to a network identifier"), L"error", MB_OK);
		return FALSE;
	}

	// if(SUCCEEDED(hr))

	// Now that we've got the network address, set up the//
	// connection structure
	CONNMGR_CONNECTIONINFO ccInfo;

	memset(&ccInfo, 0, sizeof(CONNMGR_CONNECTIONINFO));
	ccInfo.cbSize = sizeof(CONNMGR_CONNECTIONINFO);
	ccInfo.dwParams = CONNMGR_PARAM_GUIDDESTNET;
	ccInfo.dwPriority = CONNMGR_PRIORITY_USERINTERACTIVE;
	ccInfo.guidDestNet = guidNetworkObject;

	// Make the connection request
	if(ConnMgrEstablishConnection(&ccInfo, &hConnection) == E_FAIL)
	{
		return FALSE;
	}
	/*
	if( FAILED( ConnMgrSetConnectionPriority( hConnection, CONNMGR_PRIORITY_USERIDLE) ) )
	{
	AfxMessageBox( L"Can't Change Priority" );
	return;
	}
	*/

	// Poll to see if the connection has been established
	BOOL fLoop = TRUE;
	BOOL fConnected = FALSE;

	while(fLoop) 
	{
		dwStatusCheck = 0;
		if(FAILED(ConnMgrConnectionStatus(hConnection,
			&dwStatusCheck))) 
		{
			// Do some error processing here
			fLoop = FALSE;
			break;
		}

		// Got the status, do something with it:
		if(dwStatusCheck & CONNMGR_STATUS_CONNECTED) 
		{
			//				MessageBox(NULL, TEXT("Connected"), L"infomation", MB_OK);
			fLoop = FALSE;
			fConnected = TRUE;
			break;
		}

		if(dwStatusCheck & CONNMGR_STATUS_WAITINGCONNECTION)
		{
			//				MessageBox(NULL, TEXT("Establishing a connection...."), L"infomation", MB_OK);
		}
		if(dwStatusCheck & CONNMGR_STATUS_DISCONNECTED) 
		{
			//				MessageBox(NULL, TEXT("Disconnected from the network...."), L"infomation", MB_OK);
			fLoop = FALSE;
		}
	}
	return TRUE;
}

BOOL CUtil::DownloadFile( LPCTSTR szLocalFilePath, LPCTSTR szServerFileUrl, BOOL bReload, HWND hWnd, HANDLE hEvtCancel)
{
	// 쿼리를 날리고 파일을 다운로드 한다.
	HINTERNET hInet = InternetOpen( L"PPOMPPU", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0 );
	if (hInet == NULL)
	{
		SetCursor(LoadCursor(NULL, IDC_ARROW));
		MessageBox(NULL, L"InternetOpen Failed", L"Error", MB_OK);
		return FALSE;
	}

	if(InternetConnectTry() == FALSE)
	{
		return FALSE;
	}

	TCHAR errorMessage[256];
	memset(errorMessage, NULL, 256);

	DWORD dwFlags;

	if ( bReload )
	{
		dwFlags = INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE;	// Reload always
	}
	else
	{
		dwFlags = INTERNET_FLAG_RESYNCHRONIZE;	// Reaload only if modified
	}

	HINTERNET hInternetFile = InternetOpenUrl( hInet, szServerFileUrl, NULL, 0, INTERNET_FLAG_RELOAD, 0 );
	if ( ! hInternetFile )
	{
		wsprintf(errorMessage, _T("[DownloadFile] InternetOpenUrl(%s) Failed (Error Code = %d)"), szServerFileUrl, GetLastError() );
		MessageBox(NULL, errorMessage, L"error", MB_OK);
		return FALSE;
	}

	DWORD dwStatus;
	DWORD dwStatusSize = sizeof(dwStatus);
	HttpQueryInfo( hInternetFile, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &dwStatus, &dwStatusSize, NULL );
	if ( dwStatus != HTTP_STATUS_OK )
	{
		wsprintf(errorMessage, _T("[DownloadFile] HttpQueryInfo(%s) = %d "), szServerFileUrl, dwStatus );
		MessageBox(NULL, errorMessage, L"error", MB_OK);

		InternetCloseHandle( hInternetFile );
		return FALSE;
	}

	HANDLE hFile = CreateFile( szLocalFilePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH, NULL );
	if ( hFile == INVALID_HANDLE_VALUE )
	{	
		wsprintf(errorMessage, _T("[DownloadFile] CreateFile(%s) Failed, Error Code = %d"), szLocalFilePath, GetLastError() );
		MessageBox(NULL, errorMessage, L"error", MB_OK);

		InternetCloseHandle( hInternetFile );
		return FALSE;
	}

	DWORD dwRead = 0;
	DWORD dwFileSize = 0;
	DWORD dwSize = 0;
	DWORD dwRecv = 0;
	TCHAR szTemp[ MAX_BUFFER_SIZE ];

	dwSize = sizeof( dwFileSize );	

	HttpQueryInfo( hInternetFile, HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, (LPVOID)&dwFileSize, &dwSize, NULL );
	if ( dwStatus != HTTP_STATUS_OK )
	{
		wsprintf(errorMessage, _T("[DownloadFile] HttpQueryInfo(%s) = %d "), szServerFileUrl, dwStatus );
		MessageBox(NULL, errorMessage, L"error", MB_OK);

		InternetCloseHandle( hInternetFile );
		return FALSE;
	}

	while ( InternetReadFile( hInternetFile, szTemp, MAX_BUFFER_SIZE, &dwRead ) )
	{	
		if (hEvtCancel != NULL)
		{
			if (::WaitForSingleObject(hEvtCancel, 0) == WAIT_OBJECT_0)
			{
				wsprintf(errorMessage, _T("[DownloadFile] User canceled downloading") );
				MessageBox(NULL, errorMessage, L"error", MB_OK);
				break;
			}
		}

		if ( ! dwRead ) 
		{
			break;
		}

		DWORD dwWritten = 0;
		if ( WriteFile( hFile, szTemp, dwRead, &dwWritten, NULL ) == FALSE)
		{
			wsprintf(errorMessage, _T("[DownloadFile] WriteFile(%s) Failed, Error Code = %d"), szLocalFilePath, GetLastError() );
			MessageBox(NULL, errorMessage, L"error", MB_OK);
			break;
		}

		dwRecv += dwWritten;

		if (hWnd != NULL)
		{
			PostMessage( hWnd, UM_UPDATE_INFO, UPDATE_CURRENT_SIZE , dwWritten);
		}
	}

	InternetCloseHandle( hInternetFile );
	CloseHandle( hFile );

	if ( dwFileSize != 0)
	{
		if ( dwFileSize != dwRecv )
		{
			wsprintf(errorMessage, _T("[DownloadFile] dwFileSize != dwRecv Download Failed") );
			MessageBox(NULL, errorMessage, L"error", MB_OK);

			DeleteFile(szLocalFilePath);
			return FALSE;
		}
	}

	return TRUE;
}

std::wstring CUtil::GetConnectedString()
{
	DWORD dwConnectionTypes;
	if(InternetGetConnectedState(&dwConnectionTypes, 0)) // 정상적으로 검사됨
	{
		if((dwConnectionTypes & INTERNET_CONNECTION_MODEM) != 0)
			return L"Modem으로 인터넷 사용중...";
		if((dwConnectionTypes & INTERNET_CONNECTION_LAN) != 0)
			return L"LAN으로 인터넷 사용중...";
		if((dwConnectionTypes & INTERNET_CONNECTION_PROXY) != 0)
			return L"Proxy로 인터넷 사용중...";
		if((dwConnectionTypes & INTERNET_CONNECTION_MODEM_BUSY) != 0)
			return L"Modem을 다른 용도로 사용중";
		if((dwConnectionTypes & INTERNET_RAS_INSTALLED) != 0)
			return L"RAS가 설치되어 있음";
		if((dwConnectionTypes & INTERNET_CONNECTION_OFFLINE) != 0)
			return L"오프라인";
		if((dwConnectionTypes & INTERNET_CONNECTION_CONFIGURED) != 0)
			return L"인터넷 연결이 설정되었음";
	}
	return NULL;
}


std::wstring CUtil::MakeComma(int nNum)
{
	wchar_t strTmp[256]; 
	std::wstring strData;
	wsprintf(strTmp, L"%d", nNum);
	strData = strTmp;

	for(int i=wcslen(strTmp)-1,j=0;i>=0;i--,j++)
	{
		if(j != 0 && (j%3) == 0) 
		{ 
			strData.insert(i+1, L",");
			i--;
			j++;
		}
	}
	return strData;
}

void CUtil::ParseEscStr(char* src, CStringA &dst, BOOL bEsc)
{
	int n,i;
	char buff[10];
	CStringA str;
	n=strlen(src);
	dst.Empty();
	if(bEsc)
	{
		for(i=0;i<n;i++)
		{
			sprintf(buff,"%.2x",(unsigned char)src[i]);
			str=buff; /*str.MakeUpper();*/					// 대문자로...
			str= "%"+str;									// % 붙이기...
			dst+=str;										// 이전 문자열과 더하기...
		}
	}
	else
	{
		i=0;
		while(TRUE)
		{
			if(i>=n) break;
			dst+=src[i];
			i++;
		}
	}
}

void CUtil::UTF8Encoding(char* src, char* result)
{
	CStringA str=src;
	CStringA dst = "";
	char szRet[1024];
	BSTR bstr=str.AllocSysString();
	int len=WideCharToMultiByte(CP_UTF8,0,bstr,-1,NULL,0,NULL,NULL);
	if(len==0) { 
		::SysFreeString(bstr);
		strcpy(result, src);
		return; 
	}
	char* pStr=new char[len];
	len = WideCharToMultiByte(CP_UTF8,0,bstr,-1,pStr,len,NULL,NULL);
	ParseEscStr(pStr,dst,true);
	strcpy(szRet,dst.GetBuffer());

	::SysFreeString(bstr);
	delete[] pStr;
	strcpy(result, szRet);
}

void CUtil::StringReplace(std::wstring* sourceString, std::wstring findString, std::wstring replaceString)
{
	std::wstring::size_type offset = 0;

	while( true )   
	{   
		// 검색.   
		offset = sourceString->find( findString, offset );             

		if( std::wstring::npos == offset )              
			break;   

		else           
			// 찾은 문자열을 바꿀 문자열로 바꿈.   
			sourceString->replace( offset, findString.length(), replaceString );                       
	}   
}

BOOL CUtil::MakeDirectory(CString strTargetFilePath)
{
	BOOL retVal = true;

	std::wstring strTestFilePath;

	int start = 0;
	int length = strTargetFilePath.GetLength();
	int pos;

	while (retVal && (pos = strTargetFilePath.Find(L"\\", start)) != -1)
	{
		if (pos != 0)
		{
			strTestFilePath = strTargetFilePath.Left(pos);
			if (TestFileExistence(strTestFilePath) == false)
			{
				retVal = CreateDirectory(strTestFilePath.c_str(), NULL);
			}
		}

		start = pos+1;
	}

	return retVal;
}

bool CUtil::TestFileExistence(const std::wstring strFilePath)
{
	bool retVal = false;

	WIN32_FIND_DATA findFileData;
	HANDLE hFindFile;

	hFindFile = FindFirstFile(strFilePath.c_str(), &findFileData);
	if (hFindFile != INVALID_HANDLE_VALUE)
	{
		FindClose(hFindFile);
		retVal = true;
	}
	return retVal;
}

size_t CUtil::GetModulePath(wchar_t *path, int buffsize)
{
	// Get the filename of this executable
	if (GetModuleFileName(NULL, path, buffsize) == 0)
		return 0;

	for(int i=wcslen(path) -1 ;i >= 0; --i)
	{
		if( path[i] == _T('\\') )
		{
			path[i] = 0;
			return (i);
		}
	}
	return (0);
}

void CUtil::GetDefaultBasePath(std::wstring& strPath)
{
//	strPath = L"\\MITs Store\\ppomppu\\";
	WCHAR szwModulePath[MAX_PATH];
	char szModulePath[MAX_PATH];
	WCHAR sDirectoryTemp[MAX_PATH];
	GetModulePath(szwModulePath,MAX_PATH);
	swprintf(sDirectoryTemp, L"%s\\", szwModulePath);
	wcstombs(szModulePath,sDirectoryTemp,wcslen(sDirectoryTemp) + 1);
	strPath = sDirectoryTemp;
}

BOOL CUtil::IsFileExist(LPCTSTR szFileName)
{
	HANDLE hFile = CreateFile(szFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
//		HGLogPrintf(1, _T("[IsFileExist] CreateFile(%s) Failed, Error Code = %d"), szFileName, GetLastError() );	
		return FALSE;
	}
	CloseHandle(hFile);
	return TRUE;
}

HANDLE CUtil::Execute( const TCHAR *program, const TCHAR *parameter )
{
	PROCESS_INFORMATION pi; 
	ZeroMemory( &pi, sizeof(pi) );

	try{
		if ( FALSE == CreateProcess(
			program,
			parameter,
			NULL, NULL, FALSE, 0, NULL, NULL, NULL, &pi) )
			return NULL;

		return pi.hProcess;
	}catch(...)
	{
		return NULL;
	}
}

bool CUtil::LaunchBrowser(const TCHAR *url)
{
	TCHAR wUrl[256];
	TCHAR browserPath[MAX_PATH];
	_tcscpy(wUrl, url);

	if( TRUE == OperaBrowserFinder::CheckOperaRegistry(browserPath) )
	{
		Execute( browserPath, wUrl );
	}
	else if( TRUE == OperaBrowserFinder::CheckOperaFilePath(browserPath) )
	{
		Execute( browserPath, wUrl );
	}
	else if( TRUE == OperaBrowserFinder::GetDefaultBrower(browserPath) )
	{
		Execute( browserPath, wUrl );
	}
	else
	{
		return (false);
	}
	return (true);
}