// ScaleEnlargeDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "WisdomLabel.h"
#include "ScaleEnlargeDlg.h"
#include "afxdialogex.h"
#include "Com.h"

// CScaleEnlargeDlg �Ի���

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


// CScaleEnlargeDlg ��Ϣ�������


void CScaleEnlargeDlg::OnEnChangeEdit1()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialogEx::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

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
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialogEx::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

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

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	m_EquScaleBtn.SetCheck(1);
	GetDlgItem(IDC_EDIT1)->SetFocus();
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}
