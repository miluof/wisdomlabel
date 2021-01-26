#pragma once


// CMTextDlg 对话框

class CMTextDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMTextDlg)

public:
	CMTextDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMTextDlg();
	CStringA Title;
	CStringA SaveTitle;
	int Size;
	CStringA Titles[20];
	CStringA Texts[20];
	int bLl[20];
	int bSave[20];
	CStringList Datas[20];
	CStatic *pStatic;
	CComboBox *pTexts;
	CRect *pBtRc;
	int labWidth;
	int TextWidth;
// 对话框数据
	enum { IDD = IDD_MTextDlg };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnPaint();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};
