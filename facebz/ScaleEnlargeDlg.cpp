// ScaleEnlargeDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "WisdomLabel.h"
#include "ScaleEnlargeDlg.h"
#include "afxdialogex.h"
#include "Com.h"

// CScaleEnlargeDlg 对话框

IMPLEMENT_DYNAMIC(CScaleEnlargeDlg, CDialogEx)

CScaleEnlargeDlg::CScaleEnlargeDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CScaleEnlargeDlg::IDD, pParent)
{
	m_XScale=200;
	m_YScale=200;
}

CScaleEnlargeDlg::~CScaleEnlargeDlg()
{
}

void CScaleEnlargeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX,IDC_EDIT1,m_XScale);
	DDX_Text(pDX,IDC_EDIT7,m_YScale);
	DDX_Control(pDX, IDC_CHECK1, m_EquScaleBtn);
}


BEGIN_MESSAGE_MAP(CScaleEnlargeDlg, CDialogEx)
	ON_EN_CHANGE(IDC_EDIT1, &CScaleEnlargeDlg::OnEnChangeEdit1)
	ON_EN_CHANGE(IDC_EDIT7, &CScaleEnlargeDlg::OnEnChangeEdit7)
	ON_BN_CLICKED(IDC_CHECK1, &CScaleEnlargeDlg::OnBnClickedCheck1)
END_MESSAGE_MAP()


// CScaleEnlargeDlg 消息处理程序


void CScaleEnlargeDlg::OnEnChangeEdit1()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  OnBnClickedOpenAnnotation
 	if(!m_EquScaleBtn.GetCheck())
		return;
	if(this->GetFocus()!=GetDlgItem(IDC_EDIT1))
		return;
	CStringA content;
	content=GetWndTxtA(GetDlgItem(IDC_EDIT1));
	GetDlgItem(IDC_EDIT7)->SetWindowText(CString(content));
}


void CScaleEnlargeDlg::OnEnChangeEdit7()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  OnBnClickedOpenAnnotation
	if(!m_EquScaleBtn.GetCheck())
		return;
	if(this->GetFocus()!=GetDlgItem(IDC_EDIT7))
		return;
	CStringA content=GetWndTxtA(GetDlgItem(IDC_EDIT7));
	//->GetWindowText(content);
	GetDlgItem(IDC_EDIT1)->SetWindowText(CString(content));
}


void CScaleEnlargeDlg::OnBnClickedCheck1()
{
	// TODO: OnBnClickedOpenAnnotation
}


BOOL CScaleEnlargeDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_EquScaleBtn.SetCheck(1);
	GetDlgItem(IDC_EDIT1)->SetFocus();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
