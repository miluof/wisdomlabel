// TextDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "WisdomLabel.h"
#include "TextDlg.h"
#include "afxdialogex.h"


// CTextDlg �Ի���

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


// CTextDlg ��Ϣ�������
