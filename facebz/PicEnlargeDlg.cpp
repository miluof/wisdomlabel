// PicEnlargeDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "WisdomLabel.h"
#include "PicEnlargeDlg.h"
#include "afxdialogex.h"


// CPicEnlargeDlg 对话框

IMPLEMENT_DYNAMIC(CPicEnlargeDlg, CDialogEx)

CPicEnlargeDlg::CPicEnlargeDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPicEnlargeDlg::IDD, pParent)
	, m_LeftEnlarge(0)
	, m_RightEnlarge(0)
	, m_TopEnlarge(0)
	, m_BottomEnlarge(0)
{

}

CPicEnlargeDlg::~CPicEnlargeDlg()
{
}

void CPicEnlargeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX,IDC_EDIT1,m_LeftEnlarge);
	DDX_Text(pDX,IDC_EDIT2, m_RightEnlarge);
	DDX_Text(pDX,IDC_EDIT3, m_TopEnlarge);
	DDX_Text(pDX,IDC_EDIT6, m_BottomEnlarge);
}


BEGIN_MESSAGE_MAP(CPicEnlargeDlg, CDialogEx)
END_MESSAGE_MAP()


// CPicEnlargeDlg 消息处理程序
