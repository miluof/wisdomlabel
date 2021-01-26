// Text2Dlg.cpp : 实现文件
//

#include "stdafx.h"
#include "WisdomLabel.h"
#include "Text2Dlg.h"
#include "afxdialogex.h"


// CText2Dlg 对话框

IMPLEMENT_DYNAMIC(CText2Dlg, CDialogEx)

CText2Dlg::CText2Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CText2Dlg::IDD, pParent)
{

}

CText2Dlg::~CText2Dlg()
{
}

void CText2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_COMBO5, m_Text2);
	DDX_Text(pDX, IDC_COMBO1, m_Text1);
}


BEGIN_MESSAGE_MAP(CText2Dlg, CDialogEx)
END_MESSAGE_MAP()


// CText2Dlg 消息处理程序
