// RotateSettingDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "WisdomLabel.h"
#include "RotateSettingDlg.h"
#include "afxdialogex.h"


// CRotateSettingDlg 对话框

IMPLEMENT_DYNAMIC(CRotateSettingDlg, CDialogEx)

CRotateSettingDlg::CRotateSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CRotateSettingDlg::IDD, pParent)
	, m_Angle(0)
{

}

CRotateSettingDlg::~CRotateSettingDlg()
{
}

void CRotateSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX,IDC_EDIT1,m_Angle);
}
	

BEGIN_MESSAGE_MAP(CRotateSettingDlg, CDialogEx)
END_MESSAGE_MAP()


// CRotateSettingDlg 消息处理程序

