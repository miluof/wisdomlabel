#pragma once


// CPicEnlargeDlg �Ի���

class CPicEnlargeDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CPicEnlargeDlg)

public:
	CPicEnlargeDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CPicEnlargeDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG4 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	int m_LeftEnlarge;
	int m_RightEnlarge;
	int m_TopEnlarge;
	int m_BottomEnlarge;
};
