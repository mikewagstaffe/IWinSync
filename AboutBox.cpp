// AboutBox.cpp : implementation file
//

#include "stdafx.h"
#include "IWinSync.h"
#include "AboutBox.h"
#include "afxdialogex.h"


// CAboutBox dialog

IMPLEMENT_DYNAMIC(CAboutBox, CDialogEx)

CAboutBox::CAboutBox(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAboutBox::IDD, pParent)
{

}

CAboutBox::~CAboutBox()
{
}

void CAboutBox::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAboutBox, CDialogEx)
END_MESSAGE_MAP()


// CAboutBox message handlers


BOOL CAboutBox::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	GetDlgItem(IDC_VERSIONTEXT)->SetWindowText(m_szVersion);
	return (TRUE);
}
