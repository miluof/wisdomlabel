#pragma once
#include "afxwin.h"


// CScaleEnlargeDlg �Ի���

class CScaleEnlargeDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CScaleEnlargeDlg)

public:
	CScaleEnlargeDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CScaleEnlargeDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG5 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	int m_XScale;
	int m_YScale;
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnEnChangeEdit7();
	afx_msg void OnBnClickedCheck1();
	CButton m_EquScaleBtn;
	virtual BOOL OnInitDialog();
};
