// FlyoutDlg.cpp : implementation file
//

#include "stdafx.h"
#include "IWinSync.h"
#include "FlyoutDlg.h"
#include "afxdialogex.h"


// CFlyoutDlg dialog

IMPLEMENT_DYNAMIC(CFlyoutDlg, CDialogEx)

CFlyoutDlg::CFlyoutDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CFlyoutDlg::IDD, pParent)
{

}

CFlyoutDlg::~CFlyoutDlg()
{
}

void CFlyoutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CFlyoutDlg, CDialogEx)
	ON_WM_ACTIVATE()
END_MESSAGE_MAP()


// CFlyoutDlg message handlers
BOOL CFlyoutDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	return (TRUE);
}

void CFlyoutDlg::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CDialogEx::OnActivate(nState, pWndOther, bMinimized);

	if (nState == WA_INACTIVE)
	{
		GetParent()->PostMessageW(m_HideFlyoutMessage);
	}
}
