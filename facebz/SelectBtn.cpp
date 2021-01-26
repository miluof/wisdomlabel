// SelectBtn.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "WisdomLabel.h"
#include "SelectBtn.h"

#include "Com.h"
// CSelectBtn

IMPLEMENT_DYNAMIC(CSelectBtn, CButton)

CSelectBtn::CSelectBtn()
{

}

CSelectBtn::~CSelectBtn()
{
}


BEGIN_MESSAGE_MAP(CSelectBtn, CButton)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()



// CSelectBtn ��Ϣ�������




BOOL CSelectBtn::OnEraseBkgnd(CDC* pDC)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	return CButton::OnEraseBkgnd(pDC);
}


void CSelectBtn::DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/)
{
	CFont *font = GetFont();
	CDC *pDC=GetDC();
	CFont *pOldFont=pDC->SelectObject(font);
	CRect Rc;
	GetClientRect(Rc);
	pDC->RoundRect(Rc,CPoint(3,3));
	CStringA Str=GetWndTxtA(this);
	//GetWindowText(Str);
	CRect Rc1=Rc;
	Rc1.bottom-=Rc1.Height()/2;
	CRect Rc2=Rc;
	Rc2.top=Rc1.bottom-2;
	pDC->DrawText(CString(m_Title1),Rc1,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	pDC->DrawText(CString(Str),Rc2,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
	pDC->SelectObject(pOldFont);
	ReleaseDC(pDC);
	// TODO:  ������Ĵ����Ի���ָ����
}
