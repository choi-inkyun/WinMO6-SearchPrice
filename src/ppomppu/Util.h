
#if !defined(_UTIL_)
#define _UTIL_
#pragma once

#include <wininet.h>
#include <vector>
#include <atlstr.h> // CString

//#define INITGUID 
//#include <initguid.h> 
#include <connmgr.h>

#define	MAX_BUFFER_SIZE						4096
#define UM_UPDATE_INFO						WM_USER + 102

//Update State
#define UPDATE_TOTAL_SIZE 0
#define UPDATE_FILE_NAME 1
#define UPDATE_FILE_SIZE 2
#define UPDATE_CURRENT_SIZE 3

class CUtil
{
public:
	static BOOL InternetConnectTry(); // 현재 인터넷이 연결 되어 있는지 확인한 후 연결이 되어 있지 않으면 연결을 시도한다.
	static std::wstring GetConnectedString();
	static BOOL DownloadFile( LPCTSTR szLocalFilePath, LPCTSTR szServerFileUrl, BOOL bReload = TRUE, HWND hWnd = NULL, HANDLE hEvtCancel = NULL);
	
	static std::wstring MakeComma(int nNum); // 숫자를 넣으면 , 를 찍어서 String을 리턴해준다.
	static void ParseEscStr(char* src, CStringA &dst, BOOL bEsc); // UTF-8 인코딩
	static void UTF8Encoding(char* src, char* result); // URL 쿼리로 보낼 수 있도록 인코딩.
	static void StringReplace(std::wstring* sourceString, std::wstring findString, std::wstring replaceString); // 스크링 변환

	static BOOL MakeDirectory(CString strTargetFilePath); /* Secure 하지 않음 */
	static bool TestFileExistence(const std::wstring strFilePath);
	static void GetDefaultBasePath(std::wstring& strPath);
	static size_t GetModulePath(wchar_t *path, int buffsize);

	static BOOL IsFileExist(LPCTSTR szFileName);

	static HANDLE Execute( const TCHAR *program, const TCHAR *parameter ); // 특정 프로그램을 실행한다.
	static bool LaunchBrowser(const TCHAR *url); // 인자로 들어온 url로 브라우저를 띄워준다.
};

#endif