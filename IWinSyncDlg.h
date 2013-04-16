
// IWinSyncDlg.h : header file
//

#pragma once
#define VERSIONTEXT _T("1.0.2")

#include "FlyoutDlg.h"

//#define WM_TRAY_ICON_NOTIFY_MESSAGE (WM_USER + 1)
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
	 HICON m_hIcon;
	
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	//afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg HCURSOR OnQueryDragIcon();

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

	//added for tray icon
	afx_msg LRESULT OnTrayNotify(WPARAM wParam, LPARAM lParam);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);	

	afx_msg void OnTraymenuStatus(); 
	afx_msg void OnTraymenuSyncCenter();  
	afx_msg void OnTraymenuAbout(); 
	afx_msg void OnTraymenuExit();  

	DECLARE_MESSAGE_MAP()


//tray code
private:

	HINSTANCE g_hInst;
	CFlyoutDlg *m_pFlyoutDialog;
	BOOL m_bMinimizeToTray;
	BOOL m_bCanShowFlyout;
	/*BOOL			m_bTrayIconVisible;
	NOTIFYICONDATA	m_nidIconData;
	CMenu			m_mnuTrayMenu;
	UINT			m_nDefaultMenuItem;*/
	// Construction
public:
	/*void TraySetMinimizeToTray(BOOL bMinimizeToTray = TRUE);
	BOOL TraySetMenu(UINT nResourceID,UINT nDefaultPos=0);	
	BOOL TraySetMenu(HMENU hMenu,UINT nDefaultPos=0);	
	BOOL TraySetMenu(LPCTSTR lpszMenuName,UINT nDefaultPos=0);	
	BOOL TrayUpdate();
	BOOL TrayShow();
	BOOL TrayHide();
	void TraySetToolTip(LPCTSTR lpszToolTip);
	void TraySetIcon(HICON hIcon);
	void TraySetIcon(UINT nResourceID);
	void TraySetIcon(LPCTSTR lpszResourceName);

	BOOL TrayIsVisible();
		
	virtual void OnTrayLButtonDown(CPoint pt);
	virtual void OnTrayLButtonDblClk(CPoint pt);
	
	virtual void OnTrayRButtonDown(CPoint pt);
	virtual void OnTrayRButtonDblClk(CPoint pt);

	virtual void OnTrayMouseMove(CPoint pt);*/
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
