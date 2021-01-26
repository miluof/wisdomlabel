#pragma once


// CPicListCtrl

class CPicListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CPicListCtrl)

public:
	CPicListCtrl();
	virtual ~CPicListCtrl();
	int m_SelID;

protected:
	DECLARE_MESSAGE_MAP()
public:
	void SetSelectItem(int id);
	int GetSelectItem(void);
};


