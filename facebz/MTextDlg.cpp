// MTextDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "WisdomLabel.h"
#include "MTextDlg.h"
#include "afxdialogex.h"
#include "com.h"

// CMTextDlg 对话框

IMPLEMENT_DYNAMIC(CMTextDlg, CDialogEx)

CMTextDlg::CMTextDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMTextDlg::IDD, pParent)
{
	for(int i=0;i<20;i++)
		bLl[i]=0;
	labWidth=250;
	TextWidth=250;
}

CMTextDlg::~CMTextDlg()
{
}

void CMTextDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMTextDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CMTextDlg::OnBnClickedOk)
	ON_WM_PAINT()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


// CMTextDlg 消息处理程序


BOOL CMTextDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	int hasLl=0;
	int w1=labWidth;
	int w2=TextWidth;
	int top=5;//120;
	for(int i=0;i<Size;i++){
			if(bLl[i]){
				hasLl=1;
				break;
			}
	}
	if(!SaveTitle.IsEmpty()){
		for(int i=0;i<Size;i++){
			if(!bSave[i])
				continue;
			CFile fi;
			CStringA HisDir=GetModuleDir()+"\\cfg\\"+SaveTitle+IntToStr(i)+".txt";
			if(fi.Open(CString(HisDir),CFile::modeRead)){
				char Buf[4096];
				int size=fi.Read(Buf,4095);
				Buf[size]=0;
				vector<CStringA> List=SplitStr(CStringA(Buf),"\r\n");
				CStringA ss;
				for(int k=0;k<List.size();k++){
					ss=List.at(k);
				//while(fi.ReadString(ss)){
					ss.Replace("\r","");
					ss.Replace("\n","");
					Datas[i].AddTail(CString(ss));
				}
				fi.Close();
			}
		}
	}
	
	/*if(hasLl)
		w2=800;*/
	if(Size>0){
		pStatic=new CStatic[Size];
		pTexts=new CComboBox[Size];
		pBtRc=new CRect[Size];
		
		for(int i=0;i<Size;i++){
			CRect Rc(3,top+25*i,w1+3,top+25*i+20);
			CRect Rc1=Rc;
			Rc1.OffsetRect(w1+6,0);
			pBtRc[i]=Rc1;
			Rc1.bottom=Rc1.top+200;
			Rc1.right=Rc1.left+w2;
			
			pBtRc[i].OffsetRect(w2+6,0);
			pBtRc[i].right=pBtRc[i].left+80;
			Rc.top+=2;
			pStatic[i].Create(CString(Titles[i]),ES_RIGHT|WS_CHILD|WS_VISIBLE,Rc,this,3000+i);
			pTexts[i].Create(WS_CHILD|WS_VISIBLE|WS_TABSTOP|CBS_AUTOHSCROLL|CBS_DROPDOWN | WS_VSCROLL | WS_TABSTOP ,Rc1,this,4000+i);
			if(Datas[i].GetCount()>0){
				POSITION pos=Datas[i].GetHeadPosition();
				while(pos){
					CStringA tmp=CStringA(Datas[i].GetNext(pos));
					pTexts[i].AddString(CString(tmp));
				}
				//pTexts[i].SetCurSel(0);
			}
			if(Texts[i].IsEmpty()){
				pTexts[i].SetCurSel(0);
			}else
				pTexts[i].SetWindowText(CString(Texts[i]));
			if(bLl[i])
				hasLl=1;
		}
		//SetWindowText("提示");
		//GetDlgItem(IDC_EDIT1)->
		SetWindowText(CString(Title));
		
		//this->SetWindowRect(
	}
	CRect Rc(0,0,w1+w2+20,25*Size+65+top);
	if(hasLl)
			Rc.right+=85;
		MoveWindow(Rc);
		CenterWindow();
	CRect RcWindow=Rc;
	GetClientRect(RcWindow);
	CRect RcOk;
	GetDlgItem(IDOK)->GetWindowRect(RcOk);
	ScreenToClient(RcOk);
	CRect RcCancel;
	GetDlgItem(IDCANCEL)->GetWindowRect(RcCancel);
	ScreenToClient(RcCancel);
	int ww=RcCancel.right-RcOk.left;
	int xo=RcWindow.Width()-ww;
	xo/=2;
	xo=xo-RcOk.left;
	int yo=RcWindow.bottom-5-RcOk.bottom;
	RcOk.OffsetRect(xo,yo);
	RcCancel.OffsetRect(xo,yo);
	GetDlgItem(IDOK)->MoveWindow(RcOk);
	GetDlgItem(IDCANCEL)->MoveWindow(RcCancel);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CMTextDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	for(int i=0;i<Size;i++){
		Texts[i]=GetWndTxtA(&pTexts[i]);//.GetWindowText();
	}
	if(!SaveTitle.IsEmpty()){
		for(int i=0;i<Size;i++){
			if(!bSave[i])
				continue;
			if(Texts[i].IsEmpty())
				continue;
			int bNew=1;
			int j=0;
			POSITION pos=Datas[i].GetHeadPosition();
			while(pos){
				POSITION posprev=pos;
				if(Texts[i] == CStringA(Datas[i].GetNext(pos))){
					if(j==0){
						bNew=0;
						break;
					}else{
						Datas[i].RemoveAt(posprev);
					}
				}
				j++;
			}
			if(!bNew)
				continue;
			Datas[i].AddHead(CString(Texts[i]));
			CFile fi;
			CStringA HisDir=GetModuleDir()+"\\cfg\\"+SaveTitle+IntToStr(i)+".txt";
			if(fi.Open(CString(HisDir),CFile::modeCreate|CFile::modeWrite)){
				POSITION pos=Datas[i].GetHeadPosition();
				while(pos){
					CStringA Str=CStringA(Datas[i].GetNext(pos))+"\r\n";
					fi.Write(Str,Str.GetLength());
				}
				fi.Close();
			}
		}
	}
	CDialogEx::OnOK();
}


void CMTextDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialogEx::OnPaint()
	for(int i=0;i<20;i++){
		if(bLl[i]){
			dc.RoundRect(pBtRc[i],CPoint(5,5));
			dc.DrawText(CString("浏览"),CString("浏览").GetLength(),pBtRc[i],DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		}
	}
}


void CMTextDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	for(int i=0;i<20;i++){
		if(bLl[i]){
			
			if(pBtRc[i].PtInRect(point)){
				CStringA tmp=GetWndTxtA((CWnd *)&pTexts[i]);
				if(bLl[i]==1){
					CFileDialog Dlg(TRUE,NULL,NULL,NULL,NULL,NULL);
					//Dlg.m_ofn.lpstrInitialDir=CString(tmp);
					if(Dlg.DoModal()==IDOK)
						pTexts[i].SetWindowText(Dlg.GetPathName());
				}else{
					CFolderPickerDialog  Dlg(NULL,0,this,0);
					//Dlg.m_ofn.lpstrInitialDir=CString(tmp);
					if(Dlg.DoModal()==IDOK)
						pTexts[i].SetWindowText(Dlg.GetFolderPath());
				}
			}
		}
	}
	CDialogEx::OnLButtonUp(nFlags, point);
}
