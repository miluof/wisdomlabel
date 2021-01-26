// KjjDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "WisdomLabel.h"
#include "KjjDlg.h"
#include "afxdialogex.h"


// CKjjDlg 对话框

IMPLEMENT_DYNAMIC(CKjjDlg, CDialogEx)

CKjjDlg::CKjjDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CKjjDlg::IDD, pParent)
{

}

CKjjDlg::~CKjjDlg()
{
}

void CKjjDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_KjjListCtrl);
}


BEGIN_MESSAGE_MAP(CKjjDlg, CDialogEx)
END_MESSAGE_MAP()


// CKjjDlg 消息处理程序


BOOL CKjjDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	int i=0;
	m_KjjListCtrl.InsertColumn(i++,CString("功能"),LVCFMT_LEFT,100,-1);
	m_KjjListCtrl.InsertColumn(i++,CString("快捷键"),LVCFMT_LEFT,100,-1);
	m_KjjListCtrl.InsertColumn(i++,CString("快捷键解释"),LVCFMT_LEFT,200,-1);
	//m_KjjListCtrl.InsertColumn(i++,"命令ID",LVCFMT_LEFT,200,-1);
	for(map<CStringA,Kjj>::iterator it=m_KjjMap->begin();it!=m_KjjMap->end();it++){
		int j =m_KjjListCtrl.InsertItem(m_KjjListCtrl.GetItemCount(),CString(it->second.Title));
		m_KjjListCtrl.SetItemText(j,1,CString(it->first));
		m_KjjListCtrl.SetItemText(j,2,CString(it->second.KjjJs));	
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
