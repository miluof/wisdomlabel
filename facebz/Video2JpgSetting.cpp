// Video2JpgSetting.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "WisdomLabel.h"
#include "Video2JpgSetting.h"
#include "afxdialogex.h"


// CVideo2JpgSetting �Ի���

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


// CVideo2JpgSetting ��Ϣ�������


void CVideo2JpgSetting::OnEnChangeEdit1()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialogEx::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  OnBnClickedOpenAnnotation
}


void CVideo2JpgSetting::OnEnChangeEdit2()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialogEx::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  OnBnClickedOpenAnnotation
}
