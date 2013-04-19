
// IWinSyncDlg.h : header file
//

#pragma once

#define VERSIONTEXT _T("1.0.2")
#define DEFAULT_SYNC_INTERVAL 1;
#define DEFAULT_LOG_LEVEL 0;

#define LOGLEVEL_LISTBOX_DEBUG _T("DEBUG")
#define LOGLEVEL_LISTBOX_WARN _T("INFO")
#define LOGLEVEL_LISTBOX_ERROR _T("WARNINGS")

#define CONFLICT_LOG_NAME   _T("IWinSync_Conflicts.log")
#define APP_LOG_NAME  "IWinSyncApp"

#include "FlyoutDlg.h"
#include "g2logworker.h"
#include "g2log.h"

UINT const WMAPP_NOTIFYCALLBACK = WM_APP + 1;
UINT const WMAPP_HIDEFLYOUT     = WM_APP + 2;

UINT_PTR const HIDEFLYOUT_TIMER_ID = 1;

// CIWinSyncDlg dialog

class __declspec(uuid("{22D6D602-CFC7-495C-87A9-9C3CE3394141}")) 
CIWinSyncDlg : public CDialogEx
{
// Construction
public:
	CIWinSyncDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_IWINSYNC_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation
protected:

	// Generated message map functions
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnTrayNotify(WPARAM wParam, LPARAM lParam);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);	
	afx_msg void OnTraymenuStatus(); 
	afx_msg void OnTraymenuSyncCenter();  
	afx_msg void OnTraymenuAbout(); 
	afx_msg void OnTraymenuExit(); 
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg LRESULT OnWmappHideflyout(WPARAM wParam, LPARAM lParam);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	virtual BOOL OnInitDialog();


	BOOL AddNotificationIcon(HWND hwnd);
	BOOL DeleteNotificationIcon();
	void ShowContextMenu(HWND hwnd, POINT pt);
	BOOL ShowConflictBalloon();
	BOOL ShowErrorBalloon();
	BOOL ShowStatusBalloon();
	BOOL RestoreTooltip();
	void ShowFlyout();
	void HideFlyout();
	void PositionFlyout(REFGUID guidIcon);
	void ReadRegistrySettings();
	void ReadRegStringValue(TCHAR *pszKey, TCHAR *pszName, TCHAR **ppszValue);
	DWORD ReadRegDWordValue(TCHAR *pszKey, TCHAR *pszName);
	void WriteRegStringValue(TCHAR *pszKey, TCHAR *pszName, TCHAR *pszValue);
	void WriteRegDWordValue(TCHAR *pszKey, TCHAR *pszName, DWORD dwValue);
	void WriteSettings();
	void PopulateSettingsDialog();
	void SetupLogging();
	DECLARE_MESSAGE_MAP()

private:
	HICON m_hIcon;
	HINSTANCE g_hInst;
	CFlyoutDlg *m_pFlyoutDialog;
	BOOL m_bMinimizeToTray;
	BOOL m_bCanShowFlyout;	

	BOOL m_bSuppressDialogs;
	BOOL m_bDisableSync;
	BOOL m_bLoggingEnabled;
	TCHAR *m_pszCurrentSyncPath;
	TCHAR *m_pszLogPath;
	TCHAR m_pszConflictLogPath[MAX_PATH];
	TCHAR m_pszAppLogPath[MAX_PATH];
	UINT m_nSyncInterval;
	DWORD m_dwLogLevel;
	g2LogWorker *m_pLogger;
	TCHAR *m_apszLastResults[5];

public:
	afx_msg void OnBnClickedMinimise();
	afx_msg void OnBnClickedReviewconflict();
	afx_msg void OnBnClickedReviewlog();
};
