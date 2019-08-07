
#if !defined(_LISTCONTROL_)
#define _LISTCONTROL_
#pragma once

#define _WTL_CE_NO_ZOOMSCROLL
#define _WTL_CE_NO_FULLSCREEN
#define _WTL_NO_CSTRING

#include <vector>
#include <atlctrls.h>

#include "Util.h"
#include "Define.h"
#include "GoodsData.h"

#include "Urlmon.h"
#include "StatusCallBack.h"
#pragma comment(lib, "Urlmon.lib")

#define ARRAYSIZE(a)   (sizeof(a)/sizeof(*a))

typedef struct tagNMITEMACTIVATE {
	NMHDR hdr;
	int iItem;
	int iSubItem;
	UINT uNewState;
	UINT uOldState;
	UINT uChanged;
	POINT ptAction;
	LPARAM lParam;
	UINT uKeyFlags;
} NMITEMACTIVATE, *LPNMITEMACTIVATE;

class COwnerListView
{
public:
	COwnerListView()
	{
		hwndListView = NULL;
		parentWindow = NULL;
		itemHeight = 100;

		drawFromIndex = 0;
		drawToIndex = 0;

		drawIndexBuffer.clear();
	}
	~COwnerListView()
	{
		
	}

	LRESULT OnCreate(HWND hWnd, CREATESTRUCT* lParam)
	{
		parentWindow = hWnd;

		RECT rcClient;
		rcClient.top = lParam->y;
		rcClient.left = lParam->x;
		rcClient.bottom = lParam->cy;
		rcClient.right = lParam->cx;

		hwndListView = CreateWindow(WC_LISTVIEW, NULL,
			WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_NOCOLUMNHEADER | LVS_OWNERDATA | LVS_OWNERDRAWFIXED | WS_BORDER | ES_AUTOVSCROLL,
			rcClient.left, rcClient.top, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top,
			hWnd, NULL, NULL,NULL);
		if(!hwndListView)
		{
			return -1;
		}
		listCtrl = hwndListView;

		// Put the list view in full row select mode
		ListView_SetExtendedListViewStyle(hwndListView,
			ListView_GetExtendedListViewStyle(hwndListView) | LVS_EX_FULLROWSELECT);

		LV_COLUMN lvColumn;

		lvColumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_WIDTH;
		lvColumn.fmt = LVCFMT_LEFT;
		lvColumn.cx = (rcClient.right - rcClient.left) - (GetSystemMetrics(SM_CXVSCROLL) + 4);

		if(-1 == ListView_InsertColumn(hwndListView, 0, &lvColumn))
		{
			return -1;
		}

		//ListView_SetItemCount(hwndListView, 20);

		return 0;
	}

	LRESULT OnListViewNotify(HWND hWnd, LPARAM lParam)
	{
		NMHDR* pnmh = (NMHDR*) lParam;
		LRESULT lResult = 0;

		switch(pnmh->code)
		{
		case NM_CLICK:
			{
				LPNMITEMACTIVATE pnmia = (LPNMITEMACTIVATE)lParam;
				SendMessage(parentWindow, WM_COMMAND, LIST_SELECT, pnmia->iItem);
			}
			break;
		case LVN_GETDISPINFO:
			{
				LV_DISPINFO* pdi = (LV_DISPINFO*)lParam;

				if(0 == pdi->item.iSubItem)
				{
					if(pdi->item.mask & LVIF_TEXT)
					{
						//FormatMessageHelp(pdi->item.pszText, pdi->item.cchTextMax,
						//	g_szColumn0Format, pdi->item.iItem + 1);
					}

					if(pdi->item.mask & LVIF_IMAGE)
					{
						pdi->item.iImage = 0;
					}
				}
				else
				{
					if(pdi->item.mask & LVIF_TEXT)
					{
						//FormatMessageHelp(pdi->item.pszText, pdi->item.cchTextMax,
						//	g_szColumnNFormat, pdi->item.iItem + 1, pdi->item.iSubItem);
					}
				}
			}
			break;

		case LVN_ITEMACTIVATE:
			{
				// 클릭을 하였을 경우 상위 윈도우로 알려준다.
				// NMLISTVIEW* pnmlv = (NMLISTVIEW*)lParam;
				// FormattedMessageBox(hWnd, MB_OK, g_hInst, IDS_MB_CAPTION, IDS_MB_FORMAT, pnmlv->iItem + 1);
			}
			break;

			// Handle this notify if you want to use the number keys to select
			// items in the view (like how the desktop file explorer lets you
			// select files by typing the first few characters of the file name.)
			// This notify is also used then the control is sent an LVM_FINDITEM
		case LVN_ODFINDITEM:
			{
				NMLVFINDITEM* pFindItem = (NMLVFINDITEM*)lParam;
				// Used if you are trying to find an item in the list view
			}
			break;
		case LVN_ODCACHEHINT:
			{
				NMLVCACHEHINT* nmlvh = (NMLVCACHEHINT*)lParam;

				if(drawFromIndex == 0 && drawToIndex == 0)
				{
					drawFromIndex = nmlvh->iFrom;
					drawToIndex = nmlvh->iTo;
				}
				else if(nmlvh->iFrom < drawFromIndex)
				{
					int offsetIndex = drawFromIndex - nmlvh->iFrom;
					drawFromIndex = nmlvh->iFrom;
					drawToIndex-=offsetIndex;

					NMLVCACHEHINT sendData;
					sendData.iFrom = drawToIndex + 1;
					sendData.iTo = drawToIndex + offsetIndex;
					SendMessage(parentWindow, WM_COMMAND, UNLOAD_LIST, (LPARAM)&sendData);
				}
				else if(nmlvh->iTo > drawToIndex)
				{
					int offsetIndex = nmlvh->iTo - drawToIndex;
					drawToIndex = nmlvh->iTo;
					drawFromIndex+=offsetIndex;

					NMLVCACHEHINT sendData;
					sendData.iFrom = drawFromIndex - offsetIndex;;
					sendData.iTo = drawFromIndex -1;
					SendMessage(parentWindow, WM_COMMAND, UNLOAD_LIST, (LPARAM)&sendData);
				}
			}
			break;
		}

		return(lResult);
	}


	LRESULT OnMeasureItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LPMEASUREITEMSTRUCT lpmi;

		lpmi = (LPMEASUREITEMSTRUCT)lParam;

		if (lpmi->CtlType == ODT_LISTVIEW)
		{
			lpmi->itemHeight = itemHeight;
		}
		return 0;
	}

	LRESULT COwnerListView::OnNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		OnListViewNotify(parentWindow, lParam);

		// 세로 스크롤바가 무조건 생기게 한다.
		ShowScrollBar(hwndListView, SB_VERT, TRUE);
		return 0;
	}

	LRESULT OnSettingChange(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if( SETTINGCHANGE_RESET == wParam)
		{
			//Orientation change
			RECT rect;
			SystemParametersInfo(SPI_GETWORKAREA, NULL, &rect, FALSE);

			//Move the child window
			MoveWindow(hwndListView, 0, 0, rect.right - rect.left, rect.bottom - rect.top, FALSE);
		}
		return 0;
	}

	void InvalidateRect()
	{
		::InvalidateRect(hwndListView, NULL, TRUE);
	}

	void RedrawItems(int nFirst, int nLast)
	{
		listCtrl.RedrawItems(nFirst, nLast);
	}

	void DrawItemBuffer()
	{
		if(drawIndexBuffer.size() != 0)
		{
			for(int i = 0; i < (int)drawIndexBuffer.size(); ++i)
			{
				listCtrl.RedrawItems(drawIndexBuffer[i], drawIndexBuffer[i]);
			}
		}
		drawIndexBuffer.clear();
	}

protected:
	/**************************************************************************************

	FormatMessageHelp

	Variants of printf should be avoided as they don't allow re-ordering of parameters.
	This can be important when an application is required to work in multiple languages
	without code changes.

	**************************************************************************************/
	DWORD FormatMessageHelp(
		TCHAR* pszOut,
		UINT cchOut,
		const TCHAR* pszFormat,
		...
		)
	{
		va_list marker;
		DWORD dwRet;

		va_start(marker, pszFormat);
		dwRet = FormatMessage(FORMAT_MESSAGE_FROM_STRING,
			pszFormat, 0, 0, pszOut, cchOut, &marker);
		va_end(marker);
		return dwRet;
	}


	/**************************************************************************************

	FormattedMessageBox

	**************************************************************************************/
	int FormattedMessageBox(
		HWND hWnd,
		UINT uType,
		HINSTANCE hInst,
		UINT idsCaption,
		UINT idsFormat,
		...
		)
	{
		va_list marker;
		DWORD dwRet;
		// If these buffers are too small, you will get truncation
		TCHAR szCaption[100];
		TCHAR szFormat[200];
		TCHAR szText[ARRAYSIZE(szFormat) + 100];

		if(0 == LoadString(hInst, idsCaption, szCaption, ARRAYSIZE(szCaption)) ||
			0 == LoadString(hInst, idsFormat, szFormat, ARRAYSIZE(szFormat)))
		{
			// Couldn't load a needed string.
			return(0);
		}

		va_start(marker, idsFormat);
		dwRet = FormatMessage(FORMAT_MESSAGE_FROM_STRING,
			szFormat, 0, 0, szText, ARRAYSIZE(szText), &marker);
		va_end(marker);
		if(0 == dwRet)
		{
			// Something bad happened in FormatMessage.
			return(0);
		}

		return(MessageBox(hWnd, szText, szCaption, uType));
	}

	void Clear()
	{
		drawFromIndex = 0;
		drawToIndex = 0;

		drawIndexBuffer.clear();
	}

protected:
	HWND parentWindow; /* 부모 윈도우의 핸들 */
	HWND hwndListView; /* listView 객체의 핸들 */
	int itemHeight; /* 각 item 의 높이 */

	std::vector<int> drawIndexBuffer;
private:
	// 그리지 않는 영역을 판단하기 위한 변수
	int drawFromIndex;
	int drawToIndex;

	CListViewCtrl listCtrl;
};


class CListPageListView : public COwnerListView
{
public:
	CListPageListView()
	{
		itemHeight = 122;

		m_hSmallFont = CreateFont(14, // nHeight
			0, // nWidth 
			0, // nEscapement 
			0, // nOrientation 
			FW_NORMAL, // nWeight 
			FALSE, // bItalic 
			FALSE, // bUnderline 
			0, // cStrikeOut 
			DEFAULT_CHARSET, // nCharSet 
			OUT_DEFAULT_PRECIS,        // nOutPrecision
			CLIP_DEFAULT_PRECIS,       // nClipPrecision
			0, // nQuality
			DEFAULT_PITCH | FF_SWISS, 
			L"굴림");

		m_hMediumFont = CreateFont(22, // nHeight
			0, // nWidth 
			0, // nEscapement 
			0, // nOrientation 
			FW_NORMAL, // nWeight 
			FALSE, // bItalic 
			FALSE, // bUnderline 
			0, // cStrikeOut 
			DEFAULT_CHARSET, // nCharSet 
			OUT_DEFAULT_PRECIS,        // nOutPrecision
			CLIP_DEFAULT_PRECIS,       // nClipPrecision
			0, // nQuality
			DEFAULT_PITCH | FF_SWISS, 
			L"굴림");

		linePen.CreatePen(PS_SOLID, 1, RGB(210,210,210));

		// 스레드 관련 생성
		hThread = NULL;
		bRunning = false;
		dwThreadID = 0;
		bThreadCancel = false;

		GoodsClear();
	}
	~CListPageListView()
	{
		DeleteObject(m_hSmallFont);
		DeleteObject(m_hMediumFont);
		DeleteObject(linePen);
	}

	bool GoodsClear()
	{
		if(bRunning == true)
		{
			SetTimer(parentWindow, IMAGE_DOWNLOAD_WAIT_TIMER, 200, NULL);
			bThreadCancel = true;
			return false;
		}
		else
		{
			if (hThread != INVALID_HANDLE_VALUE)
			{
				CloseHandle(hThread);
			}

			Clear();

			GoodsData.clear();
			ListView_SetItemCount(hwndListView, 0);
		}
		return true;
	}

	void GoodsAdd(CGoodsData goods)
	{
		LVITEM lvI;				// list view item structure

		lvI.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM | LVIF_STATE;
		lvI.state = 0;      
		lvI.stateMask = 0;  
		lvI.iItem = GoodsData.size();
		lvI.iSubItem = 0;
		lvI.iImage = GoodsData.size();
		lvI.lParam = (LPARAM)&goods;

		if (ListView_InsertItem(hwndListView, &lvI) == -1)
			return;

		GoodsData.push_back(goods);
	}

	void GoodsAddEnd()
	{
		ListView_SetItemCount(hwndListView, GoodsData.size());

		hThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE ) run0,this,0,&dwThreadID);
		if (hThread == INVALID_HANDLE_VALUE)
		{
			return;
		}
	}

	void GoodsUpdate(CGoodsData& Goods)
	{
		// 업데이트해야 할 카툰을 검색하여 업데이트해준다.
		std::vector<CGoodsData>::iterator iterEnd = GoodsData.end();
		std::vector<CGoodsData>::iterator it = GoodsData.begin();

		// 전체 카툰 리스트에서 업데이트해야할 카툰을 찾는다. 
		// 이 부분은 나중에 개선해야 할 부분이 될 것이다.
		int index = 0;
		for( ; it != iterEnd; )
		{
			if( wcscmp((*it).tilte.c_str(), Goods.tilte.c_str()) == 0  )
			{
				break;
			}
			++index;
			++it;
		}

		// 깜빡임 현상으로 인해 나중에 업데이트 하도록 한다.
		//ListView_Update(hwndListView, index);
		KillTimer(parentWindow, 1);
		SetTimer(parentWindow, 1, 500, NULL);

		drawIndexBuffer.push_back(index);
	}

	CGoodsData* GetGoods(int iIndex)
	{
		if (iIndex < GoodsData.size())
		{
			return &GoodsData[iIndex];
		}
		return NULL;
	}

	int GetGoodsSize()
	{
		return GoodsData.size();
	}

	LRESULT OnDrawItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		HDC hdc;
		LPDRAWITEMSTRUCT lpdis;
		lpdis = (LPDRAWITEMSTRUCT)lParam;

		CGoodsData *Goods = &GoodsData.at(lpdis->itemID);
		if(Goods == NULL)
		{
			MessageBox(parentWindow, L"상품 데이터가 잘못 되었습니다.", L"에러", MB_OK);
			return 1;
		}

		if (lpdis->CtlType == ODT_LISTVIEW)
		{
			hdc = lpdis->hDC;

			COLORREF crOldTextColor = GetTextColor(hdc);
			COLORREF crOldBkColor = GetBkColor(hdc);

			// If this item is selected, set the background color 
			// and the text color to appropriate values. Also, erase
			// rect by filling it with the background color.
			HBRUSH hbrBkColor = CreateSolidBrush (crOldBkColor);
			HBRUSH hbrSEL = CreateSolidBrush (GetSysColor(COLOR_HIGHLIGHT));

			if ((lpdis->itemAction | ODA_SELECT) && (lpdis->itemState & ODS_SELECTED))
			{
				SetTextColor(hdc, GetSysColor(COLOR_HIGHLIGHTTEXT));
				FillRect(hdc, &lpdis->rcItem,hbrSEL);
			}
			else
			{
				FillRect(hdc, &lpdis->rcItem,hbrBkColor);
			}

			DeleteObject (hbrSEL);
			DeleteObject (hbrBkColor);

			// Reset the background color and the text color back to their
			// original values.
			SetTextColor(hdc, crOldTextColor);
			SetBkColor(hdc, crOldBkColor);

			RECT imageRect = { 0 };
			SetRect(&imageRect, lpdis->rcItem.left, lpdis->rcItem.top + 1,
				lpdis->rcItem.right / 4, lpdis->rcItem.bottom - 1);

			if(Goods->imageFilePath.size() != 0)
			{
				// 썸네일 이미지
				if(Goods->sampleImage.IsImageLoaded() == false)
				{
					Goods->sampleImage.LoadImage(Goods->imageFilePath.c_str());
				}
				if(Goods->sampleImage.GetWidth() != 0)
				{
					RECT rcSrc = { 0, 0, Goods->sampleImage.GetWidth(), Goods->sampleImage.GetHeight() };
					Goods->sampleImage.Draw(hdc, &imageRect, &rcSrc);
				}
			}

			TCHAR drawData[256];
			std::wstring priceData;

			// 제목
			RECT textRect = { 0 };
			SetRect(&textRect, imageRect.right + 5, lpdis->rcItem.top + 3,
				lpdis->rcItem.right - 30, lpdis->rcItem.top + 63);
			SetTextColor(hdc, RGB(0,0,0)); 
			DrawText(hdc, Goods->tilte.c_str(), wcslen(Goods->tilte.c_str()), &textRect, DT_LEFT | DT_TOP | DT_END_ELLIPSIS | DT_WORDBREAK | DT_EDITCONTROL);

			// 낮은 가격
			textRect.top+=58;
			textRect.bottom+=60;
			SetTextColor(hdc, RGB(0,0,230));
			priceData = CUtil::MakeComma(Goods->lprice);
			wsprintf(drawData, L"Low : %s", priceData.c_str());
			DrawText(hdc, drawData, wcslen(drawData), &textRect, DT_LEFT | DT_TOP | DT_END_ELLIPSIS | DT_WORDBREAK | DT_EDITCONTROL);

			// 높은 가격
			textRect.top+=28;
			textRect.bottom+=20;
			SetTextColor(hdc, RGB(230,0,0)); 
			priceData = CUtil::MakeComma(Goods->hprice);
			wsprintf(drawData, L"High : %s", priceData.c_str());
			DrawText(hdc, drawData, wcslen(drawData), &textRect, DT_LEFT | DT_TOP | DT_END_ELLIPSIS | DT_WORDBREAK | DT_EDITCONTROL);

			// 가장 하단에 라인 한줄 그어줘서 각 단을 구분할 수 있게 해준다.
			oldPen = (HPEN)SelectObject(hdc, linePen); 
			MoveToEx(hdc,0,imageRect.bottom,NULL); 
			LineTo(hdc, lpdis->rcItem.right, imageRect.bottom);
			SelectObject(hdc, oldPen); 

			ReleaseDC (hwndListView, hdc);
		}
		return 0;
	}

	void GoodsImageRelease()
	{
		std::vector<CGoodsData>::iterator iterEnd = GoodsData.end();
		std::vector<CGoodsData>::iterator it = GoodsData.begin();

		//IWMImage* image;
		//for( ; it != iterEnd; ++it)
		//{
		//	(*it)->GetThumbnail(&image);
		//	if(image != NULL)
		//	{
		//		image->UnloadImage();
		//	}
		//}
	}
	void GoodsImageRelease(int from, int to)
	{
		//IWMImage* image = NULL;
		//int index = from;
		//do 
		//{
		//	GoodsData.at(index)->GetThumbnail(&image);
		//	if(image != NULL)
		//	{
		//		image->UnloadImage();
		//	}
		//	++index;
		//} while(index <= to);
	}

	bool GetThreadRun()
	{
		return bRunning;
	}

private:
	std::vector<CGoodsData> GoodsData;
	HFONT m_hSmallFont, m_hMediumFont, hFontOld;
	CPen linePen;
	HPEN oldPen;

protected:
	static void run0(void *pParam)
	{
		((CListPageListView *)pParam)->Run(pParam);
	}
	virtual void Run(void *pParam)
	{
		bRunning = true;

		std::vector<CGoodsData>::iterator iterEnd = GoodsData.end();
		std::vector<CGoodsData>::iterator it = GoodsData.begin();

		int count = 0;
		for( ; it != iterEnd; ++it)
		{
			if(bThreadCancel == true)
			{
				bThreadCancel = false;
				break;
			}

			std::wstring strXMLFileName;
			CUtil::GetDefaultBasePath(strXMLFileName);
			strXMLFileName += L"Images\\";

			CString strTargetFilePath = (*it).imageUrl.c_str();

			// 파일의 절대 위치를 구하고 디렉토리가 없으면 생성한다.
			int pos = 0;
			int index = 0;
			int start = 0;
			while ((pos = strTargetFilePath.Find(L"/", start)) != -1)
			{			
				++index;
				if(index == 5)
				{
					strXMLFileName += strTargetFilePath.Right(strTargetFilePath.GetLength() - pos - 1);
					CUtil::StringReplace(&strXMLFileName, L"/", L"\\");

					// 이미지를 다운받을 폴더를 만든다
					if( CUtil::MakeDirectory(strXMLFileName.c_str()) == FALSE )
					{
						MessageBox(parentWindow,L"CreateDirectory Error", L"Error", MB_OK);
					}
					break;
				}
				start = pos+1;
			}

			// 파일이 없으면 파일을 다운로드 한다.
			if(CUtil::IsFileExist(strXMLFileName.c_str()) == FALSE)
			{
//				BOOL bReturn = CUtil::DownloadFile(strXMLFileName.c_str(), (*it).imageUrl.c_str());
				CCallback callback((*it).imageUrl.c_str(), GetTickCount() + 60 * 1000);
				HRESULT hr = ::URLDownloadToFile(NULL, (*it).imageUrl.c_str(), strXMLFileName.c_str(), 0, &callback);
				if(hr != S_OK)
				{
					SetCursor(LoadCursor(NULL, IDC_ARROW));
					MessageBox(parentWindow,L"네트워크 연결에 실패하였습니다.", L"Error", MB_OK);
					break;
				}
			}

			(*it).imageFilePath = strXMLFileName;
			SendMessage(parentWindow, WM_COMMAND, IMAGEFILE_DOWNLOADING, count);

			++count;
		}

		SendMessage(parentWindow, WM_COMMAND, IMAGEFILE_DOWNLOAD_COMPLETE, 0);
		bThreadCancel = false;
		bRunning = false;
	}

private:
	bool bRunning;
	HANDLE hThread;
	DWORD dwThreadID;

	bool bThreadCancel;
};

#endif