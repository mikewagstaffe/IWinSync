
// IWinSyncDlg.cpp : implementation file
//

#include "stdafx.h"
#include "IWinSync.h"
#include "IWinSyncDlg.h"
#include "afxdialogex.h"
#include "AboutBox.h"
#include "resource.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CIWinSyncDlg dialog



CIWinSyncDlg::CIWinSyncDlg(CWnd* pParent /*=NULL*/) : CDialogEx(CIWinSyncDlg::IDD, pParent)
{
	//End Of Tray Icon Init Code
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	g_hInst = AfxGetInstanceHandle();
}

void CIWinSyncDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CIWinSyncDlg, CDialogEx)
	ON_WM_QUERYDRAGICON()
	ON_WM_CREATE() //On Create Of The Dialog
	ON_WM_DESTROY() //On destroy Of the Dialog
	ON_MESSAGE(WMAPP_NOTIFYCALLBACK,OnTrayNotify)
	ON_WM_SYSCOMMAND() //Hook The Minimize request
	ON_COMMAND(ID_TRAYMENU_STATUS, OnTraymenuStatus)
	ON_COMMAND(ID_TRAYMENU_SYNCCENTRE, OnTraymenuSyncCenter)
	ON_COMMAND(ID_TRAYMENU_ABOUT, OnTraymenuAbout)
	ON_COMMAND(ID_TRAYMENU_EXIT, OnTraymenuExit)
	ON_WM_TIMER()
	ON_MESSAGE(WMAPP_HIDEFLYOUT, OnWmappHideflyout)
END_MESSAGE_MAP()


// CIWinSyncDlg message handlers

BOOL CIWinSyncDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	ShowWindow(SW_SHOWMINIMIZED);
	m_bMinimizeToTray = FALSE;
	m_bCanShowFlyout = TRUE;
	PostMessage(WM_SYSCOMMAND, SC_MINIMIZE);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CIWinSyncDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

int CIWinSyncDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}

	AddNotificationIcon(this->m_hWnd);
	return 0;
}

void CIWinSyncDlg::OnDestroy() 
{
	DeleteNotificationIcon();
	CDialog::OnDestroy();
	
}

void CIWinSyncDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if(!m_bMinimizeToTray)
	{
		if ((nID & 0xFFF0) == SC_MINIMIZE)
		{
			this->ShowWindow(SW_HIDE);
			m_bMinimizeToTray = TRUE;
		}
		else
			CDialog::OnSysCommand(nID, lParam);	
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

void CIWinSyncDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if (nIDEvent == HIDEFLYOUT_TIMER_ID)
        {
            // please see the comment in HideFlyout() for an explanation of this code.
            KillTimer(HIDEFLYOUT_TIMER_ID);
            m_bCanShowFlyout = TRUE;
        }
	CDialogEx::OnTimer(nIDEvent);
}


BOOL CIWinSyncDlg::AddNotificationIcon(HWND hwnd)
{
    NOTIFYICONDATA nid = {sizeof(nid)};
    nid.hWnd = hwnd;
    // add the icon, setting the icon, tooltip, and callback message.
    // the icon will be identified with the GUID
    nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE | NIF_SHOWTIP | NIF_GUID;
    nid.guidItem = __uuidof(CIWinSyncDlg);
    nid.uCallbackMessage = WMAPP_NOTIFYCALLBACK;
    LoadIconMetric(g_hInst, MAKEINTRESOURCE(IDR_MAINFRAME), LIM_SMALL, &nid.hIcon);
    LoadString(g_hInst, IDS_TOOLTIP, nid.szTip, ARRAYSIZE(nid.szTip));
    Shell_NotifyIcon(NIM_ADD, &nid);

    // NOTIFYICON_VERSION_4 is prefered
    nid.uVersion = NOTIFYICON_VERSION_4;
    return Shell_NotifyIcon(NIM_SETVERSION, &nid);
}

BOOL CIWinSyncDlg::DeleteNotificationIcon()
{
    NOTIFYICONDATA nid = {sizeof(nid)};
    nid.uFlags = NIF_GUID;
    nid.guidItem = __uuidof(CIWinSyncDlg);
    return Shell_NotifyIcon(NIM_DELETE, &nid);
	return TRUE;
}

void CIWinSyncDlg::ShowContextMenu(HWND hwnd, POINT pt)
{
    HMENU hMenu = LoadMenu(g_hInst, MAKEINTRESOURCE(IDC_CONTEXTMENU));
    if (hMenu)
    {
        HMENU hSubMenu = GetSubMenu(hMenu, 0);
        if (hSubMenu)
        {
            // our window must be foreground before calling TrackPopupMenu or the menu will not disappear when the user clicks away
            SetForegroundWindow();

            // respect menu drop alignment
            UINT uFlags = TPM_RIGHTBUTTON;
            if (GetSystemMetrics(SM_MENUDROPALIGNMENT) != 0)
            {
                uFlags |= TPM_RIGHTALIGN;
            }
            else
            {
                uFlags |= TPM_LEFTALIGN;
            }

            TrackPopupMenuEx(hSubMenu, uFlags, pt.x, pt.y, hwnd, NULL);
		}
		DestroyMenu(hMenu);
	}
}

BOOL CIWinSyncDlg::ShowConflictBalloon()
{
    // Display a low ink balloon message. This is a warning, so show the appropriate system icon.
    NOTIFYICONDATA nid = {sizeof(nid)};
    nid.uFlags = NIF_INFO | NIF_GUID;
    nid.guidItem = __uuidof(CIWinSyncDlg);
    // respect quiet time since this balloon did not come from a direct user action.
    nid.dwInfoFlags = NIIF_WARNING | NIIF_RESPECT_QUIET_TIME;
    LoadString(g_hInst, IDS_CONFLICT_TITLE, nid.szInfoTitle, ARRAYSIZE(nid.szInfoTitle));
    LoadString(g_hInst, IDS_CONFLICT_TEXT, nid.szInfo, ARRAYSIZE(nid.szInfo));
    return Shell_NotifyIcon(NIM_MODIFY, &nid);
}

BOOL CIWinSyncDlg::ShowErrorBalloon()
{
    // Display an out of ink balloon message. This is a error, so show the appropriate system icon.
    NOTIFYICONDATA nid = {sizeof(nid)};
    nid.uFlags = NIF_INFO | NIF_GUID;
    nid.guidItem = __uuidof(CIWinSyncDlg);
    nid.dwInfoFlags = NIIF_ERROR;
    LoadString(g_hInst, IDS_SYNCERROR_TITLE, nid.szInfoTitle, ARRAYSIZE(nid.szInfoTitle));
    LoadString(g_hInst, IDS_SYNCERROR_TEXT, nid.szInfo, ARRAYSIZE(nid.szInfo));
    return Shell_NotifyIcon(NIM_MODIFY, &nid);
}

BOOL CIWinSyncDlg::ShowStatusBalloon()
{
    // Display a balloon message for a print job with a custom icon
    NOTIFYICONDATA nid = {sizeof(nid)};
    nid.uFlags = NIF_INFO | NIF_GUID;
    nid.guidItem = __uuidof(CIWinSyncDlg);
    nid.dwInfoFlags = NIIF_USER | NIIF_LARGE_ICON;
    LoadString(g_hInst, IDS_STATUS_TITLE, nid.szInfoTitle, ARRAYSIZE(nid.szInfoTitle));
    LoadString(g_hInst, IDS_STATUS_TEXT, nid.szInfo, ARRAYSIZE(nid.szInfo));
    LoadIconMetric(g_hInst, MAKEINTRESOURCE(IDR_MAINFRAME), LIM_LARGE, &nid.hBalloonIcon);
    return Shell_NotifyIcon(NIM_MODIFY, &nid);
}

BOOL CIWinSyncDlg::RestoreTooltip()
{
    // After the balloon is dismissed, restore the tooltip.
    NOTIFYICONDATA nid = {sizeof(nid)};
    nid.uFlags = NIF_SHOWTIP | NIF_GUID;
    nid.guidItem = __uuidof(CIWinSyncDlg);
    return Shell_NotifyIcon(NIM_MODIFY, &nid);
}

LRESULT CIWinSyncDlg::OnTrayNotify(WPARAM wParam, LPARAM lParam) 
{
	 switch (LOWORD(lParam))
	 {
		case NIN_SELECT:
		//	// for NOTIFYICON_VERSION_4 clients, NIN_SELECT is prerable to listening to mouse clicks and key presses
		//	// directly.
		if (m_pFlyoutDialog != NULL)
		{
			if (m_pFlyoutDialog->IsWindowVisible())
			{
				HideFlyout();
				m_bCanShowFlyout = FALSE;
			}
			else if (m_bCanShowFlyout)
			{
				ShowFlyout();
			}
		}
		else
		{
			ShowFlyout();
		}
		break;

		case NIN_BALLOONTIMEOUT:
			RestoreTooltip();
			break;

        case NIN_BALLOONUSERCLICK:
            RestoreTooltip();
            // placeholder for the user clicking on the balloon.
            MessageBox(L"The user clicked on the balloon.", L"User click", MB_OK);
			break;

        case WM_CONTEXTMENU:
	       POINT const pt = { LOWORD(wParam), HIWORD(wParam) };
		   ShowContextMenu(this->m_hWnd, pt);
		   break;
	 }
	 return 0;
}

void CIWinSyncDlg::OnTraymenuStatus() 
{
	if(m_bMinimizeToTray)
	{
		this->ShowWindow(SW_SHOWNORMAL);
		m_bMinimizeToTray = FALSE;
	}
}

void CIWinSyncDlg::OnTraymenuSyncCenter()  
{
	TCHAR szWindowsDirectory[MAX_PATH];
	if( !GetWindowsDirectory(szWindowsDirectory,MAX_PATH))
	{
		//Failed to get the windows directory so fail
		return;
	}

	TCHAR szSyncCenterPath[MAX_PATH];
	_stprintf_s(szSyncCenterPath,_T("%s\\System32\\mobsync.exe"),szWindowsDirectory );
	ShellExecute(NULL,_T("open"),szSyncCenterPath,NULL,NULL,SW_SHOW);
}

void CIWinSyncDlg::OnTraymenuAbout()  
{
	CAboutBox Aboutdlg;
	_stprintf_s(Aboutdlg.m_szVersion,_T("%s"),VERSIONTEXT );
	Aboutdlg.DoModal();
}

void CIWinSyncDlg::OnTraymenuExit()  
{
	this->PostMessageW(WM_CLOSE,0,0);
}


void CIWinSyncDlg::PositionFlyout(REFGUID guidIcon)
{
	// find the position of our printer icon
    NOTIFYICONIDENTIFIER nii = {sizeof(nii)};
    nii.guidItem = guidIcon;
    RECT rcIcon;
    HRESULT hr = Shell_NotifyIconGetRect(&nii, &rcIcon);
    if (SUCCEEDED(hr))
    {
        // display the flyout in an appropriate position close to the printer icon
		POINT const ptAnchor = { (rcIcon.left + rcIcon.right) / 2, (rcIcon.top + rcIcon.bottom)/2 };

		RECT rcWindow;
		m_pFlyoutDialog->GetWindowRect(&rcWindow);
		SIZE sizeWindow = {rcWindow.right - rcWindow.left, rcWindow.bottom - rcWindow.top};
		
		if (CalculatePopupWindowPosition(&ptAnchor, &sizeWindow, TPM_VERTICAL | TPM_VCENTERALIGN | TPM_CENTERALIGN | TPM_WORKAREA, &rcIcon, &rcWindow))
		{
            // position the flyout and make it the foreground window
            m_pFlyoutDialog->SetWindowPos(&CWnd::wndTopMost, rcWindow.left, rcWindow.top, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
		}
    }
}

void CIWinSyncDlg::ShowFlyout()
{
	if (m_pFlyoutDialog == NULL)
	{
		m_pFlyoutDialog = new CFlyoutDlg();
		m_pFlyoutDialog->Create(IDD_FLYOUT, this);
	}
	
	m_pFlyoutDialog->m_HideFlyoutMessage = WMAPP_HIDEFLYOUT;
	PositionFlyout(__uuidof(CIWinSyncDlg));		
	m_pFlyoutDialog->ShowWindow(SW_NORMAL);
}

void CIWinSyncDlg::HideFlyout()
{
	m_pFlyoutDialog->ShowWindow(SW_HIDE);
	m_pFlyoutDialog->DestroyWindow();
	m_pFlyoutDialog = NULL;
   
    // immediately after hiding the flyout we don't want to allow showing it again, which will allow clicking
    // on the icon to hide the flyout. If we didn't have this code, clicking on the icon when the flyout is open
    // would cause the focus change (from flyout to the taskbar), which would trigger hiding the flyout
    // (see the WM_ACTIVATE handler). Since the flyout would then be hidden on click, it would be shown again instead
    // of hiding.
    SetTimer(HIDEFLYOUT_TIMER_ID, GetDoubleClickTime(), NULL);
}

afx_msg LRESULT CIWinSyncDlg::OnWmappHideflyout(WPARAM wParam, LPARAM lParam)
{
	HideFlyout();
	return 0;
}
















//int CIWinSyncDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
//{
//	if (CDialog::OnCreate(lpCreateStruct) == -1)
//	{
//		return -1;
//	}
//	m_nidIconData.hWnd = this->m_hWnd;
//	m_nidIconData.uID = 1;
//	
//	return 0;
//}
//
//void CIWinSyncDlg::OnDestroy() 
//{
//	CDialog::OnDestroy();
//	if(m_nidIconData.hWnd && m_nidIconData.uID>0 && TrayIsVisible())
//	{
//		Shell_NotifyIcon(NIM_DELETE,&m_nidIconData);
//	}
//}

//BOOL CIWinSyncDlg::TrayIsVisible()
//{
//	return m_bTrayIconVisible;
//}
//
//void CIWinSyncDlg::TraySetIcon(HICON hIcon)
//{
//	ASSERT(hIcon);
//
//	m_nidIconData.hIcon = hIcon;
//	m_nidIconData.uFlags |= NIF_ICON;
//}
//
//void CIWinSyncDlg::TraySetIcon(UINT nResourceID)
//{
//	ASSERT(nResourceID>0);
//	HICON hIcon = 0;
//	hIcon = AfxGetApp()->LoadIcon(nResourceID);
//	if(hIcon)
//	{
//		m_nidIconData.hIcon = hIcon;
//		m_nidIconData.uFlags |= NIF_ICON;
//	}
//	else
//	{
//		TRACE0("FAILED TO LOAD ICON\n");
//	}
//}
//
//void CIWinSyncDlg::TraySetIcon(LPCTSTR lpszResourceName)
//{
//	HICON hIcon = 0;
//	hIcon = AfxGetApp()->LoadIcon(lpszResourceName);
//	if(hIcon)
//	{
//		m_nidIconData.hIcon = hIcon;
//		m_nidIconData.uFlags |= NIF_ICON;
//	}
//	else
//	{
//		TRACE0("FAILED TO LOAD ICON\n");
//	}
//}
//
//void CIWinSyncDlg::TraySetToolTip(LPCTSTR lpszToolTip)
//{
//	ASSERT(_tcslen(lpszToolTip) > 0 && _tcslen(lpszToolTip) < 64);
//
//	_tcscpy_s(m_nidIconData.szTip,lpszToolTip);
//	m_nidIconData.uFlags |= NIF_TIP;
//}
//
//BOOL CIWinSyncDlg::TrayShow()
//{
//	BOOL bSuccess = FALSE;
//	if(!m_bTrayIconVisible)
//	{
//		bSuccess = Shell_NotifyIcon(NIM_ADD,&m_nidIconData);
//		if(bSuccess)
//		{
//			m_bTrayIconVisible= TRUE;
//		}
//		// Set the version
//		//Shell_NotifyIcon(NIM_SETVERSION, &m_nidIconData);
//	}
//	else
//	{
//		TRACE0("ICON ALREADY VISIBLE");
//	}
//	return bSuccess;
//}
//
//BOOL CIWinSyncDlg::TrayHide()
//{
//	BOOL bSuccess = FALSE;
//	if(m_bTrayIconVisible)
//	{
//		bSuccess = Shell_NotifyIcon(NIM_DELETE,&m_nidIconData);
//		if(bSuccess)
//		{
//			m_bTrayIconVisible= FALSE;
//		}
//	}
//	else
//	{
//		TRACE0("ICON ALREADY HIDDEN");
//	}
//	return bSuccess;
//}
//
//BOOL CIWinSyncDlg::TrayUpdate()
//{
//	BOOL bSuccess = FALSE;
//	if(m_bTrayIconVisible)
//	{
//		bSuccess = Shell_NotifyIcon(NIM_MODIFY,&m_nidIconData);
//	}
//	else
//	{
//		TRACE0("ICON NOT VISIBLE");
//	}
//	return bSuccess;
//}
//
//
//BOOL CIWinSyncDlg::TraySetMenu(UINT nResourceID,UINT nDefaultPos)
//{
//	BOOL bSuccess;
//	bSuccess = m_mnuTrayMenu.LoadMenu(nResourceID);
//	return bSuccess;
//}
//
//
//BOOL CIWinSyncDlg::TraySetMenu(LPCTSTR lpszMenuName,UINT nDefaultPos)
//{
//	BOOL bSuccess;
//	bSuccess = m_mnuTrayMenu.LoadMenu(lpszMenuName);
//	return bSuccess;
//}
//
//BOOL CIWinSyncDlg::TraySetMenu(HMENU hMenu,UINT nDefaultPos)
//{
//	m_mnuTrayMenu.Attach(hMenu);
//	return TRUE;
//}
//
//LRESULT CIWinSyncDlg::OnTrayNotify(WPARAM wParam, LPARAM lParam) 
//{ 
//    UINT uID; 
//    UINT uMsg; 
// 
//    uID = (UINT) wParam; 
//    uMsg = (UINT) lParam; 
// 
//	if (uID != 1)
//	{
//		return 0;
//	}
//
//	CPoint pt;	
//
//    switch (uMsg ) 
//	{ 
//	case WM_MOUSEMOVE:
//		GetCursorPos(&pt);
//		ClientToScreen(&pt);
//		OnTrayMouseMove(pt);
//		break;
//	case WM_LBUTTONDOWN:
//		GetCursorPos(&pt);
//		ClientToScreen(&pt);
//		OnTrayLButtonDown(pt);
//		break;
//	case WM_LBUTTONDBLCLK:
//		GetCursorPos(&pt);
//		ClientToScreen(&pt);
//		OnTrayLButtonDblClk(pt);
//		break;
//	
//	case WM_RBUTTONDOWN:
//	case WM_CONTEXTMENU:
//		GetCursorPos(&pt);
//		//ClientToScreen(&pt);
//		OnTrayRButtonDown(pt);
//		break;
//	case WM_RBUTTONDBLCLK:
//		GetCursorPos(&pt);
//		ClientToScreen(&pt);
//		OnTrayRButtonDblClk(pt);
//		break;
//    } 
//     return 0; 
// } 
//void CIWinSyncDlg::OnSysCommand(UINT nID, LPARAM lParam)
//{
//	if(m_bMinimizeToTray)
//	{
//		if ((nID & 0xFFF0) == SC_MINIMIZE)
//		{
//		
//			if( TrayShow())
//			{
//				this->ShowWindow(SW_HIDE);		
//			}
//		}
//		else
//			CDialog::OnSysCommand(nID, lParam);	
//	}
//	else
//	{
//		CDialog::OnSysCommand(nID, lParam);
//	}
//}
//void CIWinSyncDlg::TraySetMinimizeToTray(BOOL bMinimizeToTray)
//{
//	m_bMinimizeToTray = bMinimizeToTray;
//}
//
//
//void CIWinSyncDlg::OnTrayRButtonDown(CPoint pt)
//{
//	m_mnuTrayMenu.GetSubMenu(0)->TrackPopupMenu(TPM_BOTTOMALIGN|TPM_LEFTBUTTON|TPM_RIGHTBUTTON,pt.x,pt.y,this);
//	m_mnuTrayMenu.GetSubMenu(0)->SetDefaultItem(m_nDefaultMenuItem,TRUE);
//}
//
//void CIWinSyncDlg::OnTrayLButtonDown(CPoint pt)
//{
//
//}
//
//void CIWinSyncDlg::OnTrayLButtonDblClk(CPoint pt)
//{
//	if(m_bMinimizeToTray)
//	{
//		if(TrayHide())
//		{
//			this->ShowWindow(SW_SHOWNORMAL);
//		}
//	}
//}
//
//void CIWinSyncDlg::OnTrayRButtonDblClk(CPoint pt)
//{
//}
//
//void CIWinSyncDlg::OnTrayMouseMove(CPoint pt)
//{
//}
//
//
//void CIWinSyncDlg::OnTraymenuStatus() 
//{
//	if(m_bMinimizeToTray)
//	{
//		if(TrayHide())
//		{
//			this->ShowWindow(SW_SHOWNORMAL);
//		}
//	}
//}
//
//void CIWinSyncDlg::OnTraymenuSyncCenter()  
//{
//	TCHAR szWindowsDirectory[MAX_PATH];
//	if( !GetWindowsDirectory(szWindowsDirectory,MAX_PATH))
//	{
//		//Failed to get the windows directory so fail
//		return;
//	}
//
//	TCHAR szSyncCenterPath[MAX_PATH];
//	_stprintf_s(szSyncCenterPath,_T("%s\\System32\\mobsync.exe"),szWindowsDirectory );
//	ShellExecute(NULL,_T("open"),szSyncCenterPath,NULL,NULL,SW_SHOW);
//}
//
//void CIWinSyncDlg::OnTraymenuAbout()  
//{
//	CAboutBox Aboutdlg;
//	_stprintf_s(Aboutdlg.m_szVersion,_T("%s"),VERSIONTEXT );
//	Aboutdlg.DoModal();
//}
//
//void CIWinSyncDlg::OnTraymenuExit()  
//{
//	this->PostMessageW(WM_CLOSE,0,0);
//}
