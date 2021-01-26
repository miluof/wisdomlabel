// LabeCropDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "WisdomLabel.h"
#include "LabeCropDlg.h"
#include "afxdialogex.h"


// CLabeCropDlg 对话框

IMPLEMENT_DYNAMIC(CLabeCropDlg, CDialogEx)

CLabeCropDlg::CLabeCropDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CLabeCropDlg::IDD, pParent)
	, m_Label(_T(""))
	, m_Path(_T(""))
	, m_OnlyHasChild(false)
	, m_Scale(0)
{

}

CLabeCropDlg::~CLabeCropDlg()
{
}

void CLabeCropDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX,IDC_EDIT1,m_Path);
	DDX_Text(pDX,IDC_COMBO3,m_Label);
	DDX_Text(pDX,IDC_EDIT2,m_Scale);
	DDX_Control(pDX, IDC_CHECK1, m_OnlyCropHasChildBtn);
}


BEGIN_MESSAGE_MAP(CLabeCropDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CLabeCropDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDOK, &CLabeCropDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CLabeCropDlg 消息处理程序


void CLabeCropDlg::OnBnClickedButton1()
{
	// TODO: OnBnClickedOpenAnnotation
	CFolderPickerDialog  Dlg(NULL,0,this,0);
	if(!Dlg.DoModal()==IDOK)
		return;
	GetDlgItem(IDC_EDIT1)->SetWindowText(Dlg.GetPathName());
	//UpdateData(0);
}


void CLabeCropDlg::OnBnClickedOk()
{
	// TODO: OnBnClickedOpenAnnotation
	this->m_OnlyHasChild=m_OnlyCropHasChildBtn.GetCheck();
	CDialogEx::OnOK();
}
