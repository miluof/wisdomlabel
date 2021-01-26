#pragma once


// CVideo2JpgSetting2 对话框

class CVideo2JpgSetting2 : public CDialogEx
{
	DECLARE_DYNAMIC(CVideo2JpgSetting2)

public:
	CVideo2JpgSetting2(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CVideo2JpgSetting2();

// 对话框数据
	enum { IDD = IDD_DIALOG8 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
