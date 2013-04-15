#pragma once


// CAboutBox dialog

class CAboutBox : public CDialogEx
{
	DECLARE_DYNAMIC(CAboutBox)

public:
	CAboutBox(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAboutBox();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	TCHAR m_szVersion[20];
};
