#pragma once
#include "afxcmn.h"
#include <map>
using namespace std;

// CKjjDlg �Ի���
struct Kjj{
	CStringA Cmd;
	CStringA Title;
	int Id;
	CStringA KjjJs;
	Kjj(){
	};
	Kjj(CStringA cmd,CStringA title,int id){
		Cmd=cmd;
		Title=title;
		Id =id;
	};
	Kjj(CStringA cmd,CStringA title,int id,CStringA js){
		Cmd=cmd;
		Title=title;
		Id =id;
		KjjJs=js;
	};
};
class CKjjDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CKjjDlg)

public:
	CKjjDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CKjjDlg();

// �Ի�������
	enum { IDD = IDD_KJJDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_KjjListCtrl;
	map<CStringA,Kjj>*m_KjjMap;
	virtual BOOL OnInitDialog();
};
