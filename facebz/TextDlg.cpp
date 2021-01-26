// TextDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "WisdomLabel.h"
#include "TextDlg.h"
#include "afxdialogex.h"


// CTextDlg 对话框

IMPLEMENT_DYNAMIC(CTextDlg, CDialogEx)

CTextDlg::CTextDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTextDlg::IDD, pParent)
	, m_Text(_T(""))
{

}

CTextDlg::~CTextDlg()
{
}

void CTextDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX,IDC_EDIT1,m_Text);
}


BEGIN_MESSAGE_MAP(CTextDlg, CDialogEx)
END_MESSAGE_MAP()


// CTextDlg 消息处理程序
