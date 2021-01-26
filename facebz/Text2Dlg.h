#pragma once
#include "afxwin.h"


// CText2Dlg 对话框

class CText2Dlg : public CDialogEx
{
	DECLARE_DYNAMIC(CText2Dlg)

public:
	CText2Dlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CText2Dlg();

// 对话框数据
	enum { IDD = IDD_TEST2DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_Text2;
	CString m_Text1;
};
