#pragma once
#include "afxcmn.h"
#include "LabListCtrl.h"
#include <map>
#include "afxwin.h"
using namespace std;

// CDxLxDlg 对话框
class CWisdomLabelDlg;
class CDxLxDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CDxLxDlg)

public:
	CDxLxDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDxLxDlg();
	CImageList m_imList;
	map<CStringA,CBitmap *> *m_LabMap;
	map<CStringA,CStringA>*m_LabMap1;
	CWisdomLabelDlg *m_MainDlg;
// 对话框数据
	enum { IDD = IDD_DXLXDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CStringA m_DxType;
	virtual BOOL OnInitDialog();
//	afx_msg void OnMove(int x, int y);
	CLabListCtrl m_LabListCtrl;
	afx_msg void OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButton1();
	void LoadLabel(CStringA LabelName);
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	CComboBox m_LabBox;
	afx_msg void OnCbnEditchangeCombo3();
	afx_msg void OnCbnSelchangeCombo3();
	afx_msg void OnNMRDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton7();
	afx_msg void OnCbnSelchangeCombo4();
};
