#pragma once
#include "afxwin.h"


// CScaleEnlargeDlg 对话框

class CScaleEnlargeDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CScaleEnlargeDlg)

public:
	CScaleEnlargeDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CScaleEnlargeDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG5 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

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
