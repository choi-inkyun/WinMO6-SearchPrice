// ppomppu.cpp : main source file for ppomppu.exe
//
#include "stdafx.h"

#include <atlframe.h>
#include <atlctrls.h>
#include <atlmisc.h>
#include <atlscrl.h>
#include <atlwince.h>

#include "resourceppc.h"

#include "PpomppuView.h"
#include "AboutDlg.h"
#include "PpomppuFrame.h"

CAppModule _Module;
/*
* sqlite3_exec 에서 불러지는 콜백.
* 컬럼의 이름과 값을 출력한다.
*/

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
	HRESULT hRes = CPpomppuFrame::ActivatePreviousInstance(hInstance, lpstrCmdLine);

	if(FAILED(hRes) || S_FALSE == hRes)
	{
		return hRes;
	}

	hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
	ATLASSERT(SUCCEEDED(hRes));

	AtlInitCommonControls(ICC_DATE_CLASSES);
	SHInitExtraControls();

	hRes = _Module.Init(NULL, hInstance);
	ATLASSERT(SUCCEEDED(hRes));	

	int nRet = CPpomppuFrame::AppRun(lpstrCmdLine, nCmdShow);

	_Module.Term();
	::CoUninitialize();

	return nRet;
}

