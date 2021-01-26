// LabListCtrl.cpp : 实现文件
//

#include "stdafx.h"
#include "WisdomLabel.h"
#include "LabListCtrl.h"


// CLabListCtrl

IMPLEMENT_DYNAMIC(CLabListCtrl, CListCtrl)

CLabListCtrl::CLabListCtrl()
{

}

CLabListCtrl::~CLabListCtrl()
{
}


BEGIN_MESSAGE_MAP(CLabListCtrl, CListCtrl)
END_MESSAGE_MAP()



// CLabListCtrl 消息处理程序




void CLabListCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{

	// TODO:  添加您的代码以绘制指定项
	ASSERT(lpDrawItemStruct->CtlType == ODT_LISTVIEW);  
 CDC dc;   
 dc.Attach(lpDrawItemStruct->hDC);  
 ASSERT(NULL != dc.GetSafeHdc());  
 // Save these value to restore them when done drawing.  
 COLORREF crOldTextColor = dc.GetTextColor();  
 COLORREF crOldBkColor = dc.GetBkColor();  
   
 // If this item is selected, set the background color   
 // and the text color to appropriate values. Also, erase  
 // rect by filling it with the background color.  
 if (
  (lpDrawItemStruct->itemState & ODS_SELECTED))  //(lpDrawItemStruct->itemAction | ODA_SELECT) &&  
 {  
  dc.SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));  
  dc.SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));  
  dc.FillSolidRect(&lpDrawItemStruct->rcItem,   
   ::GetSysColor(COLOR_HIGHLIGHT));  
 }  
 else  
 {  
  if(lpDrawItemStruct->itemID%2)  
   dc.FillSolidRect(&lpDrawItemStruct->rcItem, RGB(128,128,128));  
  else  
   dc.FillSolidRect(&lpDrawItemStruct->rcItem, RGB(255,128,255));  
 }  
   
 // If this item has the focus, draw a red frame around the  
 // item's rect.  
 if ((lpDrawItemStruct->itemAction | ODA_FOCUS) &&  
  (lpDrawItemStruct->itemState & ODS_FOCUS))  
 {  
  CBrush br(RGB(0, 0, 128));  
  dc.FrameRect(&lpDrawItemStruct->rcItem, &br);  
 }  
  
 // Draw the text.  
 CStringA strText(_T(""));  
 CRect rcItem;  
  
 for(int i=0; i<GetHeaderCtrl()->GetItemCount(); i++)  
 {  
  strText = GetItemText(lpDrawItemStruct->itemID, i);  
  GetSubItemRect(lpDrawItemStruct->itemID, i, LVIR_LABEL, rcItem);  
  rcItem.left += 5;  
  dc.DrawText(  
   CString(strText),  
   strText.GetLength(),  
   &rcItem,  
  DT_LEFT|DT_SINGLELINE|DT_VCENTER);  
 }  
   
 // Reset the background color and the text color back to their  
 // original values.  
 dc.SetTextColor(crOldTextColor);  
 dc.SetBkColor(crOldBkColor);  
   
 dc.Detach();  
}
