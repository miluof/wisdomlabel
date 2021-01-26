// PicListCtrl.cpp : 实现文件
//

#include "stdafx.h"
#include "WisdomLabel.h"
#include "PicListCtrl.h"


// CPicListCtrl

IMPLEMENT_DYNAMIC(CPicListCtrl, CListCtrl)

CPicListCtrl::CPicListCtrl()
{
	m_SelID=-1;
}

CPicListCtrl::~CPicListCtrl()
{
}


BEGIN_MESSAGE_MAP(CPicListCtrl, CListCtrl)
END_MESSAGE_MAP()



// CPicListCtrl 消息处理程序




void CPicListCtrl::SetSelectItem(int id)
{
	if(id>=0){
		m_SelID=id;
		SetItemState(id,LVIS_SELECTED,LVIS_SELECTED);
	}else{
		SetItemState(m_SelID,LVIS_SELECTED,0);
		m_SelID=-1;
		
	}
}


int CPicListCtrl::GetSelectItem(void)
{
	if(m_SelID<GetItemCount())
		return m_SelID;
	return -1;
}
