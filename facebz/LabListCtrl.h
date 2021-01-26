#pragma once


// CLabListCtrl

class CLabListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CLabListCtrl)

public:
	CLabListCtrl();
	virtual ~CLabListCtrl();

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
};


