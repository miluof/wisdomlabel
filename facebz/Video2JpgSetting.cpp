// Video2JpgSetting.cpp : 实现文件
//

#include "stdafx.h"
#include "WisdomLabel.h"
#include "Video2JpgSetting.h"
#include "afxdialogex.h"


// CVideo2JpgSetting 对话框

IMPLEMENT_DYNAMIC(CVideo2JpgSetting, CDialogEx)

CVideo2JpgSetting::CVideo2JpgSetting(CWnd* pParent /*=NULL*/)
	: CDialogEx(CVideo2JpgSetting::IDD, pParent)
	, m_VideoDir(_T(""))
{

}
CVideo2JpgSetting::~CVideo2JpgSetting()
{
}

void CVideo2JpgSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX,IDC_EDIT1,m_VideoDir);
}

BEGIN_MESSAGE_MAP(CVideo2JpgSetting, CDialogEx)
	ON_EN_CHANGE(IDC_EDIT1, &CVideo2JpgSetting::OnEnChangeEdit1)
	ON_EN_CHANGE(IDC_EDIT2, &CVideo2JpgSetting::OnEnChangeEdit2)
END_MESSAGE_MAP()


// CVideo2JpgSetting 消息处理程序


void CVideo2JpgSetting::OnEnChangeEdit1()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  OnBnClickedOpenAnnotation
}


void CVideo2JpgSetting::OnEnChangeEdit2()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  OnBnClickedOpenAnnotation
}
