#pragma once
#include "afxwin.h"


// CLabeCropDlg �Ի���

class CLabeCropDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLabeCropDlg)

public:
	CLabeCropDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CLabeCropDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG7 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CString m_Label;
	CString m_Path;
	afx_msg void OnBnClickedButton1();
	BOOL m_OnlyHasChild;
	CButton m_OnlyCropHasChildBtn;
	afx_msg void OnBnClickedOk();
	int m_Scale;
};
