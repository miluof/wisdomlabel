#pragma once


// CRotateSettingDlg �Ի���

class CRotateSettingDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CRotateSettingDlg)

public:
	CRotateSettingDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CRotateSettingDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG9 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	int m_Angle;
};
