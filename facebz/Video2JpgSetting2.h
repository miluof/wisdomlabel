#pragma once


// CVideo2JpgSetting2 �Ի���

class CVideo2JpgSetting2 : public CDialogEx
{
	DECLARE_DYNAMIC(CVideo2JpgSetting2)

public:
	CVideo2JpgSetting2(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CVideo2JpgSetting2();

// �Ի�������
	enum { IDD = IDD_DIALOG8 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};
