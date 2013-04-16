#pragma once


// CFlyoutDlg dialog
UINT const WMAPP_HIDEFLYOUT     = WM_APP + 2;

class CFlyoutDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CFlyoutDlg)

public:
	CFlyoutDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CFlyoutDlg();

// Dialog Data
	enum { IDD = IDD_FLYOUT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
};
