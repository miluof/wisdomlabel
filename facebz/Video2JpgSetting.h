#pragma once


// CVideo2JpgSetting �Ի���

class CVideo2JpgSetting : public CDialogEx
{
	DECLARE_DYNAMIC(CVideo2JpgSetting)

public:
	CVideo2JpgSetting(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CVideo2JpgSetting();

// �Ի�������
	enum { IDD = IDD_DIALOG6 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeEdit1();
	CString m_VideoDir;
	afx_msg void OnEnChangeEdit2();
};
