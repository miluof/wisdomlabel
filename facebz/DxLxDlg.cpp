// DxLxDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "WisdomLabel.h"
#include "DxLxDlg.h"
#include "afxdialogex.h"
#include <vector>
#include <map>
#include "TextDlg.h"
#include "WisdomLabelDlg.h"
#include "stdafx.h"
#include "Com.h"
using namespace std;

// CDxLxDlg 对话框

IMPLEMENT_DYNAMIC(CDxLxDlg, CDialogEx)

CDxLxDlg::CDxLxDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDxLxDlg::IDD, pParent)
	, m_DxType(_T(""))
{

}

CDxLxDlg::~CDxLxDlg()
{
}

void CDxLxDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_LabListCtrl);
	DDX_Control(pDX, IDC_COMBO3, m_LabBox);
}


BEGIN_MESSAGE_MAP(CDxLxDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDxLxDlg::OnBnClickedOk)
//	ON_WM_MOVE()
ON_NOTIFY(NM_CLICK, IDC_LIST1, &CDxLxDlg::OnNMClickList1)
ON_BN_CLICKED(IDC_BUTTON1, &CDxLxDlg::OnBnClickedButton1)
ON_BN_CLICKED(IDC_BUTTON2, &CDxLxDlg::OnBnClickedButton2)
ON_BN_CLICKED(IDC_BUTTON3, &CDxLxDlg::OnBnClickedButton3)
ON_CBN_EDITCHANGE(IDC_COMBO3, &CDxLxDlg::OnCbnEditchangeCombo3)
ON_CBN_SELCHANGE(IDC_COMBO3, &CDxLxDlg::OnCbnSelchangeCombo3)
ON_NOTIFY(NM_RDBLCLK, IDC_LIST1, &CDxLxDlg::OnNMRDblclkList1)
ON_BN_CLICKED(IDC_BUTTON4, &CDxLxDlg::OnBnClickedButton4)
ON_BN_CLICKED(IDC_BUTTON5, &CDxLxDlg::OnBnClickedButton5)
ON_BN_CLICKED(IDC_BUTTON7, &CDxLxDlg::OnBnClickedButton7)
ON_CBN_SELCHANGE(IDC_COMBO4, &CDxLxDlg::OnCbnSelchangeCombo4)
END_MESSAGE_MAP()


// CDxLxDlg 消息处理程序

//vector<CStringA> bqs;
void CDxLxDlg::OnBnClickedOk()
{
	// TODO: OnBnClickedOpenAnnotation
	m_DxType=GetWndTxtA(GetDlgItem(IDC_COMBO3));//->GetWindowText();
	//bqs.clear();
	int bFind=0;
	for(int i=0;i<m_LabListCtrl.GetItemCount();i++){
		//if(m_LabListCtrl.GetItemState(i,LVIS_SELECTED)==LVIS_SELECTED){
			CStringA Str=CStringA(m_LabListCtrl.GetItemText(i,0));
			//bqs.push_back(Str);
			if(Str==m_DxType){
				bFind=1;
				//break;
			}
			//break;
		//}
	}
	if(!bFind){
		m_LabListCtrl.InsertItem(m_LabListCtrl.GetItemCount(),CString(m_DxType));
	}
	CStringA tName=GetWndTxtA(GetDlgItem(IDC_EDIT1));
	map<CStringA,CStringA> m_LabMap2;
	for(map<CStringA,CStringA>::iterator it=m_LabMap1->begin();it!=m_LabMap1->end();it++){
		if(it->second.IsEmpty())
			continue;
		map<CStringA,CStringA>::iterator it1=m_LabMap2.find(it->second);
		if(it1!=m_LabMap2.end()){
			AfxMessageBoxA("标签"+it->second+"重复");
			return;
		}
		m_LabMap2[it->second]=it->first;
	}
	for(map<CStringA,CStringA>::iterator it=m_LabMap1->begin();it!=m_LabMap1->end();it++){
		if(m_DxType==it->first){
			if(tName!=it->second){
				map<CStringA,CStringA>::iterator it2=m_LabMap2.find(tName);
				if(it2!=m_LabMap2.end()){
					AfxMessageBoxA("标签"+tName+"已经存在");
					return;
				}
				(*m_LabMap1)[it->first]=tName;
				bFind=0;
				break;
			}
		}
		if(tName==it->second){

		}
	}
	if(!bFind){
		//m_LabListCtrl.InsertItem(m_LabListCtrl.GetItemCount(),m_DxType);
		OnBnClickedButton2();
		//
		//bqs.push_back(m_DxType);
	}
	CDialogEx::OnOK();
}

//extern void SplitStr(vector<CStringA>*Res,CStringA Str,CStringA fgh=",");
//map<CStringA,int> LabPic;
BOOL CopyCBitmapFromSrc(CBitmap* pBitmapDest,int nw,int nh, CBitmap* pBitmapSrc){
	BOOL bFlag = FALSE; 	
	CDC dcMemSrc;	
	CDC dcMemDest;	
	BITMAP bmpSrc; 	
	BITMAP bmpDest;
	dcMemSrc.CreateCompatibleDC(NULL);	
	dcMemSrc.SelectObject(pBitmapSrc);	
	pBitmapSrc->GetBitmap(&bmpSrc); 	
	
	dcMemDest.CreateCompatibleDC(NULL);	
	pBitmapDest->CreateCompatibleBitmap(&dcMemSrc, nw, nh);
	pBitmapDest->GetBitmap(&bmpDest);
	dcMemDest.SelectObject(pBitmapDest); 
	if(bmpSrc.bmWidth==bmpDest.bmWidth && bmpSrc.bmHeight==bmpDest.bmHeight)
		bFlag = dcMemDest.BitBlt(0, 0, bmpSrc.bmWidthBytes, bmpSrc.bmHeight, &dcMemSrc, 0, 0, SRCCOPY);
	else{
		dcMemDest.SetStretchBltMode(HALFTONE);
		bFlag = dcMemDest.StretchBlt(0, 0, bmpDest.bmWidthBytes, bmpDest.bmHeight, &dcMemSrc, 0, 0, bmpSrc.bmWidthBytes, bmpSrc.bmHeight,SRCCOPY);
	}
	dcMemSrc.DeleteDC();	
	dcMemDest.DeleteDC();	
	return bFlag;
}
//BOOL CopyCBitmapFromSrc(CBitmap* pBitmapDest, CBitmap* pBitmapSrc){
//	BOOL bFlag = FALSE; 	
//	CDC dcMemSrc;	
//	CDC dcMemDest;	
//	BITMAP bmpSrc; 	
//	dcMemSrc.CreateCompatibleDC(NULL);	
//	dcMemSrc.SelectObject(pBitmapSrc);	
//	pBitmapSrc->GetBitmap(&bmpSrc); 	
//	dcMemDest.CreateCompatibleDC(NULL);	
//	pBitmapDest->CreateCompatibleBitmap(&dcMemSrc, bmpSrc.bmWidth, bmpSrc.bmHeight);	
//	dcMemDest.SelectObject(pBitmapDest); 	
//	bFlag = dcMemDest.BitBlt(0, 0, bmpSrc.bmWidthBytes, bmpSrc.bmHeight, &dcMemSrc, 0, 0, SRCCOPY);
//	dcMemSrc.DeleteDC();	
//	dcMemDest.DeleteDC();	
//	return bFlag;
//}
BOOL CDxLxDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	//UpdateData(0);
	
	int v[10]={6,8,12,16,24,32,48,64,96,128};
	for(int i=0;i<8;i++){
		CStringA Str;
		Str.Format("%d",v[i]);
		((CComboBox*)GetDlgItem(IDC_COMBO4))->AddString(CString(Str));

	}
	int nsel=0;
	for(;nsel<8;nsel++){
		if(v[nsel]==m_MainDlg->m_LabSize){
			break;
		}
	}
	((CComboBox*)GetDlgItem(IDC_COMBO4))->SetCurSel(nsel);
	//GetDlgItem(IDC_COMBOBOX4))->
	int iw=m_MainDlg->m_LabSize;
	m_imList.Create(iw,iw,ILC_COLOR24,0,20);
	/*CBitmap * pBmp = NULL;
	pBmp = new CBitmap();
	pBmp->LoadBitmap(IDB_BITMAP1);
	m_imList.Add(pBmp,RGB(0,0,0));
	delete pBmp;*/
	for(map<CStringA,CBitmap *>::iterator it=m_LabMap->begin();it!=m_LabMap->end();it++){
		if(it->second==NULL){
			//m_LabListCtrl.InsertItem(m_LabListCtrl.GetItemCount(),it->first);
		}else{
			CBitmap *Bit1=it->second;
			BITMAP bm;
			Bit1->GetBitmap(&bm);
			if(bm.bmWidth==iw || bm.bmHeight==iw)
				m_imList.Add(Bit1,RGB(0,0,0));
			else{
				CBitmap *bit2=new CBitmap;
				CopyCBitmapFromSrc(bit2,iw,iw,Bit1);
				/*CDC *pDC=GetDC();
				CDC memDC1;
				CDC memDC2;
				memDC1.CreateCompatibleDC(pDC);
				memDC2.CreateCompatibleDC(pDC);
				CBitmap *bit2=new CBitmap;
				bit2->CreateCompatibleBitmap(pDC,iw,iw);
				CBitmap *oldBit1=memDC1.SelectObject(Bit1);
				CBitmap *oldBit2=memDC2.SelectObject(bit2);
				memDC2.StretchBlt(0,0,iw,iw,pDC,0,0,bm.bmWidth,bm.bmHeight,SRCCOPY);
				memDC1.SelectObject(oldBit1);
				memDC2.SelectObject(oldBit2);
				memDC1.DeleteDC();
				memDC2.DeleteDC();
				ReleaseDC(pDC);*/
				m_imList.Add(bit2,RGB(0,0,0));
				//it->second=bit2;
				//delete bit2;
				bit2->DeleteObject();
				delete bit2;
			}
		}
	}
	m_LabListCtrl.SetImageList(&m_imList,LVSIL_NORMAL );
	int j=0;
	for(map<CStringA,CBitmap *>::iterator it=m_LabMap->begin();it!=m_LabMap->end();it++){
		if(it->second==NULL){
			LVITEM lvItem={0};
			lvItem.mask = LVIF_IMAGE|LVIF_TEXT|LVIF_STATE;	//文字、图片、状态
			lvItem.iItem = m_LabListCtrl.GetItemCount();		//行号(第一行)
			lvItem.iImage = -1;//it->second;	//图片索引号(第一幅图片)
			lvItem.iSubItem = 0;	//子列号
			int k = m_LabListCtrl.InsertItem(&lvItem);		//第一列为图片
			m_LabListCtrl.SetItemText(k,0,CString(it->first));  //第
			if(m_DxType==it->first){
				m_LabListCtrl.SetItemState(k,LVIS_SELECTED,LVIS_SELECTED);
			}
		}else{
			LVITEM lvItem={0};
			lvItem.mask = LVIF_IMAGE|LVIF_TEXT|LVIF_STATE;	//文字、图片、状态
			lvItem.iItem = m_LabListCtrl.GetItemCount();		//行号(第一行)
			lvItem.iImage = j;//it->second;	//图片索引号(第一幅图片)
			lvItem.iSubItem = 0;	//子列号
			int k = m_LabListCtrl.InsertItem(&lvItem);		//第一列为图片
			m_LabListCtrl.SetItemText(k,0,CString(it->first));  //第
			if(m_DxType==it->first){
				m_LabListCtrl.SetItemState(k,LVIS_SELECTED,LVIS_SELECTED);
			}
			j++;
		}
		m_LabBox.AddString(CString(it->first));
	}
	GetDlgItem(IDC_COMBO3)->SetWindowText(CString(m_DxType));
	GetDlgItem(IDC_EDIT1)->SetWindowText(CString((*m_LabMap1)[m_DxType]));
	CStringA tName=(*m_LabMap1)[m_DxType];
	if(tName.IsEmpty())
		tName=m_DxType;
	m_LabListCtrl.SetFocus();


	//m_LabListCtrl.InsertItem(0,"plate");
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


//void CDxLxDlg::OnMove(int x, int y)
//{
//	CDialogEx::OnMove(x, y);
//	
//	// TODO: 在此处添加消息处理程序代码
//}


void CDxLxDlg::OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: OnBnClickedOpenAnnotation
	*pResult = 0;
	for(int i=0;i<m_LabListCtrl.GetItemCount();i++){
		if(m_LabListCtrl.GetItemState(i,LVIS_SELECTED)==LVIS_SELECTED){
			CStringA Str=CStringA(m_LabListCtrl.GetItemText(i,0));
			GetDlgItem(IDC_COMBO3)->SetWindowText(CString(Str));
			GetDlgItem(IDC_EDIT1)->SetWindowText(CString((*m_LabMap1)[Str]));
			OnBnClickedOk();
			break;
		}
	}
	//AfxMessageBox("y");
}


void CDxLxDlg::OnBnClickedButton1()
{
	// TODO: OnBnClickedOpenAnnotation
	CFileDialog Dlg(TRUE,NULL,NULL,NULL,NULL,NULL);
	if(Dlg.DoModal()==IDOK){
		LoadLabel(CStringA(Dlg.GetPathName()));
	}
}


void CDxLxDlg::LoadLabel(CStringA LabelName)
{
	CFile f;
	if(f.Open(CString(LabelName),CFile::modeRead)){
		char Buf[2049];
		int size=f.Read(Buf,2048);
		f.Close();
		m_LabListCtrl.DeleteAllItems();
		if(size>0){
			Buf[size]=0;
			CStringA Str(Buf);
			vector<CStringA> Strs;
			SplitStr(&Strs,Str);
			for(int j=0;j<Strs.size();j++){
				m_LabListCtrl.InsertItem(m_LabListCtrl.GetItemCount(),CString(Strs.at(j)));
				if(m_DxType == Strs.at(j)){
					m_LabListCtrl.SetItemState(j,LVIS_SELECTED,LVIS_SELECTED);
				}
			}
			
		}
	}
}

extern CStringA GetModuleDir();
void CDxLxDlg::OnBnClickedButton2()
{
	// TODO: OnBnClickedOpenAnnotation

	//CFileDialog Dlg(TRUE,NULL,NULL,NULL,NULL,NULL);
	//if(Dlg.DoModal()==IDOK){
		CStringA All;
		for(int i=0;i<m_LabListCtrl.GetItemCount();i++){
				CStringA Str=CStringA(m_LabListCtrl.GetItemText(i,0));
				CStringA tName=(*m_LabMap1)[Str];
				if(!tName.IsEmpty()){
					Str+=","+tName;
				}
				All+=Str+"\r\n";
				//break;

		}
		CFile f;
		if(f.Open(CString(GetModuleDir()+"\\cfg\\itemsname.txt"),CFile::modeCreate|CFile::modeWrite)){
			f.Write(All,All.GetLength());
			f.Close();
		}
		//LoadLabel(Dlg.GetPathName());
	//}
}


void CDxLxDlg::OnBnClickedButton3()
{
	// TODO: OnBnClickedOpenAnnotation
	m_LabListCtrl.DeleteAllItems();
}


void CDxLxDlg::OnCbnEditchangeCombo3()
{
	// TODO: OnBnClickedOpenAnnotation
	m_DxType=GetWndTxtA(GetDlgItem(IDC_COMBO3));
	GetDlgItem(IDC_EDIT1)->SetWindowText(CString((*m_LabMap1)[m_DxType]));
}


void CDxLxDlg::OnCbnSelchangeCombo3()
{
	// TODO: OnBnClickedOpenAnnotation
	int cursel = ((CComboBox *)GetDlgItem(IDC_COMBO3))->GetCurSel();
	CString dxType;
	((CComboBox *)GetDlgItem(IDC_COMBO3))->GetLBText(cursel,dxType);
	m_DxType=CStringA(dxType);
	//GetDlgItem(IDC_COMBO3)->GetWindowText(m_DxType);
	GetDlgItem(IDC_EDIT1)->SetWindowText(CString((*m_LabMap1)[m_DxType]));
}


void CDxLxDlg::OnNMRDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: OnBnClickedOpenAnnotation
	*pResult = 0;
	/*for(int i=0;i<m_LabListCtrl.GetItemCount();i++){
		if(m_LabListCtrl.GetItemState(i,LVIS_SELECTED)==LVIS_SELECTED){
			CStringA Str=m_LabListCtrl.GetItemText(i,0);
			GetDlgItem(IDC_COMBO3)->SetWindowText(Str);
			GetDlgItem(IDC_EDIT1)->SetWindowText((*m_LabMap1)[Str]);
			OnBnClickedOk();
			break;
		}
	}*/
	*pResult = 0;
}


void CDxLxDlg::OnBnClickedButton4()
{
	// TODO: OnBnClickedOpenAnnotation

	//CStringA tName;
	//GetDlgItem(IDC_EDIT1)->GetWindowText(tName);
	//map<CStringA,CStringA> m_LabMap2;
	//for(map<CStringA,CStringA>::iterator it=m_LabMap1->begin();it!=m_LabMap1->end();it++){
	//	if(it->second.IsEmpty())
	//		continue;
	//	map<CStringA,CStringA>::iterator it1=m_LabMap2.find(it->second);
	//	if(it1!=m_LabMap2.end()){
	//		AfxMessageBox("标签"+it->second+"重复");
	//		return;
	//	}
	//	m_LabMap2[it->second]=it->first;
	//}
	//for(map<CStringA,CStringA>::iterator it=m_LabMap1->begin();it!=m_LabMap1->end();it++){
	//	if(m_DxType==it->first){
	//		if(tName!=it->second){
	//			map<CStringA,CStringA>::iterator it2=m_LabMap2.find(tName);
	//			if(it2!=m_LabMap2.end()){
	//				AfxMessageBox("标签"+tName+"已经存在");
	//				return;
	//			}
	//			(*m_LabMap1)[it->first]=tName;
	//			//bFind=0;
	//			break;
	//		}
	//	}
	//	if(tName==it->second){

	//	}
	//}
		CStringA All;
		for(int i=0;i<m_LabListCtrl.GetItemCount();i++){
				CStringA Str=CStringA(m_LabListCtrl.GetItemText(i,0));
				CStringA tName=(*m_LabMap1)[Str];
				if(!tName.IsEmpty()){
					Str=tName;
				}
				All+="'"+Str+"',";
				//break;

		}
		CTextDlg Dlg;
		Dlg.m_Text=All;
		Dlg.DoModal();

}


void CDxLxDlg::OnBnClickedButton5()
{
	// TODO: OnBnClickedOpenAnnotation
	CStringA All;
		for(int i=0;i<m_LabListCtrl.GetItemCount();i++){
				CStringA Str=CStringA(m_LabListCtrl.GetItemText(i,0));
				CStringA tName=(*m_LabMap1)[Str];
				if(!tName.IsEmpty()){
					Str=tName;
				}
				All+=Str+"\r\n";
				//break;

		}
		CTextDlg Dlg;
		Dlg.m_Text=All;
		Dlg.DoModal();
}


void CDxLxDlg::OnBnClickedButton7()
{
	// TODO: OnBnClickedOpenAnnotation
		CStringA All;
		for(int i=0;i<m_LabListCtrl.GetItemCount();i++){
			CStringA Str=CStringA(m_LabListCtrl.GetItemText(i,0));
				CStringA tName=(*m_LabMap1)[Str];
				if(!tName.IsEmpty()){
					Str+=","+tName;
				}
				All+=Str+"\r\n";
				//break;

		}
		CTextDlg Dlg;Dlg.m_Text=All;
		if(Dlg.DoModal()!=IDOK){
			return;
		}
		All=Dlg.m_Text;
		CFile f;
		if(f.Open(CString(GetModuleDir()+"\\cfg\\itemsname.txt"),CFile::modeCreate|CFile::modeWrite)){
			f.Write(All,All.GetLength());
			f.Close();
			m_MainDlg->InitLab();
			AfxMessageBoxA("标签已保存,下次打开这个对话框生效");
			//OnOK();

		}
		//Dlg.DoModal();
}


void CDxLxDlg::OnCbnSelchangeCombo4()
{
	// TODO: OnBnClickedOpenAnnotation
	CStringA Str;
	int cursel = ((CComboBox *)GetDlgItem(IDC_COMBO4))->GetCurSel();
	CString Str1;
	((CComboBox *)GetDlgItem(IDC_COMBO4))->GetLBText(cursel,Str1);
	Str=CStringA(Str1);
	//GetDlgItem(IDC_COMBO3)->GetWindowText(m_DxType);
	int size=atoi(Str);
	if(size>128)
		size=128;
	if(size<12)
		size=12;
	m_MainDlg->m_LabSize=size;
	//AfxMessageBox("尺寸已经更改,下次打开这个对话框生效");
	//OnOK();

	m_imList.DeleteImageList();
	m_LabListCtrl.DeleteAllItems();
	int iw=size;
	m_imList.Create(iw,iw,ILC_COLOR24,0,20);
	/*CBitmap * pBmp = NULL;
	pBmp = new CBitmap();
	pBmp->LoadBitmap(IDB_BITMAP1);
	m_imList.Add(pBmp,RGB(0,0,0));
	delete pBmp;*/
	for(map<CStringA,CBitmap *>::iterator it=m_LabMap->begin();it!=m_LabMap->end();it++){
		if(it->second==NULL){
			//m_LabListCtrl.InsertItem(m_LabListCtrl.GetItemCount(),it->first);
		}else{
			CBitmap *Bit1=it->second;
			BITMAP bm;
			Bit1->GetBitmap(&bm);
			if(bm.bmWidth==iw || bm.bmHeight==iw)
				m_imList.Add(Bit1,RGB(0,0,0));
			else{
				CBitmap *bit2=new CBitmap;
				CopyCBitmapFromSrc(bit2,iw,iw,Bit1);
				/*CDC *pDC=GetDC();
				CDC memDC1;
				CDC memDC2;
				memDC1.CreateCompatibleDC(pDC);
				memDC2.CreateCompatibleDC(pDC);
				CBitmap *bit2=new CBitmap;
				bit2->CreateCompatibleBitmap(pDC,iw,iw);
				CBitmap *oldBit1=memDC1.SelectObject(Bit1);
				CBitmap *oldBit2=memDC2.SelectObject(bit2);
				memDC2.StretchBlt(0,0,iw,iw,pDC,0,0,bm.bmWidth,bm.bmHeight,SRCCOPY);
				memDC1.SelectObject(oldBit1);
				memDC2.SelectObject(oldBit2);
				memDC1.DeleteDC();
				memDC2.DeleteDC();
				ReleaseDC(pDC);*/
				m_imList.Add(bit2,RGB(0,0,0));
				//it->second=bit2;
				//delete bit2;
				bit2->DeleteObject();
				delete bit2;
			}
		}
	}
	m_LabListCtrl.SetImageList(&m_imList,LVSIL_NORMAL );
	int j=0;
	for(map<CStringA,CBitmap *>::iterator it=m_LabMap->begin();it!=m_LabMap->end();it++){
		if(it->second==NULL){
			LVITEM lvItem={0};
			lvItem.mask = LVIF_IMAGE|LVIF_TEXT|LVIF_STATE;	//文字、图片、状态
			lvItem.iItem = m_LabListCtrl.GetItemCount();		//行号(第一行)
			lvItem.iImage = -1;//it->second;	//图片索引号(第一幅图片)
			lvItem.iSubItem = 0;	//子列号
			int k = m_LabListCtrl.InsertItem(&lvItem);		//第一列为图片
			m_LabListCtrl.SetItemText(k,0,CString(it->first));  //第
			if(m_DxType==it->first){
				m_LabListCtrl.SetItemState(k,LVIS_SELECTED,LVIS_SELECTED);
			}
		}else{
			LVITEM lvItem={0};
			lvItem.mask = LVIF_IMAGE|LVIF_TEXT|LVIF_STATE;	//文字、图片、状态
			lvItem.iItem = m_LabListCtrl.GetItemCount();		//行号(第一行)
			lvItem.iImage = j;//it->second;	//图片索引号(第一幅图片)
			lvItem.iSubItem = 0;	//子列号
			int k = m_LabListCtrl.InsertItem(&lvItem);		//第一列为图片
			m_LabListCtrl.SetItemText(k,0,CString(it->first));  //第
			if(m_DxType==it->first){
				m_LabListCtrl.SetItemState(k,LVIS_SELECTED,LVIS_SELECTED);
			}
			j++;
		}
		//m_LabBox.AddString(it->first);
	}
}
