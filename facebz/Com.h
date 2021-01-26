#pragma once
#include "stdafx.h"
#include <vector>
#include <map>
using namespace std;
static CStringA GetModuleDir() 
{ 
	HMODULE module = GetModuleHandle(0); 
	TCHAR pFileName[MAX_PATH]; 
	GetModuleFileName(module, pFileName, MAX_PATH); 
	CStringA csFullPath(pFileName);
	int nPos = csFullPath.ReverseFind( _T('\\') ); 
	if( nPos < 0 ) 
		return CStringA(""); 
	else 
		return csFullPath.Left( nPos ); 
}
static CStringA GetWndTxtA(CWnd *pWnd){
	CString Str;
	pWnd->GetWindowText(Str);
	return CStringA(Str);
}
static int AfxMessageBoxA(CStringA Str,UINT ID=MB_OKCANCEL){
	return AfxMessageBox(CString(Str),ID);
}
static map<CStringA,int> SplitStrToMap(CStringA Str,CStringA Sp){
	map<CStringA,int> Res;
	int pos=-1;
	while((pos=Str.Find(Sp))!=-1){
		//Res.push_back(Str.Left(pos));
		Res[Str.Left(pos)]=1;
		Str=Str.Mid(pos+Sp.GetLength());
	}
	if(Str.GetLength()>0)
		Res[Str.Left(pos)]=1;
		//Res.push_back(Str);
	return Res;
}
static vector<CStringA> SplitStr(CStringA Str,CStringA Sp){
	vector<CStringA> Res;
	int pos=-1;
	while((pos=Str.Find(Sp))!=-1){
		Res.push_back(Str.Left(pos));
		Str=Str.Mid(pos+Sp.GetLength());
	}
	if(Str.GetLength()>0)
		Res.push_back(Str);
	return Res;
}

static void SplitStr(vector<CStringA>*Res,CStringA Str,CStringA fgh=","){
	int pos=-1;
	while((pos=Str.Find(fgh))!=-1){
		Res->push_back(Str.Left(pos));
		Str=Str.Mid(pos+fgh.GetLength());
	}
	if(Str.GetLength()>0)
		Res->push_back(Str);
}
static CString IntToStrW(int i){
	CString Str;
	Str.Format(_T("%d"),i);
	return Str;
}
static CStringA IntToStr(int i){
	CStringA Str;
	Str.Format("%d",i);
	return Str;
}
static CStringA FloatToStr(float f ){
	CStringA Str;
	Str.Format("%f",f );
	return Str;
}