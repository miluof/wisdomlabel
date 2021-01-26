#pragma once
#include "afxwin.h"


// CLabeCropDlg 对话框

class CLabeCropDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLabeCropDlg)

public:
	CLabeCropDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CLabeCropDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG7 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

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
