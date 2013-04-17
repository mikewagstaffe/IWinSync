
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
	ON_WM_ACTIVATE()
	ON_MESSAGE(WMAPP_NOTIFYCALLBACK,OnTrayNotify)
	ON_WM_SYSCOMMAND() //Hook The Minimize request
	ON_COMMAND(ID_TRAYMENU_STATUS, OnTraymenuStatus)
	ON_COMMAND(ID_TRAYMENU_SYNCCENTRE, OnTraymenuSyncCenter)
	ON_COMMAND(ID_TRAYMENU_ABOUT, OnTraymenuAbout)
	ON_COMMAND(ID_TRAYMENU_EXIT, OnTraymenuExit)
	ON_WM_TIMER()
	ON_MESSAGE(WMAPP_HIDEFLYOUT, OnWmappHideflyout)
	ON_BN_CLICKED(IDC_MINIMISE, &CIWinSyncDlg::OnBnClickedMinimise)
	ON_BN_CLICKED(IDC_REVIEWCONFLICT, &CIWinSyncDlg::OnBnClickedReviewconflict)
	ON_BN_CLICKED(IDC_REVIEWLOG, &CIWinSyncDlg::OnBnClickedReviewlog)
END_MESSAGE_MAP()


// CIWinSyncDlg message handlers

BOOL CIWinSyncDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	m_bLoggingEnabled = FALSE; // We have not obtained log details yet
	SetupLogging();

	//Populate the Debug Level Listbox
	CListBox* pDebugLevelList = NULL;
	pDebugLevelList = (CListBox*) GetDlgItem(IDC_LIST_LOGLEVEL);
	if (pDebugLevelList != NULL)
	{
		pDebugLevelList->AddString(LOGLEVEL_LISTBOX_DEBUG);
		pDebugLevelList->AddString(LOGLEVEL_LISTBOX_WARN);
		pDebugLevelList->AddString(LOGLEVEL_LISTBOX_ERROR);
	}
	//Load Application Settings from the registry
	ReadRegistrySettings();
	m_bSuppressDialogs = FALSE;
	m_bDisableSync = FALSE;
	PopulateSettingsDialog();
	
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
	if (m_pszCurrentSyncPath != NULL)
	{
		free(m_pszCurrentSyncPath);
	}
	if (m_pszLogPath != NULL)
	{
		free(m_pszLogPath);
	}
	for(int i=0;i<5;i++)
	{
		if(m_apszLastResults[i] != NULL)
		{
			free(m_apszLastResults[i]);
		}
	}
	if (m_pFlyoutDialog != NULL)
	{
		m_pFlyoutDialog->DestroyWindow();
		delete(m_pFlyoutDialog);
		m_pFlyoutDialog = NULL;
	}
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


void CIWinSyncDlg::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CDialogEx::OnActivate(nState, pWndOther, bMinimized);

	if (nState == WA_ACTIVE || nState == WA_CLICKACTIVE)
	{
		PopulateSettingsDialog();
	}
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
	delete(m_pFlyoutDialog);
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

void CIWinSyncDlg::SetupLogging()
{
	m_bLoggingEnabled = FALSE;

	//Read The Current Log Path
	ReadRegStringValue(_T("Software\\IWinSync"), _T("LogPath"),&m_pszLogPath);
	if(m_pszLogPath !=NULL) 
	{
		_stprintf_s(m_pszConflictLogPath,MAX_PATH,_T("%s\\%s"),m_pszLogPath,CONFLICT_LOG_NAME);
		_stprintf_s(m_pszAppLogPath,MAX_PATH,_T("%s\\%s"),m_pszLogPath,APP_LOG_NAME);
	}
	m_bLoggingEnabled = TRUE;
}

void CIWinSyncDlg::ReadRegistrySettings()
{
	m_pszCurrentSyncPath = NULL;
	m_apszLastResults[0] = NULL;
	m_apszLastResults[1] = NULL;
	m_apszLastResults[2] = NULL;
	m_apszLastResults[3] = NULL;
	m_apszLastResults[4] = NULL;

	//Read The current SyncPath
	ReadRegStringValue(_T("Software\\IWinSync"), _T("CurSyncPath"),&m_pszCurrentSyncPath);
	
	//Read The Result Strings
	ReadRegStringValue(_T("Software\\IWinSync"), _T("LastResult1"),&m_apszLastResults[0]);
	ReadRegStringValue(_T("Software\\IWinSync"), _T("LastResult2"),&m_apszLastResults[1]);
	ReadRegStringValue(_T("Software\\IWinSync"), _T("LastResult3"),&m_apszLastResults[2]);
	ReadRegStringValue(_T("Software\\IWinSync"), _T("LastResult4"),&m_apszLastResults[3]);
	ReadRegStringValue(_T("Software\\IWinSync"), _T("LastResult5"),&m_apszLastResults[4]);

	//Read The Sync Interval
	DWORD dwSyncInterval = ReadRegDWordValue(_T("Software\\IWinSync"), _T("SyncInterval"));
	m_nSyncInterval = dwSyncInterval * 60000; //Convert from minutes to milliseconds
	if (m_nSyncInterval < 10000) //If for some reason we calulate the interval to be less than 10 seconds then force ten seconds as the interval
	{
		m_nSyncInterval = DEFAULT_SYNC_INTERVAL
	}

	//Read The Current Log Level
	m_dwLogLevel = ReadRegDWordValue(_T("Software\\IWinSync"), _T("LogLevel"));
}

void CIWinSyncDlg::PopulateSettingsDialog()
{
	//Set the Current Sync Path
	CEdit* pPathEditBox = NULL;
	pPathEditBox = (CEdit*) GetDlgItem(IDC_EDIT_PATH);
	if (pPathEditBox != NULL)
	{
		pPathEditBox->SetWindowTextW(m_pszCurrentSyncPath);
	}
	pPathEditBox = NULL;

	//Set the Current Sync Interval
	CEdit* pIntervalEditBox = NULL;
	pIntervalEditBox = (CEdit*) GetDlgItem(IDC_EDIT_INTERVAL);
	if (pIntervalEditBox != NULL)
	{
		TCHAR szSyncInterval[50];
		_stprintf_s(szSyncInterval,50,_T("%u"),(m_nSyncInterval/60000));
		pIntervalEditBox->SetWindowTextW(szSyncInterval);
	}
	pIntervalEditBox = NULL;


	//Set the Suppress Dialogs Check box
	CButton* pEnableDialogsBtn = (CButton*) GetDlgItem(IDC_CHECK_SUPPRESS);
	if (pEnableDialogsBtn != NULL)
	{
		pEnableDialogsBtn->SetCheck(m_bSuppressDialogs);
	}
	pEnableDialogsBtn = NULL;

	//Set the Disable Sync Check box
	CButton* pDisableSyncBtn = (CButton*) GetDlgItem(IDC_CHECK_DISABLE);
	if (pDisableSyncBtn != NULL)
	{
		pDisableSyncBtn->SetCheck(m_bDisableSync);
	}
	pEnableDialogsBtn = NULL;

	//Set The Last Result Strings
	for (int i=0;i<5;i++)
	{
		CStatic *pResultText = NULL;
		switch(i)
		{
		case 0:
			pResultText = (CStatic*) GetDlgItem(IDC_RESULT1);
			break;
		case 1:
			pResultText = (CStatic*) GetDlgItem(IDC_RESULT2);
			break;
		case 2:
			pResultText = (CStatic*) GetDlgItem(IDC_RESULT3);
			break;
		case 3:
			pResultText = (CStatic*) GetDlgItem(IDC_RESULT4);
			break;
		case 4:
			pResultText = (CStatic*) GetDlgItem(IDC_RESULT5);
			break;
		}
		if (pResultText != NULL)
		{
			if (m_apszLastResults[i] != NULL)
			{
				pResultText->SetWindowTextW(m_apszLastResults[i]);
			}
			else
			{
				pResultText->SetWindowTextW(_T(""));
			}
		}
		pResultText = NULL;
	}

	//Select The Current Logging Level
	CListBox * pLoggingLevelList = (CListBox*) GetDlgItem(IDC_LIST_LOGLEVEL);
	if (pLoggingLevelList != NULL)
	{
		
		if (m_dwLogLevel < 0 || m_dwLogLevel > 2)
		{
			m_dwLogLevel = DEFAULT_LOG_LEVEL;
		}
		pLoggingLevelList->SetCurSel(m_dwLogLevel);
	}
	pLoggingLevelList = NULL;
	UpdateData(FALSE);
}

void CIWinSyncDlg::ReadRegStringValue(TCHAR *pszKey, TCHAR *pszName, TCHAR **ppszValue)
{
	HKEY hKey;
	TCHAR szBuffer[1024];
	DWORD dwDataBufSize = 1024;
	DWORD dwKeyDataType;

	*ppszValue = NULL;
	if (RegOpenKeyEx(HKEY_CURRENT_USER, pszKey, 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS)
	{
		//Opened the Path Key
		if (RegQueryValueEx(hKey, pszName, NULL, &dwKeyDataType, (LPBYTE) &szBuffer, &dwDataBufSize) == ERROR_SUCCESS)
		{
			if (dwKeyDataType = REG_SZ)
			{
				*ppszValue = (TCHAR *)malloc(dwDataBufSize);
				_stprintf_s(*ppszValue,(size_t)(dwDataBufSize/sizeof(TCHAR)),_T("%s"),szBuffer);
			}
		}
		RegCloseKey(hKey);
	}
}

DWORD CIWinSyncDlg::ReadRegDWordValue(TCHAR *pszKey, TCHAR *pszName)
{
	HKEY hKey;
	DWORD dwValue = 0;
	DWORD dwDataBufSize = 4;
	DWORD dwKeyDataType;

	if (RegOpenKeyEx(HKEY_CURRENT_USER, pszKey, 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS)
	{
		//Opened the Path Key
		if (RegQueryValueEx(hKey, pszName, NULL, &dwKeyDataType, (LPBYTE) &dwValue, &dwDataBufSize) != ERROR_SUCCESS)
		{
			dwValue=0;
		}
		RegCloseKey(hKey);
	}
	return dwValue;
}

void CIWinSyncDlg::WriteSettings()
{
	//Get the path to sync and save in registry
	CEdit* pPathEditBox = NULL;
	pPathEditBox = (CEdit*) GetDlgItem(IDC_EDIT_PATH);
	if (pPathEditBox != NULL)
	{
		TCHAR szTempPath[MAX_PATH];
		pPathEditBox->GetWindowTextW(szTempPath,MAX_PATH);
		if(m_pszCurrentSyncPath != NULL)
		{
			free(m_pszCurrentSyncPath);
			m_pszCurrentSyncPath = (TCHAR *) malloc((_tcslen(szTempPath)+1) *sizeof(TCHAR));
		}
		_tcscpy_s(m_pszCurrentSyncPath,(_tcslen(szTempPath)+1),szTempPath);
	}
	pPathEditBox = NULL;
	WriteRegStringValue(_T("Software\\IWinSync"), _T("CurSyncPath"),m_pszCurrentSyncPath);

	//get the sync interval
	CEdit* pIntervalEditBox = NULL;
	pIntervalEditBox = (CEdit*) GetDlgItem(IDC_EDIT_INTERVAL);
	if (pIntervalEditBox != NULL)
	{
		TCHAR szSyncInterval[50];
		pIntervalEditBox->GetWindowTextW(szSyncInterval,50);
		DWORD dwScanInterval = 0;
		_stscanf_s(szSyncInterval,_T("%u"),&dwScanInterval);
		m_nSyncInterval = dwScanInterval * 60000;
	}
	pIntervalEditBox = NULL;
	WriteRegDWordValue(_T("Software\\IWinSync"), _T("SyncInterval"),m_nSyncInterval/60000);

	CListBox * pLoggingLevelList = (CListBox*) GetDlgItem(IDC_LIST_LOGLEVEL);
	if (pLoggingLevelList != NULL)
	{
		m_dwLogLevel = pLoggingLevelList->GetCurSel();
	}
	pLoggingLevelList = NULL;

	if (m_dwLogLevel < 0 || m_dwLogLevel > 2)
	{
		m_dwLogLevel = DEFAULT_LOG_LEVEL;
	}
	WriteRegDWordValue(_T("Software\\IWinSync"), _T("LogLevel"),m_dwLogLevel);
}

void CIWinSyncDlg::WriteRegStringValue(TCHAR *pszKey, TCHAR *pszName, TCHAR *pszValue)
{
	HKEY hKey;
	DWORD dwDisposition;
	if (RegCreateKeyEx(HKEY_CURRENT_USER, pszKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey,&dwDisposition) == ERROR_SUCCESS)
	{
		DWORD dwDataBufSize = 1024;
		//We have either created or open the registry key
		if (RegSetValueEx(hKey, pszName, 0, REG_SZ, (const BYTE *) pszValue, ((_tcslen(pszValue)+1) *sizeof(TCHAR))) != ERROR_SUCCESS)
		{
			//Log Error
		}
		RegCloseKey(hKey);
	}
}

void CIWinSyncDlg::WriteRegDWordValue(TCHAR *pszKey, TCHAR *pszName, DWORD dwValue)
{
	HKEY hKey;
	DWORD dwDisposition;
	if (RegCreateKeyEx(HKEY_CURRENT_USER, pszKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey,&dwDisposition) == ERROR_SUCCESS)
	{
		DWORD dwTemp = dwValue;
		//We have either created or open the registry key
		if (RegSetValueEx(hKey, pszName, 0, REG_DWORD, (const BYTE *) &dwTemp, sizeof(DWORD)) != ERROR_SUCCESS)
		{
			//Log Error
		}
		RegCloseKey(hKey);
	}
}

void CIWinSyncDlg::OnBnClickedMinimise()
{
	WriteSettings();
	PostMessage(WM_SYSCOMMAND, SC_MINIMIZE);
}


void CIWinSyncDlg::OnBnClickedReviewconflict()
{
	DWORD attr = GetFileAttributes(m_pszConflictLogPath);
	if(attr == INVALID_FILE_ATTRIBUTES || (attr & FILE_ATTRIBUTE_DIRECTORY))
	{
		return;   // does not exist
	}
	ShellExecute(NULL,_T("open"),m_pszConflictLogPath,NULL,NULL,SW_SHOW);
}


void CIWinSyncDlg::OnBnClickedReviewlog()
{
	DWORD attr = GetFileAttributes(m_pszConflictLogPath);
	if(attr == INVALID_FILE_ATTRIBUTES || (attr & FILE_ATTRIBUTE_DIRECTORY))
	{
		return;   // does not exist
	}
	ShellExecute(NULL,_T("open"),m_pszAppLogPath,NULL,NULL,SW_SHOW);

}
