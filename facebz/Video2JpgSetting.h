#pragma once


// CVideo2JpgSetting 对话框

class CVideo2JpgSetting : public CDialogEx
{
	DECLARE_DYNAMIC(CVideo2JpgSetting)

public:
	CVideo2JpgSetting(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CVideo2JpgSetting();

// 对话框数据
	enum { IDD = IDD_DIALOG6 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeEdit1();
	CString m_VideoDir;
	afx_msg void OnEnChangeEdit2();
};
