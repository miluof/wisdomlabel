#pragma once
#include "afxwin.h"


// CText2Dlg �Ի���

class CText2Dlg : public CDialogEx
{
	DECLARE_DYNAMIC(CText2Dlg)

public:
	CText2Dlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CText2Dlg();

// �Ի�������
	enum { IDD = IDD_TEST2DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CString m_Text2;
	CString m_Text1;
};
