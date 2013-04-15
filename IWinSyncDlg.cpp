
// IWinSyncDlg.cpp : implementation file
//

#include "stdafx.h"
#include "IWinSync.h"
#include "IWinSyncDlg.h"
#include "afxdialogex.h"
#include "AboutBox.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CIWinSyncDlg dialog



CIWinSyncDlg::CIWinSyncDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CIWinSyncDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	//Start Of Tray Icon Iniit Code
	m_nidIconData.cbSize			= sizeof(NOTIFYICONDATA);
	
	m_nidIconData.hWnd				= 0;
	m_nidIconData.uID				= 1;

	m_nidIconData.uCallbackMessage	= WM_TRAY_ICON_NOTIFY_MESSAGE;

	m_nidIconData.hIcon				= 0;
	m_nidIconData.szTip[0]			= 0;	
	m_nidIconData.uFlags			= NIF_MESSAGE;

	m_bTrayIconVisible				= FALSE;

	m_nDefaultMenuItem				= 0;

	m_bMinimizeToTray				= TRUE;
	//End Of Tray Icon Init Code
}

void CIWinSyncDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CIWinSyncDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CREATE() //added tray icon
	ON_WM_DESTROY() //added tray icon
	ON_WM_SYSCOMMAND() //added tray icon
	ON_MESSAGE(WM_TRAY_ICON_NOTIFY_MESSAGE,OnTrayNotify) //addedtray icon
	ON_COMMAND(ID_TRAYMENU_STATUS, OnTraymenuStatus)
	ON_COMMAND(ID_TRAYMENU_SYNCCENTRE, OnTraymenuSyncCenter)
	ON_COMMAND(ID_TRAYMENU_ABOUT, OnTraymenuAbout)
	ON_COMMAND(ID_TRAYMENU_EXIT, OnTraymenuExit)
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
	//ShowWindow(SW_SHOWNORMAL);
	// TODO: Add extra initialization here
	TraySetIcon(IDR_MAINFRAME);
    TraySetToolTip(_T("Inteligent Windows Syncronisation"));
    TraySetMenu(IDR_MENU1);

	PostMessage(WM_SYSCOMMAND, SC_MINIMIZE);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CIWinSyncDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
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
	m_nidIconData.hWnd = this->m_hWnd;
	m_nidIconData.uID = 1;
	
	return 0;
}

void CIWinSyncDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	if(m_nidIconData.hWnd && m_nidIconData.uID>0 && TrayIsVisible())
	{
		Shell_NotifyIcon(NIM_DELETE,&m_nidIconData);
	}
}

BOOL CIWinSyncDlg::TrayIsVisible()
{
	return m_bTrayIconVisible;
}

void CIWinSyncDlg::TraySetIcon(HICON hIcon)
{
	ASSERT(hIcon);

	m_nidIconData.hIcon = hIcon;
	m_nidIconData.uFlags |= NIF_ICON;
}

void CIWinSyncDlg::TraySetIcon(UINT nResourceID)
{
	ASSERT(nResourceID>0);
	HICON hIcon = 0;
	hIcon = AfxGetApp()->LoadIcon(nResourceID);
	if(hIcon)
	{
		m_nidIconData.hIcon = hIcon;
		m_nidIconData.uFlags |= NIF_ICON;
	}
	else
	{
		TRACE0("FAILED TO LOAD ICON\n");
	}
}

void CIWinSyncDlg::TraySetIcon(LPCTSTR lpszResourceName)
{
	HICON hIcon = 0;
	hIcon = AfxGetApp()->LoadIcon(lpszResourceName);
	if(hIcon)
	{
		m_nidIconData.hIcon = hIcon;
		m_nidIconData.uFlags |= NIF_ICON;
	}
	else
	{
		TRACE0("FAILED TO LOAD ICON\n");
	}
}

void CIWinSyncDlg::TraySetToolTip(LPCTSTR lpszToolTip)
{
	ASSERT(_tcslen(lpszToolTip) > 0 && _tcslen(lpszToolTip) < 64);

	_tcscpy_s(m_nidIconData.szTip,lpszToolTip);
	m_nidIconData.uFlags |= NIF_TIP;
}

BOOL CIWinSyncDlg::TrayShow()
{
	BOOL bSuccess = FALSE;
	if(!m_bTrayIconVisible)
	{
		bSuccess = Shell_NotifyIcon(NIM_ADD,&m_nidIconData);
		if(bSuccess)
			m_bTrayIconVisible= TRUE;
	}
	else
	{
		TRACE0("ICON ALREADY VISIBLE");
	}
	return bSuccess;
}

BOOL CIWinSyncDlg::TrayHide()
{
	BOOL bSuccess = FALSE;
	if(m_bTrayIconVisible)
	{
		bSuccess = Shell_NotifyIcon(NIM_DELETE,&m_nidIconData);
		if(bSuccess)
		{
			m_bTrayIconVisible= FALSE;
		}
	}
	else
	{
		TRACE0("ICON ALREADY HIDDEN");
	}
	return bSuccess;
}

BOOL CIWinSyncDlg::TrayUpdate()
{
	BOOL bSuccess = FALSE;
	if(m_bTrayIconVisible)
	{
		bSuccess = Shell_NotifyIcon(NIM_MODIFY,&m_nidIconData);
	}
	else
	{
		TRACE0("ICON NOT VISIBLE");
	}
	return bSuccess;
}


BOOL CIWinSyncDlg::TraySetMenu(UINT nResourceID,UINT nDefaultPos)
{
	BOOL bSuccess;
	bSuccess = m_mnuTrayMenu.LoadMenu(nResourceID);
	return bSuccess;
}


BOOL CIWinSyncDlg::TraySetMenu(LPCTSTR lpszMenuName,UINT nDefaultPos)
{
	BOOL bSuccess;
	bSuccess = m_mnuTrayMenu.LoadMenu(lpszMenuName);
	return bSuccess;
}

BOOL CIWinSyncDlg::TraySetMenu(HMENU hMenu,UINT nDefaultPos)
{
	m_mnuTrayMenu.Attach(hMenu);
	return TRUE;
}

LRESULT CIWinSyncDlg::OnTrayNotify(WPARAM wParam, LPARAM lParam) 
{ 
    UINT uID; 
    UINT uMsg; 
 
    uID = (UINT) wParam; 
    uMsg = (UINT) lParam; 
 
	if (uID != 1)
	{
		return 0;
	}

	CPoint pt;	

    switch (uMsg ) 
	{ 
	case WM_MOUSEMOVE:
		GetCursorPos(&pt);
		ClientToScreen(&pt);
		OnTrayMouseMove(pt);
		break;
	case WM_LBUTTONDOWN:
		GetCursorPos(&pt);
		ClientToScreen(&pt);
		OnTrayLButtonDown(pt);
		break;
	case WM_LBUTTONDBLCLK:
		GetCursorPos(&pt);
		ClientToScreen(&pt);
		OnTrayLButtonDblClk(pt);
		break;
	
	case WM_RBUTTONDOWN:
	case WM_CONTEXTMENU:
		GetCursorPos(&pt);
		//ClientToScreen(&pt);
		OnTrayRButtonDown(pt);
		break;
	case WM_RBUTTONDBLCLK:
		GetCursorPos(&pt);
		ClientToScreen(&pt);
		OnTrayRButtonDblClk(pt);
		break;
    } 
     return 0; 
 } 
void CIWinSyncDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if(m_bMinimizeToTray)
	{
		if ((nID & 0xFFF0) == SC_MINIMIZE)
		{
		
			if( TrayShow())
			{
				this->ShowWindow(SW_HIDE);		
			}
		}
		else
			CDialog::OnSysCommand(nID, lParam);	
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}
void CIWinSyncDlg::TraySetMinimizeToTray(BOOL bMinimizeToTray)
{
	m_bMinimizeToTray = bMinimizeToTray;
}


void CIWinSyncDlg::OnTrayRButtonDown(CPoint pt)
{
	m_mnuTrayMenu.GetSubMenu(0)->TrackPopupMenu(TPM_BOTTOMALIGN|TPM_LEFTBUTTON|TPM_RIGHTBUTTON,pt.x,pt.y,this);
	m_mnuTrayMenu.GetSubMenu(0)->SetDefaultItem(m_nDefaultMenuItem,TRUE);
}

void CIWinSyncDlg::OnTrayLButtonDown(CPoint pt)
{

}

void CIWinSyncDlg::OnTrayLButtonDblClk(CPoint pt)
{
	if(m_bMinimizeToTray)
	{
		if(TrayHide())
		{
			this->ShowWindow(SW_SHOWNORMAL);
		}
	}
}

void CIWinSyncDlg::OnTrayRButtonDblClk(CPoint pt)
{
}

void CIWinSyncDlg::OnTrayMouseMove(CPoint pt)
{
}


void CIWinSyncDlg::OnTraymenuStatus() 
{
	if(m_bMinimizeToTray)
	{
		if(TrayHide())
		{
			this->ShowWindow(SW_SHOWNORMAL);
		}
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
