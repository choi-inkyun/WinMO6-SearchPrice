
#include "stdafx.h"
#include "PpomppuView.h"
#include "WmSqLite/sqlite3.h"
#include <assert.h>

#include "pugxml.h" //Use xml parser
using namespace pug;

// URLDownloadToFile 함수를 사용하기 위함
#include "Urlmon.h"
#pragma comment(lib, "Urlmon.lib")

//std::string에 대한 sqlite3의 처리가 안전하지 않기 때문인지 db파일 저장이 제대로 이루어지지 않는다.
// 이를 위해 char로 대체. 테스트 필요. - 회사에선 char로 했을 때 db파일이 잘 남았음.
char g_DBName[]	= "Wish";

CPpomppuView::CPpomppuView()
{
	searchEditBox = NULL;
	titleStatic = NULL;
	searchResultStatic = NULL;
	backButton = NULL;
	frontButton = NULL;
	imageDownloadProgressCtrl = NULL;

	goodsStartIndex = 0;
	goodsMaxIndex = 0;
	goodsDisplayCount = 0;

	buttonClickIndex = 0;

	displayCount = 10;
	sortOption = L"sim";
}

BOOL CPpomppuView::PreTranslateMessage(MSG* pMsg)
{
	pMsg;
	return FALSE;
}

void CPpomppuView::SetDisplayCountOption(int count)
{
	displayCount = count;
}

int CPpomppuView::GetDisplayCountOption()
{
	return displayCount;
}

void CPpomppuView::SetSortOption(std::wstring sort)
{
	sortOption = sort;
}

std::wstring CPpomppuView::GetSortOption()
{
	return sortOption;
}

void CPpomppuView::SetXMLFileToData(void* document)
{
	// xml 파일 읽은 정보를 가지고 정보를 얻는다.
	xml_node varNode = static_cast<xml_node*>(document)->first_element_by_path( _T("/rss/channel/") );
	if ( !varNode.empty() )
	{
		xml_node::iterator iter = varNode.begin();
		xml_node::iterator end = varNode.end();

		for( ; iter != end; ++iter)
		{
			// XML 데이터를 가지고 데이터를 넣어준다.
			if(wcscmp(iter->name(), L"item") == 0)
			{
				CGoodsData goodsData;

				xml_node::child_iterator iter2 = iter->children_begin();
				xml_node::child_iterator end2 = iter->children_end();

				// 각 칼럼에 따라 데이터를 넣어준다.
				for( ; iter2 != end2; ++iter2)
				{
					if( wcscmp(iter2->name(), L"title") == 0 )
					{
						goodsData.tilte = iter2->child(0).value();

						// 태그나 특수 문자는 빼고 출력해준다
						CUtil::StringReplace(&goodsData.tilte, L"&lt;b&gt;", L""); // <b>
						CUtil::StringReplace(&goodsData.tilte, L"&lt;/b&gt;", L""); // </b>
						CUtil::StringReplace(&goodsData.tilte, L"&lt;center&gt;", L""); // <center>
						CUtil::StringReplace(&goodsData.tilte, L"&lt;/center&gt;", L""); // </center>
						CUtil::StringReplace(&goodsData.tilte, L"&lt;l&gt;", L""); // <l>
						CUtil::StringReplace(&goodsData.tilte, L"&lt;/l&gt;", L""); // </l>
						CUtil::StringReplace(&goodsData.tilte, L"&lt;u&gt;", L""); // <u>
						CUtil::StringReplace(&goodsData.tilte, L"&lt;/u&gt;", L""); // </u>
						CUtil::StringReplace(&goodsData.tilte, L"&nbsp;", L" "); // 공백
						CUtil::StringReplace(&goodsData.tilte, L"&quot;", L"'"); // "
					}
					// 해당 제품 링크
					else if( wcscmp(iter2->name(), L"link") == 0 )
					{
						goodsData.link = iter2->child(0).value();
					}
					// 썸네일 이미지
					else if( wcscmp(iter2->name(), L"image") == 0 )
					{
						goodsData.imageUrl = iter2->child(0).value();
					}
					// 낮은 가격
					else if( wcscmp(iter2->name(), L"lprice") == 0 )
					{
						goodsData.lprice = _wtoi(iter2->child(0).value());
					}
					// 높은 가격
					else if( wcscmp(iter2->name(), L"hprice") == 0 )
					{
						goodsData.hprice = _wtoi(iter2->child(0).value());
					}
				}

				goodsListView.GoodsAdd(goodsData);
			}
			else if(wcscmp(iter->name(), L"title") == 0)
			{
				titleStatic.SetWindowTextW(iter->child(0).value());
			}
			else if(wcscmp(iter->name(), L"total") == 0)
			{
				goodsMaxIndex = _wtoi(iter->child(0).value());

				searchResultStatic.SetWindowTextW(CUtil::MakeComma(goodsMaxIndex).c_str());
			}
			else if(wcscmp(iter->name(), L"start") == 0)
			{
				goodsStartIndex = _wtoi(iter->child(0).value());
			}
			else if(wcscmp(iter->name(), L"display") == 0)
			{
				goodsDisplayCount = _wtoi(iter->child(0).value());

				int goodDisplayMaxCount = goodsStartIndex + goodsDisplayCount;

				if(goodDisplayMaxCount < goodsMaxIndex)
				{
					frontButton.EnableWindow(TRUE);
				}
				else
				{
					frontButton.EnableWindow(FALSE);
				}
				if(goodsStartIndex > 1)
				{
					backButton.EnableWindow(TRUE);
				}
				else
				{
					backButton.EnableWindow(FALSE);
				}

				// 검색 결과 범위 출력
				TCHAR displayRangeText[128];
				wsprintf(displayRangeText, L"%d~%d", goodsStartIndex, goodDisplayMaxCount);
				displayRangeStatic.SetWindowTextW(displayRangeText);
			}
		}
	}
}

LRESULT CPpomppuView::GoodsSearch(int searchStartIndex)
{
	//if(CUtil::InternetConnectTry() == FALSE)
	//{
	//	MessageBox(L"네트워크 연결에 실패하였습니다.", L"Error", MB_OK);
	//	return FALSE;
	//}

	// 상품 검색을 눌렀을 때 처리
	SetCursor(LoadCursor(NULL, IDC_WAIT));
	if(goodsListView.GoodsClear() == false)
	{
		return -1;
	}
	// 검색 텍스트를 얻어온다.
	TCHAR searchText[128];
	char searchTextTemp[128];
	searchEditBox.GetWindowText(searchText, 128);

	if( wcscmp(searchTextData, searchText) != 0 )
	{
		wcscpy( searchTextData, searchText );
	}

	if( wcscmp(searchText, L"") == 0 )
	{
		SetCursor(LoadCursor(NULL, IDC_ARROW));
		MessageBox(L"검색어를 넣어주세요.", L"Warning", MB_OK);
		return -1;
	}

	WideCharToMultiByte(CP_ACP, 0, searchText, -1, searchTextTemp, 128, NULL, NULL);

	// 검색 텍스트를 인코딩한다.
	char resultDataTemp[128];
	CUtil::UTF8Encoding(searchTextTemp, resultDataTemp);
	TCHAR resultData[128];
	MultiByteToWideChar( CP_UTF8, 0, resultDataTemp, -1, resultData, 128);

	// open api URL을 보내서 XML을 다운로드 받는다.
	TCHAR requestURL[256];
	wsprintf(requestURL,  L"http://openapi.naver.com/search?key=8a2f90871362257f9853bfa27aa6132f&query=%s&display=%d&start=%d&target=shop&sort=%s", 
		resultData, GetDisplayCountOption(), searchStartIndex, GetSortOption().c_str());

	// 다운받을 경로를 만든다.
	TCHAR strXMLFileName[256];
	wsprintf(strXMLFileName, L"%s%s", g_szTempPath.c_str(), DEFAULT_DOWNLOAD_SETUP_CONF);

	CCallback callback(requestURL, GetTickCount() + 60 * 1000);
	HRESULT hr = ::URLDownloadToFile(NULL, requestURL, strXMLFileName, 0, &callback);
//	BOOL bReturn = CUtil::DownloadFile(strXMLFileName, requestURL);
	if(hr != S_OK)
	{
		SetCursor(LoadCursor(NULL, IDC_ARROW));

		TCHAR errorMessage[256];
		wsprintf(errorMessage, _T("네트워크 연결에 실패하였습니다. %d(Error Code = %d)"), hr, GetLastError() );
		MessageBox(errorMessage, L"error", MB_OK);
		return -1;
	}

	// 다운받은 xml 파일을 파싱한다.
	xml_parser xml(parse_minimal);
	if (xml.parse_file( (LPCWSTR)strXMLFileName ) == false)
	{
		SetCursor(LoadCursor(NULL, IDC_ARROW));
		MessageBox(L"Invalid XML File (parse_file failed).", L"Error", MB_OK);
		return -1;
	}

	// xml 파일에서 정보를 읽는다.
	SetXMLFileToData(&xml.document());

	// 추가 작업 완료.
	goodsListView.GoodsAddEnd();
	SetCursor(LoadCursor(NULL, IDC_ARROW));
	return 0;
}

LRESULT CPpomppuView::OnSearchButton(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	buttonClickIndex = ID_SEARCH_BUTTON;
	LRESULT result = GoodsSearch();
	return result;
}

LRESULT CPpomppuView::OnBackButton(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	buttonClickIndex = ID_BACK_BUTTON;

	int searchIndex = goodsStartIndex - goodsDisplayCount;
	if(searchIndex < 1)
	{
		GoodsSearch();
	}
	else
	{
		GoodsSearch(searchIndex);
	}
	return 0;
}

LRESULT CPpomppuView::OnFrontButton(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	buttonClickIndex = ID_FRONT_BUTTON;

	int searchIndex = goodsStartIndex + goodsDisplayCount;
	GoodsSearch(searchIndex);
	return 0;
}

LRESULT CPpomppuView::OnWishAddButton(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	SetCursor(LoadCursor(NULL, IDC_WAIT));

	sqlite3* db;

	int result;
	result = sqlite3_open(g_DBName, &db);
	assert(result==SQLITE_OK);

	char* errmsg;

	if ( sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS WishList (ID INTEGER PRIMARY KEY, Name NVARCHAR(100), LowPrice bigint)", 0, 0, &errmsg) != SQLITE_OK )
	{
		std::cerr << errmsg << std::endl;
		sqlite3_free(errmsg);
		sqlite3_close(db);
		return 1;
	}

	// begin transaction
	sqlite3_exec(db, "BEGIN transaction", NULL, NULL, NULL);

	sqlite3_stmt* statement;
	sqlite3_prepare(db, "INSERT INTO WishList (Name, LowPrice) VALUES ( ?, ? )", -1, &statement, NULL);

	// To 인균: 요 스트링에다가 현재 입력돼있는 검색어를 넣어주면 된당.

	// 검색 텍스트를 얻어온다.
	TCHAR searchText[128];
	char searchTextTemp[128];
	searchEditBox.GetWindowText(searchText, 128);

	if( wcscmp(searchTextData, searchText) != 0 )
	{
		wcscpy( searchTextData, searchText );
	}

	// 검색어를 입력 했는지 체크
	if( wcscmp(searchText, L"") == 0 )
	{
		SetCursor(LoadCursor(NULL, IDC_ARROW));
		MessageBox(L"검색어를 넣어주세요.", L"Warning", MB_OK);
		return -1;
	}

	WideCharToMultiByte(CP_ACP, 0, searchText, -1, searchTextTemp, 128, NULL, NULL);

	std::string strInputData = searchTextTemp;
	double	iLowPrice = 0;

	sqlite3_reset(statement);
	sqlite3_bind_text(statement, 1, strInputData.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_double(statement, 2, iLowPrice);

	if ( sqlite3_step(statement) == SQLITE_ROW ) // there's returned rows
	{
		int nRows = sqlite3_column_count(statement);
	}

	// commit transaction
	sqlite3_exec(db, "COMMIT transaction", NULL, NULL, NULL);

	sqlite3_finalize(statement);	

	sqlite3_free(errmsg);
	sqlite3_close(db);

	// 추가 작업 완료.		
	SetCursor(LoadCursor(NULL, IDC_ARROW));
	return 0;
}

LRESULT CPpomppuView::OnWishShowButton(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	SetCursor(LoadCursor(NULL, IDC_WAIT));

	sqlite3* db;

	int result;
	result = sqlite3_open(g_DBName, &db);
	assert(result==SQLITE_OK);

	char* errmsg;

	if ( sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS WishList (ID INTEGER PRIMARY KEY, name TEXT)", 0, 0, &errmsg) != SQLITE_OK )
	{
		std::cerr << errmsg << std::endl;
		sqlite3_free(errmsg);
		sqlite3_close(db);
		return 1;
	}

	sqlite3_stmt* select_sql;

	char sql[] = "SELECT * FROM WishList";
	sqlite3_prepare(db, sql, (int)strlen(sql), &select_sql, NULL);
	sqlite3_reset(select_sql);

	while ((result = sqlite3_step(select_sql)) == SQLITE_ROW)
	{
		// To 인균: 이 부분을 ListBox에 새로 보여주면 된당 ㅇㅋ?
		// sqlite3_column_int(select_sql, 0) 에서 반환하는 값이 고유 ID값이고,
		// sqlite3_column_text(select_sql, 1) 에서 반환하는 값이 wish 상품 이름이당
		// 여깄는거 ListBox에 출력해주고 선택하면 그걸로 다시 goods검색 해줌 될듯.
		//printf("[%d, %s]\n", sqlite3_column_int(select_sql, 0), sqlite3_column_text(select_sql, 1));			
		TCHAR szString[256] = { 0, };
		const unsigned char *pName = 0;
		pName = sqlite3_column_text(select_sql, 1);
		int lLowPrice = (int)sqlite3_column_double(select_sql, 2);			
		wsprintf(szString, L"[%d, %s, %d]", sqlite3_column_int(select_sql, 0), pName, lLowPrice);
		MessageBox(szString, L"Warning", MB_OK);
	}

	if (result != SQLITE_DONE)
	{
		// 실패
		fprintf(stderr, "sqlite3_step(SELECT): %s\n", sqlite3_errmsg(db));
	}

	sqlite3_finalize(select_sql);
	sqlite3_close(db);

	// 추가 작업 완료.		
	SetCursor(LoadCursor(NULL, IDC_ARROW));
	return 0;
}

void CPpomppuView::DeleteWishList(int id)
{
	sqlite3* db;

	int result;
	result = sqlite3_open(g_DBName, &db);
	assert(result==SQLITE_OK);

	sqlite3_stmt* delete_sql;
	int err = -1;

	char sql[] = "delete from WishList where ID = ?";

	sqlite3_prepare(db, sql, (int)strlen(sql), &delete_sql, NULL);
	sqlite3_reset(delete_sql);
	sqlite3_bind_int(delete_sql, 1, id);

	err = sqlite3_step(delete_sql);
	if( err != SQLITE_DONE )
	{
		// 실패 시 ErrorMsg 알림
		fprintf(stderr, "sqlite3_step(delete): %s\n", sqlite3_errmsg(db));
	}

	sqlite3_finalize(delete_sql);
	sqlite3_close(db);
}


void CPpomppuView::UpdateWishList(int id, double iLowPrice)
{
	sqlite3* db;

	int result;
	result = sqlite3_open(g_DBName, &db);
	assert(result==SQLITE_OK);

	sqlite3_stmt* update_sql;
	int err = -1;

	char sql[] = "update WishList set LowPrice= ? where ID = ?";

	sqlite3_prepare(db, sql, (int)strlen(sql), &update_sql, NULL);
	sqlite3_reset(update_sql);
	sqlite3_bind_double(update_sql, 1, iLowPrice);
	sqlite3_bind_int(update_sql, 2, id);

	err = sqlite3_step(update_sql);
	if( err != SQLITE_DONE )
	{
		// 실패 시 ErrorMsg 알림
		fprintf(stderr, "sqlite3_step(delete): %s\n", sqlite3_errmsg(db));
	}

	sqlite3_finalize(update_sql);
	sqlite3_close(db);
}

LRESULT CPpomppuView::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CPaintDC dc(m_hWnd);

	return 0;
}

LRESULT CPpomppuView::OnTimer(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	if(wParam == IMAGE_DOWNLOAD_WAIT_TIMER)
	{
		if(goodsListView.GetThreadRun() == false)
		{
			BOOL tempData;
			switch(buttonClickIndex)
			{
			case ID_SEARCH_BUTTON:
				OnSearchButton(NULL, NULL, NULL, tempData);
				break;
			case ID_BACK_BUTTON:
				OnBackButton(NULL, NULL, NULL, tempData);
				break;
			case ID_FRONT_BUTTON:
				OnFrontButton(NULL, NULL, NULL, tempData);
				break;
			}
			buttonClickIndex = 0;
			KillTimer(IMAGE_DOWNLOAD_WAIT_TIMER);
		}
	}
	else
	{
		KillTimer(wParam);
		goodsListView.DrawItemBuffer();
	}
	return 0;
}

LRESULT CPpomppuView::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	// 버튼 폰트 설정
	m_hFont = CreateFont(20, // nHeight
		0, // nWidth 
		0, // nEscapement 
		0, // nOrientation 
		FW_SEMIBOLD, // nWeight 
		FALSE, // bItalic 
		FALSE, // bUnderline 
		0, // cStrikeOut 
		DEFAULT_CHARSET, // nCharSet 
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		0, // nQuality
		DEFAULT_PITCH | FF_SWISS, 
		L"굴림");

	GetClientRect(&clientRect);

	// 검색 editbox 생성
	searchEditBox = CreateWindow(L"edit",NULL,WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
		10, 50, 300, 50, m_hWnd,(HMENU)ID_SEARCH_EDIT,NULL,NULL);
	searchEditBox.SetLimitText(30); // 30 글자 제한

	// 검색 버튼 생성
	HWND buttonHwnd = CreateWindow(_T("button"),_T("검색"),WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 
		320, 50, 150, 50, m_hWnd,(HMENU)ID_SEARCH_BUTTON,NULL,NULL); 
	SendMessage(buttonHwnd, WM_SETFONT, (WPARAM)m_hFont, MAKELPARAM(TRUE,0));

	CreateWindow(L"static", L"검색된 상품 수 :",WS_CHILD | WS_VISIBLE | SS_LEFTNOWORDWRAP,
		10,110,220,30,m_hWnd,(HMENU)-1,NULL,NULL);
	searchResultStatic = CreateWindow(L"static", L"",WS_CHILD | WS_VISIBLE | SS_LEFTNOWORDWRAP,
		232,110,100,30,m_hWnd,(HMENU)-1,NULL,NULL);

	CreateWindow(L"static", L"현재 범위 :",WS_CHILD | WS_VISIBLE | SS_LEFTNOWORDWRAP,
		10,150,170,30,m_hWnd,(HMENU)-1,NULL,NULL);
	displayRangeStatic = CreateWindow(L"static", L"",WS_CHILD | WS_VISIBLE | SS_LEFTNOWORDWRAP,
		182,150,135,30,m_hWnd,(HMENU)-1,NULL,NULL);

	buttonHwnd = CreateWindow(_T("button"),_T("->"),WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 
		400,142,70,40, m_hWnd,(HMENU)ID_FRONT_BUTTON,NULL,NULL); 
	SendMessage(buttonHwnd, WM_SETFONT, (WPARAM)m_hFont, MAKELPARAM(TRUE,0));
	frontButton = buttonHwnd;
	frontButton.EnableWindow(FALSE);

	buttonHwnd = CreateWindow(_T("button"),_T("<-"),WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 
		320,142,70,40, m_hWnd,(HMENU)ID_BACK_BUTTON,NULL,NULL); 
	SendMessage(buttonHwnd, WM_SETFONT, (WPARAM)m_hFont, MAKELPARAM(TRUE,0));
	backButton = buttonHwnd;
	backButton.EnableWindow(FALSE);

	imageDownloadProgressCtrl = CreateWindow(PROGRESS_CLASS,NULL,WS_CHILD|WS_VISIBLE|WS_BORDER,
		0, 185, 480,15, m_hWnd,(HMENU)1010,NULL,NULL);
	imageDownloadProgressCtrl.SetRange(0, 100);
	imageDownloadProgressCtrl.ShowWindow(SW_HIDE);

	//HWND wishaddHwnd = CreateWindow(_T("button"),_T("add"),WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 
	//	150, 190, 150, 50, m_hWnd,(HMENU)ID_ADD_WISH_BUTTON,NULL,NULL); 
	//SendMessage(wishaddHwnd, WM_SETFONT, (WPARAM)m_hFont, MAKELPARAM(TRUE,0));

	//HWND wishshowHwnd = CreateWindow(_T("button"),_T("show"),WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 
	//	320, 190, 150, 50, m_hWnd,(HMENU)ID_SHOW_WISH_BUTTON,NULL,NULL); 
	//SendMessage(wishshowHwnd, WM_SETFONT, (WPARAM)m_hFont, MAKELPARAM(TRUE,0));

	titleStatic = CreateWindow(L"static", L"뽐뿌 - 상품 가격 검색 프로그램",WS_CHILD | WS_VISIBLE | SS_LEFTNOWORDWRAP,
		10,10,460,30,m_hWnd,(HMENU)-1,NULL,NULL);

	CREATESTRUCT* createData = (CREATESTRUCT*)lParam;
	createData->y = 200; // 리스트 박스의 y 좌표.
	goodsListView.OnCreate(m_hWnd, createData);

	memset(searchTextData, NULL, 128);

	//::GetTempPath(MAX_PATH, g_szTempPath);
	CUtil::GetDefaultBasePath(g_szTempPath);
	CUtil::MakeDirectory(g_szTempPath.c_str());
	return 0;
}
LRESULT CPpomppuView::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	DeleteObject(m_hFont);
	return 0;
}

LRESULT CPpomppuView::OnCommand(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	switch (LOWORD(wParam))
	{
		// 검색 Text 박스 관련 메시지 처리
	case ID_SEARCH_EDIT:
		switch (HIWORD(wParam))
		{
		case EN_SETFOCUS:
			// 텍스트 박스에 포커스가 오면 검색어를 지워주고 입력 창을 띄워준다. 
			searchEditBox.SetWindowTextW(L"");
			SHSipPreference(m_hWnd, SIP_UP);
			break;
		case EN_KILLFOCUS:
			SHSipPreference(m_hWnd, SIP_FORCEDOWN);
			break;
		}
		break;
	case IMAGEFILE_DOWNLOADING:
		// 썸네일 이미지가 받아진 ItemPanel의 Index가 lParam으로 넘어온다
		imageDownloadProgressCtrl.ShowWindow(SW_SHOW);
		imageDownloadProgressCtrl.SetPos(static_cast<int>( static_cast<float>(lParam + 1) / static_cast<float>(goodsListView.GetGoodsSize()) * 100.0f ));
		goodsListView.RedrawItems(lParam, lParam);
		break;

	case IMAGEFILE_DOWNLOAD_COMPLETE:
		imageDownloadProgressCtrl.ShowWindow(SW_HIDE);
		imageDownloadProgressCtrl.SetPos(0);
		// 이미지 썸네일 파일을 다운로드 완료 하였을때 온다
		break;
	case LIST_SELECT: // 리스트에서 Item을 Click 하였을 때
		{
			CGoodsData *pGoodData;
			pGoodData = goodsListView.GetGoods((int)lParam);
			if(pGoodData != NULL)
			{
				if(pGoodData->link.length() > 0)
				{
					CUtil::LaunchBrowser(pGoodData->link.c_str());
				}
			}
		}
		break;
	}
	return 0;
}

LRESULT CPpomppuView::OnDrawItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	goodsListView.OnDrawItem(uMsg, wParam, lParam, bHandled);
	return 0;
}

LRESULT CPpomppuView::OnMeasureItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	goodsListView.OnMeasureItem(uMsg, wParam, lParam, bHandled);
	return 0;
}

LRESULT CPpomppuView::OnNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	goodsListView.OnNotify(uMsg, wParam, lParam, bHandled);
	return 0;
}

LRESULT CPpomppuView::OnSettingChange(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	RECT rect;
	SystemParametersInfo(SPI_GETWORKAREA, NULL, &rect, FALSE);
	MoveWindow(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, FALSE);

	goodsListView.OnSettingChange(uMsg, wParam, lParam, bHandled);
	return 0;
}