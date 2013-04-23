
// IWinSyncDlg.cpp : implementation file
//

#include "stdafx.h"
#include "IWinSync.h"
#include "IWinSyncDlg.h"
#include "afxdialogex.h"
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
	m_pConflictLogThread = NULL;
	m_bSyncPathSet = FALSE;
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
	ON_MESSAGE(WMAPP_SYNCCONFLICT, OnSyncConflict)
	ON_MESSAGE(WMAPP_SYNCCOMPLETE,OnSyncComplete)
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
	
	if (m_bSyncPathSet)
	{
		ShowWindow(SW_SHOWMINIMIZED);
	}
	else
	{
		ShowWindow(SW_SHOWNORMAL);
	}
	
	m_bMinimizeToTray = FALSE;
	m_bCanShowFlyout = TRUE;

	if ( m_bSyncPathSet)
	{
		LOG(G2L_DEBUG) << "Application Is Started - Starting OfflineFiles Service";
		if ( !InitSyncClient() ) //Need to pass event details
		{
			PostQuitMessage(0); // Shutdown because the application did not initialise
			return TRUE;
		}
		//start The sync Timer
		m_puSyncTimer = SetTimer(SYNC_TIMER_ID, m_nSyncInterval, NULL);
		if(  m_puSyncTimer == 0)
		{
			LOG(G2L_WARNING) << "Failed To start the sync Timer";
			PostQuitMessage(0); // Shutdown because the application did not initialise
			return TRUE;
		}
		LOG(G2L_DEBUG) << "Application Is Started - Started OfflineFiles Service";
		PostMessage(WM_SYSCOMMAND, SC_MINIMIZE);
	}
	else
	{
		LOG(G2L_DEBUG) << "Application Is Started - Sync Folder Is Not set";
	}
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
	if( !AddNotificationIcon(this->m_hWnd))
	{
		MessageBox(_T("Failed Creating System Tray Icon."),_T("IWinSync Fatal Error"),MB_ICONERROR | MB_OK);
		return -1;
	}
	return 0;
}

void CIWinSyncDlg::OnDestroy() 
{
	if (m_puSyncTimer != NULL)
	{
		KillTimer(m_puSyncTimer); // Kill The Synchronisation timer
		m_puSyncTimer = NULL;
	}

	//Shut down the synchronisation system
	if (m_pOfflineFilesClient != NULL)
	{
		m_pOfflineFilesClient->Cleanup();
		free(m_pOfflineFilesClient);
		m_pOfflineFilesClient = NULL;
	}

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
	switch(nIDEvent)
	{
	case HIDEFLYOUT_TIMER_ID:
		// please see the comment in HideFlyout() for an explanation of this code.
        KillTimer(HIDEFLYOUT_TIMER_ID);
        m_bCanShowFlyout = TRUE;
		break;
	case SYNC_TIMER_ID:
		KillTimer(m_puSyncTimer);
		//Start the Synchronisation thread
		if (!m_bDisableSync)
		{
			m_bSyncInProgress = TRUE;
			m_bConflictOccured = FALSE;
			AfxBeginThread(SyncThreadProc,(LPVOID)m_pOfflineFilesClient);
		}
		else
		{
			m_puSyncTimer = SetTimer(SYNC_TIMER_ID, m_nSyncInterval, NULL);
			if(  m_puSyncTimer == 0)
			{
				LOG(G2L_WARNING) << "Failed To start the sync Timer";
				MessageBox(_T("Failed To Restart Synchronisation Timer.\n\rCheck the log for details."),_T("Synchronisation Error"), MB_ICONERROR | MB_OK);
				PostQuitMessage(0); // Shutdown because the application did not initialise
			}
		}
		break;
	}
	if (nIDEvent == HIDEFLYOUT_TIMER_ID)
	{
            
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
    nid.uTimeout = 5000;
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
    // Display an Error balloon message. This is a error, so show the appropriate system icon.
    NOTIFYICONDATA nid = {sizeof(nid)};
    nid.uFlags = NIF_INFO | NIF_GUID;
    nid.guidItem = __uuidof(CIWinSyncDlg);
    nid.dwInfoFlags = NIIF_ERROR;
    LoadString(g_hInst, IDS_SYNCERROR_TITLE, nid.szInfoTitle, ARRAYSIZE(nid.szInfoTitle));
    LoadString(g_hInst, IDS_SYNCERROR_TEXT, nid.szInfo, ARRAYSIZE(nid.szInfo));
    return Shell_NotifyIcon(NIM_MODIFY, &nid);
}

BOOL CIWinSyncDlg::ShowStatusBalloon(UINT uID)
{
    // Display a balloon message for a sync job with a custom icon
    NOTIFYICONDATA nid = {sizeof(nid)};
    nid.uFlags = NIF_INFO | NIF_GUID;
    nid.guidItem = __uuidof(CIWinSyncDlg);
    nid.dwInfoFlags = NIIF_USER | NIIF_LARGE_ICON;
	LoadString(g_hInst, IDS_STATUS_TITLE, nid.szInfoTitle, ARRAYSIZE(nid.szInfoTitle));
    LoadString(g_hInst, uID, nid.szInfo, ARRAYSIZE(nid.szInfo));
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
            //MessageBox(L"The user clicked on the balloon.", L"User click", MB_OK);
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
	ReadRegistrySettings();
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
	//Dont Want a flyout so just return
	return;
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
		_stprintf_s(m_pszAppLogPath,MAX_PATH,_T("%s"),m_pszLogPath);
	}

	m_dwLogLevel = ReadRegDWordValue(_T("Software\\IWinSync"), _T("LogLevel"));

	int size_needed = WideCharToMultiByte(CP_UTF8, 0, m_pszLogPath, (int)_tcslen(m_pszLogPath), NULL, 0, NULL, NULL);
    std::string strLogPath( size_needed, 0 );
    WideCharToMultiByte(CP_UTF8, 0, m_pszLogPath, (int)_tcslen(m_pszLogPath), &strLogPath[0], size_needed, NULL, NULL);
	
	m_pLogger = new g2LogWorker(APP_LOG_NAME, strLogPath);
	g_iLoggingLevel = m_dwLogLevel;
	g2::initializeLogging(m_pLogger);
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
	if(_tcslen(m_pszCurrentSyncPath) > 0)
	{
		//there is a sync path set
		m_bSyncPathSet = TRUE;
	}
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

void CIWinSyncDlg::LogSyncResultReg(BOOL bError, BOOL bConflicts, HRESULT hr)
{
	DWORD dwLastLine = ReadRegDWordValue(_T("Software\\IWinSync"), _T("LastLine"));
	if (dwLastLine < 0 || dwLastLine > 4)
	{
		//We do not have a valid line so start again
		dwLastLine = 0;
	}
	TCHAR szResult[100];
	TCHAR szErrorString[256];
	
	if(!bError && !bConflicts)
	{
		_stprintf_s(szResult,_T(" Without Error (Result %d)"),hr);
	}
	else if (bError && bConflicts)
	{
		_stprintf_s(szResult,_T(" With Errors and Conflicts (Result %d)"),hr);
	}
	else if (bError)
	{
		_stprintf_s(szResult,_T(" With Errors (Result %d)"),hr);
	}
	else if (bConflicts)
	{
		_stprintf_s(szResult,_T(" With Conflicts (Result %d)"),hr);
	}
	else
	{
		_stprintf_s(szResult,_T(" With Unknown Result (Result %d)"),hr);
	}


	SYSTEMTIME st;
    GetSystemTime(&st);

	_stprintf_s(szErrorString,_T("%2d:%2d:%2d On %2d/%2d/%d - Synchronisation Completed%s"),	st.wHour,
																								st.wMinute,
																								st.wSecond,
																								st.wDay,
																								st.wMonth,
																								st.wYear,
																								szResult);

	switch(dwLastLine)
	{
		case 0:
			WriteRegStringValue(_T("Software\\IWinSync"), _T("LastResult1"),szErrorString);
			break;
		case 1:
			WriteRegStringValue(_T("Software\\IWinSync"), _T("LastResult2"),szErrorString);
			break;
		case 2:
			WriteRegStringValue(_T("Software\\IWinSync"), _T("LastResult3"),szErrorString);
			break;
		case 3:
			WriteRegStringValue(_T("Software\\IWinSync"), _T("LastResult4"),szErrorString);
			break;
		case 4:
			WriteRegStringValue(_T("Software\\IWinSync"), _T("LastResult5"),szErrorString);
			break;
	}
	
	if (++dwLastLine > 4)
	{
		dwLastLine = 0;
	}
	WriteRegDWordValue(_T("Software\\IWinSync"), _T("LastLine"),dwLastLine);
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
	if(!m_bSyncPathSet && _tcslen(m_pszCurrentSyncPath) > 0)
	{
		MessageBox(_T("To begin synchronisation, IWinSync needs to be restarted."),_T("IWinSync Restart Required"), MB_ICONINFORMATION | MB_OK);
		PostQuitMessage(0); // Shutdown to allow the application to be restarted
	}
	else
	{
		PostMessage(WM_SYSCOMMAND, SC_MINIMIZE);
	}
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
	if (m_pLogger == NULL)
	{
		return;
	}
	
	#ifdef UNICODE
		std::wstring stemp = s2ws(m_pLogger->szlogFileName()); // Temporary buffer is required
		LPCWSTR result = stemp.c_str();
	#else
		LPCWSTR result = s.c_str();
	#endif
	DWORD attr = GetFileAttributes(result);
	if(attr == INVALID_FILE_ATTRIBUTES || (attr & FILE_ATTRIBUTE_DIRECTORY))
	{
		return;   // does not exist
	}
	ShellExecute(NULL,_T("open"),result,NULL,NULL,SW_SHOW);

}


std::wstring CIWinSyncDlg::s2ws(const std::string& s)
{
 int len;
 int slength = (int)s.length() + 1;
 len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0); 
 wchar_t* buf = new wchar_t[len];
 MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
 std::wstring r(buf);
 delete[] buf;
 return r;
}

BOOL CIWinSyncDlg::InitSyncClient()
{
	bool bInitComplete = FALSE;
	m_pOfflineFilesClient = new COfflineFilesClient(WMAPP_SYNCCOMPLETE,WMAPP_SYNCCONFLICT,this->m_hWnd);
	if(m_pOfflineFilesClient == NULL)
	{
		LOG(G2L_WARNING) << "m_pOfflineFilesClient = new COfflineFilesClient() Returned NULL";
		MessageBox(_T("Failed To Setup Offline Files Client"),_T("OfflineFiles Error"), MB_ICONERROR | MB_OK);
	}

	if ( !m_pOfflineFilesClient->Init())
	{
		LOG(G2L_WARNING) << "m_pOfflineFilesClient->Init() Returned False";
		MessageBox(_T("Failed To Setup Offline Files Client"),_T("OfflineFiles Error"), MB_ICONERROR | MB_OK);
	}

	OFFLINEFILESCLIENT_INIT_STATE initState = m_pOfflineFilesClient->GetInitialiseState();
	if (initState == OFFLINEFILESCLIENT_INIT_STATE_RESTART_PENDING)
	{
		LOG(G2L_INFO) << "m_pOfflineFilesClient->GetInitialiseState() returned OFFLINEFILESCLIENT_INIT_STATE_RESTART_PENDING";
		MessageBox(_T("Your PC Must be restarted to start Offline files Service"),_T("OfflineFiles"), MB_ICONINFORMATION | MB_OK);
	}
	else if (initState == OFFLINEFILESCLIENT_INIT_STATE_STARTED)
	{
		LOG(G2L_DEBUG) << "m_pOfflineFilesClient->GetInitialiseState() returned OFFLINEFILESCLIENT_INIT_STATE_STARTED";

		if (m_pszCurrentSyncPath != NULL)
		{
			size_t SyncPathLen = _tcslen(m_pszCurrentSyncPath);
			LPCWSTR pCurrentSyncPath = (LPCWSTR) malloc((SyncPathLen +1) * sizeof(wchar_t));
			if (pCurrentSyncPath != NULL)
			{
				#ifdef UNICODE
				// The username is defined as unicode /so we can jjust copy the bytes
				memcpy((void *)pCurrentSyncPath,m_pszCurrentSyncPath,((SyncPathLen +1) * sizeof(wchar_t)));
				#else
				//The username is defined as standard
				MultiByteToWideChar(CP_UTF8, 0,m_pszCurrentSyncPath,-1,pCurrentSyncPath,((SyncPathLen +1));
				#endif

				if(!m_pOfflineFilesClient->InitCache(&pCurrentSyncPath))
				{
					LOG(G2L_WARNING) << "m_pOfflineFilesClient->InitCache Returned False, with path: " << m_pszCurrentSyncPath;
					MessageBox(_T("Failed To Initialise Offline Files Cache"),_T("OfflineFiles Error"), MB_ICONERROR | MB_OK);
				}
				else
				{
					LOG(G2L_DEBUG) << "Offline Files Cache Was Initiated, with path:" << m_pszCurrentSyncPath;
					//OfflineFilesClient.RegisterSyncEvents(); //Do not need events
					bInitComplete = TRUE;
				}
				free((void*)pCurrentSyncPath);
				pCurrentSyncPath = NULL;
			}
			else
			{
				LOG(G2L_WARNING) << "pCurrentSyncPath is NULL ";
				MessageBox(_T("Failed To Initialise Offline Files Cache"),_T("OfflineFiles Error"), MB_ICONERROR | MB_OK);
			}
		}
		else
		{
			LOG(G2L_WARNING) << "m_pszCurrentSyncPath is NULL ";
			MessageBox(_T("Failed To Initialise Offline Files Cache"),_T("OfflineFiles Error"), MB_ICONERROR | MB_OK);
		}
	}
	else
	{
		LOG(G2L_WARNING) << "m_pOfflineFilesClient->GetInitialiseState Returned an invalid state: " << initState;
		MessageBox(_T("Failed To start The Offline Files Service"),_T("OfflineFiles Error"), MB_ICONERROR | MB_OK);
	}

	LOG_IF(G2L_DEBUG,bInitComplete) << "Offline Files Initialisation Completed";
	LOG_IF(G2L_DEBUG,!bInitComplete) << "Offline Files Initialisation Failed";
	return bInitComplete;
}

LRESULT CIWinSyncDlg::OnSyncConflict(WPARAM wParam, LPARAM lParam)
{
	if (m_bSyncInProgress)
	{
		m_bConflictOccured = TRUE;
	}
	//Build the Conflict Message
	
	
	CConflictResult *pConflictResult = (CConflictResult *) lParam;
	if (pConflictResult->m_uResolution ==  CONFLICT_RESOLUTION_POLICY_RESOLVED)
	{
		_stprintf_s(m_szLogMessage,_T("Conflict Resolved By Policy For File: %s "),pConflictResult->m_pszConflictedFile);	
	}
	else if (pConflictResult->m_uResolution == CONFLICT_RESOLUTION_RESOLVED)
	{
		_stprintf_s(m_szLogMessage,_T("Conflict Resolved By Policy For File: %s Renamed To: %s"),pConflictResult->m_pszConflictedFile, pConflictResult->m_pszNewFile);	
	}
	else //not resolved
	{
		_stprintf_s(m_szLogMessage,_T("Conflict Not Resolved For File (See Sync Centre): %s "),pConflictResult->m_pszConflictedFile);	
	}
	delete(pConflictResult);

	//If the thread is not null then it is already running
	if (m_pConflictLogThread != NULL)
	{
		//Wait for the thread to finish
		if(WaitForSingleObject(m_pConflictLogThread->m_hThread, 60000)==WAIT_TIMEOUT) //wait 1 minute if not then log the error in our normal log
		{
			//The thread did not terminate in time
			LOG(G2L_WARNING) << "";
		}
	}
	m_pConflictLogThread = AfxBeginThread(ConflictLogThreadProc,(LPVOID)this);
	
	return 0;
}

LRESULT CIWinSyncDlg::OnSyncComplete(WPARAM wParam, LPARAM lParam)
{
	m_bSyncInProgress = FALSE;
	BOOL bLogError = FALSE;
	//If wparam is 0 then the sync completed else show an error
	if (wParam != 0)
	{
		bLogError = TRUE;
		if (m_bSuppressDialogs)
		{
			ShowErrorBalloon();
		}
		else
		{
			MessageBox(_T("Failed to run synchronisation. Check the log for details."),_T("Synchronisation Error"), MB_ICONERROR | MB_OK);
			//show a message box
		}
		return 0;
	}
	
	//Dont Show the dialog on succesfull completion
	//if (lParam == S_OK && !m_bConflictOccured)
	//{
	//	ShowStatusBalloon(IDS_COMPLETE_MESSAGE);
	//}
	
	if (lParam != S_OK || m_bConflictOccured)
	{
		if (m_bConflictOccured)
		{
			if (m_bSuppressDialogs)
			{
				ShowStatusBalloon(IDS_COMPLETE_CONFLICT);
			}
			else
			{
				MessageBox(_T("Synchronisation completed, but conflicts were detected.\r\nFiles may have been modified. Check the log for details."),_T("Synchronisation Conflicts"), MB_ICONINFORMATION | MB_OK);
			}
		}
		else
		{
			bLogError = TRUE;
			if (m_bSuppressDialogs)
			{
				ShowStatusBalloon(IDS_COMPLETE_ERRORS);				
			}
			else
			{
				MessageBox(_T("Synchronisation completed, but errors were detected.\r\nFiles may have been modified and further synchronisation request may fail.\n\rCheck the log for details."),_T("Synchronisation Error"), MB_ICONERROR | MB_OK);
			}
		}
	}
	//Log The result of the last sync
	LogSyncResultReg(bLogError,m_bConflictOccured, (HRESULT) lParam);
	
	//Restart the timer
	m_puSyncTimer = SetTimer(SYNC_TIMER_ID, m_nSyncInterval, NULL);
	if(  m_puSyncTimer == 0)
	{
		LOG(G2L_WARNING) << "Failed To start the sync Timer";
		MessageBox(_T("Failed To Restart Synchronisation Timer.\n\rCheck the log for details."),_T("Synchronisation Error"), MB_ICONERROR | MB_OK);
		PostQuitMessage(0); // Shutdown because the application did not initialise
	}

	return 0;
}

UINT CIWinSyncDlg::SyncThreadProc( LPVOID pParam )
{
	COfflineFilesClient* pOfflineFilesClient = (COfflineFilesClient*)pParam;
	pOfflineFilesClient->Synchronise();
	pOfflineFilesClient = NULL;
	return 0;
} 

UINT CIWinSyncDlg::ConflictLogThreadProc( LPVOID pParam )
{
	CIWinSyncDlg *instance =   (CIWinSyncDlg *) pParam;
	//Check The Size Of the File
	
	BOOL bFileTooBig = FALSE;
	HANDLE hFile = CreateFile(instance->m_pszConflictLogPath, // file to open
                       GENERIC_READ,          // open for reading
                       FILE_SHARE_READ,       // share for reading
                       NULL,                  // default security
                       OPEN_EXISTING,         // existing file only
                       FILE_ATTRIBUTE_NORMAL, // normal file
                       NULL);                 // no attr. template
	if (hFile != INVALID_HANDLE_VALUE) 
    { 
		LARGE_INTEGER lFileSize;
		GetFileSizeEx(hFile,&lFileSize);
		if (lFileSize.LowPart > 0x4C4B40) //file is bigger than ~5 MB
		{
			bFileTooBig = TRUE;		
		}
		CloseHandle(hFile);
	}

	if (bFileTooBig)
	{
		TCHAR szLogFileCopy[MAX_PATH];
		TCHAR *pExtension = _tcsrchr(instance->m_pszConflictLogPath,_T('.'));
		if (pExtension != NULL)
		{
			int iLength  = pExtension - instance->m_pszConflictLogPath + 1;
			_tcsncpy_s(szLogFileCopy, instance->m_pszConflictLogPath, iLength);
			_tcscat_s(szLogFileCopy,_T(".log2"));
			if(CopyFile(instance->m_pszConflictLogPath,szLogFileCopy,FALSE) != 0)
			{
				//now delete our file
				DeleteFile(instance->m_pszConflictLogPath);
			}
		}
	}
	//At this point if the file was too big it is copied to another file and deleted if not just log
	

	CStdioFile ConflictLogFile;
	if( !ConflictLogFile.Open( instance->m_pszConflictLogPath, CFile::modeCreate| CFile::modeWrite | CFile::typeText ) ) 
	{
		LOG(G2L_WARNING) << "Failed Open Conflict Log File";
	}
	TCHAR szLogMessage[(MAX_PATH*2)+512];
	SYSTEMTIME st;
    GetSystemTime(&st);

	_stprintf_s(szLogMessage,_T("%2d:%2d:%2d On %2d/%2d/%d -%s"),	st.wHour,
																	st.wMinute,
																	st.wSecond,
																	st.wDay,
																	st.wMonth,
																	st.wYear,
																	instance->m_szLogMessage);

	//Log The message Here

	ConflictLogFile.WriteString(szLogMessage);
	ConflictLogFile.Flush();
	ConflictLogFile.Close();
	return 0;
} 
