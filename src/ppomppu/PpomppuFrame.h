// PpomppuFrame.h : interface of the CPpomppuFrame class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

class CPpomppuFrame : 
	public CFrameWindowImpl<CPpomppuFrame>, 
	public CUpdateUI<CPpomppuFrame>,
	public CAppWindow<CPpomppuFrame>,
	public CMessageFilter, public CIdleHandler
{
public:

	DECLARE_APP_FRAME_CLASS(NULL, IDR_MAINFRAME, L"Software\\WTL\\Ppomppu")

	CPpomppuView m_view;

	CPpomppuFrame()
	{
		checkDisplayMenuID = ID_DISPLAY_10;
		unCheckDisplayMenuID = 0;

		checkSortMenuID = ID_SORT_SIMILARITY;
		unCheckSortMenuID = 0;
	}
	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		if(CFrameWindowImpl<CPpomppuFrame>::PreTranslateMessage(pMsg))
			return TRUE; 

		return m_view.IsWindow() ? m_view.PreTranslateMessage(pMsg) : FALSE;
	}

// CAppWindow operations
	bool AppHibernate( bool bHibernate)
	{
		// Insert your code here or delete member if not relevant
		return bHibernate;
	}

	bool AppNewInstance( LPCTSTR lpstrCmdLine)
	{
		// Insert your code here or delete member if not relevant
		return false;
	}

	void AppSave()
	{
		CAppInfo info;
		// Insert your code here
	}

	virtual BOOL OnIdle()
	{
		UIUpdateToolBar();
		return FALSE;
	}

	BEGIN_UPDATE_UI_MAP(CPpomppuFrame)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CPpomppuFrame)
		COMMAND_ID_HANDLER(ID_APP_EXIT, OnFileExit)
		COMMAND_ID_HANDLER(ID_FILE_NEW, OnFileNew)
		COMMAND_ID_HANDLER(ID_ACTION, OnAction)
		COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)

		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_INITMENUPOPUP, OnInitPopup)
		MESSAGE_HANDLER(WM_COMMAND, OnCommand)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		CHAIN_MSG_MAP(CAppWindow<CPpomppuFrame>)
		CHAIN_MSG_MAP(CUpdateUI<CPpomppuFrame>)
		CHAIN_MSG_MAP(CFrameWindowImpl<CPpomppuFrame>)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		CAppInfo info;

		CreateSimpleCEMenuBar();
		UIAddToolBar(m_hWndCECommandBar);

		RECT clientRect;
		GetClientRect(&clientRect);
		m_hWndClient = m_view.Create(m_hWnd, clientRect, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

		// register object for message filtering and idle updates
		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->AddMessageFilter(this);
		pLoop->AddIdleHandler(this);

		return 0;
	}

	LRESULT OnInitPopup(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		if(unCheckDisplayMenuID != checkDisplayMenuID)
		{
			CheckMenuItem((HMENU)wParam, unCheckDisplayMenuID,MF_UNCHECKED);
			CheckMenuItem((HMENU)wParam, checkDisplayMenuID,MF_CHECKED);
			unCheckDisplayMenuID = checkDisplayMenuID;
		}
		if(unCheckSortMenuID != checkSortMenuID)
		{
			CheckMenuItem((HMENU)wParam, unCheckSortMenuID,MF_UNCHECKED);
			CheckMenuItem((HMENU)wParam, checkSortMenuID,MF_CHECKED);
			unCheckSortMenuID = checkSortMenuID;
		}
		return 0;
	}

	LRESULT OnCommand(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		switch (LOWORD(wParam))
		{
		case ID_DISPLAY_10:
			m_view.SetDisplayCountOption(10);
			checkDisplayMenuID = ID_DISPLAY_10;
			break;
		case ID_DISPLAY_30:
			m_view.SetDisplayCountOption(30);
			checkDisplayMenuID = ID_DISPLAY_30;
			break;
		case ID_DISPLAY_50:
			m_view.SetDisplayCountOption(50);
			checkDisplayMenuID = ID_DISPLAY_50;
			break;
		case ID_DISPLAY_75:
			m_view.SetDisplayCountOption(75);
			checkDisplayMenuID = ID_DISPLAY_75;
			break;
		case ID_DISPLAY_100:
			m_view.SetDisplayCountOption(100);
			checkDisplayMenuID = ID_DISPLAY_100;
			break;
		case ID_SORT_SIMILARITY:
			m_view.SetSortOption(L"sim");
			checkSortMenuID = ID_SORT_SIMILARITY;
			break;
		case ID_SORT_UPDATE:
			m_view.SetSortOption(L"date");
			checkSortMenuID = ID_SORT_UPDATE;
			break;
		case ID_SORT_HIGHPRICE:
			m_view.SetSortOption(L"dsc");
			checkSortMenuID = ID_SORT_HIGHPRICE;
			break;
		case ID_SORT_LOWPRICE:
			m_view.SetSortOption(L"asc");
			checkSortMenuID = ID_SORT_LOWPRICE;
			break;
		}
		return 0;
	}

	LRESULT OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		// 최소화 되지 않고 프로그램을 종료 시킨다.
		if(wParam == SIZE_MINIMIZED)
		{
			::PostMessage(m_hWnd,  WM_CLOSE, 0, 0);
		}
		else
		{
			//int nWidth = LOWORD(lParam); 
			//int nHeight = HIWORD(lParam);

			//::MoveWindow(m_hWnd, 0, GetSystemMetrics(SM_CYCAPTION), nWidth, nHeight, TRUE);
		}
		return 0;
	}

	LRESULT OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		PostMessage(WM_CLOSE);
		return 0;
	}

	LRESULT OnFileNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		// TODO: add code to initialize document

		return 0;
	}

	LRESULT OnAction(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		// TODO: add code

		return 0;
	}

	LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CAboutDlg dlg;
		dlg.DoModal();
		return 0;
	}
private:
	int checkDisplayMenuID;
	int unCheckDisplayMenuID;

	int checkSortMenuID;
	int unCheckSortMenuID;
};
