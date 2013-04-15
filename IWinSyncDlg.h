
// IWinSyncDlg.h : header file
//

#pragma once
#define VERSIONTEXT _T("1.0.2")

#define WM_TRAY_ICON_NOTIFY_MESSAGE (WM_USER + 1)

// CIWinSyncDlg dialog
class CIWinSyncDlg : public CDialogEx
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
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg HCURSOR OnQueryDragIcon();

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
	BOOL m_bMinimizeToTray;

	BOOL			m_bTrayIconVisible;
	NOTIFYICONDATA	m_nidIconData;
	CMenu			m_mnuTrayMenu;
	UINT			m_nDefaultMenuItem;
	// Construction
public:
	void TraySetMinimizeToTray(BOOL bMinimizeToTray = TRUE);
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

	virtual void OnTrayMouseMove(CPoint pt);
};
