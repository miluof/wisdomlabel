#pragma once


// CRotateSettingDlg 对话框

class CRotateSettingDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CRotateSettingDlg)

public:
	CRotateSettingDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CRotateSettingDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG9 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	int m_Angle;
};
