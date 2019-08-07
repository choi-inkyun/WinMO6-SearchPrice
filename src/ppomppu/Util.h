
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
	static BOOL InternetConnectTry(); // ���� ���ͳ��� ���� �Ǿ� �ִ��� Ȯ���� �� ������ �Ǿ� ���� ������ ������ �õ��Ѵ�.
	static std::wstring GetConnectedString();
	static BOOL DownloadFile( LPCTSTR szLocalFilePath, LPCTSTR szServerFileUrl, BOOL bReload = TRUE, HWND hWnd = NULL, HANDLE hEvtCancel = NULL);
	
	static std::wstring MakeComma(int nNum); // ���ڸ� ������ , �� �� String�� �������ش�.
	static void ParseEscStr(char* src, CStringA &dst, BOOL bEsc); // UTF-8 ���ڵ�
	static void UTF8Encoding(char* src, char* result); // URL ������ ���� �� �ֵ��� ���ڵ�.
	static void StringReplace(std::wstring* sourceString, std::wstring findString, std::wstring replaceString); // ��ũ�� ��ȯ

	static BOOL MakeDirectory(CString strTargetFilePath); /* Secure ���� ���� */
	static bool TestFileExistence(const std::wstring strFilePath);
	static void GetDefaultBasePath(std::wstring& strPath);
	static size_t GetModulePath(wchar_t *path, int buffsize);

	static BOOL IsFileExist(LPCTSTR szFileName);

	static HANDLE Execute( const TCHAR *program, const TCHAR *parameter ); // Ư�� ���α׷��� �����Ѵ�.
	static bool LaunchBrowser(const TCHAR *url); // ���ڷ� ���� url�� �������� ����ش�.
};

#endif