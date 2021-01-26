#pragma once


// CPicEnlargeDlg 对话框

class CPicEnlargeDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CPicEnlargeDlg)

public:
	CPicEnlargeDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPicEnlargeDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG4 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	int m_LeftEnlarge;
	int m_RightEnlarge;
	int m_TopEnlarge;
	int m_BottomEnlarge;
};
