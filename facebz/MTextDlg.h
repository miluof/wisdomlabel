#pragma once


// CMTextDlg �Ի���

class CMTextDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMTextDlg)

public:
	CMTextDlg(CWnd* pParent = NULL);   // ��׼���캯��
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
// �Ի�������
	enum { IDD = IDD_MTextDlg };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnPaint();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};
