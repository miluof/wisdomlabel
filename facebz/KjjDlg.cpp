// KjjDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "WisdomLabel.h"
#include "KjjDlg.h"
#include "afxdialogex.h"


// CKjjDlg �Ի���

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


// CKjjDlg ��Ϣ�������


BOOL CKjjDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	int i=0;
	m_KjjListCtrl.InsertColumn(i++,CString("����"),LVCFMT_LEFT,100,-1);
	m_KjjListCtrl.InsertColumn(i++,CString("��ݼ�"),LVCFMT_LEFT,100,-1);
	m_KjjListCtrl.InsertColumn(i++,CString("��ݼ�����"),LVCFMT_LEFT,200,-1);
	//m_KjjListCtrl.InsertColumn(i++,"����ID",LVCFMT_LEFT,200,-1);
	for(map<CStringA,Kjj>::iterator it=m_KjjMap->begin();it!=m_KjjMap->end();it++){
		int j =m_KjjListCtrl.InsertItem(m_KjjListCtrl.GetItemCount(),CString(it->second.Title));
		m_KjjListCtrl.SetItemText(j,1,CString(it->first));
		m_KjjListCtrl.SetItemText(j,2,CString(it->second.KjjJs));	
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}
