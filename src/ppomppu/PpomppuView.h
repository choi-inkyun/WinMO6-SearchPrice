// PpomppuView.h : interface of the CPpomppuView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Util.h"
#include "Define.h"
#include "GoodsData.h"
#include "ListControl.h"

//#include <iostream>
//#include <cassert>

#define ID_SEARCH_EDIT 1000
#define ID_SEARCH_BUTTON 1001
#define ID_BACK_BUTTON 1002
#define ID_FRONT_BUTTON 1003
#define ID_ADD_WISH_BUTTON 1004
#define ID_SHOW_WISH_BUTTON 1005

class CPpomppuView : 
	public CWindowImpl<CPpomppuView>
{
private:
	CListPageListView goodsListView;

	CEdit searchEditBox;		/* 검색 editbox */
	CStatic titleStatic;		/* 상단에 타이틀 텍스트 */
	CStatic searchResultStatic; /* 검색 결과 숫자 표시 */
	CStatic displayRangeStatic; /* 현재 화면에 출력되는 범위 표시 */
	CButton backButton;
	CButton frontButton;
	CProgressBarCtrl imageDownloadProgressCtrl;

	int goodsStartIndex;
	int goodsMaxIndex;
	int goodsDisplayCount;

	int buttonClickIndex;

	int displayCount;			// 출력하려고 하는 상품의 갯수
	std::wstring sortOption;	// 출력하고자 하는 상품의 정렬 상태

	HFONT m_hFont, m_hFontOld;

	RECT clientRect;			/* window Rect */

	std::wstring g_szTempPath;
	TCHAR searchTextData[128];

public:
	DECLARE_WND_CLASS(NULL)

	CPpomppuView();

	BOOL PreTranslateMessage(MSG* pMsg);

	BEGIN_MSG_MAP(CPpomppuView)
		COMMAND_ID_HANDLER(ID_SEARCH_BUTTON, OnSearchButton)
		COMMAND_ID_HANDLER(ID_BACK_BUTTON, OnBackButton)
		COMMAND_ID_HANDLER(ID_FRONT_BUTTON, OnFrontButton)
		COMMAND_ID_HANDLER(ID_ADD_WISH_BUTTON,	OnWishAddButton)
		COMMAND_ID_HANDLER(ID_SHOW_WISH_BUTTON,	OnWishShowButton)
		

		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_COMMAND, OnCommand)

		// 리스트 콘트롤 관련 메시지
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		MESSAGE_HANDLER(WM_MEASUREITEM, OnMeasureItem)
		MESSAGE_HANDLER(WM_DRAWITEM, OnDrawItem)
		MESSAGE_HANDLER(WM_NOTIFY, OnNotify)
		MESSAGE_HANDLER(WM_SETTINGCHANGE, OnSettingChange)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	void SetDisplayCountOption(int count);
	int GetDisplayCountOption();

	void SetSortOption(std::wstring sort);
	std::wstring GetSortOption();

	void SetXMLFileToData(void* document);

	// 상품 검색을 눌렀을 떄
	LRESULT GoodsSearch(int searchStartIndex = 1);

	LRESULT OnSearchButton(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBackButton(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnFrontButton(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnWishAddButton(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnWishShowButton(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	void DeleteWishList(int id);
	void UpdateWishList(int id, double iLowPrice);

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnTimer(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/);
	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnCommand(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/);
	LRESULT OnDrawItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMeasureItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSettingChange(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
};

