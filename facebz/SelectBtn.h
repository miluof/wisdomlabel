#pragma once


// CSelectBtn

class CSelectBtn : public CButton
{
	DECLARE_DYNAMIC(CSelectBtn)

public:
	CSelectBtn();
	CStringA m_Title1;
	//CFont Font;
	virtual ~CSelectBtn();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
};


