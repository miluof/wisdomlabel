
// WisdomLabelDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "WisdomLabel.h"
#include "WisdomLabelDlg.h"
#include "afxdialogex.h"
#include <math.h>
#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <algorithm>
#include "DxLxDlg.h"
#include "PicEnlargeDlg.h"
#include "ScaleEnlargeDlg.h"
#include "RotateSettingDlg.h"
#include "LabeCropDlg.h"
#include "TextDlg.h"
#include "Text2Dlg.h"
#include "MTextDlg.h"
#include "convert.h"
#include "Com.h"
using namespace std;
//#include <atlimage.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CWisdomLabelDlg 对话框

#include <dbghelp.h>

#pragma comment(lib, "Dbghelp.lib")

LONG WINAPI MyUnhandledFilter(struct _EXCEPTION_POINTERS *lpExceptionInfo)
{
  LONG ret = EXCEPTION_EXECUTE_HANDLER;

  TCHAR szFileName[MAX_PATH];
  SYSTEMTIME st;

  ::GetLocalTime(&st);

    wsprintf(szFileName, _T("c:\\hstplayctrl_%04d-%02d-%02d-%02d-%02d-%02d-%02d-%02d.dmp"), 
      st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, rand()%100);
  //wsprintf(szFileName, _T("d:\\test.dmp"));

  HANDLE hFile = ::CreateFile(szFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
  if (hFile != INVALID_HANDLE_VALUE)
  {
    MINIDUMP_EXCEPTION_INFORMATION ExInfo;

    ExInfo.ThreadId = ::GetCurrentThreadId();
    ExInfo.ExceptionPointers = lpExceptionInfo;
    ExInfo.ClientPointers = false;

    // write the dump

    BOOL bOK = MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &ExInfo, NULL, NULL );

    if (bOK)
    {
      printf("Create Dump File Success!\n");
    }
    else
    {
      printf("MiniDumpWriteDump Failed: %d\n", GetLastError());
    }

    ::CloseHandle(hFile);
  }
  else
  {
    printf("Create File %s Failed %d\n", szFileName, GetLastError());
  }
  return ret;
}
#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
//ref:https://github.com/serge-rgb/TinyJPEG/blob/master/tiny_jpeg.h
#define TJE_IMPLEMENTATION
#include "tiny_jpeg.h"
#include "timing.h"
/**用这个偏色严重**/
unsigned char *loadImage(const char *filename, int *Width, int *Height, int *Channels) {
	unsigned char *res= stbi_load(filename, Width, Height, Channels, 3);
	return res;
}

int saveImg(const char *savePath, unsigned char *data,int width, int height, int channels)
{
	int ret = tje_encode_to_file(savePath, width, height, channels, true, data);

	if(ret==0)
	{
		//printf_err("tje_encode_to_file failed!\n");
		return -1;
	}else
	{
		return 0;
	}
}
unsigned char *GetImgDatajx(CImage *img,int cn,int &w,int &h);
int saveImg(const char *savePath,CImage *pImg)
{
	int cn=3;
	int w,h;
	unsigned char *pData=GetImgDatajx(pImg,cn,w,h);
	int res=saveImg(savePath,pData,w,h,cn);
	delete []pData;
	return res;
}
/*HRESULT SaveJpeg(CImage*pImg, LPCTSTR pszFileName,int quality)
{
    LONG paramValue = quality;
    Gdiplus::EncoderParameter ep;
    ep.Guid = Gdiplus::EncoderQuality;
    ep.NumberOfValues = 1;
    ep.Type = Gdiplus::EncoderParameterValueType::EncoderParameterValueTypeLong;
    ep.Value = &paramValue;

    Gdiplus::EncoderParameters eps;
    eps.Count = 1;
    eps.Parameter[0] = ep;
    return pImg->Save(pszFileName,Gdiplus::ImageFormatJPEG,&eps);
} */


CWisdomLabelDlg::CWisdomLabelDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CWisdomLabelDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_MouseState=MOUSESTATENONE;
	m_Angle=0;
	m_pImg=NULL;
	m_DrawLk=1;
	m_TextColor=RGB(255,0,0);
	m_bInit=0;
	m_IconW=32;
	m_SectPtW=64;
	m_CmdState=CMDNONE;
	m_CropImg=m_pImg=NULL;
	m_CropMb=NULL;
	m_BackOFront=0;
	m_AutoBzPicSize="512";
	m_SelLkID=m_SelPtID=m_SelLineID=-1;
	m_AutoBzSizeType="0";
	m_IsPCl=0;
	m_PTimes=0;
	m_PiThreadRun=0;
	m_PiBtnId=0;
	 m_LeftWidth=175;
	 m_RightWidth=200;
	//m_TarRect.left=100;
	//m_TarRect.top=5;
}

void CWisdomLabelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK1, m_AutoHdBtn);
	DDX_Control(pDX, IDC_CHECK2, m_ZdZfxBtn);
	DDX_Control(pDX, IDC_CHECK3, m_KeduBtn);
	DDX_Control(pDX, IDC_IMGLIST, m_pImgListCtrl);
	DDX_Control(pDX, IDC_COMBO1, m_FyTjBox);
	DDX_Control(pDX, IDC_CHECK5, m_DisLabTextBtn);
	DDX_Control(pDX, IDC_CHECK6, m_DisLabIconBtn);
	DDX_Control(pDX, IDC_MAINEXPLORER, m_ie);
	DDX_Control(pDX, IDC_CHECK7, m_KeduBtn32);
	DDX_Control(pDX, IDC_CHECK4, m_KeBtn8);
	DDX_Control(pDX, IDC_CHECK8, m_AutoFzDx);
	DDX_Control(pDX, IDC_CHECK9, m_ZdZfxjBtn);
	DDX_Control(pDX, IDC_CHECK10, m_Auto16b9jBtn);
	DDX_Control(pDX, IDSEARCHOPTIONS, m_BlSxXBtn);
	DDX_Control(pDX, IDPICSPACE, m_KdBtn);
	DDX_Control(pDX, IDNEXTPIC4, m_MrBzBtn);
	DDX_Control(pDX, IDSETOPPOINTDIR, m_TmpDirBtn);
	DDX_Control(pDX, IDSETSEARCHFILENAME, m_FindNrBtn);
	DDX_Control(pDX, IDNEXTPIC7, m_InputSizeBtn);
	DDX_Control(pDX, IDC_LITTLEEXPLORER, m_ie1);
}

BEGIN_MESSAGE_MAP(CWisdomLabelDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOPENPIC, &CWisdomLabelDlg::OnBnClickedOpenpic)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDCROPFACE, &CWisdomLabelDlg::OnBnClickedCropface)
	ON_WM_LBUTTONUP()
	ON_BN_CLICKED(IDSAVEANNOTATION, &CWisdomLabelDlg::OnBnClickedSaveAnnotation)
	ON_BN_CLICKED(IDOPENANNOTATION, &CWisdomLabelDlg::OnBnClickedOpenAnnotation)
	//ON_BN_CLICKED(IDCREATELABEL, &CWisdomLabelDlg::OnBnClickedCreatelabel)
	ON_WM_RBUTTONUP()
	ON_BN_CLICKED(IDCSETPOINT, &CWisdomLabelDlg::OnBnClickedCsetpoint)
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_CBN_SELCHANGE(IDC_COMBO2, &CWisdomLabelDlg::OnCbnSelchangeCombo2)
	ON_BN_CLICKED(IDNEXTPIC, &CWisdomLabelDlg::OnBnClickedOpenpic2)
	ON_BN_CLICKED(IDPREVPIC, &CWisdomLabelDlg::OnBnClickedOpenpic3)
	ON_BN_CLICKED(IDCROPFACE2, &CWisdomLabelDlg::OnBnClickedCropface2)
	//ON_MESSAGE(WM_MOUSEWHEEL,&CWisdomLabelDlg::OnMouseHWheel)
	ON_WM_MOUSEHWHEEL()
	ON_WM_MOUSEWHEEL()
	ON_BN_CLICKED(IDOPENPIC4, &CWisdomLabelDlg::OnBnClickedOpenpic4)
	ON_BN_CLICKED(IDCREATELABEL2, &CWisdomLabelDlg::OnBnClickedCreatelabel2)
	ON_BN_CLICKED(IDCREATELABEL3, &CWisdomLabelDlg::OnBnClickedCreatelabel3)
	ON_BN_CLICKED(IDCREATELABEL4, &CWisdomLabelDlg::OnBnClickedCreatelabel4)
	ON_BN_CLICKED(IDCREATELABEL5, &CWisdomLabelDlg::OnBnClickedCreatelabel5)
	ON_BN_CLICKED(IDDBX, &CWisdomLabelDlg::OnBnClickedDbx)
	ON_BN_CLICKED(IDCSETPOINT2, &CWisdomLabelDlg::OnBnClickedCsetpoint2)
	ON_BN_CLICKED(IDIDCREATEDATE, &CWisdomLabelDlg::OnBnClickedIdcreatedate)
	ON_BN_CLICKED(IDIDCREATEDATE3, &CWisdomLabelDlg::OnBnClickedIdcreatedate3)
	ON_BN_CLICKED(IDIDCREATEDATE4, &CWisdomLabelDlg::OnBnClickedIdcreatedate4)
	ON_BN_CLICKED(IDIDCREATEDATE5, &CWisdomLabelDlg::OnBnClickedIdcreatedate5)
	ON_BN_CLICKED(IDIDCREATEDATE6, &CWisdomLabelDlg::OnBnClickedIdcreatedate6)
	ON_BN_CLICKED(IDOPENPIC5, &CWisdomLabelDlg::OnBnClickedOpenpic5)
	ON_BN_CLICKED(IDDELETEPIC, &CWisdomLabelDlg::OnBnClickedOpenpic6)
	ON_BN_CLICKED(IDIDCREATEDATE7, &CWisdomLabelDlg::OnBnClickedIdcreatedate7)
	ON_BN_CLICKED(IDIDCREATEDATE8, &CWisdomLabelDlg::OnBnClickedIdcreatedate8)
	ON_BN_CLICKED(IDIDCREATEDATE9, &CWisdomLabelDlg::OnBnClickedIdcreatedate9)
	ON_BN_CLICKED(IDOPENPIC7, &CWisdomLabelDlg::OnBnClickedOpenpic7)
	ON_BN_CLICKED(IDOPENPIC8, &CWisdomLabelDlg::OnBnClickedOpenpic8)
	ON_BN_CLICKED(IDOPENPIC9, &CWisdomLabelDlg::OnBnClickedOpenpic9)
	ON_BN_CLICKED(IDIDCREATEDATE10, &CWisdomLabelDlg::OnBnClickedIdcreatedate10)
	ON_BN_CLICKED(IDDBX2, &CWisdomLabelDlg::OnBnClickedDbx2)
	ON_BN_CLICKED(IDOPENPIC10, &CWisdomLabelDlg::OnBnClickedOpenpic10)
	ON_BN_CLICKED(IDIDCREATEDATE11, &CWisdomLabelDlg::OnBnClickedIdcreatedate11)
	ON_BN_CLICKED(IDDBX3, &CWisdomLabelDlg::OnBnClickedDbx3)
	ON_BN_CLICKED(IDIDCREATEDATE12, &CWisdomLabelDlg::OnBnClickedIdcreatedate12)
	ON_BN_CLICKED(IDIDCREATEDATE13, &CWisdomLabelDlg::OnBnClickedIdcreatedate13)
	ON_BN_CLICKED(IDSAVEPICAS, &CWisdomLabelDlg::OnBnClickedOpenpic11)
	ON_BN_CLICKED(IDOPENPIC12, &CWisdomLabelDlg::OnBnClickedOpenpic12)
	ON_BN_CLICKED(IDIDCREATEDATE14, &CWisdomLabelDlg::OnBnClickedIdcreatedate14)
	ON_BN_CLICKED(IDIDCREATEDATE15, &CWisdomLabelDlg::OnBnClickedIdcreatedate15)
	ON_BN_CLICKED(IDIDCREATEDATE16, &CWisdomLabelDlg::OnBnClickedIdcreatedate16)
	ON_BN_CLICKED(IDIDCREATEDATE17, &CWisdomLabelDlg::OnBnClickedIdcreatedate17)
	ON_BN_CLICKED(IDIDCREATEDATE19, &CWisdomLabelDlg::OnBnClickedIdcreatedate19)
	ON_BN_CLICKED(IDBRIGHTNESSADD0, &CWisdomLabelDlg::OnBnClickedIdcreatedate20)
	ON_BN_CLICKED(IDDBX4, &CWisdomLabelDlg::OnFgTpX)
	ON_BN_CLICKED(IDDBX5, &CWisdomLabelDlg::OnFgTpY)
	ON_BN_CLICKED(IDCLK, &CWisdomLabelDlg::OnBnClickedClk)
	ON_BN_CLICKED(IDRUIHUA, &CWisdomLabelDlg::OnBnClickedRuihua)
	ON_BN_CLICKED(IDZZLB, &CWisdomLabelDlg::OnBnClickedZzlb)
	ON_BN_CLICKED(IDZZLB2, &CWisdomLabelDlg::OnBnClickedZzlb2)
	ON_BN_CLICKED(IDJUAN, &CWisdomLabelDlg::OnBnClickedJuan)
	ON_BN_CLICKED(IDQUAN, &CWisdomLabelDlg::OnBnClickedQuan)
	ON_BN_CLICKED(IDCDISBIAO, &CWisdomLabelDlg::OnBnClickedCdisbiao)
	ON_BN_CLICKED(IDQUAN2, &CWisdomLabelDlg::OnBnClickedQuan2)
	ON_BN_CLICKED(IDCLK2, &CWisdomLabelDlg::OnBnClickedClk2)
	ON_BN_CLICKED(IDBRIGHTNESSADD1, &CWisdomLabelDlg::OnBnClickedIdcreatedate21)
	ON_BN_CLICKED(IDOPENPIC13, &CWisdomLabelDlg::OnBnClickedOpenpic13)
	ON_BN_CLICKED(IDC_BUTTON1, &CWisdomLabelDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDREDBLUEEXCHANGE, &CWisdomLabelDlg::OnBnClickedCsetpoint3)
	ON_BN_CLICKED(IDC_BUTTON2, &CWisdomLabelDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CWisdomLabelDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CWisdomLabelDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CWisdomLabelDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDOPENPIC14, &CWisdomLabelDlg::OnBnClickedOpenpic14)
	ON_WM_CHAR()
	ON_BN_CLICKED(IDSAVEPIC, &CWisdomLabelDlg::OnBnClickedButton6)
	ON_BN_CLICKED(IDQUAN3, &CWisdomLabelDlg::OnBnClickedQuan3)
	ON_BN_CLICKED(IDCDISBIAO2, &CWisdomLabelDlg::OnBnClickedCdisbiao2)
	ON_BN_CLICKED(IDC_BUTTON7, &CWisdomLabelDlg::OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON8, &CWisdomLabelDlg::OnBnClickedButton8)
	ON_BN_CLICKED(IDBRIGHTNESSADD2, &CWisdomLabelDlg::OnBnClickedIdcreatedate22)
	ON_BN_CLICKED(IDCLK3, &CWisdomLabelDlg::OnBnClickedClk3)
	ON_BN_CLICKED(IDCSETPOINT4, &CWisdomLabelDlg::OnBnClickedCsetpoint4)
	ON_BN_CLICKED(IDOPENPICDIR, &CWisdomLabelDlg::OnBnClickedOpenpicDir)
	ON_BN_CLICKED(IDC_BUTTON9, &CWisdomLabelDlg::OnBnClickedButton9)
	ON_BN_CLICKED(IDC_CHECK5, &CWisdomLabelDlg::OnBnClickedCheck5)
	ON_BN_CLICKED(IDC_CHECK6, &CWisdomLabelDlg::OnBnClickedCheck6)
	ON_BN_CLICKED(IDZOOMALL, &CWisdomLabelDlg::OnBnClickedIdcreatedate24)
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDOPENPIC17, &CWisdomLabelDlg::OnBnClickedOpenpic17)
	ON_BN_CLICKED(IDBRIGHTNESSADD, &CWisdomLabelDlg::OnBnClickedIdcreatedate2)
	ON_BN_CLICKED(IDBRIGHTNESSSUB, &CWisdomLabelDlg::OnBnClickedIdcreatedate18)
	ON_BN_CLICKED(IDVIDEOTOONEPIC, &CWisdomLabelDlg::OnBnClickedVideoToOnePic)
	ON_BN_CLICKED(IDC_BUTTON12, &CWisdomLabelDlg::OnBnClickedButton12)
	ON_BN_CLICKED(IDC_BUTTON13, &CWisdomLabelDlg::OnBnClickedButton13)
	ON_BN_CLICKED(IDC_BUTTON15, &CWisdomLabelDlg::OnBnClickedButton15)
	ON_BN_CLICKED(IDQUAN4, &CWisdomLabelDlg::OnBnClickedQuan4)
	ON_BN_CLICKED(IDQUAN5, &CWisdomLabelDlg::OnBnClickedQuan5)
	ON_BN_CLICKED(IDBRIGHTNESSADD5, &CWisdomLabelDlg::OnBnClickedIdcreatedate25)
	ON_BN_CLICKED(IDQUAN6, &CWisdomLabelDlg::OnBnClickedQuan6)
	ON_BN_CLICKED(IDBRIGHTNESSADD7, &CWisdomLabelDlg::OnBnClickedIdcreatedate27)
	ON_BN_CLICKED(IDBRIGHTNESSADD8, &CWisdomLabelDlg::OnBnClickedIdcreatedate28)
	ON_BN_CLICKED(IDBRIGHTNESSADD9, &CWisdomLabelDlg::OnBnClickedIdcreatedate29)
	ON_BN_CLICKED(IDIDCREATEDATE30, &CWisdomLabelDlg::OnBnClickedIdcreatedate30)
	ON_BN_CLICKED(IDC_BUTTON17, &CWisdomLabelDlg::OnBnClickedButton17)
	ON_BN_CLICKED(IDIDCREATEDATE31, &CWisdomLabelDlg::OnBnClickedIdcreatedate31)
	ON_BN_CLICKED(IDIDCREATEDATE32, &CWisdomLabelDlg::OnBnClickedIdcreatedate32)
	ON_BN_CLICKED(IDIDCREATEDATE33, &CWisdomLabelDlg::OnBnClickedIdcreatedate33)
	ON_BN_CLICKED(IDIDCREATEDATE34, &CWisdomLabelDlg::OnBnClickedIdcreatedate34)
	ON_BN_CLICKED(IDIDCREATEDATE35, &CWisdomLabelDlg::OnBnClickedIdcreatedate35)
	ON_NOTIFY(HDN_ITEMCHANGED, 0, &CWisdomLabelDlg::OnHdnItemchangedList3)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_IMGLIST, &CWisdomLabelDlg::OnLvnItemchangedList3)
	ON_NOTIFY(NM_CLICK, IDC_IMGLIST, &CWisdomLabelDlg::OnNMClickList3)
	ON_BN_CLICKED(IDC_BUTTON14, &CWisdomLabelDlg::OnBnClickedButton14)
	ON_BN_CLICKED(IDEXPANDTO, &CWisdomLabelDlg::OnBnClickedButton11)
	ON_BN_CLICKED(IDIDCREATEDATE36, &CWisdomLabelDlg::OnBnClickedIdcreatedate36)
	ON_BN_CLICKED(IDOPENANNOTATIONDIR, &CWisdomLabelDlg::OnBnClickedOpenAnnotationsDir)
	ON_BN_CLICKED(IDIDCREATEDATE37, &CWisdomLabelDlg::OnBnClickedIdcreatedate37)
	ON_BN_CLICKED(IDIDCREATEDATE38, &CWisdomLabelDlg::OnBnClickedIdcreatedate38)
	ON_BN_CLICKED(IDIDCREATEDATE39, &CWisdomLabelDlg::OnBnClickedIdcreatedate39)
	ON_BN_CLICKED(IDIDCREATEDATE40, &CWisdomLabelDlg::OnBnClickedIdcreatedate40)
	ON_BN_CLICKED(IDIDCREATEDATE41, &CWisdomLabelDlg::OnBnClickedIdcreatedate41)
	ON_BN_CLICKED(IDIDCREATEDATE42, &CWisdomLabelDlg::OnBnClickedIdcreatedate42)
	ON_BN_CLICKED(IDIDCREATEDATE43, &CWisdomLabelDlg::OnBnClickedIdcreatedate43)
	ON_BN_CLICKED(IDIDCREATEDATE44, &CWisdomLabelDlg::OnBnClickedIdcreatedate44)
	ON_BN_CLICKED(IDIDCREATEDATE45, &CWisdomLabelDlg::OnBnClickedIdcreatedate45)
	ON_BN_CLICKED(IDRANDSELECT, &CWisdomLabelDlg::OnBnClickedButton18)
	ON_BN_CLICKED(IDPREVBACKPIC, &CWisdomLabelDlg::OnBnClickedIdcreatedate46)
	ON_BN_CLICKED(IDPREVPIC1, &CWisdomLabelDlg::OnBnClickedIdcreatedate47)
	ON_BN_CLICKED(IDC_BUTTON19, &CWisdomLabelDlg::OnBnClickedButton19)
	ON_BN_CLICKED(IDC_BUTTON20, &CWisdomLabelDlg::OnBnClickedButton20)
	ON_BN_CLICKED(IDC_BUTTON22, &CWisdomLabelDlg::OnBnClickedButton22)
	ON_BN_CLICKED(IDFINDFILEBYNAMECONTINUE, &CWisdomLabelDlg::OnBnClickedButton23)
	ON_BN_CLICKED(IDC_BUTTON24, &CWisdomLabelDlg::OnBnClickedButton24)
	ON_BN_CLICKED(IDC_BUTTON25, &CWisdomLabelDlg::OnBnClickedButton25)
	ON_BN_CLICKED(IDCDISBIAO3, &CWisdomLabelDlg::OnBnClickedCdisbiao3)
	ON_BN_CLICKED(IDFINDFILEBYNAMEFROMFIRST, &CWisdomLabelDlg::OnBnClickedButton26)
	ON_BN_CLICKED(IDCDISBIAO4, &CWisdomLabelDlg::OnBnClickedCdisbiao4)
	ON_BN_CLICKED(IDRELOADPIC, &CWisdomLabelDlg::OnBnClickedCdisbiao5)
	ON_BN_CLICKED(IDCDISBIAO6, &CWisdomLabelDlg::OnBnClickedCdisbiao6)
	ON_BN_CLICKED(IDCDISBIAO7, &CWisdomLabelDlg::OnBnClickedCdisbiao7)
	ON_BN_CLICKED(IDCDISBIAO8, &CWisdomLabelDlg::OnBnClickedCdisbiao8)
	ON_BN_CLICKED(IDCOPYTOOPPOINTDIR, &CWisdomLabelDlg::OnBnClickedCdisbiao9)
	ON_BN_CLICKED(IDIDCREATEDATE48, &CWisdomLabelDlg::OnBnClickedIdcreatedate48)
	ON_BN_CLICKED(IDIDCREATEDATE49, &CWisdomLabelDlg::OnBnClickedIdcreatedate49)
	ON_BN_CLICKED(IDOPENPIC18, &CWisdomLabelDlg::OnBnClickedOpenpic18)
	ON_BN_CLICKED(IDRUIHUA2, &CWisdomLabelDlg::OnBnClickedRuihua2)
	ON_BN_CLICKED(IDOPENPIC19, &CWisdomLabelDlg::OnBnClickedOpenpic19)
	ON_BN_CLICKED(IDDELETENOPICANNOTATION, &CWisdomLabelDlg::OnBnClickedCdisbiao10)
	ON_BN_CLICKED(IDCDISBIAO11, &CWisdomLabelDlg::OnBnClickedCdisbiao11)
	ON_BN_CLICKED(IDIDCREATEDATE50, &CWisdomLabelDlg::OnBnClickedIdcreatedate50)
	ON_COMMAND(ID_GETPICBYLABLE, &CWisdomLabelDlg::OnGetpicbylable)
	ON_COMMAND(ID_CHECKNAME, &CWisdomLabelDlg::OnCheckname)
	ON_COMMAND(ID_CHECKBOX, &CWisdomLabelDlg::OnCheckbox)
	ON_BN_CLICKED(IDRUIHUA3, &CWisdomLabelDlg::OnBnClickedRuihua3)
	ON_COMMAND(ID_RUIHUA3, &CWisdomLabelDlg::OnRuihua3)
	ON_COMMAND(ID_CREATEPICMODEL, &CWisdomLabelDlg::OnCreatepicmodel)
	ON_COMMAND(ID_MHHYTJ, &CWisdomLabelDlg::OnMhhytj)
	ON_COMMAND(ID_MHHY, &CWisdomLabelDlg::OnMhhy)
	ON_WM_CLOSE()
	ON_COMMAND(ID_STOPMHHYTJ, &CWisdomLabelDlg::OnStopmhhytj)
	ON_COMMAND(ID_OPENVIDEOFRAME, &CWisdomLabelDlg::OnOpenvideoframe)
	ON_COMMAND(ID_32789, &CWisdomLabelDlg::On32789)
	ON_BN_CLICKED(IDMOVETOOPPOINTDIR, &CWisdomLabelDlg::OnBnClickedCdisbiao12)
	ON_BN_CLICKED(IDVIDEOTOMULPICS, &CWisdomLabelDlg::OnBnClickedIdcreatedate51)
	ON_COMMAND(ID_NEXTALL, &CWisdomLabelDlg::OnNextall)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDNEXTPICMUL, &CWisdomLabelDlg::OnBnClickedOpenpic20)
	ON_BN_CLICKED(IDPREVPICMUL, &CWisdomLabelDlg::OnBnClickedOpenpic21)
	ON_COMMAND(ID_FGJZLB, &CWisdomLabelDlg::OnFgjzlb)
	ON_BN_CLICKED(IDMOVENOANNOTATION, &CWisdomLabelDlg::OnBnClickedCdisbiao13)
	ON_BN_CLICKED(IDRANDMOVONOANNOTATION, &CWisdomLabelDlg::OnBnClickedCdisbiao14)
	ON_BN_CLICKED(IDAUTOLABEL, &CWisdomLabelDlg::OnBnClickedIdcreatedate52)
	ON_BN_CLICKED(IDIDCREATEDATE53, &CWisdomLabelDlg::OnBnClickedIdcreatedate53)
	ON_BN_CLICKED(IDIDCREATEDATE54, &CWisdomLabelDlg::OnBnClickedIdcreatedate54)
	ON_BN_CLICKED(IDIDCREATEDATE55, &CWisdomLabelDlg::OnBnClickedIdcreatedate55)
	ON_BN_CLICKED(IDC_BUTTON21, &CWisdomLabelDlg::OnBnClickedButton21)
	ON_BN_CLICKED(IDC_BUTTON28, &CWisdomLabelDlg::OnBnClickedButton28)
	ON_BN_CLICKED(IDSEARCHOPTIONS, &CWisdomLabelDlg::OnBnClickedOpenpic22)
	ON_BN_CLICKED(IDPICSPACE, &CWisdomLabelDlg::OnBnClickedOpenpic23)
	ON_BN_CLICKED(IDNEXTPIC4, &CWisdomLabelDlg::OnBnClickedOpenpic24)
	ON_BN_CLICKED(IDSETOPPOINTDIR, &CWisdomLabelDlg::OnBnClickedOpenpic25)
	ON_BN_CLICKED(IDSETSEARCHFILENAME, &CWisdomLabelDlg::OnBnClickedOpenpic26)
	ON_BN_CLICKED(IDIDCREATEDATE56, &CWisdomLabelDlg::OnBnClickedIdcreatedate56)
	ON_BN_CLICKED(IDIDCREATEDATE57, &CWisdomLabelDlg::OnBnClickedIdcreatedate57)
	
	ON_BN_CLICKED(IDNEXTPIC7, &CWisdomLabelDlg::OnBnClickedOpenpic27)
	ON_BN_CLICKED(IDIDCREATEDATE58, &CWisdomLabelDlg::OnBnClickedIdcreatedate58)
	ON_COMMAND(ID_HAND2AUTO, &CWisdomLabelDlg::OnHand2auto)
	ON_COMMAND(ID_TESTIOU, &CWisdomLabelDlg::OnTestiou)
	ON_COMMAND(ID_OPENLISTBYTXT, &CWisdomLabelDlg::OnOpenlistbytxt)
	ON_COMMAND(ID_YOLOTOXML, &CWisdomLabelDlg::OnYolotoxml)
	ON_COMMAND(ID_HEIGHTERR, &CWisdomLabelDlg::OnHeighterr)
	ON_COMMAND(ID_FINDSAME, &CWisdomLabelDlg::OnFindsame)
	ON_COMMAND(ID_RENAME, &CWisdomLabelDlg::OnRename)
	ON_COMMAND(ID_FINDSAME2, &CWisdomLabelDlg::OnFindsame2)
	ON_COMMAND(ID_SEARCHPD, &CWisdomLabelDlg::OnSearchpd)
	ON_COMMAND(ID_NOPD, &CWisdomLabelDlg::OnNopd)
	ON_COMMAND(ID_SIZETO, &CWisdomLabelDlg::OnSizeto)
	ON_COMMAND(ID_PICSTOVIDEO, &CWisdomLabelDlg::OnPicstovideo)
	ON_COMMAND(ID_PDDXNAME, &CWisdomLabelDlg::OnPddxname)
	ON_COMMAND(ID_DELETELABPIXEL, &CWisdomLabelDlg::OnDeletelabpixel)
	ON_COMMAND(ID_PIDELETEPIXEL, &CWisdomLabelDlg::OnPideletepixel)
	ON_COMMAND(ID_COPYTOTRAINDIR, &CWisdomLabelDlg::OnMzcopyto)
	ON_COMMAND(ID_widefacetovoc, &CWisdomLabelDlg::Onwidefacetovoc)
	ON_COMMAND(ID_LABELSCALE, &CWisdomLabelDlg::OnLabelscale)
	ON_COMMAND(ID_SCALETO, &CWisdomLabelDlg::OnScaleto)
	ON_BN_CLICKED(IDMULVIDEOSTOMULPICS, &CWisdomLabelDlg::OnBnClickedIdcreatedate59)
	ON_COMMAND(ID_OPENRGB, &CWisdomLabelDlg::OnOpenrgb)
	ON_COMMAND(ID_PIDELETEPIXELDBC, &CWisdomLabelDlg::OnPideletepixeldbc)
	ON_BN_CLICKED(IDC_DELETELABEL, &CWisdomLabelDlg::OnBnClickedDeletelabel)
	ON_COMMAND(ID_CLEANLABEL, &CWisdomLabelDlg::OnCleanlabel)
	ON_COMMAND(ID_CALDBCRECT, &CWisdomLabelDlg::OnCaldbcrect)
	ON_COMMAND(ID_PIDBCNORECT, &CWisdomLabelDlg::OnPidbcnorect)
	ON_COMMAND(ID_STOPP, &CWisdomLabelDlg::OnStopp)
	ON_BN_CLICKED(IDTOGRAY, &CWisdomLabelDlg::OnBnClickedTogray)
	ON_BN_CLICKED(IDTOGRAYPI, &CWisdomLabelDlg::OnBnClickedTogray2)
	ON_MESSAGE(PIMESSAGE,OnPiMessage)
	ON_BN_CLICKED(IDTOZFX, &CWisdomLabelDlg::OnBnClickedTozfx)
	ON_BN_CLICKED(IDTOZFXPI, &CWisdomLabelDlg::OnBnClickedTozfxpi)
	ON_BN_CLICKED(IDRENAMEPIC, &CWisdomLabelDlg::OnBnClickedRenamepic)
	ON_BN_CLICKED(IDRENAMEPICPI, &CWisdomLabelDlg::OnBnClickedRenamepic2)
	ON_BN_CLICKED(IDCUTTOZFX, &CWisdomLabelDlg::OnBnClickedCuttozfx)
	ON_BN_CLICKED(IDCUTTOZFXPI, &CWisdomLabelDlg::OnBnClickedCuttozfxpi)
	ON_BN_CLICKED(IDTO16_9, &CWisdomLabelDlg::OnBnClicked9)
	ON_BN_CLICKED(IDTO16_9pi, &CWisdomLabelDlg::OnBnClicked9pi)
	ON_BN_CLICKED(IDBRIGHTNESSADDPI, &CWisdomLabelDlg::OnBnClickedBrightnessaddpi)
	ON_BN_CLICKED(IDBRIGHTNESSSUBPI, &CWisdomLabelDlg::OnBnClickedBrightnesssubpi)
	ON_BN_CLICKED(IDREDBLUEEXCHANGEPI, &CWisdomLabelDlg::OnBnClickedRedblueexchangepi)
	ON_BN_CLICKED(IDEXPANDTOPI, &CWisdomLabelDlg::OnBnClickedExpandtopi)
	ON_BN_CLICKED(IDAUTOLABELPI, &CWisdomLabelDlg::OnBnClickedAutolabelpi)
	
	END_MESSAGE_MAP()


// CWisdomLabelDlg 消息处理程序
//extern map<CStringA,int> LabPic;



void CWisdomLabelDlg::AddLab(CStringA ss){
	if(ss.IsEmpty())
		return;
	CStringA Dir=GetModuleDir();
	vector<CStringA>tmp;
	SplitStr(&tmp,ss,",");
	ss=tmp.at(0);
	if(tmp.size()>1){
		m_LabMap1[ss]=tmp.at(1);
		m_LabMap2[tmp.at(1)]=ss;
	}else{
		m_LabMap1[ss]="";
	}
	CImage img;
	img.Load(CString(Dir+"\\cfg\\"+ss+".bmp"));
	if(img.IsNull())
		img.Load(CString(Dir+"\\cfg\\"+ss+".jpg"));
	if(img.IsNull())
		img.Load(CString(Dir+"\\cfg\\"+ss+".png"));
	if(!img.IsNull()){
		CDC *pDC=GetDC();
		CDC memDC;
		memDC.CreateCompatibleDC(pDC);
		CBitmap *bit=new CBitmap();
		bit->CreateCompatibleBitmap(pDC,img.GetWidth(),img.GetHeight());
		CBitmap *poldBit=memDC.SelectObject(bit);
		img.Draw(memDC.GetSafeHdc(),0,0);
		memDC.SelectObject(poldBit);
		memDC.DeleteDC();
		ReleaseDC(pDC);
		m_LabMap[ss]=bit;
	}else{
		m_LabMap[ss]=NULL;
	}
}
BOOL TraversalChildWnd(vector<CWnd *> &vec, CWnd * Parent)
{
	vec.clear();
	try
	{
		
		CWnd *child=Parent->GetWindow(GW_CHILD);
		while (child)
		{
			vec.push_back(child);
			//CStringA Str;
			TCHAR sz[MAX_PATH]={0};
			::GetClassName(child->GetSafeHwnd(), sz, MAX_PATH-1);		
			if(CStringA(sz)=="Button"){
				CRect Rc;
				child->GetWindowRect(&Rc);
				Parent->ScreenToClient(Rc);
				if(Rc.Width()!=70){
					Rc.right=Rc.left+70;
					child->MoveWindow(&Rc);
				}
			}
			child = child->GetWindow( GW_HWNDNEXT);
		}
	}
	catch (...)
	{
		OutputDebugStringA(__FUNCTION__);
	}
	return vec.size() > 0;
}

//int test()
//{
//	for 
//	{
//		printf("窗口%08X", var);
//		GetWindowThreadProcessId(var, &dwPid);
//		printf(" 进程%08X|%d", dwPid, dwPid);
//		::GetWindowText(var, sz, MAX_PATH * sizeof(TCHAR) - sizeof(TCHAR));
//		printf(" 标题%S", sz);
//		memset(sz, 0, MAX_PATH * sizeof(TCHAR));
//		::GetClassName(var, sz, MAX_PATH * sizeof(TCHAR) - sizeof(TCHAR));		
//		printf(" 类名%S", sz);
//		printf("\r\n");
//	}
//
//	return 0;
//}

int Limit(int v,int min,int max){
	if(v<min)
		v=min;
	if(v>max)
		v=max;
	return v;
}
BOOL CWisdomLabelDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	m_MinNum=1;
	m_MaxNum=10000000000;
	m_bInit=1;
	m_LabSize=12;
	m_ChangePic=0;
	m_BlSxXBtn.m_Title1="包含";
	m_KdBtn.m_Title1="设置跨度";
	m_MrBzBtn.m_Title1="设默认标注";
	m_TmpDirBtn.m_Title1="设置指定目录";
	m_FindNrBtn.m_Title1="要查找的文件";
	m_InputSizeBtn.m_Title1="输入尺寸设置";
	m_KdBtn.SetWindowText(CString("100"));
	m_MrBzBtn.SetWindowText(CString(""));
	m_TmpDirBtn.SetWindowText(CString(""));
	m_FindNrBtn.SetWindowText(CString(""));
	m_InputSizeBtn.SetWindowText(CString("416"));
	GetDlgItem(IDC_CMDEDIT)->SetWindowText(CString("输入命令,按空格或回车键执行,按ESC键清空命令,当前命令:"));
	InitLab();
	InitLookupTable();
	InitConvertTable();
	m_FyTjBox.SetCurSel(0);
	//m_LabPic.LoadBitmap(IDB_BITMAP1);
	// TODO: 在此添加额外的初始化代码
	m_TmpGraphRect=CRect(-10,-10,-10,-10);
	m_DrawScale=1.0;
	
	/*CreateDirectory(GetDataDir()+"\\无",NULL);
	CreateDirectory(GetDataDir()+"\\玩手机",NULL);
	CreateDirectory(GetDataDir()+"\\非玩手机",NULL);
	CreateDirectory(GetDataDir()+"\\抽烟",NULL);
	CreateDirectory(GetDataDir()+"\\非抽烟",NULL);*/
	
	
	
	m_RectType="无";
	int i=0;
	m_pImgListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP);
	m_pImgListCtrl.InsertColumn(i++,CString("图片名称"),LVCFMT_LEFT,200);
	m_pImgListCtrl.InsertColumn(i++,CString("图片路径"),LVCFMT_LEFT,800);
	m_pImgListCtrl.InsertColumn(i++,CString("是否标注"),LVCFMT_LEFT,80);
	m_pImgListCtrl.InsertColumn(i++,CString("修改日期"),LVCFMT_LEFT,200);
	srand(GetTickCount());
	this->m_DisLabTextBtn.SetCheck(1);
	if(m_ie.m_hWnd){
		//m_ie.Navigate("http://127.0.0.1:80",NULL,NULL,NULL,NULL);//http://115.159.100.120:80/1 https://blog.csdn.net/miluofumin/article/details/107333368
		m_ie.Navigate(CString("http://115.159.100.120/1.html"),NULL,NULL,NULL,NULL);//http://115.159.100.120:80/1 https://blog.csdn.net/miluofumin/article/details/107333368
		m_ie.put_Silent(TRUE);
	}if(m_ie1.m_hWnd){
		//m_ie.Navigate("http://127.0.0.1:80",NULL,NULL,NULL,NULL);//http://115.159.100.120:80/1 https://blog.csdn.net/miluofumin/article/details/107333368
		m_ie1.Navigate(CString("http://115.159.100.120/2.html"),NULL,NULL,NULL,NULL);//http://115.159.100.120:80/1 https://blog.csdn.net/miluofumin/article/details/107333368
		m_ie1.put_Silent(TRUE);
	}
	//m_ie
	vector<CWnd *> wnds;
	//TraversalChildWnd(wnds,this);
	//GetDlgItem(IDC_EDIT8)->SetWindowText("在此输入\"输入尺寸\",默认412");
	//GetDlgItem(IDC_EDIT9)->SetWindowText("1");
	ShowWindow(SW_SHOWMAXIMIZED);
	CStringA HisDir=GetModuleDir()+"\\cfg";
	CreateDirectory(CString(HisDir),NULL);
	//m_KjjMap["L"]=Kjj("L","下一图片",IDNEXTPIC);
	//m_KjjMap["J"]=Kjj("J","上一图片",IDPREVPIC);
	//m_KjjMap["M"]=Kjj("M","矩形去像素",IDC_BUTTON17);
	//m_KjjMap["I"]=Kjj("I","修改标签",IDDBX3);
	//m_KjjMap["U"]=Kjj("I","删除标注",IDOPENPIC14);
	m_KjjMap["F"]=Kjj("F","下一图片",IDNEXTPIC,"forward");
	m_KjjMap["B"]=Kjj("B","上一图片",IDPREVPIC,"backward");
	m_KjjMap["FF"]=Kjj("FF","下n图片",IDNEXTPICMUL,"m forward");
	m_KjjMap["BB"]=Kjj("BB","上n图片",IDPREVPICMUL,"m backward");
	m_KjjMap["DC"]=Kjj("DC","矩形去像素",IDC_BUTTON17,"delete color");
	m_KjjMap["EA"]=Kjj("EA","修改标签",IDDBX3,"edit Annotations");
	m_KjjMap["DA"]=Kjj("DA","删除标注",IDOPENPIC14,"delete Annotations");
	m_KjjMap["R"]=Kjj("R","矩形",IDCROPFACE,"rect");
	m_KjjMap["EG"]=Kjj("R","编辑形状",IDCLK2,"edit shape");
	m_KjjMap["SA"]=Kjj("DS","隐/显标注",IDCDISBIAO,"show Annotations");
	m_KjjMap["RS"]=Kjj("RA","随机选图",IDRANDSELECT,"random select");//IDC_BUTTON3
	m_KjjMap["ZA"]=Kjj("ZA","显示百分百",IDZOOMALL,"zoom all");
	m_KjjMap["DDC"]=Kjj("DDC","多边形删除像素",IDIDCREATEDATE5,"");
	m_KjjMap["DBC"]=Kjj("DBC","绘制多边形",IDCLK3,"");
	m_KjjMap["CT"]=Kjj("CT","复制到指定",IDCOPYTOOPPOINTDIR,"Copy To ");
	m_KjjMap["YT"]=Kjj("YT","移动到指定",IDMOVETOOPPOINTDIR,"Move To ");
	//OnBnClickedCdisbiao9()
	//IDCLK3
	//IDIDCREATEDATE36
	for(map<CStringA ,Kjj>::iterator it=m_KjjMap.begin();it!=m_KjjMap.end();it++){
		CWnd *pWnd=GetDlgItem(it->second.Id);
		if(pWnd){
			pWnd->SetWindowText(CString(it->second.Title+" "+it->first));
		}
	}
	CStringA meFile="e:\\miluo.txt";
	CFileStatus status;
	if(CFile::GetStatus(CString(meFile),status)){
		CMenu Menu;
		Menu.LoadMenu(IDR_MENU1);
		SetMenu(&Menu);
	}
	srand(GetTickCount());
	//CStringA Dir=GetModuleDir();
	//zzCString 
	//img.Load(CString(Dir+"\\cfg\\"+ss+".bmp"));
//	PostMessage(WM_PAINT);
	//::SetUnhandledExceptionFilter(MyUnhandledFilter);
	//无;玩手机;非玩手机;抽烟;非抽烟;
	m_PiTimes=m_PiNeedTimes=0;
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。
CRect CWisdomLabelDlg::ToTar(CRect rcSrc){
	CRect tRc=rcSrc;
	tRc.OffsetRect(-m_SrcRect.left,-m_SrcRect.top);
	tRc.left/=m_DrawScale;
	tRc.right/=m_DrawScale;
	tRc.bottom/=m_DrawScale;
	tRc.top/=m_DrawScale;
	tRc.OffsetRect(m_TarRect.left,m_TarRect.top);
	return tRc;
}
CPoint CWisdomLabelDlg::ToTar(CPoint ptSrc){
	CPoint pt(ptSrc);
	pt.Offset(-m_SrcRect.left,-m_SrcRect.top);
	pt.x/=m_DrawScale;
	pt.y/=m_DrawScale;
	pt.Offset(m_TarRect.left,m_TarRect.top);
	return pt;
}
CPoint CWisdomLabelDlg::ToSrc(CPoint ptTar){
	CPoint pt((float)(ptTar.x-m_TarRect.left)*m_DrawScale,(float)(ptTar.y-m_TarRect.top)*m_DrawScale);
	pt.Offset(m_SrcRect.left,m_SrcRect.top);
	return pt;
}
CRect CWisdomLabelDlg::ToSrc(CRect Rc){
	return CRect(ToSrc(Rc.TopLeft()),ToSrc(Rc.BottomRight()));
}
CRect ToZfx(CRect Rc){
	int w=Rc.Width();
	int h=Rc.Height();
	CPoint pt=Rc.CenterPoint();
	w=(w+h)/4;
	return CRect(pt.x-w,pt.y-w,pt.x+w,pt.y+w);
}
CRect DbxToRect(vector<CPoint> *pts){
	int maxx=-100000000;
	int minx=100000000;
	int maxy=-100000000;
	int miny=100000000;
	for(int i=0;i<pts->size();i++){
		CPoint pt=pts->at(i);
		if(pt.x>maxx)
			maxx=pt.x;
		if(pt.y>maxy)
			maxy=pt.y;
		if(pt.x<minx)
			minx=pt.x;
		if(pt.y<miny)
			miny=pt.y;
	}
	return CRect(minx,miny,maxx,maxy);
}
CRect ToZfx1(CRect Rc){
	int w=Rc.Width();
	int h=Rc.Height();
	CPoint pt=Rc.CenterPoint();
	if(w<h)
		w=h;
	w/=2;
	return CRect(pt.x-w,pt.y-w,pt.x+w,pt.y+w);
}
#define PI 3.1416926
CPoint BottomLeft(CRect Rc){
	return CPoint(Rc.left,Rc.bottom);
}
CPoint  TopRight(CRect Rc){
	return CPoint(Rc.right,Rc.top);
}
void CWisdomLabelDlg::DrawRect2(CDC *pDC,CRect Rc){
	CRect tRc=ToTar(Rc);
	pDC->MoveTo(tRc.TopLeft());
	pDC->LineTo(tRc.left,tRc.bottom);
	pDC->LineTo(tRc.right,tRc.bottom);
	pDC->LineTo(tRc.right,tRc.top);
	pDC->LineTo(tRc.TopLeft());
	//pDC->MoveTo(tRc.TopLeft());
	//pDC->LineTo(tRc.BottomRight());
	//pDC->MoveTo(TopRight(tRc));
	//pDC->LineTo(BottomLeft(tRc));
}
void DrawRect22(CDC *pDC,CRect Rc){
	CRect tRc=Rc;
	pDC->MoveTo(tRc.TopLeft());
	pDC->LineTo(tRc.left,tRc.bottom);
	pDC->LineTo(tRc.right,tRc.bottom);
	pDC->LineTo(tRc.right,tRc.top);
	pDC->LineTo(tRc.TopLeft());
}
void CWisdomLabelDlg::DrawRect1(CDC *pDC,CRect Rc,COLORREF Col,int LineType,int LineWidth){
	CPen pen(LineType,LineType,Col);
	CPen *pOldPen=pDC->SelectObject(&pen);
	CRect tRc=ToTar(Rc);
	pDC->MoveTo(tRc.TopLeft());
	pDC->LineTo(tRc.left,tRc.bottom);
	pDC->LineTo(tRc.right,tRc.bottom);
	pDC->LineTo(tRc.right,tRc.top);
	pDC->LineTo(tRc.TopLeft());
	pDC->SelectObject(pOldPen);
}
int IsPtInLine(fLine l,fPoint p){
	if((p.x-l.p1.x)*(p.x-l.p2.x)<=0 &&(p.y-l.p1.y)*(p.y-l.p2.y)<=0)
		return 1;
	return 0;
}
fPoint point2line(fLine l,fPoint p)
{
	fPoint Q;
	double a,b,c;
	double A,B,C;

	a=l.p2.x-l.p1.x;
	b=l.p2.y-l.p1.y;
	c=l.p2.z-l.p1.z;

	A=a*p.x+b*p.y+c*p.z;
	B=b*l.p1.x-a*l.p1.y;
	C=c*l.p1.x-a*l.p1.z;

	if (a!=0)
	{
	  Q.x=(A*a+B*b+C*c)/(a*a+b*b+c*c);
	  Q.y=(b*Q.x-B)/a;
	  Q.z=(c*Q.x-C)/a;
	}
	else
	{
	  double D,temp;
	  D=c*l.p1.y-b*l.p1.z;
	  temp=b*b+c*c;
	  Q.y=(A*b+D*c)/temp;
	  Q.z=(A*c-D*b)/temp;
	  Q.x=(B+a*Q.y)/b;
	}
	return Q;
}
//计算两条直线的交点
int getCross(fLine line1, fLine line2,fPoint *res)
{
    fPoint CrossP;
    //y = a * x + b;
	if(line1.p1.x - line1.p2.x==0 &&line2.p1.x - line2.p2.x==0){
		return 0;
	}else if(line1.p1.x - line1.p2.x==0){
		CrossP.x=line1.p1.x;
		CrossP.y=line2.p1.y+(line1.p1.x-line2.p1.x)*(line2.p1.y-line2.p2.y)/(line2.p1.x - line2.p2.x);
	}else if(line2.p1.x - line2.p2.x==0){
		CrossP.x=line2.p1.x;
		CrossP.y=line1.p1.y+(line2.p1.x-line1.p1.x)*(line1.p1.y-line1.p2.y)/(line1.p1.x - line1.p2.x);
	}
	else{
		float a1 = (line1.p1.y - line1.p2.y) / (line1.p1.x - line1.p2.x);
		float b1 = line1.p1.y - a1 * (line1.p1.x);

		float a2 = (line2.p1.y - line2.p2.y) / (line2.p1.x - line2.p2.x);
		float b2 = line2.p1.y - a1 * (line2.p1.x);

		if(a2-a1==0){
			return 0;
		}
		CrossP.x = (b1 - b2) / (a2 - a1);
		CrossP.y = a1 * CrossP.x + b1;
		
	}
	*res=CrossP;
    return 1;
}
CRect LkToRect(Lk lk){
	//if(lk.LkType==TYPERECT)
		return CRect(lk.Pts.at(0),lk.Pts.at(1));
	/*else if(lk.LkType==TYPESPLINE){
		for(int i=0;i<lk.Pts.size()-1;i++){
		}
	}*/
}
fPoint CPt2Fpt(CPoint cpt){
	return fPoint(cpt.x,cpt.y);
}
vector<fLine> RcTofLine(CRect Rc){
	vector<fLine>Res;
	Res.push_back(fLine(fPoint(Rc.left,Rc.top),fPoint(Rc.left,Rc.bottom)));
	Res.push_back(fLine(fPoint(Rc.left,Rc.bottom),fPoint(Rc.right,Rc.bottom)));
	Res.push_back(fLine(fPoint(Rc.right,Rc.bottom),fPoint(Rc.right,Rc.top)));
	Res.push_back(fLine(fPoint(Rc.right,Rc.top),fPoint(Rc.left,Rc.top)));
	return Res;
}
CPoint Offset(CPoint pt ,int x,int y){
	pt.Offset(x,y);
	return pt;
}
vector<fLine> LkToLine(Lk lk){
	vector<fLine>Res;
	if(lk.LkType==TYPERECT){
		Res=RcTofLine(LkToRect(lk));
	}else if(lk.LkType==TYPESPLINE||lk.LkType==TYPEDBC){
		
		for(int i=0;i<lk.Pts.size()-1;i++){
			Res.push_back(fLine(
				CPt2Fpt(lk.Pts.at(i)),
				CPt2Fpt(lk.Pts.at(i+1))));
		}
		if(lk.LkType==TYPEDBC){
			Res.push_back(fLine(
				CPt2Fpt(lk.Pts.at(lk.Pts.size()-1)),
				CPt2Fpt(lk.Pts.at(0))));
		}
	}else if(lk.LkType==TYPEPOINT){
		if(lk.Pts.size()>1){
			for(int i=0;i<lk.Pts.size()-1;i++){
			Res.push_back(fLine(
				CPt2Fpt(lk.Pts.at(i)),
				CPt2Fpt(lk.Pts.at(i+1))));
			}
		}else if( lk.Pts.size()==1){
			CPoint pt=lk.Pts.at(0);
			CRect Rc(Offset(pt,-3,-3),Offset(pt,3,3));
			Res=RcTofLine(Rc);
		}
	}
	if(lk.BqPicRc.Width()>0){
		vector<fLine>Res1=RcTofLine(lk.BqPicRc);
		Res.insert(Res.end(),Res1.begin(),Res1.end());
	}
	if(lk.BqRc.Width()>0){
		vector<fLine>Res1=RcTofLine(lk.BqRc);
		Res.insert(Res.end(),Res1.begin(),Res1.end());
	}
	return Res;
}
vector<fLine> LkToLine1(Lk lk){//不包含文字的框线
	vector<fLine>Res;
	if(lk.LkType==TYPERECT){
		Res=RcTofLine(LkToRect(lk));
	}else if(lk.LkType==TYPESPLINE||lk.LkType==TYPEDBC){
		
		for(int i=0;i<lk.Pts.size()-1;i++){
			Res.push_back(fLine(
				CPt2Fpt(lk.Pts.at(i)),
				CPt2Fpt(lk.Pts.at(i+1))));
		}
		if(lk.LkType==TYPEDBC){
			Res.push_back(fLine(
				CPt2Fpt(lk.Pts.at(lk.Pts.size()-1)),
				CPt2Fpt(lk.Pts.at(0))));
		}
	}else if(lk.LkType==TYPEPOINT){
		if(lk.Pts.size()>1){
			for(int i=0;i<lk.Pts.size()-1;i++){
			Res.push_back(fLine(
				CPt2Fpt(lk.Pts.at(i)),
				CPt2Fpt(lk.Pts.at(i+1))));
			}
		}else if( lk.Pts.size()==1){
			CPoint pt=lk.Pts.at(0);
			CRect Rc(Offset(pt,-5,-5),Offset(pt,5,5));
			Res=RcTofLine(Rc);
		}
	}
	//if(lk.BqPicRc.Width()>0){
	//	vector<fLine>Res1=RcTofLine(lk.BqPicRc);
	//	Res.insert(Res.end(),Res1.begin(),Res1.end());
	//}
	//if(lk.BqRc.Width()>0){
	//	vector<fLine>Res1=RcTofLine(lk.BqRc);
	//	Res.insert(Res.end(),Res1.begin(),Res1.end());
	//}
	return Res;
}



double DisPt2Line(fLine l,fPoint p){
	fPoint Q=point2line(l,p);
	if((Q.x-l.p1.x)*(Q.x-l.p2.x)<=0 &&(Q.y-l.p1.y)*(Q.y-l.p2.y)<=0)
		return sqrt(pow(p.y - Q.y, 2) + pow(p.x - Q.x, 2));
	double dis1=sqrt(pow(p.y - l.p1.y, 2) + pow(p.x - l.p1.x, 2));
	double dis2=sqrt(pow(p.y - l.p2.y, 2) + pow(p.x - l.p2.x, 2));
//	double dis3=sqrt(pow(p.y - Q.y, 2) + pow(p.x - Q.x, 2));
	return min(dis1,dis2);
}
double DisPt2Lk(CPoint pt,Lk lk,int &LineID){
	double mindis=1000000000;
	vector<fLine> Lines=LkToLine(lk);
	for(int i=0;i<Lines.size();i++){
		double dis=DisPt2Line(Lines.at(i),CPt2Fpt(pt));
		if(dis<mindis){
			mindis=dis;
			LineID=i;
		}
	}
	return mindis;
}
double DisPt2Lk1(CPoint pt,Lk lk,int &LineID){//不包含文字框线，修改图形用
	double mindis=1000000000;
	vector<fLine> Lines=LkToLine1(lk);
	for(int i=0;i<Lines.size();i++){
		double dis=DisPt2Line(Lines.at(i),CPt2Fpt(pt));
		if(dis<mindis){
			mindis=dis;
			LineID=i;
		}
	}
	return mindis;
}
double Dis (double x0,double y0,double x1,double y1,double x2,double y2){
	double res=(fabs((y2 - y1) * x0 +(x1 - x2) * y0 + ((x2 * y1) -(x1 * y2)))) / (sqrt(pow(y2 - y1, 2) + pow(x1 - x2, 2))); 
	//double res1=sqrt(pow(y0 - y1, 2) + pow(x0 - x1, 2));
	//double res2=sqrt(pow(y0 - y2, 2) + pow(x0 - x2, 2));
	return res;
}
void DeletePixel(CImage *Img,CRect Rc){
		if(Img->GetBPP()==24){
		for(int j=Rc.top;j<Rc.bottom&&j<Img->GetHeight();j++){
			for(int i=Rc.left;i<Rc.right&&i<Img->GetWidth();i++){
				unsigned char *b=(unsigned char *)Img->GetPixelAddress(i,j);
				unsigned char *g=b+1;
				unsigned char *r=g+1;

				*b=128;
				*g=128;
				*r=128;
			}
		}
	}
}
CImage * BrightnessReduction(CImage *Img,CRect Rc){
	CImage *nImg=new CImage();
	nImg->Create(Rc.Width(),Rc.Height(),24);
	if(Img->GetBPP()==24){
		for(int j=Rc.top;j<Rc.bottom&&j<Img->GetHeight();j++){
			for(int i=Rc.left;i<Rc.right&&i<Img->GetWidth();i++){
				unsigned char *b=(unsigned char *)Img->GetPixelAddress(i,j);
				unsigned char *g=b+1;
				unsigned char *r=g+1;

				unsigned char *b2=(unsigned char *)nImg->GetPixelAddress(i-Rc.left,j-Rc.top);
				unsigned char *g2=b2+1;
				unsigned char *r2=g2+1;

				*b2=*b;
				*g2=*g;
				*r2=*r;
				*b2-=*b2/4;
				*g2-=*g2/4;
				*r2-=*r2/4;
			}
		}
	}
	return nImg;
}
void BrightnessReduction1(CImage *Img,CRect Rc){
	if(Img->GetBPP()==24){
		for(int j=max(0,Rc.top);j<Rc.bottom&&j<Img->GetHeight();j++){
			for(int i=max(0,Rc.left);i<Rc.right&& i<Img->GetWidth();i++){
				unsigned char *b=(unsigned char *)Img->GetPixelAddress(i,j);
				unsigned char *g=b+1;
				unsigned char *r=g+1;

				

				*b-=*b/4;
				*g-*g/4;
				*r-=*r/4;
				
			}
		}
	}
	//return nImg;
}
void shujing(CImage *Img,float *mb,vector<Lk>*LkList=NULL){
	//int midy
	for(int i=0;i<Img->GetWidth();i++){
		for(int j=0;j<Img->GetHeight()/2;j++){
			int j2=Img->GetHeight()-1-j;
			unsigned char *b1=(unsigned char *)Img->GetPixelAddress(i,j);
			unsigned char *g1=b1+1;
			unsigned char *r1=g1+1;
			unsigned char *b2=(unsigned char *)Img->GetPixelAddress(i,j2);
			unsigned char *g2=b2+1;
			unsigned char *r2=g2+1;
			int pos1=j*Img->GetWidth()+i;
			int pos2=j2*Img->GetWidth()+i;
			int b,g,r,m;
			b=*b1;
			g=*g1;
			r=*r1;
			*b1=*b2;
			*g1=*g2;
			*r1=*r2;
			*b2=b;
			*g2=g;
			*r2=r;
			m=mb[pos1];
			mb[pos1]=mb[pos2];
			mb[pos2]=m;
		}
	}
	int midy=Img->GetHeight()/2;
	for(int j=0;j<LkList->size();j++){
		vector<Lk>::iterator  itlk=LkList->begin()+j;
		for(vector<CPoint>::iterator it=itlk->Pts.begin();it!=itlk->Pts.end();it++){
			it->y=midy+(midy-it->y);
		}
		if(itlk->LkType==TYPERECT&&itlk->Pts.size()>1){
			vector<CPoint>::iterator it1=itlk->Pts.begin();
			vector<CPoint>::iterator it2=itlk->Pts.begin()+1;
			int tmp=it1->y;
			it1->y=it2->y;
			it2->y=tmp;
		}
	}
}
unsigned char UcXz(int v){
	if(v>255)
		v=255;
	if(v<0)
		v=0;
	return v;
}
unsigned char *GetImgDatajx(CImage *img,int cn,int &w,int &h){//数据交叉型
	w=img->GetWidth();
	h=img->GetHeight();
	unsigned char *data=new unsigned char[w*h*cn];
	int bpp=img->GetBPP();
	if(bpp>=24){
		for(int j=0;j<h;j++){
			for(int i=0;i<w;i++){
				unsigned char *b=(unsigned char *)img->GetPixelAddress(i,j);
				unsigned char *g=b+1;
				unsigned char *r=g+1;
				if(cn==1){
					unsigned v=UcXz((299*(*r) + 587*(*g) + 114*(*b))/1000);;//(*r+*g+*b)/3; //UcXz((299*(*r) + 587*(*g) + 114*(*b)+500)/1000);
					int pos=j*w+i;
					data[pos]=v;
				}else if(cn==3){
					int pos1=j*w*3+i*3;
					int pos2=pos1+1;
					int pos3=pos2+1;
					data[pos1]=*r;
					data[pos2]=*g;
					data[pos3]=*b;
				}
			}
		}
	}else if(bpp==8){
		for(int j=0;j<h;j++){
			for(int i=0;i<w;i++){
				unsigned char *b=(unsigned char *)img->GetPixelAddress(i,j);
				//unsigned char *g=b+1;
				//unsigned char *r=g+1;
				if(cn==1){
					unsigned v=*b; //UcXz((299*(*r) + 587*(*g) + 114*(*b)+500)/1000);
					int pos=j*w+i;
					data[pos]=v;
				}else if(cn==3){
					int pos1=j*w*3+i*3;
					int pos2=pos1+1;
					int pos3=pos2+1;
					data[pos1]=*b;
					data[pos2]=*b;
					data[pos3]=*b;
				}
			}
		}
	}
	return data;
}
unsigned char *GetImgData(CImage *img,int cn,int &w,int &h){
	w=img->GetWidth();
	h=img->GetHeight();
	unsigned char *data=new unsigned char[w*h*cn];
	int bpp=img->GetBPP();
	if(bpp>=24){
		for(int j=0;j<h;j++){
			for(int i=0;i<w;i++){
				unsigned char *b=(unsigned char *)img->GetPixelAddress(i,j);
				unsigned char *g=b+1;
				unsigned char *r=g+1;
				if(cn==1){
					unsigned v=UcXz((299*(*r) + 587*(*g) + 114*(*b))/1000);;//(*r+*g+*b)/3; //UcXz((299*(*r) + 587*(*g) + 114*(*b)+500)/1000);
					int pos=j*w+i;
					data[pos]=v;
				}else if(cn==3){
					int pos1=j*w+i;
					int pos2=pos1+w*h;
					int pos3=pos2+w*h;
					data[pos1]=*r;
					data[pos2]=*g;
					data[pos3]=*b;
				}
			}
		}
	}else if(bpp==8){
		for(int j=0;j<h;j++){
			for(int i=0;i<w;i++){
				unsigned char *b=(unsigned char *)img->GetPixelAddress(i,j);
				//unsigned char *g=b+1;
				//unsigned char *r=g+1;
				if(cn==1){
					unsigned v=*b; //UcXz((299*(*r) + 587*(*g) + 114*(*b)+500)/1000);
					int pos=j*w+i;
					data[pos]=v;
				}else if(cn==3){
					int pos1=j*w+i;
					int pos2=pos1+w*h;
					int pos3=pos2+w*h;
					data[pos1]=*b;
					data[pos2]=*b;
					data[pos3]=*b;
				}
			}
		}
	}
	return data;
}
CImage *DataToImg(unsigned char *pData,int cn,int w,int h){
	CImage *img=new CImage();
	img->Create(w,h,24,0);
	if(img->IsNull()){
		delete img;
		return NULL;
	}
	for(int j=0;j<h;j++){
		for(int i=0;i<w;i++){
			unsigned char *b=(unsigned char *)img->GetPixelAddress(i,j);
			unsigned char *g=b+1;
			unsigned char *r=g+1;
			if(cn==1){
				*b=*g=*r=pData[j*w+i];
			}else if(cn==3){
				int pos1=j*w+i;
				int pos2=pos1+w*h;
				int pos3=pos2+w*h;
				*r=pData[pos1];
				*g=pData[pos2];
				*b=pData[pos3];
			}
		}
	}
	return img;
}
CImage * r90(CImage *Img){
	//int midy
	int w,h,cn;
	cn=3;
	unsigned char * pData=GetImgData(Img,cn,w,h);
	int w1=h;
	int h1=w;
	unsigned char *pData1=new unsigned char[w1*h1*cn];
	for(int c=0;c<cn;c++){
		for(int j=0;j<h1;j++){
			for(int i=0;i<w1;i++){
				int pos1=c*w1*h1+j*w1+i;
				int pos=c*w*h+i*w+(w-1-j);
				pData1[pos1]=pData[pos];
			}
		}
	}
	CImage *res=DataToImg(pData1,cn,w1,h1);
	delete []pData1;
	delete []pData;
	return res;

}
void Fugai(CImage *bkImg,CImage*img,float *mb,int x,int y){
	if(x<0)
		return;
	if(y<0)
		return;
	if(x>=bkImg->GetWidth())
		return;
	if(y>=bkImg->GetHeight())
		return;
	int w=img->GetWidth();
	if(x+w>bkImg->GetWidth())
		w=bkImg->GetWidth()-x;
	int h=img->GetHeight();
	if(y+h>bkImg->GetHeight())
		h=bkImg->GetHeight()-y;
	for(int j=0;j<h;j++){
		for(int i=0;i<w;i++){
			int pos=j*img->GetWidth()+i;
			if(mb[pos]<0.5){
				continue;
			}
			unsigned char *b=(unsigned char *)img->GetPixelAddress(i,j);
			unsigned char *g=b+1;
			unsigned char *r=g+1;
			
			int jj=j+y;
			int ii=i+x;
			unsigned char *bb=(unsigned char *)bkImg->GetPixelAddress(ii,jj);
			unsigned char *bg=bb+1;
			unsigned char *br=bg+1;
			*bb=*b;
			*bg=*g;
			*br=*r;
		}
	}
}
void shuipinjunzhi(unsigned char *data,int w,int h,int x1,int y1,int x2,int y2){
	int *yv=new int[x2-x1+2];
	for(int i=x1;i<=x2;i++){
		yv[i-x1]=0;
		for(int j=y1;j<=y2;j++){
			yv[i-x1]+=data[j*w+i];
		}
		yv[i-x1]/=(y2-y1+1);
	}
	int yvj=0;
	for(int i=x1;i<=x2;i++){
		yvj+=yv[i-x1];
	}
	yvj/=(x2-x1+1);

	for(int i=x1;i<=x2;i++){
		int cz=yvj-yv[i-x1];
		for(int j=y1;j<=y2;j++){
			data[j*w+i]+=cz;
		}
		
	}
	delete []yv;
}
void TwoValue(unsigned char *data,int w,int h,int x1,int y1,int x2,int y2,int kk){
	int k=kk;
	int k1=0;
	int kd=0;
	int kdn=1;
	int kx=0;
	int kxn=1;
	while(k1!=k){
		for(int j=max(0,y1);j<=y2&&j<h;j++){
			for(int i=max(0,x1);i<=x2&&i<w;i++){
				int v=data[j*w+i];
				if(v>=k){
					kd+=v;
					kdn++;
				}else{
					kx+=v;
					kxn++;
				}
			}
		}
		int kdp=kd/kdn;
		int kxp=kx/kxn;
		int k1=(kdp+kxp)/2;
		if(k1==k)
			break;
		k=k1;
	}
	for(int j=max(0,y1);j<=y2&&j<h;j++){
		for(int i=max(0,x1);i<=x2&&i<w;i++){
			int v=data[j*w+i];
			if(v<k)
				data[j*w+i]=0;
			else
				data[j*w+i]=255;
		}
	}
}
void CWisdomLabelDlg::DrawLk(CDC *pDC,Lk *lk,int bSel,int bDrawHead){
	if(lk->Pts.size()<=0)
		return;
	lk->BqPicRc=lk->BqRc=CRect(-1000,-1000,-1000,-1000);
	vector<CPoint> Pts=lk->Pts;
	if(lk->LkType==TYPERECT){
		CPoint pt0=Pts.at(0);
		CPoint pt1=Pts.at(1);

		Pts.push_back(CPoint(pt0.x,pt1.y));
		Pts.push_back(CPoint(pt1.x,pt0.y));
		Pts.push_back(CPoint((pt0.x+pt1.x)/2,(pt0.y+pt1.y)/2));
	}
	
	{

		CPoint pt=ToTar(lk->Pts.at(0));
		if(m_DisLabIconBtn.GetCheck()){

			map<CStringA,CBitmap *>::iterator it=m_LabMap.find(lk->DxType);
		
			if(it!=m_LabMap.end()){
				if(it->second!=NULL){
					BITMAP bm;
					it->second->GetBitmap(&bm);
					pt.Offset(0,-m_IconW);
					CDC memDC;
					memDC.CreateCompatibleDC(pDC);
					CBitmap *pOld=memDC.SelectObject(it->second);
					pDC->SetStretchBltMode(HALFTONE);
					pDC->StretchBlt(pt.x,pt.y,32,32,&memDC,0,0,bm.bmWidth,bm.bmHeight,SRCCOPY);
					memDC.SelectObject(pOld);
					memDC.DeleteDC();
					lk->BqPicRc=ToSrc(CRect(pt.x,pt.y,pt.x+m_IconW,pt.y+m_IconW));
					if(bSel)
						DrawRect1(pDC,lk->BqPicRc,RGB(0,255,0),PS_SOLID,2);
					pt.Offset(0,m_IconW);
				}
			}


		}
		if(m_DisLabTextBtn.GetCheck()){
			if(bDrawHead){
				int TextHeight=20;
				CRect Rc(lk->Pts.at(0),lk->Pts.at(1));
				Rc.bottom=Rc.top+TextHeight;
				Rc.OffsetRect(0,-TextHeight);
				pDC->FillRect(&Rc,&CBrush(RGB(255,0,255)));
			}
			pt.Offset(0,-15);
			pDC->SetTextColor(m_TextColor);
			pDC->SetBkMode(TRANSPARENT);
			pDC->TextOut(pt.x,pt.y,CString(lk->DxType));
			CSize size=pDC->GetTextExtent(CString(lk->DxType));
			lk->BqRc=ToSrc(CRect(pt.x,pt.y,pt.x+size.cx,pt.y+size.cy));
			if(bSel)
				DrawRect1(pDC,lk->BqRc,RGB(0,255,0),PS_SOLID,2);
			
		}

		
	}
	if(lk->LkType==TYPERECT){
		CRect Rc(lk->Pts.at(0),lk->Pts.at(1));
		DrawRect2(pDC,Rc);
	}else if(lk->LkType==TYPESPLINE){
		//CPoint pt=ToTar(lk->Pts.at(0));
		for(int i=0;i<lk->Pts.size();i++){
			CPoint pt=ToTar(lk->Pts.at(i));
			CRect Rc(CPoint(pt.x-3,pt.y-3),CPoint(pt.x+3,pt.y+3));
			/*if(i==m_PtId)
				Rc=CRect(CPoint(pt.x-4,pt.y-4),CPoint(pt.x+4,pt.y+4));*/
			if(m_MouseState==SELECTDX ||m_MouseState==GAIDIAN)
				pDC->Ellipse(&Rc);
			if(i==0)
				pDC->MoveTo(pt);
			else
				pDC->LineTo(pt);
		}
	}else if(lk->LkType==TYPEDBC){
		//CPoint pt=ToTar(lk->Pts.at(0));
		for(int i=0;i<lk->Pts.size();i++){
			CPoint pt=ToTar(lk->Pts.at(i));
			CRect Rc(CPoint(pt.x-3,pt.y-3),CPoint(pt.x+3,pt.y+3));
			/*if(i==m_PtId)
				Rc=CRect(CPoint(pt.x-8,pt.y-8),CPoint(pt.x+8,pt.y+8));*/
			if(m_MouseState==SELECTDX ||m_MouseState==GAIDIAN)
				pDC->Ellipse(&Rc);
			if(i==0)
				pDC->MoveTo(pt);
			else
				pDC->LineTo(pt);
		}
		CPoint pt=ToTar(lk->Pts.at(0));
		pDC->LineTo(pt);
	}else if(lk->LkType==TYPEPOINT){
		for(int i=0;i<lk->Pts.size();i++){
			CPoint pt=ToTar(lk->Pts.at(i));
			pDC->MoveTo(Offset(pt,-3,-3));
			pDC->LineTo(Offset(pt,4,4));
			pDC->MoveTo(Offset(pt,-3,3));
			pDC->LineTo(Offset(pt,+4,-4));
		}
	}
	for(int i=0;i<Pts.size();i++){
		CPoint pt=ToTar(Pts.at(i));
		CRect Rc(CPoint(pt.x-3,pt.y-3),CPoint(pt.x+4,pt.y+4));
		/*if(i==m_PtId)
			Rc=CRect(CPoint(pt.x-4,pt.y-4),CPoint(pt.x+4,pt.y+4));*/
		if(m_MouseState==SELECTDX ||m_MouseState==GAIDIAN
			||m_MouseState==DELETEDX|| m_MouseState==GAIBIOQIAN||m_MouseState==SETCOPYBZDX
			||m_MouseState==SELCROPDBC||m_MouseState==ERZHIHUA)
			pDC->Ellipse(&Rc);
	}
}

void CWisdomLabelDlg::DrawLk1(CDC *pDC,Lk *lk,CStringA Hz){
	static int cols[80][3];
	static int First=1;
	if(First){
		First=0;
		for(int i=0;i<80;i++){
			cols[i][0]=255;
			cols[i][1]=255;
			cols[i][2]=0;
			/*for(int j=0;j<3;j++){
				cols[i][j]=rand()%255;
			}*/
		}
	}

	//cols[cid][0],cols[cid][1],cols[cid][2]
	if(lk->Pts.size()<=0)
		return;
				int cid=0;
		/*	for(map<CStringA,CBitmap *>::iterator it=m_LabMap.begin();it!=m_LabMap.end();it++,cid++){
				if(it->first == lk->DxType)
					break;
			}*/
				CPen pen1(PS_SOLID,2,RGB(cols[cid][0],cols[cid][1],cols[cid][2]));
					CPen *pOldPen=pDC->SelectObject(&pen1);
	lk->BqPicRc=lk->BqRc=CRect(-1000,-1000,-1000,-1000);
	//for(int i=0;i<lk->Pts.size();i++){
	//	CPoint pt=ToTar(lk->Pts.at(i));
	//	CRect Rc(CPoint(pt.x-3,pt.y-3),CPoint(pt.x+4,pt.y+4));
	//	/*if(i==m_PtId)
	//		Rc=CRect(CPoint(pt.x-4,pt.y-4),CPoint(pt.x+4,pt.y+4));*/
	//	if(m_MouseState==SELECTDX ||m_MouseState==GAIDIAN
	//		||m_MouseState==DELETEDX|| m_MouseState==GAIBIOQIAN)
	//		pDC->Ellipse(&Rc);
	//}
	{

		CPoint pt=ToTar(lk->Pts.at(0));
	
		if(m_DisLabTextBtn.GetCheck() &&lk->DxType == "plate"){
			int TextHeight=25;
			if(lk->DxType != "plate")
				TextHeight=14;
			if(1){
				
				CRect Rc(lk->Pts.at(0),lk->Pts.at(1));
				Rc.bottom=Rc.top+TextHeight;
				Rc.OffsetRect(0,-TextHeight);
				pDC->FillRect(&Rc,&CBrush(RGB(cols[cid][0],cols[cid][1],cols[cid][2])));
			}
			static CFont *font=NULL;
			static CFont *font1=NULL;
			if(font==NULL){
				font=new CFont();
				font->CreateFont(
				   25,                        // nHeight
				   0,                         // nWidth
				   0,                         // nEscapement
				   0,                         // nOrientation
				   FW_HEAVY ,                 // nWeight
				   FALSE,                     // bItalic
				   FALSE,                     // bUnderline
				   0,                         // cStrikeOut
				   ANSI_CHARSET,              // nCharSet
				   OUT_DEFAULT_PRECIS,        // nOutPrecision
				   CLIP_DEFAULT_PRECIS,       // nClipPrecision
				   DEFAULT_QUALITY,           // nQuality
				   DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
				   _T("宋体")); 
				font1=new CFont();
				font1->CreateFont(
				   14,                        // nHeight
				   0,                         // nWidth
				   0,                         // nEscapement
				   0,                         // nOrientation
				   FW_HEAVY ,                 // nWeight
				   FALSE,                     // bItalic
				   FALSE,                     // bUnderline
				   0,                         // cStrikeOut
				   ANSI_CHARSET,              // nCharSet
				   OUT_DEFAULT_PRECIS,        // nOutPrecision
				   CLIP_DEFAULT_PRECIS,       // nClipPrecision
				   DEFAULT_QUALITY,           // nQuality
				   DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
				   _T("宋体")); 
			}
			CFont *poldFont=NULL;
			if(lk->DxType != "plate")
				poldFont=pDC->SelectObject(font1);
			else
				poldFont=pDC->SelectObject(font);
			pt.Offset(0,-TextHeight);
			pDC->SetTextColor(RGB(0,0,0));
			pDC->SetBkMode(OPAQUE);//TRANSPARENT
			pDC->SetBkColor(RGB(cols[cid][0],cols[cid][1],cols[cid][2]));
			int zxd=70+rand()%25;
			CStringA tStr;
			if(!Hz.IsEmpty())
				tStr.Format("%s (%s)",lk->DxType,Hz);
			else
				tStr=lk->DxType;
			pDC->TextOut(pt.x+2,pt.y,CString(tStr));
			CSize size=pDC->GetTextExtent(CString(tStr));
			lk->BqRc=ToSrc(CRect(pt.x+2,pt.y,pt.x+size.cx+2,pt.y+size.cy));
			
			pDC->SelectObject(poldFont);
			
		}

		
	}
	if(lk->LkType==TYPERECT){
		CRect Rc(lk->Pts.at(0),lk->Pts.at(1));
		DrawRect2(pDC,Rc);
	}else if(lk->LkType==TYPESPLINE){
		//CPoint pt=ToTar(lk->Pts.at(0));
		for(int i=0;i<lk->Pts.size();i++){
			CPoint pt=ToTar(lk->Pts.at(i));
			CRect Rc(CPoint(pt.x-3,pt.y-3),CPoint(pt.x+3,pt.y+3));
			/*if(i==m_PtId)
				Rc=CRect(CPoint(pt.x-4,pt.y-4),CPoint(pt.x+4,pt.y+4));*/
			if(m_MouseState==SELECTDX ||m_MouseState==GAIDIAN)
				pDC->Ellipse(&Rc);
			if(i==0)
				pDC->MoveTo(pt);
			else
				pDC->LineTo(pt);
		}
	}else if(lk->LkType==TYPEDBC){
		//CPoint pt=ToTar(lk->Pts.at(0));
		for(int i=0;i<lk->Pts.size();i++){
			CPoint pt=ToTar(lk->Pts.at(i));
			CRect Rc(CPoint(pt.x-3,pt.y-3),CPoint(pt.x+3,pt.y+3));
			/*if(i==m_PtId)
				Rc=CRect(CPoint(pt.x-8,pt.y-8),CPoint(pt.x+8,pt.y+8));*/
			if(m_MouseState==SELECTDX ||m_MouseState==GAIDIAN)
				pDC->Ellipse(&Rc);
			if(i==0)
				pDC->MoveTo(pt);
			else
				pDC->LineTo(pt);
		}
		CPoint pt=ToTar(lk->Pts.at(0));
		pDC->LineTo(pt);
	}else if(lk->LkType==TYPEPOINT){
		for(int i=0;i<lk->Pts.size();i++){
			CPoint pt=ToTar(lk->Pts.at(i));
			pDC->MoveTo(Offset(pt,-3,-3));
			pDC->LineTo(Offset(pt,4,4));
			pDC->MoveTo(Offset(pt,-3,3));
			pDC->LineTo(Offset(pt,+4,-4));
		}
	}
	pDC->SelectObject(pOldPen);
}


CImage * CImageCopy(const CImage *srcImage)
{
	CImage *destImage=new CImage();
	int i,j;//循环变量
	if(srcImage->IsNull())
		return FALSE;
	//源图像参数
	BYTE* srcPtr=(BYTE*)srcImage->GetBits();
	int srcBitsCount=srcImage->GetBPP();
	int srcWidth=srcImage->GetWidth();
	int srcHeight=srcImage->GetHeight();
	int srcPitch=srcImage->GetPitch(); 
	//销毁原有图像
	if( !destImage->IsNull())
	{
		destImage->Destroy();
	}
	//创建新图像
	if(srcBitsCount==32)   //支持alpha通道
	{
		destImage->Create(srcWidth,srcHeight,srcBitsCount,1);
	}
	else
	{
		destImage->Create(srcWidth,srcHeight,srcBitsCount,0);
	}
	//加载调色板
	if(srcBitsCount<=8&&srcImage->IsIndexed())//需要调色板
	{
		RGBQUAD pal[256];
		int nColors=srcImage->GetMaxColorTableEntries();
		if(nColors>0)
		{     
			srcImage->GetColorTable(0,nColors,pal);
			destImage->SetColorTable(0,nColors,pal);//复制调色板程序
		}   
	} 
	//目标图像参数
	BYTE *destPtr=(BYTE*)destImage->GetBits();
	int destPitch=destImage->GetPitch();
	//复制图像数据
	for(i=0 ; i<srcHeight;i++)
	{
		memcpy( destPtr+i*destPitch, srcPtr+i*srcPitch, abs(srcPitch) );
	} 

	return destImage;
} 
#define MIN_DET_SIZE 70
void ScaleData(unsigned char *od,int ow, int oh ,int cn,unsigned char *d, int w, int h ) {
	/*int xs = (ow <<13) / w;
	int ys = (oh << 13) / h;
	//int pj[8] = { 0 };
	for (int j = 0; j < h; j++) {
		int jw = j * w*cn;
		for (int i = 0; i < w; i++) {
			int jj = j * ys;
			int ii = i * xs;
			int left = (ii >> 13);
			int right = left + 1;
			int top = (jj >> 13);
			int bottom = top + 1;
			if (bottom >= oh)
				bottom = oh-1;
			if (right >= ow)
				right = ow-1;

			int rightScale = (ii - (left << 13));
			int leftScale = (8192 - rightScale);
			int bottomScale = (jj - (top << 13));
			int topScale = (8192 - bottomScale);
			for (int k = 0; k < cn; k++) {
				int xtop = (leftScale*od[left*cn + top * ow*cn+k] + rightScale * od[right*cn + top * ow*cn+k]) >> 13;
				int xbottom = (leftScale*od[left*cn + bottom * ow*cn+k] + rightScale * od[right*cn + bottom * ow*cn+k]) >> 13;
				d[jw + i * cn+k] = (topScale*xtop + bottomScale * xbottom) >> 13;
				//pj[k] += d[jw + i * cn + k];
			}
		}
	}*/
	float xs = (float)ow  / (float)w;
	float ys = (float)oh  / (float)h;
	//int pj[8] = { 0 };

	for (int j = 0; j < h; j++) {
		int jw = j * w*cn;
		for (int i = 0; i < w; i++) {
			float jj = (float)j * ys;
			float ii = (float)i * xs;
			int left = ii ;
			int right = left + 1;
			int top = jj ;
			int bottom = top + 1;
			if (bottom >= oh)
				bottom = oh-1;
			if (right >= ow)
				right = ow-1;

			int rightScale = (float)ii - left ;
			int leftScale = 1.0 - rightScale;
			int bottomScale = jj - top ;
			int topScale = 1.0 - bottomScale;
			for (int k = 0; k < cn; k++) {
				float xtop = (leftScale*od[k*ow*oh+ top*ow+left] + rightScale * od[k*ow*oh+ top*ow+right]);
				float xbottom = (leftScale*od[k*ow*oh+ bottom*ow+left] + rightScale * od[k*ow*oh+ bottom*ow+right]);
				d[k*w*h+j*w+i] =UcXz(0.5+ (topScale*xtop + bottomScale * xbottom)) ;
				//pj[k] += d[jw + i * cn + k];
			}
		}
	}
	/*int Size = w * h;
	for (int k = 0; k < cn; k++) {
		pj[k] /= Size;
		pj[k] = 128 - pj[k];
	}
	for (int j = 0; j < h; j++) {
		int jw = j * w*cn;
		for (int i = 0; i < w; i++) {
			for (int k = 0; k < cn; k++) {
				d[jw + i * cn + k] = min(pj[k]+ d[jw + i * cn + k],255);
			}
		}
	}*/
}

void ScaleData(float *od,int ow, int oh ,int cn,float *d, int w, int h ) {
	
	float xs = (float)ow  / (float)w;
	float ys = (float)oh  / (float)h;
	//int pj[8] = { 0 };
	for (int j = 0; j < h; j++) {
		int jw = j * w*cn;
		for (int i = 0; i < w; i++) {
			float jj = (float)j * ys;
			float ii = (float)i * xs;
			int left = ii ;
			int right = left + 1;
			int top = jj ;
			int bottom = top + 1;
			if (bottom >= oh)
				bottom = oh-1;
			if (right >= ow)
				right = ow-1;

			int rightScale = (float)ii - left ;
			int leftScale = 1.0 - rightScale;
			int bottomScale = jj - top ;
			int topScale = 1.0 - bottomScale;
			for (int k = 0; k < cn; k++) {
				float xtop = (leftScale*od[k*ow*oh+ top*ow+left] + rightScale * od[k*ow*oh+ top*ow+right]);
				float xbottom = (leftScale*od[k*ow*oh+ bottom*ow+left] + rightScale * od[k*ow*oh+ bottom*ow+right]);
				d[k*w*h+j*w+i] =(topScale*xtop + bottomScale * xbottom) ;
				//pj[k] += d[jw + i * cn + k];
			}
		}
	}
	
}
CImage *DatajxToImg(unsigned char *pData,int cn,int w,int h){
	CImage *img=new CImage();
	img->Create(w,h,24,0);
	if(img->IsNull()){
		delete img;
		return NULL;
	}
	for(int j=0;j<h;j++){
		for(int i=0;i<w;i++){
			unsigned char *b=(unsigned char *)img->GetPixelAddress(i,j);
			unsigned char *g=b+1;
			unsigned char *r=g+1;
			if(cn==1){
				*b=*g=*r=pData[j*w+i];
			}else if(cn==3){
				int pos1=j*w*3+i*3;
					int pos2=pos1+1;
					int pos3=pos2+1;
				*r=pData[pos1];
				*g=pData[pos2];
				*b=pData[pos3];
			}else if(cn==4){
				int pos1=j*w*4+i*4;
					int pos2=pos1+1;
					int pos3=pos2+1;
				*r=pData[pos1];
				*g=pData[pos2];
				*b=pData[pos3];
			}
		}
	}
	return img;
}

void CWisdomLabelDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);

		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		//CDialogEx::OnPaint();
		CPaintDC dc(this); 
		CRect Rc;
		GetClientRect(&Rc);
		CDC memDC;
		memDC.CreateCompatibleDC(&dc);
		CBitmap bit;
		bit.CreateCompatibleBitmap(&dc,Rc.Width(),Rc.Height());
		CBitmap *pOldBit=memDC.SelectObject(&bit);
		
		memDC.FillRect(&Rc,&CBrush(RGB(240,240,240)));
		if(m_pImg&&!m_pImg->IsNull()&&Rc.Width()>m_LeftWidth+m_RightWidth+10){
			CImage *cImg=CImageCopy(m_pImg);
			CRect Rc1=Rc;
			Rc1.left=m_LeftWidth;//m_TarRect.left;
			Rc1.top=5;//m_TarRect.top;
			Rc1.right-=m_RightWidth;
			Rc1.bottom-=26;
			float Scalex=(float)m_SrcRect.Width()/Rc1.Width();
			float Scaley=(float)m_SrcRect.Height()/Rc1.Height();
			m_DrawScale=Scalex;
			if(Scaley>Scalex)
				m_DrawScale=Scaley;
			int xOffset=(Rc1.Width()-m_SrcRect.Width()/m_DrawScale)/2;
			int yOffset=(Rc1.Height()-m_SrcRect.Height()/m_DrawScale)/2;
			
			CRect Rc2=m_SrcRect.ToCRect();
			TRACE("before:(%d,%d,%d,%d,%d,%d)\n",Rc2.left,Rc2.top,Rc2.right,Rc2.bottom,m_pImg->GetWidth(),m_pImg->GetHeight());

			//TRACE("after:(%d,%d,%d,%d,%d,%d)\n",Rc2.left,Rc2.top,Rc2.right,Rc2.bottom,m_pImg->GetWidth(),m_pImg->GetHeight());
			Rc1.left+=xOffset;
			Rc1.right-=xOffset;
			Rc1.top+=yOffset;
			Rc1.bottom-=yOffset;
			m_TarRect=Rc1;
			if(xOffset>0&&Rc2.left>0){
				if(xOffset>(double)Rc2.left/m_DrawScale){
					Rc1.left-=Rc2.left/m_DrawScale;
					Rc2.left=0;
				}else{
					Rc2.left-=(double)xOffset*m_DrawScale;
					Rc1.left-=xOffset;
				}
			}
			if(xOffset>0&&Rc2.right<m_pImg->GetWidth()){
				if(xOffset>(double)(m_pImg->GetWidth()-Rc2.right)/m_DrawScale){
					Rc1.right+=(m_pImg->GetWidth()-Rc2.right)/m_DrawScale;
					Rc2.right=m_pImg->GetWidth();
				}else{
					Rc2.right+=(double)xOffset*m_DrawScale;
					Rc1.right+=xOffset;
				}
			}
			if(yOffset>0&&Rc2.top>0){
				if(yOffset>(double)Rc2.top/m_DrawScale){
					Rc1.top-=Rc2.top/m_DrawScale;
					Rc2.top=0;
				}else{
					Rc2.top-=(double)yOffset*m_DrawScale;
					Rc1.top-=yOffset;
				}
			}
			if(yOffset>0&&Rc2.bottom<m_pImg->GetHeight()){
				if(yOffset>(double)(m_pImg->GetHeight()-Rc2.bottom)/m_DrawScale){
					Rc1.bottom+=(m_pImg->GetHeight()-Rc2.bottom)/m_DrawScale;
					Rc2.bottom=m_pImg->GetHeight();
				}else{
					Rc2.bottom+=(double)yOffset*m_DrawScale;
					Rc1.bottom+=yOffset;
				}
			}
			TRACE("after:(%d,%d,%d,%d,%d,%d)\n",Rc2.left,Rc2.top,Rc2.right,Rc2.bottom,m_pImg->GetWidth(),m_pImg->GetHeight());
			//m_TarRect1=Rc1;
			//m_TarRect.left=Rc1.left;
			//m_TarRect.top=Rc1.top;
			memDC.SetStretchBltMode(HALFTONE);

			SelectAreaBrightnessReduction(cImg);
			

			if(m_MouseState==SETCOPYIMGPOS&&m_CropImg && !m_CropImg->IsNull()){
				CPoint ptTmp=ToSrc(m_MousePos);
				float cScale=(float)m_pImg->GetWidth()/(float)m_CopyOrWidth;
				if(m_pImg->GetWidth()==m_CopyOrWidth)
					Fugai(cImg,m_CropImg,m_CropMb,ptTmp.x,ptTmp.y);
				else{
					
					
					int ow,oh;
					unsigned char *od=GetImgData(m_CropImg,3,ow,oh);
					int tw=(float)ow*cScale;
					int th=(float)oh*cScale;
					unsigned char *d=new unsigned char[3*tw*th];
					float *mb=new float[tw*th];
					ScaleData(m_CropMb,ow,oh,1,mb,tw,th);
					ScaleData(od,ow, oh ,3,d, tw, th );
					
					CImage *tImg=DataToImg(d,3,tw,th);
					Fugai(cImg,tImg,mb,ptTmp.x,ptTmp.y);
					tImg->Destroy();
					delete tImg;
					delete []od;
					delete []d;
					delete []mb;
				}
				
			}
			cImg->Draw(memDC.GetSafeHdc(),Rc1,Rc2);
			cImg->Destroy();
			delete cImg;

			if(m_MouseState==SETCOPYIMGPOS&&m_CropImg && !m_CropImg->IsNull()){
				CPoint ptTmp=ToSrc(m_MousePos);
				float cScale=(float)m_pImg->GetWidth()/(float)m_CopyOrWidth;
				for(int j=0;j<m_FzList.size();j++){
					Lk lk=m_FzList.at(j);
					CPoint ofpt=ptTmp;//m_CropNewPos;//-m_CropPos;
					vector<CPoint >::iterator it=lk.Pts.begin();
					for(;it!=lk.Pts.end();it++){
						it->x=cScale*it->x;
						it->y=cScale*it->y;
						//TRACE("x:%d,y:%d,ox:%d,oy:%d,sc:%f\n",it->x,it->y,ptTmp.x,ptTmp.y,cScale);
						it->Offset(ofpt);
					}
					DrawLk(&memDC,&lk);
					//m_LkList.push_back(lk);
				}
			}
			
			CStringA state="无";
			if(m_MouseState>=0&&m_MouseState<10){
				state=MSName[m_MouseState];
			}else if(m_MouseState>=0)
				state.Format("%d",m_MouseState);
			memDC.TextOut(Rc1.left,0,CString(state));//Rc1.top
			CPoint tttt=ToSrc(m_MousePos);
			state.Format("width:%d,%d;height:%d,%d,curpos:%d,%d",m_pImg->GetWidth(),(int)(m_pImg->GetWidth()/m_DrawScale),m_pImg->GetHeight(),(int)(m_pImg->GetHeight()/m_DrawScale),tttt.x,tttt.y);
			memDC.TextOut(Rc1.left,20,CString(state));
			//m_pImg->Draw(memDC.GetSafeHdc(),m_TarRect.left,m_TarRect.top,m_pImg->GetWidth(),m_pImg->GetHeight());
			/*m_Angle=30;
			if(m_Angle>45)
				m_Angle=45;
			else if(m_Angle<-45)
				m_Angle=-45;*/


			if(m_DrawLk){
				//DrawRect1(&memDC,ToZfx1(m_GraphRect),RGB(0,255,0),PS_DASH,1);

				if(m_TmpGraphRect.right>0){
					
					CRect tRc=ToTar(m_TmpGraphRect);
					/*CStringA Str;
					Str.Format("%s %0.2f",m_RectType,(float)m_TmpGraphRect.Width()/(float)m_pImg->GetWidth());
					memDC.SetTextColor(RGB(255,0,0));
					memDC.SetBkMode(1);
					memDC.TextOut(tRc.left,tRc.bottom,Str);
					memDC.SetTextColor(RGB(0,255,0));
					memDC.TextOut(tRc.left+1,tRc.bottom+1,Str);*/
					//if(m_TmpGraphRect.Width()>0 && m_TmpGraphRect.Height()>0){
					//	if(tRc.Width()>0 && tRc.Height()>0){
					//		CImage *nImg=BrightnessReduction(m_pImg,m_TmpGraphRect);
					//		memDC.SetStretchBltMode(HALFTONE);
					//		nImg->Draw(memDC.GetSafeHdc(),tRc);//nRc
					//		nImg->Destroy();
					//		delete nImg;
					//	}
					//}
					DrawRect1(&memDC,m_TmpGraphRect,RGB(255,0,0),PS_DASH,1);
					memDC.MoveTo(tRc.TopLeft());
					memDC.LineTo(tRc.BottomRight());
					memDC.MoveTo(TopRight(tRc));
					memDC.LineTo(BottomLeft(tRc));
					/*CRect tRc1=m_TmpGraphRect;
					tRc1.DeflateRect(0.2*tRc1.Width(),0.2*tRc1.Height());
					DrawRect1(&memDC,tRc1,RGB(255,0,0),PS_DASH,1);	*/
				}
				//DrawRect1(&memDC,m_GraphRect,RGB(255,0,0),PS_SOLID,1);
				/*CRect tRc1=m_GraphRect;
				tRc1.DeflateRect(0.2*tRc1.Width(),0.2*tRc1.Height());
				DrawRect1(&memDC,tRc1,RGB(32,128,128),PS_SOLID,1);	*/
				CPen pen(PS_DASH,1,RGB(255,255,0));
				CPen *pOldPen=memDC.SelectObject(&pen);
				//CRect tRc=ToTar(m_GraphRect);	
				/*tRc.left/=m_DrawScale;
				tRc.right/=m_DrawScale;
				tRc.bottom/=m_DrawScale;
				tRc.top/=m_DrawScale;
				tRc.OffsetRect(m_TarRect.left,m_TarRect.top);*/
				
			/*	memDC.MoveTo(tRc.TopLeft());
				memDC.LineTo(tRc.left,tRc.bottom+1);
				memDC.LineTo(tRc.right+1,tRc.bottom+1);
				memDC.LineTo(tRc.right+1,tRc.top);
				memDC.LineTo(tRc.TopLeft());*/
				{
					/*CPen pen1(PS_DASH,1,RGB(255,0,255));
					memDC.SelectObject(&pen1);
					CRect tRc=ToZfx(m_GraphRect);	
					tRc.left/=m_DrawScale;
					tRc.right/=m_DrawScale;
					tRc.bottom/=m_DrawScale;
					tRc.top/=m_DrawScale;
					tRc.OffsetRect(m_TarRect.left,m_TarRect.top);
					memDC.MoveTo(tRc.TopLeft());
					memDC.LineTo(tRc.left,tRc.bottom+1);
					memDC.LineTo(tRc.right+1,tRc.bottom+1);
					memDC.LineTo(tRc.right+1,tRc.top);
					memDC.LineTo(tRc.TopLeft());
					memDC.SelectObject(&pen);*/
				}
				//if(1){//!m_RectType.IsEmpty()
				//	CStringA Str;
				//	Str.Format("%s %0.2f",m_RectType,(float)m_GraphRect.Width()/(float)m_pImg->GetWidth());
				//	memDC.SetTextColor(RGB(255,0,0));
				//	memDC.SetBkMode(1);
				//	memDC.TextOut(tRc.left,tRc.bottom,Str);
				//	memDC.SetTextColor(RGB(0,255,0));
				//	memDC.TextOut(tRc.left+1,tRc.bottom+1,Str);
				//}
				/*if(m_GraphRect.right<m_GraphRect.left||m_GraphRect.bottom<m_GraphRect.top){
					memDC.SetTextColor(RGB(255,0,255));
					memDC.TextOut(tRc.left,tRc.bottom,"错误的区域");
				}*/

				//for(int i=0;i<5;i++){
				//	CPen pen1(PS_SOLID,1,RGB(0,255,0));
				//	if(i==m_MouseState-SET1POINT){
				//		memDC.SelectObject(&pen1);
				//	}else
				//		memDC.SelectObject(&pen);
				//	CPoint pt=ToTar(m_Point[i]);
				//	memDC.MoveTo(pt.x-1,pt.y-1);
				//	memDC.LineTo(pt.x+2,pt.y+2);
				//	memDC.MoveTo(pt.x-1,pt.y+1);
				//	memDC.LineTo(pt.x+2,pt.y-2);
				//	memDC.SetBkMode(1);
				//	CStringA ss;
				//	ss.Format("%d",i);
				//	memDC.TextOut(pt.x-3,pt.y+3,ss);
				//
				//	/*CRect tRc=m_GraphRect;
				//	tRc.OffsetRect(m_Point[i]-m_GraphRect.CenterPoint());
				//	if(i==m_MouseState-SET1POINT)
				//		DrawRect1(&memDC,tRc,RGB(128,66,255),PS_DASH,1);
				//	else
				//		DrawRect1(&memDC,tRc,RGB(255,0,255),PS_DASH,1);*/
				////	memDC.SetPixel(,,RGB(255,0,0));
				//	//memDC.SetPixel(m_Point[i].x+m_TarRect.left-1,m_Point[i].y+m_TarRect.top,RGB(255,0,0));
				//}

				/*
				memDC.SelectObject(&pen1);
				tRc=CRect(63,109,119,165);
				tRc.OffsetRect(m_TarRect.left,m_TarRect.top);
				memDC.MoveTo(tRc.TopLeft());
				memDC.LineTo(tRc.left,tRc.bottom+1);
				memDC.LineTo(tRc.right+1,tRc.bottom+1);
				memDC.LineTo(tRc.right+1,tRc.top);
				memDC.LineTo(tRc.TopLeft());*/
			
			
				//if(m_Dbx.size()>0){
				//	CPoint pt=ToTar(m_Dbx.at(0));
				//	memDC.MoveTo(pt);
				//	for(int i=1;i<m_Dbx.size();i++){
				//		CPoint pt=ToTar(m_Dbx.at(i));
				//		memDC.LineTo(pt);
				//	}
				//	memDC.LineTo(pt);
				///*	CRect tRc=DbxToRect(&m_Dbx);
				//	DrawRect1(&memDC,tRc,RGB(0,255,0),PS_DASH,1);
				//	DrawRect1(&memDC,ToZfx1(tRc),RGB(0,128,255),PS_DASH,1);*/
				//}
				//if(m_DbxMin.size()>0){
				//	CPen pen1(PS_SOLID,1,RGB(255,255,0));
				//	memDC.SelectObject(&pen1);
				//	CPoint pt=ToTar(m_DbxMin.at(0));
				//	memDC.MoveTo(pt);
				//	for(int i=1;i<m_DbxMin.size();i++){
				//		CPoint pt=ToTar(m_DbxMin.at(i));
				//		memDC.LineTo(pt);
				//	}
				//	memDC.LineTo(pt);
				//	/*CRect tRc=DbxToRect(&m_DbxMin);
				//	DrawRect1(&memDC,tRc,RGB(0,255,0),PS_DASH,1);
				//	DrawRect1(&memDC,ToZfx1(tRc),RGB(0,128,255),PS_DASH,1);*/
				//}
				//if(m_OldDbx.size()>0){
				//	CPen pen2(PS_DOT,1,RGB(0,0,255));
				//	memDC.SelectObject(pen2);
				//	CPoint pt=ToTar(m_OldDbx.at(0));
				//	memDC.MoveTo(pt);
				//	for(int i=1;i<m_OldDbx.size();i++){
				//		CPoint pt=ToTar(m_OldDbx.at(i));
				//		memDC.LineTo(pt);
				//	}
				//	memDC.LineTo(pt);
				//}
				//if(m_OldDbxMin.size()>0){
				//	CPen pen2(PS_DOT,1,RGB(0,0,255));
				//	memDC.SelectObject(pen2);
				//	CPoint pt=ToTar(m_OldDbxMin.at(0));
				//	memDC.MoveTo(pt);
				//	for(int i=1;i<m_OldDbxMin.size();i++){
				//		CPoint pt=ToTar(m_OldDbxMin.at(i));
				//		memDC.LineTo(pt);
				//	}
				//	memDC.LineTo(pt);
				//}
				//if(m_TmpDbx.size()>0){
				//	CPen pen1(PS_DASH,1,RGB(255,0,0));
				//	memDC.SelectObject(&pen1);
				//	CPoint pt=ToTar(m_TmpDbx.at(0));
				//	memDC.MoveTo(pt);
				//	for(int i=1;i<m_TmpDbx.size();i++){
				//		CPoint pt=ToTar(m_TmpDbx.at(i));
				//		memDC.LineTo(pt);
				//	}
				//	memDC.LineTo(pt);
				//}
				if(m_TmpLk[1].Pts.size()>1){
					
					//CRect nRc=LkToRect(m_TmpLk[1]);
					//CRect tRc=ToTar(nRc);
					//if(m_TmpLk[1].LkType==TYPERECT&&nRc.Width()>0 && nRc.Height()>0 && tRc.Width()>0 && tRc.Height()>0){
					//	CImage *nImg=BrightnessReduction(m_pImg,nRc);
					//	memDC.SetStretchBltMode(HALFTONE);
					//	nImg->Draw(memDC.GetSafeHdc(),tRc);//nRc
					//	nImg->Destroy();
					//	delete nImg;
					//}
					CPen pen1(PS_DASH,1,RGB(255,0,0));
					memDC.SelectObject(&pen1);
					DrawLk(&memDC,&m_TmpLk[1]);
					//memDC.LineTo(pt);
				}
				for(int j=0;j<m_LkList.size();j++){
					static COLORREF cols[6]={RGB(0,0,255),RGB(0,255,0),RGB(255,255,0),RGB(128,128,0),RGB(128,0,128),RGB(0,128,128)};
					Lk lk=m_LkList.at(j);

					if(lk.Pts.size()<=0){
						continue;
					}
					CPen pen1;//(PS_SOLID,1,RGB(255,0,255));
					if(lk.DxType=="phone"){
						pen1.CreatePen(PS_SOLID,1,cols[0]);
					}else if(lk.DxType=="smoke"){
						pen1.CreatePen(PS_SOLID,1,cols[1]);
					}
					else if(lk.DxType=="sideface"){
						pen1.CreatePen(PS_SOLID,1,cols[3]);
					}
					else if(lk.DxType=="face"){
						pen1.CreatePen(PS_SOLID,1,cols[2]);
					}
					else if(lk.DxType=="closeeye"){
						pen1.CreatePen(PS_SOLID,1,RGB(0,0,0));
					}
					else
						pen1.CreatePen(PS_SOLID,1,RGB(255,0,255));
					memDC.SelectObject(&pen1);
					memDC.SetTextColor(RGB(255,0,255));
					DrawLk(&memDC,&lk);
					vector<Lk >::iterator itlk=m_LkList.begin()+j;
					itlk->BqPicRc=lk.BqPicRc;
					itlk->BqRc=lk.BqRc;
					CPoint pt=ToTar(lk.Pts.at(0));
					pen1.DeleteObject();
					//memDC.MoveTo(pt);
					
				}
				CPen pen2(PS_DASH,1,RGB(0,0,255));
				memDC.SelectObject(pen2);
				{
					int leftx=Rc1.left;
					int disx=m_MousePos.x-leftx;
					int y=disx*tan(m_Angle*PI/180);
					int lefty=m_MousePos.y-y;
					memDC.MoveTo(m_MousePos);
					memDC.LineTo(leftx,lefty);
				}
				{
					int rightx=Rc1.right;
					int disx=rightx-m_MousePos.x;
					int y=disx*tan(m_Angle*PI/180);
					int lefty=m_MousePos.y+y;
					memDC.MoveTo(m_MousePos);
					memDC.LineTo(rightx,lefty);
				}
				{
					int topy=Rc1.top;
					int disy=m_MousePos.y-topy;
					int x=disy*tan(m_Angle*PI/180);
					int topx=m_MousePos.x+x;
					memDC.MoveTo(m_MousePos);
					memDC.LineTo(topx,topy);
				}
				{
					int bottomy=Rc1.bottom;
					int disy=m_MousePos.y-bottomy;
					int x=disy*tan(m_Angle*PI/180);
					int bottomx=m_MousePos.x+x;
					memDC.MoveTo(m_MousePos);
					memDC.LineTo(bottomx,bottomy);
				}
				if(m_KeduBtn.GetCheck()){
					CPoint  xnk[3]={CPoint(10,14),CPoint(  23,27),CPoint(37,58)};
					CStringA sSize=GetWndTxtA(&m_InputSizeBtn);
					int Size=atoi(sSize);
					if(Size==0)
						Size=416;
					float scale1=(float)m_pImg->GetWidth()/(float)Size;
					CPen pen4(PS_SOLID,1,RGB(0,255,255));
					memDC.SelectObject(pen4);
					int n=Size/16;
					for(int i=0;i<n+1;i++){
						CPoint pt1=ToTar(CPoint((float)i*(float)m_pImg->GetWidth()/(float)n,0));
						CPoint pt2=ToTar(CPoint((float)i*(float)m_pImg->GetWidth()/(float)n,m_pImg->GetHeight()));
						memDC.MoveTo(pt1);
						memDC.LineTo(pt2);
					}
					for(int i=0;i<n;i++){
						CPoint pt1=ToTar(CPoint(0,((float)i+0.5)*(float)m_pImg->GetHeight()/(float)n));
						CPoint pt2=ToTar(CPoint(m_pImg->GetWidth(),((float)i+0.5)*(float)m_pImg->GetHeight()/(float)n));
						memDC.MoveTo(pt1);
						memDC.LineTo(pt2);
					}
					for(int i=0;i<3;i++){
						CPen pen5(PS_SOLID,2,RGB(255,0,0));
					    memDC.SelectObject(pen5);
						CRect RcXnk(0,0,xnk[i].x*scale1,xnk[i].y*scale1);
						RcXnk.OffsetRect(m_pImg->GetWidth()/2,m_pImg->GetHeight()/2);
						RcXnk.OffsetRect(-RcXnk.Width()/2,-RcXnk.Height()/2);
						DrawRect2(&memDC,RcXnk);
					}
				}
				if(m_KeduBtn32.GetCheck()){
					CPoint  xnk[3]={ CPoint(81,82),   CPoint(135,169),   CPoint(344,319)};

					CStringA sSize=GetWndTxtA(&m_InputSizeBtn);
					
					int Size=atoi(sSize);
					if(Size==0)
						Size=416;
					float scale1=(float)m_pImg->GetWidth()/(float)Size;
					CPen pen4(PS_SOLID,1,RGB(255,255,0));
					memDC.SelectObject(pen4);
					int n=Size/32;
					TRACE("grid num:%d\n",n);
					for(int i=0;i<n+1;i++){
						CPoint pt1=ToTar(CPoint((float)i*(float)m_pImg->GetWidth()/(float)n,0));
						CPoint pt2=ToTar(CPoint((float)i*(float)m_pImg->GetWidth()/(float)n,m_pImg->GetHeight()));
						memDC.MoveTo(pt1);
						memDC.LineTo(pt2);
					}
					for(int i=0;i<n;i++){
						CPoint pt1=ToTar(CPoint(0,((float)i)*(float)m_pImg->GetHeight()/(float)n));
						CPoint pt2=ToTar(CPoint(m_pImg->GetWidth(),((float)i)*(float)m_pImg->GetHeight()/(float)n));
						memDC.MoveTo(pt1);
						memDC.LineTo(pt2);
					}
					for(int i=0;i<3;i++){
						CPen pen5(PS_SOLID,2,RGB(255,0,0));
					    memDC.SelectObject(pen5);
						CRect RcXnk(0,0,xnk[i].x*scale1,xnk[i].y*scale1);
						RcXnk.OffsetRect(m_pImg->GetWidth()/2,m_pImg->GetHeight()/2);
						RcXnk.OffsetRect(-RcXnk.Width()/2,-RcXnk.Height()/2);
						DrawRect2(&memDC,RcXnk);
					}
				}
				if(m_KeBtn8.GetCheck()){
					CStringA sSize=GetWndTxtA(&m_InputSizeBtn);
					
					int Size=atoi(sSize);
					if(Size==0)
						Size=416;
					CPen pen4(PS_SOLID,1,RGB(255,255,255));
					memDC.SelectObject(pen4);
					int n=Size/8;
					for(int i=0;i<n+1;i++){
						CPoint pt1=ToTar(CPoint((float)i*(float)m_pImg->GetWidth()/(float)n,0));
						CPoint pt2=ToTar(CPoint((float)i*(float)m_pImg->GetWidth()/(float)n,m_pImg->GetHeight()));
						memDC.MoveTo(pt1);
						memDC.LineTo(pt2);
					}
					for(int i=0;i<n;i++){
						CPoint pt1=ToTar(CPoint(0,((float)i+0.5)*(float)m_pImg->GetHeight()/(float)n));
						CPoint pt2=ToTar(CPoint(m_pImg->GetWidth(),((float)i+0.5)*(float)m_pImg->GetHeight()/(float)n));
						memDC.MoveTo(pt1);
						memDC.LineTo(pt2);
					}
				}
				if(m_MouseState>=SET1POINT && m_MouseState<=SET5POINT){
					CRect tRc=m_GraphRect;
					CPoint tPt=m_MousePos;
					tPt.x=(float)(tPt.x-m_TarRect.left)*m_DrawScale;
					tPt.y=(float)(tPt.y-m_TarRect.top)*m_DrawScale;
					tRc.OffsetRect(tPt-m_GraphRect.CenterPoint());
					DrawRect1(&memDC,tRc,RGB(128,0,128),PS_DASH,1);
				}
				else if(m_MouseState==SETPOINT ){
					CPen pen6(PS_SOLID,2,RGB(0,255,0));
					memDC.SelectObject(pen6);
					memDC.MoveTo(Offset(m_MousePos,-4,-4));
					memDC.LineTo(Offset(m_MousePos,4,4));
					memDC.MoveTo(Offset(m_MousePos,-4,4));
					memDC.LineTo(Offset(m_MousePos,4,-4));
					/*CRect tRc=m_GraphRect;
					CPoint tPt=m_MousePos;
					tPt.x=(float)(tPt.x-m_TarRect.left)*m_DrawScale;
					tPt.y=(float)(tPt.y-m_TarRect.top)*m_DrawScale;
					tRc.OffsetRect(tPt-m_GraphRect.CenterPoint());
					DrawRect1(&memDC,tRc,RGB(128,0,128),PS_DASH,1);*/
				}else if(m_MouseState==SETFACERECTFIRSTPOINT){
					
					CPen pen6(PS_SOLID,2,RGB(0,255,0));
					CPen pen7(PS_SOLID,2,RGB(255,0,0));
					memDC.SelectObject(pen6);
					CPoint pt1=Offset(m_MousePos,-10,-8);
					CPoint pt2=Offset(m_MousePos,10,8);
					CRect tRc(pt1,pt2);
					if(m_CmdState==CMDDELETEPIXEL){
						memDC.FillRect(&tRc,&CBrush(RGB(255,128,128)));
						memDC.SelectObject(pen7);
					}else{
					}
					DrawRect22(&memDC,tRc);
					memDC.Ellipse(CRect(pt1.x-5,pt1.y-5,pt1.x+5,pt1.y+5));
					memDC.Ellipse(CRect(pt2.x-5,pt2.y-5,pt2.x+5,pt2.y+5));
					
					
				}
				else if(m_MouseState==SETLK && m_TmpLk[0].LkType==TYPEDBC){
					CPen pen6(PS_SOLID,2,RGB(0,255,0));
					memDC.SelectObject(pen6);
					CPoint pt[6];
					pt[0]=Offset(m_MousePos,-6,-6);
					pt[1]=Offset(m_MousePos,-10,0);
					pt[2]=Offset(m_MousePos,-6,6);
					pt[3]=Offset(m_MousePos,6,6);
					pt[4]=Offset(m_MousePos,10,0);
					pt[5]=Offset(m_MousePos,6,-6);
					memDC.MoveTo(pt[5]);
					for(int k=0;k<6;k++){
						memDC.LineTo(pt[k]);
						//memDC.Ellipse(CRect(pt[k].x-5,pt[k].y-5,pt[k].x+5,pt[k].y+5));
					}
				}else if(m_MouseState==SETLK && m_TmpLk[0].LkType==TYPESPLINE){
					CPen pen6(PS_SOLID,2,RGB(0,255,0));
					memDC.SelectObject(pen6);
					CPoint pt[4];
					pt[0]=Offset(m_MousePos,-10,4);
					pt[1]=Offset(m_MousePos,-5,-4);
					pt[2]=Offset(m_MousePos,5,4);
					pt[3]=Offset(m_MousePos,10,-4);
					
					memDC.MoveTo(pt[0]);
					for(int k=0;k<4;k++){
						memDC.LineTo(pt[k]);
						//memDC.Ellipse(CRect(pt[k].x-5,pt[k].y-5,pt[k].x+5,pt[k].y+5));
					}
				}
				else if((m_MouseState==SELECTDX)){
					if(m_CurPt.x>=0||m_CurLk.Pts.size()>0){
						//if(m_CurLk.Pts.size()>0)
						//{
						//	CRect nRc=LkToRect(m_CurLk);
						//	CRect tRc=ToTar(nRc);
						//	if(nRc.Width()>0 && nRc.Height()>0 && tRc.Width()>0 && tRc.Height()>0){
						//		CImage *nImg=BrightnessReduction(m_pImg,nRc);
						//		memDC.SetStretchBltMode(HALFTONE);
						//		nImg->Draw(memDC.GetSafeHdc(),tRc);//nRc
						//		nImg->Destroy();
						//		delete nImg;
						//	}
						//}
						CPen pen6(PS_SOLID,2,RGB(0,255,0));
						memDC.SelectObject(pen6);
						if(m_CurLk.LkType!=TYPERECT||m_SelLineID<0){
							CPoint Pt=ToTar(m_CurPt);
							CRect tRc(-m_SectPtW/2,-m_SectPtW/2,m_SectPtW/2,m_SectPtW/2);
							tRc.OffsetRect(Pt);
							memDC.Arc(tRc,CPoint(tRc.left,tRc.CenterPoint().y),CPoint(tRc.right,tRc.CenterPoint().y));
							memDC.Arc(tRc,CPoint(tRc.right,tRc.CenterPoint().y),CPoint(tRc.left,tRc.CenterPoint().y));
							CRect tRc1(CPoint(-3,-3),CPoint(4,4));
							tRc1.OffsetRect(Pt);
							memDC.Ellipse(tRc1);
							DrawLk(&memDC,&m_CurLk);
						}else{
							vector<fLine> lines=LkToLine1(m_CurLk);
							fLine fLine=lines.at(m_SelLineID);
							CPoint Pt=ToTar(CPoint((fLine.p1.x+fLine.p2.x)/2,(fLine.p1.y+fLine.p2.y)/2));
							CRect tRc(-m_SectPtW/2,-m_SectPtW/2,m_SectPtW/2,m_SectPtW/2);
							tRc.OffsetRect(Pt);
							if(abs(fLine.p1.x-fLine.p2.x)<abs(fLine.p1.y-fLine.p2.y)){
								memDC.MoveTo(Offset(Pt,-15,0));
								memDC.LineTo(Offset(Pt,-3,0));
								memDC.LineTo(Offset(Pt,-8,-4));
								memDC.MoveTo(Offset(Pt,-3,0));
								memDC.LineTo(Offset(Pt,-8,4));

								memDC.MoveTo(Offset(Pt,15,0));
								memDC.LineTo(Offset(Pt,3,0));
								memDC.LineTo(Offset(Pt,8,-4));
								memDC.MoveTo(Offset(Pt,3,0));
								memDC.LineTo(Offset(Pt,8,4));
							}else{
								memDC.MoveTo(Offset(Pt,0,-15));
								memDC.LineTo(Offset(Pt,0,-3));
								memDC.LineTo(Offset(Pt,-4,-8));
								memDC.MoveTo(Offset(Pt,0,-3));
								memDC.LineTo(Offset(Pt,4,-8));

								memDC.MoveTo(Offset(Pt,0,15));
								memDC.LineTo(Offset(Pt,0,3));
								memDC.LineTo(Offset(Pt,-4,8));
								memDC.MoveTo(Offset(Pt,0,3));
								memDC.LineTo(Offset(Pt,4,8));
							}
							//memDC.Arc(tRc,CPoint(tRc.left,tRc.CenterPoint().y),CPoint(tRc.right,tRc.CenterPoint().y));
							//memDC.Arc(tRc,CPoint(tRc.right,tRc.CenterPoint().y),CPoint(tRc.left,tRc.CenterPoint().y));
							DrawLk(&memDC,&m_CurLk);
						}



						/*memDC.MoveTo(Pt);
						memDC.LineTo(m_MousePos);*/
					}else{
						CPen pen6(PS_SOLID,1,RGB(0,255,0));
						memDC.SelectObject(pen6);
						CRect tRc(-m_SectPtW/2,-m_SectPtW/2,m_SectPtW/2,m_SectPtW/2);
						tRc.OffsetRect(m_MousePos);
						memDC.Arc(tRc,CPoint(tRc.left,tRc.CenterPoint().y),CPoint(tRc.right,tRc.CenterPoint().y));
						memDC.Arc(tRc,CPoint(tRc.right,tRc.CenterPoint().y),CPoint(tRc.left,tRc.CenterPoint().y));

						CPoint pt[4];
						pt[0]=Offset(m_MousePos,-10,0);
						pt[1]=Offset(m_MousePos,10,0);
						pt[2]=Offset(pt[1],-12,-6);
						pt[3]=Offset(pt[1],-12,6);
					
						memDC.MoveTo(pt[1]);
						memDC.LineTo(pt[0]);
						memDC.MoveTo(pt[1]);
						memDC.LineTo(pt[2]);
						memDC.MoveTo(pt[1]);
						memDC.LineTo(pt[3]);
						memDC.Ellipse(CRect(pt[0].x-5,pt[0].y-5,pt[0].x+5,pt[0].y+5));
						memDC.Ellipse(CRect(pt[1].x-5,pt[1].y-5,pt[1].x+5,pt[1].y+5));
					}
				}else if((m_MouseState==MOHU2)){
					CPen pen6(PS_SOLID,1,RGB(0,255,0));
						memDC.SelectObject(pen6);
						CRect tRc(-m_SectPtW/2,-m_SectPtW/2,m_SectPtW/2,m_SectPtW/2);
						tRc.OffsetRect(m_MousePos);
						memDC.Arc(tRc,CPoint(tRc.left,tRc.CenterPoint().y),CPoint(tRc.right,tRc.CenterPoint().y));
						memDC.Arc(tRc,CPoint(tRc.right,tRc.CenterPoint().y),CPoint(tRc.left,tRc.CenterPoint().y));
				}
				else if(m_MouseState==GAIBIOQIAN||m_MouseState==DELETEDX||m_MouseState==SETCOPYBZDX
					||m_MouseState==SELCROPDBC||m_MouseState==ERZHIHUA){
					//if(m_CurLk.Pts.size()>0)
					//{
					//		CRect nRc=LkToRect(m_CurLk);
					//		CRect tRc=ToTar(nRc);
					//		if(nRc.Width()>0 && nRc.Height()>0 && tRc.Width()>0 && tRc.Height()>0){
					//			CImage *nImg=BrightnessReduction(m_pImg,nRc);
					//			memDC.SetStretchBltMode(HALFTONE);
					//			nImg->Draw(memDC.GetSafeHdc(),tRc);//nRc
					//			nImg->Destroy();
					//			delete nImg;
					//		}
					//}
					CPen pen6(PS_SOLID,1,RGB(0,255,0));
					memDC.SelectObject(pen6);
					CRect tRc(-m_SectPtW/2,-m_SectPtW/2,m_SectPtW/2,m_SectPtW/2);
					tRc.OffsetRect(m_MousePos);
					/*static int tt=0;
					CStringA Str;
					Str.Format("%d,%d,%d",tt++,m_MousePos.x,m_MousePos.y);
					memDC.TextOut(Rc1.left,Rc1.top+30,Str);*/
					memDC.Arc(tRc,CPoint(tRc.left,tRc.CenterPoint().y),CPoint(tRc.right,tRc.CenterPoint().y));
					memDC.Arc(tRc,CPoint(tRc.right,tRc.CenterPoint().y),CPoint(tRc.left,tRc.CenterPoint().y));
					//DrawRect22(&memDC,tRc);
					if(m_CurLk.Pts.size()>0){
						CPen pen6(PS_SOLID,1,RGB(0,255,0));
						memDC.SelectObject(pen6);
						DrawLk(&memDC,&m_CurLk,1);
					}
					if(m_MouseState==GAIBIOQIAN){
						CPoint pt[4];
						pt[0]=Offset(m_MousePos,-10,0);
						pt[1]=Offset(m_MousePos,10,0);
						pt[2]=Offset(pt[1],-12,-6);
						pt[3]=Offset(pt[1],-12,6);
					
						CSize s1=memDC.GetTextExtent(CString("LA"));
						memDC.TextOut(pt[0].x-s1.cx,pt[0].y-s1.cy,CString("LA"));
						memDC.TextOut(pt[1].x,pt[1].y-s1.cy,CString("LB"));
						memDC.MoveTo(pt[1]);
						memDC.LineTo(pt[0]);
						memDC.MoveTo(pt[1]);
						memDC.LineTo(pt[2]);
						memDC.MoveTo(pt[1]);
						memDC.LineTo(pt[3]);
					}
					if(m_MouseState==DELETEDX){
						CPoint pt[4];
						pt[0]=Offset(m_MousePos,-10,-6);
						pt[1]=Offset(m_MousePos,10,6);
						pt[2]=Offset(m_MousePos,10,-6);
						pt[3]=Offset(m_MousePos,-10,6);
						memDC.SetTextColor(RGB(0,255,0));
						CSize s1=memDC.GetTextExtent(CString("LAB"));
						memDC.TextOut(m_MousePos.x-s1.cx/2,m_MousePos.y-s1.cy/2,CString("LAB"));
				
						CPen pen7(PS_SOLID,1,RGB(255,0,0));
						memDC.SelectObject(pen7);

						memDC.MoveTo(pt[1]);
						memDC.LineTo(pt[0]);
						memDC.MoveTo(pt[2]);
						memDC.LineTo(pt[3]);
						
					}else if(m_MouseState==ERZHIHUA){
					}
				}
				memDC.SelectObject(pOldPen);
			}
		}
		dc.BitBlt(0,0,Rc.Width(),Rc.Height(),&memDC,0,0,SRCCOPY);
		memDC.SelectObject(pOldBit);
		memDC.DeleteDC();
		bit.DeleteObject();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CWisdomLabelDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CWisdomLabelDlg::OnBnClickedOpenpic()
{
	// TODO: 在此添加控件通知处理程序代码

	CFileDialog Dlg(TRUE,FALSE,FALSE,FALSE,FALSE,FALSE);
	if(Dlg.DoModal()!=IDOK)
		return;
	CStringA FileName=CStringA(Dlg.GetFileName());
	OpenDir(CStringA(Dlg.GetFolderPath()));
	for(int i=0;i<m_pImgListCtrl.GetItemCount();i++){
		CStringA Str=CStringA(m_pImgListCtrl.GetItemText(i,0));
		if(Str==FileName){
			m_pImgListCtrl.SetSelectItem(i);
			//m_pImgListCtrl.SetItemState(i,LVIS_SELECTED,LVIS_SELECTED);
			break;
		}
	}
	//m_MouseState=SELECTDX;
	//CImage Img;
	//if(Img.Load(Dlg.getP
}


BOOL CWisdomLabelDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	return 1;
	return CDialogEx::OnEraseBkgnd(pDC);
}


void CWisdomLabelDlg::OnBnClickedCropface()
{
	// TODO: 在此添加控件通知处理程序代码
	CWnd *pBtn=GetDlgItem(IDCROPFACE);
	CStringA Text=GetWndTxtA(pBtn);
	
	//if(Text=="框脸"){
		m_MouseState=SETFACERECTFIRSTPOINT;
		//pBtn->SetWindowText("停止框脸");
	//}else{
	//	m_MouseState=MOUSESTATENONE;
	//	pBtn->SetWindowText("框脸");
	//}
		m_CmdState=CMDNONE;
}

unsigned char *GetImgData(CImage *img,int cn,int &w,int &h);
unsigned char *CopyData(unsigned char *orData,int orw,int orh,int left,int top,int w,int h,int c=1);
unsigned char *CopyData3(unsigned char *orData,int orw,int orh,int cn,int left,int top,int w,int h);
CImage *DataToImg(unsigned char *pData,int cn,int w,int h);
void CWisdomLabelDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialogEx::OnLButtonUp(nFlags, point);
}


void CWisdomLabelDlg::OnBnClickedSaveAnnotation()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_PicPath.IsEmpty())
		return;

	if(m_LkList.size()>0)
	{
		SaveXml(this->GetXmlFileName(),m_PicName,m_PicPath,m_FolderPath,
			m_pImg->GetWidth(),m_pImg->GetHeight(),m_pImg->GetBPP()/8,&m_LkList);
	}else{
		DeleteFile(CString(this->GetXmlFileName()));
		DeleteFile(CString(m_PicPath+".lk"));
	}
}

void CWisdomLabelDlg::OnBnClickedOpenAnnotation()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_PicPath.IsEmpty())
		return;
	/*if(m_PicTitle=="u=1033329380,4013535641&fm=27&gp=0.jpg"){
		int a=3;
	}*/
	m_Dbx.clear();
	m_DbxMin.clear();
	m_TmpDbx.clear();
	m_LkList.clear();
	m_TmpLk[0].Pts.clear();
	m_TmpLk[1].Pts.clear();
	
	OpenXml(GetXmlFileName(),&m_LkList);
	if(m_LkList.size()<=0)
	{
		CStringA fn=m_PicPath;
		//m_PicPath.Replace("_mohu_","");
		//if(m_PicPath.Find()!=-1)
		CFile f;
		if(!f.Open(CString(fn+".lk"),CFile::modeRead))
			return;
		char Buf[65536];
		int Size=f.Read(Buf,65535);f.Close();
		Buf[Size]=0;
		CStringA Lksss(Buf);
		vector<CStringA> Lkss=SplitStr(Lksss,"\r\n");
		for(int j=0;j<Lkss.size();j++){
			vector<CStringA>Lks=SplitStr(Lkss.at(j),",");
			Lk lk;
			lk.LkType=atoi(Lks.at(2));
			lk.DxType=Lks.at(1);
			for(int i=3;i<Lks.size();i+=2){
				CPoint pt(atoi(Lks.at(i)),atoi(Lks.at(i+1)));
				lk.Pts.push_back(pt);
			}
			if(lk.Pts.size()>0)
				m_LkList.push_back(lk);
		}
		//f.Close();
	}
}

CStringA SelFilePath()
{
      TCHAR           szFolderPath[MAX_PATH] = {0};  
      CString         strFolderPath = TEXT("");  
  
      BROWSEINFO      sInfo;  
      ::ZeroMemory(&sInfo, sizeof(BROWSEINFO));  
      sInfo.pidlRoot   = 0;  
      sInfo.lpszTitle   = _T("请选择处理结果存储路径");  
     sInfo.ulFlags   = BIF_RETURNONLYFSDIRS|BIF_EDITBOX|BIF_DONTGOBELOWDOMAIN;  
     sInfo.lpfn     = NULL;  
 
     // 显示文件夹选择对话框  
     LPITEMIDLIST lpidlBrowse = ::SHBrowseForFolder(&sInfo);   
     if (lpidlBrowse != NULL)  
     {  
         // 取得文件夹名  
         if (::SHGetPathFromIDList(lpidlBrowse,szFolderPath))    
         {  
             strFolderPath = szFolderPath;  
         }  
     }  
     if(lpidlBrowse != NULL)  
     {  
         ::CoTaskMemFree(lpidlBrowse);  
     }  
 
     return CStringA(strFolderPath);  
 }
void CWisdomLabelDlg::OnBnClickedCreatelabel()
{
	// TODO: 在此添加控件通知处理程序代码
	
	CStringA Dir=SelFilePath();
	CFile fl;
	if(!fl.Open(CString(Dir+"\\label.txt"),CFile::modeCreate|CFile::modeWrite))
		return;
	CFileFind ff;
	int res=ff.FindFile(CString(Dir+"\\*.*"));
	while(res){
		res=ff.FindNextFile();
		if(ff.IsDots() || ff.IsDirectory())
			continue;
		CStringA fileName=CStringA(ff.GetFilePath());
		if(fileName.Right(9)==".facerect"){
			CFile f;
			if(f.Open(CString(fileName),CFile::modeRead)){
				char Buf[256];
				int size=f.Read(Buf,255);
				Buf[size]=0;
				CStringA Str(Buf);
				Str+="\r\n";
				fl.Write(Str,Str.GetLength());
			}
			f.Close();
		}
	}
	ff.Close();
	fl.Close();
}


void CWisdomLabelDlg::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(m_MouseState==SETDBX||m_MouseState==SETDBXMIN){
		OnBnClickedSaveAnnotation();
	}
	if(m_MouseState==JUAN){
		m_MouseState=MOUSESTATENONE;
	}else if(m_MouseState == SETCOPYIMGPOS){
		
		m_MouseState=MOUSESTATENONE;
	}
	else if(m_MouseState==GAIDIAN){
		if(m_TmpLk[1].Pts.size()>0){
			m_LkList.push_back(m_TmpLk[1]);
		}
		m_TmpLk[1].Pts.clear();
		m_TmpLk[0].Pts.clear();
		m_MouseState=SELECTDX;
		ReDrawItem();
	}else if(m_MouseState==SELECTDX||m_MouseState==GAIBIOQIAN){
		m_MouseState=SETFACERECTFIRSTPOINT;
		//OnBnClickedSaveAnnotation();
		ReDrawItem();
	}
	else if(m_MouseState==SETLK){
		if(m_TmpLk[0].Pts.size()>0){
			m_LkList.push_back(m_TmpLk[0]);
			m_TmpLk[0].Pts.clear();
			m_TmpLk[1].Pts.clear();
			OnBnClickedSaveAnnotation();
			ReDrawItem();
		}else{
			m_MouseState=MOUSESTATENONE;
		}
	}
	else if(m_MouseState==SET1POINT
		||m_MouseState==SET1POINT+1
		||m_MouseState==SET1POINT+2
		||m_MouseState==SET1POINT+3){
			m_MouseState++;
			ReDrawItem();
	}else if(m_MouseState==SET1POINT+4){
		m_MouseState=MOUSESTATENONE;
		ReDrawItem();
	}else if(m_MouseState==SELCROPDBC){
		OnBnClickedIdcreatedate42();
	}else if(m_MouseState==SETCOPYBZDX){
		m_MouseState=SETCOPYIMGPOS;
	}
	//else if(m_MouseState==MOHU2){
	//	m_MouseState=MOHU1;
	//}
	else
		m_MouseState=MOUSESTATENONE;
	m_TmpGraphRect=CRect(-10,-10,-10,-10);
	m_TmpDbx.clear();
	m_OldDbx.clear();
	ReDrawItem();
	/*if(m_MouseState==SETFACERECTFIRSTPOINT){
		m_MouseState=SETFACERECTSECONDPOINT;
	}else if(m_MouseState==SETFACERECTSECONDPOINT){
		m_MouseState=SETFACERECTFIRSTPOINT;
	}else if(m_MouseState==SET1POINT
		||m_MouseState==SET1POINT+1
		||m_MouseState==SET1POINT+2
		||m_MouseState==SET1POINT+3){
			m_MouseState++;
			ReDrawItem();
	}else if(m_MouseState==SET1POINT+4){
		m_MouseState=SET1POINT;
		ReDrawItem();
	}*/
	CDialogEx::OnRButtonUp(nFlags, point);
}


void CWisdomLabelDlg::OnBnClickedCsetpoint()
{
	// TODO: 在此添加控件通知处理程序代码
	m_MouseState=SETPOINT;
	ReDrawItem();
}


void CWisdomLabelDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	// TODO: 在此处添加消息处理程序代码
	if(!m_bInit)
		return;
	CRect rcBtn;
	GetDlgItem(IDOPENANNOTATIONDIR)->GetWindowRect(rcBtn);
	ScreenToClient(rcBtn);
	m_LeftWidth=rcBtn.right+5;
	CRect RcListCtrl(cx-m_RightWidth+5,5,cx-5,(cy-15)*2/3);
	CRect RcIe1(cx-m_RightWidth+5,RcListCtrl.bottom+5,cx-5,cy-5);
	m_pImgListCtrl.MoveWindow(RcListCtrl);
	if(m_ie.m_hWnd)
		m_ie.MoveWindow(m_LeftWidth+5,5,cx-m_LeftWidth-m_RightWidth-10,cy-30);
	if(m_ie1.m_hWnd)
		m_ie1.MoveWindow(RcIe1);
	GetDlgItem(IDC_CMDEDIT)->MoveWindow(m_LeftWidth+5,cy-25,cx-m_LeftWidth-m_RightWidth-10,20);
	//SendMessage(WM_PAINT);
	Invalidate();
	//ReDrawItem();
}

void mohu2(CImage *img,int x,int y,int dis){
	int w=img->GetWidth();
	int h=img->GetHeight();
	int x1=x-dis;
	int x2=x+dis;
	int y1=y-dis;
	int y2=y+dis;
	if(x1<1)
		x1=1;
	if(x2>w-2)
		x2=w-2;
	if(y1<1)
		y1=1;
	if(y2>h-2)
		y2=h-2;
	static  short data[100][100][3]={-1};
	for(int a=0;a<100;a++){
		for(int b=0;b<100;b++){
			for(int c=0;c<3;c++)
				data[a][b][c]=-1;
		}
	}
	for(int j=y1;j<=y2;j++){
		for(int i=x1;i<=x2;i++){
			if(sqrt((float)(j-y)*(j-y)+(float)(i-x)*(i-x))>dis)
						continue;
			int v[3]={0};
			for(int jj=-1;jj<=1;jj++){
				for(int ii=-1;ii<=1;ii++){
					
					unsigned char *b=(unsigned char *)img->GetPixelAddress(ii+i,jj+j);
					unsigned char *g=b+1;
					unsigned char *r=g+1;
					v[0]+=*b;
					v[1]+=*g;
					v[2]+=*r;
				}
			}
			data[j-y1][i-x1][0]=v[0]/9;
			data[j-y1][i-x1][1]=v[1]/9;
			data[j-y1][i-x1][2]=v[2]/9;
		}
	}
	for(int j=y1;j<=y2;j++){
		for(int i=x1;i<=x2;i++){
			if(data[j-y1][i-x1][0]<=0)
				continue;
			unsigned char *b=(unsigned char *)img->GetPixelAddress(i,j);
			unsigned char *g=b+1;
			unsigned char *r=g+1;
			*b=data[j-y1][i-x1][0];
			*g=data[j-y1][i-x1][1];
			*r=data[j-y1][i-x1][2];
		}
	}
}
void mohu(CImage *img,int x1,int y1,int x2,int y2){
	int w=img->GetWidth();
	int h=img->GetHeight();
	if(x1<1)
		x1=1;
	if(x2>w-2)
		x2=w-2;
	if(y1<1)
		y1=1;
	if(y2>h-2)
		y2=h-2;
	unsigned short data[10][10][3]={0};
	for(int j=y1;j<=y2;j++){
		for(int i=x1;i<=x2;i++){
			int v[3]={0};
			for(int jj=-1;jj<=1;jj++){
				for(int ii=-1;ii<=1;ii++){
					unsigned char *b=(unsigned char *)img->GetPixelAddress(ii+i,jj+j);
					unsigned char *g=b+1;
					unsigned char *r=g+1;
					v[0]+=*b;
					v[1]+=*g;
					v[2]+=*r;
				}
			}
			data[j-y1][i-x1][0]=v[0]/9;
			data[j-y1][i-x1][1]=v[1]/9;
			data[j-y1][i-x1][2]=v[2]/9;
		}
	}
	for(int j=y1;j<=y2;j++){
		for(int i=x1;i<=x2;i++){
			unsigned char *b=(unsigned char *)img->GetPixelAddress(i,j);
			unsigned char *g=b+1;
			unsigned char *r=g+1;
			*b=data[j-y1][i-x1][0];
			*g=data[j-y1][i-x1][1];
			*r=data[j-y1][i-x1][2];
		}
	}
}

void CWisdomLabelDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//if(this->GetFocus()==&m_pImgListCtrl){
		GetDlgItem(IDNEXTPIC)->SetFocus();
	//}
	m_MousePos=point;
	CPoint ptSrc=ToSrc(point);
	if(m_MouseState==SETFACERECTFIRSTPOINT){
		//m_MouseState=GETFACERECTSECONDPOINT;
		//m_TmpGraphRect=m_GraphRect;
		//m_TmpGraphRect.left=ptSrc.x;//(float)(point.x-m_TarRect.left)*m_DrawScale;;
		//m_TmpGraphRect.top=ptSrc.y;//(float)(point.y-m_TarRect.top)*m_DrawScale;
		///*if(m_GraphRect.left<0)
		//	m_GraphRect.left=0;
		//if(m_GraphRect.top<0)
		//	m_GraphRect.top=0;*/
		////m_GraphRect.right=point.x;
		////m_GraphRect.bottom=point.y;
		////m_TmpGraphRect=ToZfx(m_TmpGraphRect);
		//if(m_TmpGraphRect.left<0){
		//	m_TmpGraphRect.OffsetRect(0-m_TmpGraphRect.left,0);
		//}if(m_TmpGraphRect.right>=m_pImg->GetWidth()){
		//	m_TmpGraphRect.OffsetRect(m_pImg->GetWidth()-m_TmpGraphRect.right+1,0);
		//}if(m_TmpGraphRect.top<0){
		//	m_TmpGraphRect.OffsetRect(0,0-m_TmpGraphRect.top);
		//}if(m_TmpGraphRect.bottom>=m_pImg->GetHeight()){
		//	m_TmpGraphRect.OffsetRect(0,m_pImg->GetHeight()-m_TmpGraphRect.bottom+1);
		//}
		/*CRect Rc;
		GetClientRect(&Rc);
		Rc.left+=100;
		InvalidateRect(Rc);*/
	}else if(m_MouseState==SETFACERECTSECONDPOINT){
		//m_MouseState=GETFACERECTFIRSTPOINT;
		m_TmpGraphRect=m_GraphRect;
		m_TmpGraphRect.right=ptSrc.x;//(float)(point.x-m_TarRect.left)*m_DrawScale;;
		m_TmpGraphRect.bottom=ptSrc.y;//(float)(point.y-m_TarRect.top)*m_DrawScale;
		//m_TmpGraphRect=ToZfx(m_TmpGraphRect);
		if(m_TmpGraphRect.left<0){
			m_TmpGraphRect.OffsetRect(0-m_TmpGraphRect.left,0);
		}if(m_TmpGraphRect.right>=m_pImg->GetWidth()){
			m_TmpGraphRect.OffsetRect(m_pImg->GetWidth()-m_TmpGraphRect.right+1,0);
		}if(m_TmpGraphRect.top<0){
			m_TmpGraphRect.OffsetRect(0,0-m_TmpGraphRect.top);
		}if(m_TmpGraphRect.bottom>=m_pImg->GetHeight()){
			m_TmpGraphRect.OffsetRect(0,m_pImg->GetHeight()-m_TmpGraphRect.bottom+1);
		}
	/*	CRect Rc;
		GetClientRect(&Rc);
		Rc.left+=100;
		InvalidateRect(Rc);*/
	}else if(m_MouseState==SETFACECENTER){
		//this->SetFocus();
		
		if(m_TmpGraphRect.Width()==0){
			m_TmpGraphRect.right=m_TmpGraphRect.left+60;
			m_TmpGraphRect.bottom=m_TmpGraphRect.top+60;
		}
		float x=ptSrc.x-m_TmpGraphRect.CenterPoint().x;
		float y=ptSrc.y-m_TmpGraphRect.CenterPoint().y;
		m_TmpGraphRect.OffsetRect(x,y);
		m_TmpGraphRect=ToZfx(m_TmpGraphRect);
		/*if(m_TmpGraphRect.left<0){
			m_TmpGraphRect.OffsetRect(0-m_TmpGraphRect.left,0);
		}if(m_TmpGraphRect.right>=m_pImg->GetWidth()){
			m_TmpGraphRect.OffsetRect(m_pImg->GetWidth()-m_TmpGraphRect.right-2,0);
		}if(m_TmpGraphRect.top<0){
			m_TmpGraphRect.OffsetRect(0,0-m_TmpGraphRect.top);
		}if(m_TmpGraphRect.bottom>=m_pImg->GetHeight()){
			m_TmpGraphRect.OffsetRect(0,m_pImg->GetHeight()-m_TmpGraphRect.bottom-2);
		}*/
		//OnBnClickedSaveAnnotation();
		
	}else if(m_MouseState==SETDBX){
		m_TmpDbx=m_Dbx;
		CPoint newPt(ptSrc);
		m_TmpDbx.push_back(newPt);
	}
	else if(m_MouseState==SETLK){
		if(m_TmpLk[0].Pts.size()>0){
			m_TmpLk[1]=m_TmpLk[0];
			m_TmpLk[1].Pts=m_TmpLk[0].Pts;
			CPoint newPt(ptSrc);
			m_TmpLk[1].Pts.push_back(newPt);
		}
	}
	else if(m_MouseState==SETDBXMIN){
		m_TmpDbx=m_DbxMin;
		CPoint newPt(ptSrc);
		m_TmpDbx.push_back(newPt);
	
	}else if(m_MouseState==GAIDIAN){
		if(m_TmpLk[1].LkType!=TYPERECT||m_SelLineID<0){
			if(m_PtId>=0 &&m_TmpLk[1].Pts.size()>0){//&&m_PtId<2
				if(m_TmpLk[1].LkType!=TYPERECT
					||(m_TmpLk[1].LkType==TYPERECT&&m_PtId<2)){
					vector<CPoint>::iterator it=m_TmpLk[1].Pts.begin()+m_PtId;
					it->x=ptSrc.x;
					it->y=ptSrc.y;
					ReDrawItem();
				}else{
					if(m_PtId==2){
						vector<CPoint>::iterator it=m_TmpLk[1].Pts.begin();
						it->x=ptSrc.x;
						it=m_TmpLk[1].Pts.begin()+1;
						it->y=ptSrc.y;
					}
					else if(m_PtId==3){
						vector<CPoint>::iterator it=m_TmpLk[1].Pts.begin();
						it->y=ptSrc.y;
						it=m_TmpLk[1].Pts.begin()+1;
						it->x=ptSrc.x;
					}else if(m_PtId==4){
						vector<CPoint>::iterator it0=m_TmpLk[1].Pts.begin();
						vector<CPoint>::iterator it1=m_TmpLk[1].Pts.begin()+1;
						int cx=(it0->x+it1->x)/2;
						int cy=(it0->y+it1->y)/2;
						int mx=ptSrc.x-cx;
						int my=ptSrc.y-cy;
						it0->x+=mx;
						it1->x+=mx;
						it0->y+=my;
						it1->y+=my;
					}
					ReDrawItem();
				}
			}
		}else{
			if(m_SelLineID>=0 &&m_TmpLk[1].Pts.size()>0){
				if(m_SelLineID==0){
					vector<CPoint>::iterator it=m_TmpLk[1].Pts.begin();
					it->x=ptSrc.x;
				}
				else if(m_SelLineID==1){
					vector<CPoint>::iterator it=m_TmpLk[1].Pts.begin()+1;
					it->y=ptSrc.y;
				}else if(m_SelLineID==2){
					vector<CPoint>::iterator it=m_TmpLk[1].Pts.begin()+1;
					it->x=ptSrc.x;
				}else if(m_SelLineID==3){
					vector<CPoint>::iterator it=m_TmpLk[1].Pts.begin();
					it->y=ptSrc.y;
				}
				/*vector<CPoint>::iterator it=m_TmpLk[1].Pts.begin()+m_PtId;
				it->x=ptSrc.x;
				it->y=ptSrc.y;*/
				ReDrawItem();
			}
		}
	}else if(m_MouseState==GAIBIOQIAN||m_MouseState==DELETEDX||m_MouseState==SETCOPYBZDX
		||m_MouseState==SELCROPDBC||m_MouseState==ERZHIHUA){
		if(m_LkList.size()>0){
			double MinDis=100000000;
			Lk MinLk;
			int MinIDJ=-1;
			int MinIDI=-1;
			int MinLineID=-1;
			for(int j=0;j<m_LkList.size();j++){
				Lk lk=m_LkList.at(j);
				int LineID=-1;
				int Dis=DisPt2Lk(ptSrc,lk,LineID);//(pt.x-point.x)*(pt.x-point.x)+(pt.y-point.y)*(pt.y-point.y);
				if(Dis<MinDis){
					MinDis=Dis;
					MinIDJ=j;
					//MinIDI=i;
					MinLk=lk;
					MinLineID=LineID;
				}
				
			}
			MinDis/=m_DrawScale;
			if(MinIDJ!=-1 && MinDis<=m_SectPtW/2){
				//Lk lk=m_LkList.at(MinIDJ);
				m_CurLk=MinLk;
			}else{
				m_CurPt.x=-100;
				m_CurLk.Pts.clear();
			}
		}
	}
	else if(m_MouseState==SELECTDX){
		if(m_LkList.size()>0){
			double MinDis=100000000;
			Lk MinLk;
			int MinIDLk=-1;
			int MinIDPt=-1;
			int MinIDLine=-1;
			for(int j=0;j<m_LkList.size();j++){
				Lk lk=m_LkList.at(j);
				if(lk.LkType!=TYPERECT)
					continue;
				for(int i=0;i<lk.Pts.size();i++){
					CPoint pt=ToTar(lk.Pts.at(i));
					int LineID=-1;
					double Dis=DisPt2Lk1(ptSrc,lk,LineID);
					if(Dis<=MinDis){
						MinDis=Dis;
						MinIDLk=j;
						MinIDPt=i;
						MinLk=lk;
						MinIDLine=LineID;
						m_CurPt=pt;
					}
				}
			}
			MinDis+=6*m_DrawScale;
			for(int j=0;j<m_LkList.size();j++){
				Lk lk=m_LkList.at(j);
				//if(lk.LkType==TYPERECT)
					//continue;
				for(int i=0;i<lk.Pts.size();i++){
					CPoint pt=lk.Pts.at(i);
					double Dis=sqrt(pow((float)(pt.x-ptSrc.x),2)+pow((float)(pt.y-ptSrc.y),2));
					if(Dis<=MinDis){
						MinDis=Dis;
						MinIDLk=j;
						MinIDPt=i;
						MinLk=lk;
						MinIDLine=-1;
						m_CurPt=pt;
					}
					
				}
				if(lk.LkType==TYPERECT){
					CPoint pt0=lk.Pts.at(0);
					CPoint pt1=lk.Pts.at(1);
					vector<CPoint> pts;
					pts.push_back(CPoint(pt0.x,pt1.y));
					pts.push_back(CPoint(pt1.x,pt0.y));
					pts.push_back(CPoint((pt0.x+pt1.x)/2,(pt0.y+pt1.y)/2));
					for(int i=0;i<pts.size();i++){
						CPoint pt=pts.at(i);
						double Dis=sqrt(pow((float)(pt.x-ptSrc.x),2)+pow((float)(pt.y-ptSrc.y),2));
						if(Dis<=MinDis){
							MinDis=Dis;
							MinIDLk=j;
							MinIDPt=i+2;
							MinLk=lk;
							MinIDLine=-1;
							m_CurPt=pt;
						}
					}
				}
			}

			//MinDis=sqrt(MinDis);
			MinDis/=m_DrawScale;
			if(MinIDLk!=-1 && MinDis<=m_SectPtW/2){
				Lk lk=m_LkList.at(MinIDLk);
				//m_CurPt=lk.Pts.at(MinIDPt);
				m_CurLk=MinLk;
				m_SelLineID=MinIDLine;
				m_SelLkID=MinIDLk;
				m_SelPtID=MinIDPt;
			}else{
				m_CurPt.x=-100;
				m_CurLk.Pts.clear();
				m_SelLkID=m_SelPtID=m_SelLineID=-1;
			}
		}
	}else if(m_MouseState==MOHU2){
		
	}
	//else if(m_MouseState==JUAN){
	//	COLORREF col=m_pImg->GetPixel(ptSrc.x,ptSrc.y);
	//	COLORREF col1=RGB(GetRValue(col)-5,GetGValue(col)-5,GetBValue(col)-5);
	//	m_pImg->SetPixel(ptSrc.x,ptSrc.y,col1);
	//	ReDrawItem();
	//}
	else{
		m_TmpGraphRect=CRect(-10,-10,-10,-10);
	}
	if(m_pImg)
		this->ReDrawItem();
	CDialogEx::OnMouseMove(nFlags, point);
}


CStringA CWisdomLabelDlg::GetRectType(void)
{
	CStringA RectType=GetWndTxtA(GetDlgItem(IDC_COMBO2));
	return RectType;
}


void CWisdomLabelDlg::OnCbnSelchangeCombo2()
{
	m_RectType=GetRectType();
	OnBnClickedSaveAnnotation();
	//OnBnClickedSaveAnnotation();
	ReDrawItem();
	// TODO: 在此添加控件通知处理程序代码
}


void CWisdomLabelDlg::OnBnClickedOpenpic2()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_pImgListCtrl.GetItemCount()<=0)
		return;
	if(m_ChangePic){
		if(AfxMessageBoxA(CStringA("您已经修改了图片，是否要保存图片?"),MB_YESNO)==IDYES){
			m_pImg->Save(CString(m_PicPath));
			SaveXml(GetXmlFileName(),m_PicName,m_PicPath,m_FolderPath,m_pImg->GetWidth(),m_pImg->GetHeight(),m_pImg->GetBPP()/8,&m_LkList);
			m_ChangePic=0;
		}
	}
	CStringA SsLx=GetWndTxtA(&this->m_BlSxXBtn);
	if(SsLx=="全部"){
		/*CStringA PicName=m_PicList.RemoveHead();
		m_PicList.AddTail(PicName);
		PicName=m_PicList.GetHead();*/
		CStringA sTmp;
			
		//GetDlgItem(IDC_EDIT9)->GetWindowText(sTmp);
		int jg=1;//atoi(sTmp);
		int id=GetSelID();
		if(id<m_pImgListCtrl.GetItemCount()-1){
			id+=jg;
			if(id>m_pImgListCtrl.GetItemCount()-1)
				id=m_pImgListCtrl.GetItemCount()-1;
			m_pImgListCtrl.SetSelectItem(id);
		}
		//OpenNewPic(PicName);
	}else if(SsLx=="未标注"){
		CStringA PicName1;
		int id=GetSelID();
		int Size=m_pImgListCtrl.GetItemCount();
		for(int j=id+1;j<Size;j++){
			PicName1=m_pImgListCtrl.GetItemText(j,0);
			CStringA CropName=m_LabelPath+"\\"+PicName1.Left(PicName1.ReverseFind('.'))+".xml";
			CFileStatus status;
			if(CFile::GetStatus(CString(CropName),status)!=1){
				m_pImgListCtrl.SetSelectItem(j);
				break;
			}
		}
		if(PicName1.IsEmpty()){
			AfxMessageBoxA("没找到");
			return;
		}
	}else if(SsLx=="已标注"){
		CStringA PicName1;
		int id=GetSelID();
		int Size=m_pImgListCtrl.GetItemCount();
		for(int j=id+1;j<Size;j++){
			CStringA PicName=CStringA(m_pImgListCtrl.GetItemText(j,0));
			CStringA CropName=m_LabelPath+"\\"+PicName.Left(PicName.ReverseFind('.'))+".xml";
			CFileStatus status;
			if(CFile::GetStatus(CString(CropName),status)==1){
				m_pImgListCtrl.SetSelectItem(j);
				break;
			}
		}
		if(PicName1.IsEmpty()){
			//AfxMessageBoxA("没找到");
			return;
		}
	}else if(SsLx=="未标注数字或字母"){
		CStringA PicName1;
		int id1=-1;
		int id=GetSelID();
		int Size=m_pImgListCtrl.GetItemCount();
		
		for(int k=id+1;k<Size&&PicName1.IsEmpty();k++){
		
			CStringA PicName=CStringA(m_pImgListCtrl.GetItemText(k,1));
			CStringA CropName=PicName.Left(PicName.ReverseFind('.'))+".xml";
			vector<Lk >lks;
			OpenXml(CropName,&lks);int bFind=0;
			for(int j=0;j<lks.size();j++){
				CStringA DxType=lks.at(j).DxType;
				
				if(DxType.GetLength()==1 ){
					if((DxType[0]>='0' && DxType[0]<='9')
						||(DxType[0]>='A' && DxType[0]<='Z')){
							bFind=1;
						//PicName1=PicName;
						break;
					}
				}
				
			}
			if(!bFind){
				id1=k;
					PicName1=PicName;
				}
		}
		if(id1<0){
			AfxMessageBoxA("没找到");
			return;
		}
		m_pImgListCtrl.SetSelectItem(id1);
		//OpenNewPic(PicName1);
	}
	else{// if(!SsLx.IsEmpty())
		CStringA PicName1;
		int id1=-1;
		int id=GetSelID();
		int Size=m_pImgListCtrl.GetItemCount();
		
		for(int k=id+1;k<Size&&PicName1.IsEmpty();k++){
			CStringA PicName=CStringA(m_pImgListCtrl.GetItemText(k,0));
			CStringA CropName=m_LabelPath+"\\"+PicName.Left(PicName.ReverseFind('.'))+".xml";
			vector<Lk >lks;
			OpenXml(CropName,&lks);
			int nn=0;
			if(m_BlSxXBtn.m_Title1=="包含"){
				for(int j=0;j<lks.size();j++){
					if(lks.at(j).DxType == SsLx ){
						
						nn++;
						//id1=k;
						//break;
					}
				}
				if(nn>=m_MinNum && nn<=m_MaxNum){
					PicName1=PicName;
					id1=k;
				}
			}else{
				int bbh=0;
				for(int j=0;j<lks.size();j++){
					if(lks.at(j).DxType == SsLx ){
						bbh=1;
						nn++;
						id1=k;
						break;
					}
				}
				if(!bbh){
						PicName1=PicName;
						nn++;
						id1=k;
				}
			}
			/*CStringA sTmp;
			
			GetDlgItem(IDC_EDIT9)->GetWindowText(sTmp);
			int yn=atoi(sTmp);
			if(nn>0&&nn<=yn){
				PicName1=PicName;
				id1=k;
			}*/
		}
		if(id1<0){
			AfxMessageBoxA("没找到");
			return;
		}
		m_pImgListCtrl.SetSelectItem(id1);
		//OpenNewPic(PicName1);
	}
	if(m_AutoFzDx.GetCheck()){
		OnBnClickedCdisbiao3();
	}
	GetDlgItem(IDNEXTPIC)->SetFocus();
}


int CWisdomLabelDlg::OpenNewPic(CStringA PicPath)
{
	if(PicPath==m_PicPath)
		return 1;
	if(PicPath.IsEmpty())
		return 1;
	if(m_pImg==NULL)
		m_pImg=new CImage();
	if(!m_pImg->IsNull())
		m_pImg->Destroy();
	
	m_PicPath=PicPath;	
	
	try{
		//int c=3;
		int w,h,c;
		unsigned char *data=loadImage(m_PicPath,&w,&h,&c);
		if(data &&c==3){
			m_pImg=DatajxToImg(data,c,w,h);
		}else{
			m_pImg->Load(CString(m_PicPath));
		}
		if(data)
			free(data);
	//
	}catch(...){
		AfxMessageBoxA(m_PicPath);
		AfxMessageBoxA("error");
		return 1;
	}
	if(m_pImg->IsNull())
		return 0;
	
	if(m_ie.m_hWnd)
		m_ie.ShowWindow(SW_HIDE);
	for(int i=0;i<5;i++){
		m_Point[i].x=0;
		m_Point[i].y=0;
	}
	m_ChangePic=0;
	m_PicName=m_PicPath.Right(m_PicPath.GetLength()-m_PicPath.ReverseFind('\\')-1);
	m_PicTitle=m_PicName.Left(m_PicName.ReverseFind('.'));
	CStringA Hz=m_PicName.Right(m_PicName.GetLength()-m_PicTitle.GetLength()-1);
	if(Hz=="jpeg" || Hz=="JPEG"){
		m_PicName=m_PicTitle+".jpg";
		CStringA NewName=m_PicPath.Left(m_PicPath.GetLength()-Hz.GetLength())+"jpg";
		rename(m_PicPath,NewName);
		m_PicPath=NewName;
		//OnBnClickedSaveAnnotation();
		m_pImgListCtrl.SetItemText(GetSelID(),1,CString(m_PicPath));
	}
	m_PtId=-1;
	m_CurPt.x=-100;
	m_MouseState=MOUSESTATENONE;
	m_TmpGraphRect=m_GraphRect=CRect(0,0,0,0);

	m_RectType="";
	CComboBox *Box=(CComboBox *)GetDlgItem(IDC_COMBO2);
	Box->SetCurSel(Box->FindString(0,CString("无")));
	OnBnClickedOpenAnnotation();
	SetWindowText(CString("WisdomLabel  ")+CString(m_PicPath));//慧标
	m_SrcRect=FRect(0,0,m_pImg->GetWidth(),m_pImg->GetHeight());
	ReDrawItem();
	CStringA Path=GetXmlFileName();
	CFileStatus Status;
	if(!CFile::GetStatus(CString(Path),Status)&&m_LkList.size()>0){
		OnBnClickedSaveAnnotation();
	}
	if(m_pImg->GetBPP()==8){
		m_pImg->Save(CString(m_PicPath));
	}
	
	
	if(m_pImg->GetWidth()>3000||m_pImg->GetHeight()>3000){
		int cn=3;
		if(m_AutoHdBtn.GetCheck())
			cn=1;
		int w,h;
		unsigned char *pData=GetImgData(m_pImg,cn,w,h);
		
		
		unsigned char *pData1=new unsigned char[w*h*cn/4];
		ScaleData(pData,w,h,cn,pData1,w/2,h/2);//CopyData3(pData,w,h,cn,x,y,w1,h1);
		m_pImg->Destroy();
		delete m_pImg;
		m_pImg=DataToImg(pData1,cn,w/2,h/2);
		
		saveImg(m_PicPath,m_pImg);
	
		delete []pData;
		delete []pData1;



		for(int j=0;j<m_LkList.size();j++){
			vector<Lk >::iterator it=m_LkList.begin()+j;
			for(vector<CPoint>::iterator it1=it->Pts.begin();it1!=it->Pts.end();it1++){
				it1->x/=2;
				it1->y/=2;;
				
			}
		}
		OnBnClickedSaveAnnotation();
		m_SrcRect=FRect(0,0,m_pImg->GetWidth(),m_pImg->GetHeight());
		ReDrawItem();
		//OpenNewPic(m_PicPath);
	}

	if(m_LkList.size()<=0)
		m_MouseState=SETFACERECTFIRSTPOINT;
	return 1;
}


void CWisdomLabelDlg::OnBnClickedOpenpic3()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_pImgListCtrl.GetItemCount()<=0)
		return;
	if(m_ChangePic){
		if(AfxMessageBoxA("您已经修改了图片，是否要保存图片?",MB_YESNO)==IDYES){
			m_pImg->Save(CString(m_PicPath));
			SaveXml(GetXmlFileName(),m_PicName,m_PicPath,m_FolderPath,m_pImg->GetWidth(),m_pImg->GetHeight(),m_pImg->GetBPP()/8,&m_LkList);
			m_ChangePic=0;
		}
	}
	CStringA SsLx=GetWndTxtA(&this->m_BlSxXBtn);
	//.GetWindowText(CString(SsLx));
	if(SsLx=="全部"){
		int id=GetSelID();
		if(id<=0)
			return;
		m_pImgListCtrl.SetSelectItem(id-1);
		//CStringA PicName=m_pImgListCtrl.GetItemText(id-1,1);
		//OpenNewPic(PicName);
	}else if(SsLx=="未标注"){
		CStringA PicName1;
		int id1=-1;
		int id=GetSelID();
		int Size=m_pImgListCtrl.GetItemCount();
		for(int j=id-1;j>=0;j--){

			CStringA PicName=CStringA(m_pImgListCtrl.GetItemText(j,0));
			CStringA CropName=m_LabelPath+"\\"+PicName.Left(PicName.ReverseFind('.'))+".xml";
			CFileStatus status;
			if(CFile::GetStatus(CString(CropName),status)!=1){
				PicName1=PicName;
				id1=j;
				m_pImgListCtrl.SetSelectItem(j);
				break;
			}
		}
		if(id1<0){
			AfxMessageBoxA("没找到");
			return;
		}
		m_pImgListCtrl.SetSelectItem(id1);
	}else if(SsLx=="已标注"){
		CStringA PicName1;
		int id1=-1;
		int id=GetSelID();
		int Size=m_pImgListCtrl.GetItemCount();
		for(int j=id-1;j>=0;j--){
			CStringA PicName=CStringA(m_pImgListCtrl.GetItemText(j,0));
			CStringA CropName=m_LabelPath+"\\"+PicName.Left(PicName.ReverseFind('.'))+".xml";
			CFileStatus status;
			if(CFile::GetStatus(CString(CropName),status)==1){
				PicName1=PicName;
				id1=j;
				m_pImgListCtrl.SetSelectItem(j);
				break;
			}
		}
		if(id1<0){
			AfxMessageBoxA("没找到");
			return;
		}
		//OpenNewPic(PicName1);
	}else if(SsLx=="未标注数字或字母"){
		CStringA PicName1;
		int id1=-1;
		int id=GetSelID();
		int Size=m_pImgListCtrl.GetItemCount();
		
		for(int k=id-1;k>=0&&PicName1.IsEmpty();k--){
			CStringA PicName=CStringA(m_pImgListCtrl.GetItemText(k,1));
			CStringA CropName=PicName.Left(PicName.ReverseFind('.'))+".xml";
			vector<Lk >lks;
			OpenXml(CropName,&lks);
			int bFind=0;
			for(int j=0;j<lks.size();j++){
				CStringA DxType=lks.at(j).DxType;
				
				if(DxType.GetLength()==1 ){
					if((DxType[0]>='0' && DxType[0]<='9')
						||(DxType[0]>='A' && DxType[0]<='Z')){
							bFind=1;
						//PicName1=PicName;
						break;
					}
				}
				
			}
			if(!bFind){
					PicName1=PicName;
					id1=k;

			}
		}
		if(id1<0){
			AfxMessageBoxA("没找到");
			return;
		}
		m_pImgListCtrl.SetSelectItem(id1);
		//OpenNewPic(PicName1);
	}
	else if(!SsLx.IsEmpty()){
		CStringA PicName1;
		int id1=-1;
		int id=GetSelID();
		int Size=m_pImgListCtrl.GetItemCount();
		
		for(int k=id-1;k>=0&&PicName1.IsEmpty();k--){
			CStringA PicName=CStringA(m_pImgListCtrl.GetItemText(k,0));
			CStringA CropName=m_LabelPath+"\\"+PicName.Left(PicName.ReverseFind('.'))+".xml";
			vector<Lk >lks;
			OpenXml(CropName,&lks);
			if(m_BlSxXBtn.m_Title1=="包含"){
				for(int j=0;j<lks.size();j++){
					if(lks.at(j).DxType == SsLx ){
						PicName1=PicName;
						//nn++;
						id1=k;
						break;
					}
				}
			}else{
				int bbh=0;
				for(int j=0;j<lks.size();j++){
					if(lks.at(j).DxType == SsLx ){
						bbh=1;
						//nn++;
						id1=k;
						break;
					}
				}
				if(!bbh){
						PicName1=PicName;
						//nn++;
						id1=k;
				}
			}
		}
		if(id1<0){
			AfxMessageBoxA("没找到");
			return;
		}
		m_pImgListCtrl.SetSelectItem(id1);
		//OpenNewPic(PicName1);
	}
}


void CWisdomLabelDlg::OnBnClickedCropface2()
{
	// TODO: 在此添加控件通知处理程序代码
	//if(m_GraphRect.Width()>0)
	//	m_TmpGraphRect=m_GraphRect;
	//else if(m_Dbx.size()>0){
	//	m_GraphRect=m_TmpGraphRect=ToZfx(DbxToRect(&m_Dbx));
	//	OnBnClickedSaveAnnotation();
	//	ReDrawItem();
	//}
	//if(m_TmpGraphRect.Width()>m_pImg->GetWidth()/2||m_TmpGraphRect.Height()>m_pImg->GetHeight()/2){
		m_TmpGraphRect=CRect(-10,-10,0,0);
	//}
	m_MouseState=SETFACECENTER;
	//this->SetFocus();
}


void CWisdomLabelDlg::OnMouseHWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// 此功能要求 Windows Vista 或更高版本。
	// _WIN32_WINNT 符号必须 >= 0x0600。
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialogEx::OnMouseHWheel(nFlags, zDelta, pt);
}


BOOL CWisdomLabelDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if(pMsg->message==WM_MOUSEWHEEL){
	/*	int zDelta=pMsg->wParam>>16;
		UINT nFlags=pMsg->wParam-(zDelta<<16);
		CPoint pt(pMsg->lParam%65536,pMsg->lParam>>16);
		OnMouseWheel(nFlags,zDelta,pt);
		return TRUE;*/
	}else if(pMsg->message==WM_KEYDOWN){
		static CStringA Cmd="";
		static CStringA PrevCmd="";

		if(pMsg->wParam==38){
			m_TmpGraphRect.left-=10;
			m_TmpGraphRect.top-=10;
			m_TmpGraphRect.right+=10;
			m_TmpGraphRect.bottom+=10;
			return TRUE;
		}else if(pMsg->wParam==40){
			m_TmpGraphRect.left+=10;
			m_TmpGraphRect.top+=10;
			m_TmpGraphRect.right-=10;
			m_TmpGraphRect.bottom-=10;
			return TRUE;
		}
		else if(pMsg->wParam==39){
			OnBnClickedOpenpic2();
			OnBnClickedCropface();
			return TRUE;
		}else if(pMsg->wParam==37){
			OnBnClickedOpenpic3();
			OnBnClickedCropface();
			return TRUE;
		}else{
			/*CWnd *pFocus=GetFocus();
			static vector<CWnd *>EditList;
			if(EditList.size()==0){
				EditList.push_back(GetDlgItem(IDC_COMBO1));
				EditList.push_back(GetDlgItem(IDC_EDIT9));
				EditList.push_back(GetDlgItem(IDC_EDIT11));
				EditList.push_back(GetDlgItem(IDC_EDIT8));
				EditList.push_back(GetDlgItem(IDC_EDIT10));
			}
			BOOL bEdit=0;
			for(int i=0;i<EditList.size();i++){
				if(EditList.at(i)==pFocus){
					bEdit=1;
					break;
				}
			}*/
			/*IDC_COMBO1
				IDC_EDIT9
				IDC_EDIT11
				IDC_EDIT8
				IDC_EDIT10*/
			/*CRuntimeClass *pClassInfo=pFocus->GetRuntimeClass();
			CStringA pClassName=CStringA(pClassInfo->m_lpszClassName);
			AfxMessageBoxA(pClassName);*/
			if(1){
				if(pMsg->wParam==VK_RETURN||pMsg->wParam==VK_SPACE){
					if(Cmd==""&&PrevCmd!=""){
						Cmd=PrevCmd;
					}
					map<CStringA,Kjj>::iterator it=m_KjjMap.find(Cmd);
					if(it!=m_KjjMap.end()){
						SendMessage(WM_COMMAND,MAKEWPARAM(it->second.Id,BN_CLICKED),0);
						ReDrawItem();
					}
					PrevCmd=Cmd;
					Cmd="";
					
					//GetDlgItem(IDNEXTPIC)->SetFocus();
				}else if(pMsg->wParam == VK_BACK){
					if(Cmd.GetLength()>0)
						Cmd=Cmd.Left(Cmd.GetLength()-1);
				}else if(pMsg->wParam==VK_ESCAPE){
					Cmd="";
				}
				else
					Cmd+=CStringA((char)pMsg->wParam);
				
			}
			GetDlgItem(IDC_CMDEDIT)->SetWindowText(CString("输入命令,按空格或回车键执行,按ESC键清空命令,当前命令:"+Cmd));
		}
		if(pMsg->wParam==VK_RETURN ||pMsg->wParam==VK_ESCAPE||pMsg->wParam==VK_SPACE){
			return TRUE;
		}
		
	}
	else if(pMsg->message==WM_CHAR){

		//SendMessage(pMsg->message,pMsg->wParam,pMsg->lParam);
		//return TRUE;
	}else if(pMsg->message==WM_KEYUP){
		

	/*	CRect Rc;
		GetClientRect(&Rc);
		Rc.left+=100;
		InvalidateRect(Rc);*/
		//return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}


//LRESULT CWisdomLabelDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
//{
//	// TODO: 在此添加专用代码和/或调用基类
//	if(message==WM_MOUSEWHEEL){
//		//SendMessage(message,wParam,lParam);
//		//return TRUE;
//	}
//	return CDialogEx::DefWindowProc(message, wParam, lParam);
//}


BOOL CWisdomLabelDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(!m_pImg){
		return CDialogEx::OnMouseWheel(nFlags, zDelta, pt);
	}
	//GetDlgItem(IDNEXTPIC)->SetFocus();
	if(m_MouseState==SETFACECENTER && !(0x8000 & GetKeyState(VK_SHIFT))){
		if(zDelta>0){
			m_TmpGraphRect.left-=1;
			m_TmpGraphRect.top-=1;
			m_TmpGraphRect.right+=1;
			m_TmpGraphRect.bottom+=1;
		}else{
			m_TmpGraphRect.left+=1;
			m_TmpGraphRect.top+=1;
			m_TmpGraphRect.right-=1;
			m_TmpGraphRect.bottom-=1;
		}
		/*if(m_TmpGraphRect.left<0){
			m_TmpGraphRect.OffsetRect(0-m_TmpGraphRect.left,0);
		}if(m_TmpGraphRect.right>=m_pImg->GetWidth()){
			m_TmpGraphRect.OffsetRect(m_pImg->GetWidth()-m_TmpGraphRect.right+1,0);
		}if(m_TmpGraphRect.top<0){
			m_TmpGraphRect.OffsetRect(0,0-m_TmpGraphRect.top);
		}if(m_TmpGraphRect.bottom>=m_pImg->GetHeight()){
			m_TmpGraphRect.OffsetRect(0,m_pImg->GetHeight()-m_TmpGraphRect.bottom+1);
		}*/
		CRect Rc;
		GetClientRect(&Rc);
		Rc.left+=100;
		InvalidateRect(Rc);
	}else{
		if(zDelta>0){
			CPoint ptSrc=ToSrc(m_MousePos);
			float x1=m_SrcRect.left-ptSrc.x;
			float x2=m_SrcRect.right-ptSrc.x;
			float y1=m_SrcRect.top-ptSrc.y;
			float y2=m_SrcRect.bottom-ptSrc.y;
			x1*=0.9;
			x2*=0.9;
			y1*=0.9;
			y2*=0.9;
			m_SrcRect=FRect(ptSrc.x+x1,ptSrc.y+y1,ptSrc.x+x2,ptSrc.y+y2);
			
			
		}else{
			CPoint ptSrc=ToSrc(m_MousePos);
			float x1=m_SrcRect.left-ptSrc.x;
			float x2=m_SrcRect.right-ptSrc.x;
			float y1=m_SrcRect.top-ptSrc.y;
			float y2=m_SrcRect.bottom-ptSrc.y;
			x1/=0.9;
			x2/=0.9;
			y1/=0.9;
			y2/=0.9;
			m_SrcRect=FRect(ptSrc.x+x1,ptSrc.y+y1,ptSrc.x+x2,ptSrc.y+y2);
			/*if(m_SrcRect.Width()>m_pImg->GetWidth()||m_SrcRect.Height()>m_pImg->GetHeight()){
				m_SrcRect=FRect(0,0,m_pImg->GetWidth(),m_pImg->GetHeight());
			}*/
		
		}	
	/*	if(m_SrcRect.left<0){
			m_SrcRect.OffsetRect(-m_SrcRect.left,0);
		}
		if(m_SrcRect.right>m_pImg->GetWidth()){
			m_SrcRect.OffsetRect(m_pImg->GetWidth()-m_SrcRect.right,0);
		}
		if(m_SrcRect.top<0){
			m_SrcRect.OffsetRect(0,-m_SrcRect.top);
		}
		if(m_SrcRect.bottom>m_pImg->GetHeight()){
			m_SrcRect.OffsetRect(0,m_pImg->GetHeight()-m_SrcRect.bottom);
		}*/
		ReDrawItem();
	}
	return CDialogEx::OnMouseWheel(nFlags, zDelta, pt);
}


void CWisdomLabelDlg::OnBnClickedOpenpic4()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_pImgListCtrl.GetItemCount()<=0)
		return;
	CStringA PicName1;
	int id1=-1;
	int id=GetSelID();
	int Size=m_pImgListCtrl.GetItemCount();
	for(int j=id+1;j<Size;j++){
		if(j>=m_pImgListCtrl.GetItemCount())
			break;
		CStringA PicName=CStringA(m_pImgListCtrl.GetItemText(j,1));
		CStringA CropName=PicName+".lk";
		CFileStatus status;
		if(CFile::GetStatus(CString(CropName),status)!=1){
			id1=j;
			PicName1=PicName;
			break;
		}
	}
	if(id1<0){
		AfxMessageBoxA("没找到");
		return;
	}
	m_pImgListCtrl.SetSelectItem(id1);
	//OpenNewPic(PicName1);
}


int inuc(int v){
	if(v>255)
		v=255;
	else if(v<0)
		v=0;
	return v;
}
CImage *DataToImg(unsigned int *pData,int cn,int w,int h){
	CImage *img=new CImage();
	img->Create(w,h,32,0);
	if(img->IsNull()){
		delete img;
		return NULL;
	}
	for(int j=0;j<h;j++){
		for(int i=0;i<w;i++){
			unsigned char *b=(unsigned char *)img->GetPixelAddress(i,j);
			unsigned char *g=b+1;
			unsigned char *r=g+1;
			if(cn==1){
				*b=*g=*r=inuc(pData[j*w+i]);
			}else if(cn==3){
				int pos1=j*w+i;
				int pos2=pos1+w*h;
				int pos3=pos2+w*h;
				*r=pData[pos1];
				*g=pData[pos2];
				*b=pData[pos3];
			}
		}
	}
	return img;
}
CImage *DataToImg(float *pData,int cn,int w,int h){
	CImage *img=new CImage();
	img->Create(w,h,32,0);
	if(img->IsNull()){
		delete img;
		return NULL;
	}
	for(int j=0;j<h;j++){
		for(int i=0;i<w;i++){
			unsigned char *b=(unsigned char *)img->GetPixelAddress(i,j);
			unsigned char *g=b+1;
			unsigned char *r=g+1;
			if(cn==1){
				*b=*g=*r=inuc(pData[j*w+i]);
			}else if(cn==3){
				int pos1=j*w+i;
				int pos2=pos1+w*h;
				int pos3=pos2+w*h;
				*r=pData[pos1];
				*g=pData[pos2];
				*b=pData[pos3];
			}
		}
	}
	return img;
}
void ToGray(CImage *img){
	int bpp=img->GetBPP();
	int h=img->GetHeight();
	int w=img->GetWidth();
	if(bpp>=24){
		for(int j=0;j<h;j++){
			for(int i=0;i<w;i++){
				unsigned char *b=(unsigned char *)img->GetPixelAddress(i,j);
				unsigned char *g=b+1;
				unsigned char *r=g+1;
				unsigned v=UcXz((299*(*r) + 587*(*g) + 114*(*b))/1000);
				*b=*g=*r=v;
			}
		}
	}
}
unsigned char *GetBitmapData(CBitmap *bit,int cn,int &w,int &h){
	BITMAP bt;
	bit->GetBitmap(&bt);
	w=bt.bmWidth;
	h=bt.bmHeight;
	int bpp=bt.bmBitsPixel/8;
	unsigned char *pData1=new unsigned char[w*h*bpp];
	unsigned char *pData2=new unsigned char[w*h];
	bit->GetBitmapBits(w*h*bpp,pData1);
	if(bpp==4){
		unsigned char *v=pData1;
		for(int j=0;j<h;j++){
			for(int i=0;i<w;i++){
				unsigned char *b=v;
				unsigned char *g=b+1;
				unsigned char *r=g+1;
				if(cn==1){
					unsigned v=UcXz((299*(*r) + 587*(*g) + 114*(*b))/1000);;//(*r+*g+*b)/3; //UcXz((299*(*r) + 587*(*g) + 114*(*b)+500)/1000);
					int pos=j*w+i;
					pData2[pos]=v;
				}
				v+=4;
			}
		}
	}
	delete []pData1;
	return pData2;
}

unsigned char *CopyData3(unsigned char *orData,int orw,int orh,int cn,int left,int top,int w,int h){
	unsigned char *data=new unsigned char[w*h*cn];
	for(int c=0;c<cn;c++){
		int possrc=w*h*c;
		int posor=orw*orh*c;
		for(int j=0;j<h;j++){
			for(int i=0;i<w;i++){
				int jj=top+j;
				int ii=left+i;
				if(ii<0||ii>=orw||jj<0||jj>=orh){
					data[j*w+i+possrc]=128;
					continue;
				}
				data[j*w+i+possrc]=orData[jj*orw+ii+posor];
			}
		}
	}
	return data;
}
unsigned char *FugaiData(unsigned char *orData,int orw,int orh,unsigned char *data,int left,int top,int w,int h,int c){
	
	for(int cn=0;cn<c;cn++){
		for(int j=0;j<h;j++){
			for(int i=0;i<w;i++){
				int jj=top+j;
				int ii=left+i;
				if(ii<0||ii>=orw||jj<0||jj>=orh){
					//data[j*w+i]=128;
					continue;
				}
				orData[cn*orw*orh+jj*orw+ii]=data[cn*w*h+j*w+i];
			}
		}
	}
	return data;
}
unsigned char *CopyData(unsigned char *orData,int orw,int orh,int left,int top,int w,int h,int c){
	unsigned char *data=new unsigned char[w*h*c];
	for(int cn=0;cn<c;cn++){
		for(int j=0;j<h;j++){
			for(int i=0;i<w;i++){
				int jj=top+j;
				int ii=left+i;
				if(ii<0||ii>=orw||jj<0||jj>=orh){
					data[j*w+i]=128;
					continue;
				}
				data[cn*w*h+j*w+i]=orData[cn*orw*orh+jj*orw+ii];
			}
		}
	}
	return data;
}
#define PI 3.1415926535
//角度到弧度转化
#define RADIAN(angle) ((angle)*PI/180.0)
unsigned char *CopyAndRotateData(unsigned char *orData,int orw,int orh,int left,int top,int nw,int nh,float angle){
	unsigned char *data=new unsigned char[nw*nh];
	for (int j = 0; j < nh; j++){
		for(int i = 0; i < nw ;i++){
              //转换到以图像为中心的坐标系，并进行逆旋转
			int cx=nw/2;
			int cy=nh/2;
			  float tX = (float)(j - cx)*cos(RADIAN(-90.0 +angle)) + (float)(-i + cy)*sin(RADIAN(-90.0 +angle));
              float tY = (float)(j - cx)*sin(RADIAN(-90.0 +angle)) - (float)(-i + cy)*cos(RADIAN(-90.0+ angle));
              //如果这个坐标不在原图像内，则不赋值
			  //tX=0-tX;
              /*if (tX > w / 2 || tX < -w / 2 || tY > h / 2 || tY < -h / 2)
              {
                  continue;
              }*/
             //再转换到原坐标系下

              float tXN = tX + (float)left+(float)cx; 
			  float tYN = abs(tY - (float)top-(float)cy);
			  if(tXN<0 || tYN<0){
					delete []data;
					return NULL;//continue;
			  }
			  int left=tXN;
			  int right=tXN+1;
			  int top=tYN;
			  int bottom=tYN+1;
			  if(left<0||right>=orw||top<0||bottom>=orh)
				  continue;

			  float rightScale=tXN-(float)left;
			  float leftScale=1.0-rightScale;
			  float bottomScale=tYN-(float)top;
			  float topScale=1.0-bottomScale;
			 /* float leftTopScale=(leftScale+topScale)/2;
			  float rightTopScale=(rightScale+topScale)/2;
			  float leftBottomScale=(leftScale+bottomScale)/2;
			  float rightBottomScale=(rightScale+bottomScale)/2;*/
			  
			  float leftTopScale=(leftScale*topScale);
			  float rightTopScale=(rightScale*topScale);
			  float leftBottomScale=(leftScale*bottomScale);
			  float rightBottomScale=(rightScale*bottomScale);
			  float zz=leftTopScale+rightTopScale+leftBottomScale+rightBottomScale;
			  float ss=1.0/zz;
			  leftTopScale*=ss;
			  rightTopScale*=ss;
			  leftBottomScale*=ss;
			  rightBottomScale*=ss;

             //值拷贝
			  data[j*nw+i]=orData[top*orw+left]*leftTopScale
				  +orData[bottom*orw+left]*leftBottomScale
				  +orData[top*orw+right]*rightTopScale
				  +orData[bottom*orw+right]*rightBottomScale;
            //  memcpy(&desBuf[i * desLineSize + j * bitCount / 8],&srcBuf[tYN * lineSize + tXN * bitCount / 8],3);            
          }
    }
	return data;
}
int in(int v,int minv,int maxv){
	if(v>maxv)
		v=maxv;
	if(v<minv)
		v=minv;
	return v;
}
void RotatePt1(float x,float y,float angle,float cx,float cy,float &resx,float &resy){
	resx = (float)(x - cx)*cos(RADIAN(angle)) - (float)(y - cy)*sin(RADIAN(angle));
	resy = (float)(x - cx)*sin(RADIAN(angle)) + (float)(y - cy)*cos(RADIAN(angle));
}
void RotatePt(float x,float y,float angle,float cx,float cy,float &resx,float &resy){
	resx = (float)(x - cx)*cos(RADIAN(angle)) - (float)(y - cy)*sin(RADIAN(angle));
	resy = (float)(x - cx)*sin(RADIAN(angle)) + (float)(y - cy)*cos(RADIAN(angle));
}
unsigned char *CopyAndRotateData(unsigned char *orData,int orw,int orh,int left,int top,int nw,int nh,float angle,int ncx,int ncy){
	unsigned char *data=new unsigned char[nw*nh];
	for (int j = 0; j < nh; j++){
		for(int i = 0; i < nw ;i++){
              //转换到以图像为中心的坐标系，并进行逆旋转
			//int cx=ncx;
			//int cy=ncy;
			float tX,tY;
			RotatePt(i,j,angle,ncx,ncy,tX,tY);
			 // float tX = (float)(i - cx)*cos(RADIAN(angle)) - (float)(j - cy)*sin(RADIAN(angle));
              //float tY = (float)(i - cx)*sin(RADIAN(angle)) + (float)(j - cy)*cos(RADIAN(angle));
              //如果这个坐标不在原图像内，则不赋值
			  //tX=0-tX;
              /*if (tX > w / 2 || tX < -w / 2 || tY > h / 2 || tY < -h / 2)
              {
                  continue;
              }*/
             //再转换到原坐标系下

              float tXN = (float)left+tX +(float)ncx; 
			  float tYN = (float)top+tY  +(float)ncy;
			  //if(tXN<0 || tYN<0){
					//delete []data;
					//return NULL;//continue;
			  //}
			 /* int left=in((int)tXN,0,orw);
			  int right=in(left+1,0,orw);
			  int top=in((int)tYN,0,orh);
			  int bottom=in(top+1,0,orh);*/

			  int left=tXN;
			  int right=left+1;
			  int top=tYN;
			  int bottom=top+1;
			 // if(left<0)
			  if(left<0||right>=orw||top<0||bottom>=orh){
				  data[j*nw+i]=128;
				  continue;
			  }

			  float rightScale=tXN-(float)left;
			  float leftScale=1.0-rightScale;
			  float bottomScale=tYN-(float)top;
			  float topScale=1.0-bottomScale;
			 /* float leftTopScale=(leftScale+topScale)/2;
			  float rightTopScale=(rightScale+topScale)/2;
			  float leftBottomScale=(leftScale+bottomScale)/2;
			  float rightBottomScale=(rightScale+bottomScale)/2;*/
			  
			  float leftTopScale=(leftScale*topScale);
			  float rightTopScale=(rightScale*topScale);
			  float leftBottomScale=(leftScale*bottomScale);
			  float rightBottomScale=(rightScale*bottomScale);
			  float zz=leftTopScale+rightTopScale+leftBottomScale+rightBottomScale;
			  float ss=1.0/zz;
			  leftTopScale*=ss;
			  rightTopScale*=ss;
			  leftBottomScale*=ss;
			  rightBottomScale*=ss;

             //值拷贝
			  data[j*nw+i]=orData[top*orw+left]*leftTopScale
				  +orData[bottom*orw+left]*leftBottomScale
				  +orData[top*orw+right]*rightTopScale
				  +orData[bottom*orw+right]*rightBottomScale;
            //  memcpy(&desBuf[i * desLineSize + j * bitCount / 8],&srcBuf[tYN * lineSize + tXN * bitCount / 8],3);            
          }
    }
	return data;
}
void CWisdomLabelDlg::OnBnClickedCreatelabel2()
{
	// TODO: 在此添加控件通知处理程序代码
	int picId=0;
	float minSize=33;
	float minl = min(m_pImg->GetWidth(),m_pImg->GetHeight());
	float m = (float)MIN_DET_SIZE / minSize;
	minl *= m;
	float pre_facetor=0.8;
	float factor = pre_facetor;
	vector<float> Scales;
	while (minl>MIN_DET_SIZE) {
		Scales.push_back(m);
		minl *= factor;
		m = m*factor;
	}
	sort(Scales.begin(),Scales.end());
	int cn=1;
	int w,h;
	unsigned char *pData=GetImgData(m_pImg,cn,w,h);
	float SetpPercent=0.1;
	for(int k=0;k<Scales.size();k++){
		float Scale=Scales.at(k);
		
		int nw=w*Scale;
		int nh=h*Scale;
		unsigned char *nd=new unsigned char[nw*nh];
		ScaleData(pData,w,h,1,nd,nw,nh);
		int Step=nw*SetpPercent;
		if(nh<nw)
			Step=nh*SetpPercent;
		for(int j=0;j<nh-MIN_DET_SIZE;j+=Step){
			for(int i=0;i<nw-MIN_DET_SIZE;i+=Step){
				unsigned char *tmp=CopyData(nd,nw,nh,i,j,MIN_DET_SIZE,MIN_DET_SIZE);
				CImage *pImg=DataToImg(tmp,1,MIN_DET_SIZE,MIN_DET_SIZE);
				CStringA Str;
				Str.Format("E:\\testtrain\\%d.jpg",picId++);
				pImg->Save(CString(Str));
				pImg->Destroy();
				delete pImg;
				delete []tmp;
			}
		}
		
		delete []nd;
	}delete []pData;
}
int IsIn(CRect BigRect,CRect MinRect){
	if(BigRect.PtInRect(MinRect.TopLeft())
		&& BigRect.PtInRect(MinRect.BottomRight())){
			return 1;
	}
	return 0;
}
float Xiangjiaobi_InMinRect(CRect BigRect,CRect MinRect){
	CRect iRect;
	if(iRect.IntersectRect(BigRect,MinRect)){
		float v= ((float)iRect.Width()*iRect.Height())/((float)MinRect.Width()*MinRect.Height());
		return v;
	}else
		return 0;

}
void CWisdomLabelDlg::OnBnClickedCreatelabel3()
{
	// TODO: 在此添加控件通知处理程序代码

	CStringA zName=GetDataDir()+"\\zpicnum.txt";
	CStringA fName=GetDataDir()+"\\fpicnum.txt";
	if(m_RectType=="玩手机"){
	}
	//else if(m_RectType=="非玩手机"){
	//}
	else{
		return;
	}
	char cPicId[32]={0};
	CFile file;
	if(file.Open(CString(zName),CFile::modeRead)){
		int Size=file.Read(cPicId,32);
		file.Close();
		cPicId[Size]=0;
	}	
	int zPicId=atoi(cPicId);
	if(file.Open(CString(fName),CFile::modeRead)){
		int Size=file.Read(cPicId,32);
		file.Close();
		cPicId[Size]=0;
	}
	int fPicId=atoi(cPicId);
	//if(PicId<=0)
	//	return;
	CRect Rc=m_GraphRect;
	float x=Rc.Width()*0.1;
	float y=Rc.Height()*0.1;
	CRect minRc(Rc.left+x,Rc.top+y,Rc.right-x,Rc.bottom-y);
	CRect MaxRc(Rc.left-x,Rc.top-y,Rc.right+x,Rc.bottom+y);
	int MaxW=max(MaxRc.Width(),MaxRc.Height());
	int MinW=max(minRc.Width(),minRc.Height());

	int cn=1;
	int w,h;
	
	unsigned char *pData=GetImgData(m_pImg,cn,w,h);
	CRect ImgRc(0,0,w,h);
	int Step=max(2.0,(MaxW-MinW)*0.08);
	for(int k=MinW;k<MaxW;k+=Step){
		for(int j=0;j<h;j+=Step){
			for(int i=0;i<w;i+=Step){
				MSG message; 
				while(PeekMessage (&message,NULL,0,0,PM_REMOVE)) 
				{ 
					TranslateMessage (&message); 
					DispatchMessage (&message); 
					if(message.message==WM_CLOSE){
						return;
					}
				} 
				CRect tmpRc(i,j,i+k,j+k);
				if(IsIn(ImgRc,tmpRc)==0){
					continue;
				}
				int SaveType=1;
				if(IsIn(tmpRc,minRc)==0){
					SaveType=0;
					if(Xiangjiaobi_InMinRect(tmpRc,minRc)<0.01 &&Xiangjiaobi_InMinRect(tmpRc,MaxRc)>0){
					}else
						continue;//不然太多
				}
				for(int a=-20;a<=20;a+=5){
					unsigned char *tmp=NULL;//CopyData(pData,w,h,i,j,k,k);
					if(a==0){
						tmp=CopyData(pData,w,h,i,j,k,k);
					}else{
						tmp=CopyAndRotateData(pData,w,h,i,j,k,k,(float)a);
					}
					if(!tmp){
						continue;
					}
					unsigned char *tmp1=new unsigned char[MIN_DET_SIZE*MIN_DET_SIZE];
					ScaleData(tmp,k,k,1,tmp1,MIN_DET_SIZE,MIN_DET_SIZE);
					delete []tmp;
					CImage *pImg=DataToImg(tmp1,1,MIN_DET_SIZE,MIN_DET_SIZE);
					if(SaveType==1){
						CStringA Str;
						zPicId++;
						if(zPicId%10000==0){
							Str.Format(GetDataDir()+"\\%s\\%d","玩手机",zPicId/10000);
							CreateDirectory(CString(Str),NULL);
						}
						Str.Format(GetDataDir()+"\\%s\\%d\\%d.jpg","玩手机",zPicId/10000,zPicId);
						pImg->Save(CString(Str));
					}else{
						CStringA Str;
						fPicId++;
						if(fPicId%10000==0){
							Str.Format(GetDataDir()+"\\%s\\%d","非玩手机",fPicId/10000);
							CreateDirectory(CString(Str),NULL);
						}
						Str.Format(GetDataDir()+"\\%s\\%d\\%d.jpg","非玩手机",fPicId/10000,fPicId);
						pImg->Save(CString(Str));
					}
					pImg->Destroy();
					delete pImg;
					delete []tmp1;
				}
				
			}
		}
	}
	CStringA Str;
	Str.Format("%d",zPicId);
	//CFile file;
	if(file.Open(CString(zName),CFile::modeCreate|CFile::modeWrite)){
		file.Write(Str,Str.GetLength());
		file.Close();
	}
	Str.Format("%d",fPicId);
	if(file.Open(CString(fName),CFile::modeCreate|CFile::modeWrite)){
		file.Write(Str,Str.GetLength());
		file.Close();
	}
	//AfxMessageBoxA(Str);
}


void CWisdomLabelDlg::OnBnClickedCreatelabel4()
{
	// TODO: 在此添加控件通知处理程序代码
	CImage img1,img2;
	img1.Load(CString("pic1.jgp"));
	img2.Load(CString("pic2.jpg"));
	int cn=1;
	int w1,h1,w2,h2;
	unsigned char *pData1=GetImgData(&img1,cn,w1,h1);
	unsigned char *pData2=GetImgData(&img2,cn,w2,h2);
	if(w1!=w2||h1!=h2){
		delete []pData1;
		delete []pData2;
		img1.Destroy();
		img2.Destroy();
		return;
	}
	int PjDis=0;
	int BSize=16;
	for(int j=0;j<w1-BSize;j++){
		for(int i=0;i<h1-BSize;i++){
			unsigned char *block1=CopyData(pData1,w1,h1,i,j,BSize,BSize);
			int minDis=1000000000;
			int minii,minjj;
			for(int jj=0;jj<w2-BSize;jj++){
				for(int ii=0;ii<h2-BSize;ii++){
					unsigned char *block2=CopyData(pData2,w2,h2,ii,jj,BSize,BSize);
					int Dis=0;
					for(int jjj=0;jjj<BSize;jjj++){
						for(int iii=0;iii<BSize;iii++){
							Dis+=block1[jjj*BSize+iii]-block2[jjj*BSize+iii];
							if(Dis<minDis){
								minDis=Dis;
								minii=ii;
								minjj=jj;
							}
						}
					}
				}
			}
			PjDis+=abs(minii-i)+abs(minjj-j);
		}
	}
	PjDis/=w1*h1;
	if(PjDis<3){
	}else{
	}
}


void CWisdomLabelDlg::OnBnClickedCreatelabel5()
{
	// TODO: 在此添加控件通知处理程序代码
	CFileDialog Dlg(TRUE,FALSE,FALSE,FALSE,FALSE,FALSE);
	if(!Dlg.DoModal()==IDOK)
		return;
	//CStringAList PicList;
	//PicList.RemoveAll();
	//CStringA PicDir=PicDir;//Dlg.GetFolderPath();
	CFileFind ff;
	int res=ff.FindFile(Dlg.GetFolderPath()+"\\*.*");
	while(res){
		res=ff.FindNextFile();
		if(ff.IsDots() || ff.IsDirectory())
			continue;
		CStringA FilePath=CStringA(ff.GetFilePath());
		//if(FilePath==m_PicPath)
		//	continue;
		CStringA FileName=CStringA(ff.GetFileName());
		CStringA FileTitle=CStringA(ff.GetFileTitle());
		CStringA hz=FilePath.Right(FileName.GetLength()-FileTitle.GetLength()-1);
		if(hz.CompareNoCase("jpg")==0
			||hz.CompareNoCase("bmp")==0
			||hz.CompareNoCase("png")==0){
				OpenNewPic(FilePath);
				MSG message; 
				while(PeekMessage (&message,NULL,0,0,PM_REMOVE)) 
				{ 
					TranslateMessage (&message); 
					DispatchMessage (&message); 
					if(message.message==WM_CLOSE){
						return;
					}
				} 
				//if(m_RectType=="玩手机"){
				OnBnClickedIdcreatedate3();
				//OnBnClickedIdcreatedate();
					//OnBnClickedCreatelabel3();
				//}
				//PicList.AddTail(FilePath);
		}
		//CStringA hz1=hz;
	}

}


void CWisdomLabelDlg::OnBnClickedDbx()
{
	// TODO: 在此添加控件通知处理程序代码
	m_OldDbx=m_Dbx;
	m_Dbx.clear();
	m_MouseState=SETDBX;
}

void CWisdomLabelDlg::OnBnClickedDbx3()
{
	// TODO: 在此添加控件通知处理程序代码
	/*m_OldDbxMin=m_DbxMin;
	m_DbxMin.clear();
	m_MouseState=SETDBXMIN;*/
	m_TmpLk[0].Pts.clear();
	m_TmpLk[1].Pts.clear();
	m_PtId=-1;
	m_MouseState=GAIBIOQIAN;
}

void CWisdomLabelDlg::OnBnClickedCsetpoint2()
{
	// TODO: 在此添加控件通知处理程序代码
	srand(GetTickCount());
	int w=m_pImg->GetWidth();
	int h=m_pImg->GetHeight();
	w-=m_GraphRect.Width()+3;
	h-=m_GraphRect.Height()+3;
	for(int i=0;i<5;i++){
		int x=rand()%w+m_GraphRect.Width()/2+1;
		int y=rand()%h+m_GraphRect.Height()/2+1;
		CPoint tPt(x,y);
		CRect Rc=m_GraphRect;
		Rc.OffsetRect(tPt-m_GraphRect.CenterPoint());
		CRect Rc1;
		if(Rc1.IntersectRect(m_GraphRect,Rc)==0)
			m_Point[i]=CPoint(x,y);
		else{
			AfxMessageBoxA("相交");
		}
	}
	ReDrawItem();
	OnBnClickedSaveAnnotation();
}


void CWisdomLabelDlg::OnBnClickedIdcreatedate()
{
	// TODO: 在此添加控件通知处理程序代码
	CStringA zName=GetDataDir()+"\\zpicnum.txt";
	//CStringA fName=GetDataDir()+"\\fpicnum.txt";
	if(m_RectType=="玩手机"){
	}
	//else if(m_RectType=="非玩手机"){
	//}
	else{
		return;
	}
	char cPicId[32]={0};
	CFile file;
	if(file.Open(CString(zName),CFile::modeRead)){
		int Size=file.Read(cPicId,32);
		file.Close();
		cPicId[Size]=0;
	}	
	int zPicId=atoi(cPicId);
	/*if(file.Open(fName,CFile::modeRead)){
		int Size=file.Read(cPicId,32);
		file.Close();
		cPicId[Size]=0;
	}
	int fPicId=atoi(cPicId);*/
	//if(PicId<=0)
	//	return;
	CRect Rc=m_GraphRect;
	float x=Rc.Width()*0.1;
	float y=Rc.Height()*0.1;
	CRect minRc(Rc.left+x,Rc.top+y,Rc.right-x,Rc.bottom-y);
	CRect rcCt=Rc;//(Rc.left+x,Rc.top+y,Rc.right-x,Rc.bottom-y);
	rcCt.DeflateRect(0.2*rcCt.Width(),0.2*rcCt.Height());
	CRect MaxRc(Rc.left-x,Rc.top-y,Rc.right+x,Rc.bottom+y);
	//CRect MaxRcCt=MaxRc;
	//MaxRcCt.DeflateRect(0.2*MaxRcCt.Width(),0.2*MaxRcCt.Height());
	int MaxW=max(MaxRc.Width(),MaxRc.Height());
	int MinW=max(minRc.Width(),minRc.Height());

	int cn=1;
	int w,h;
	
	unsigned char *pData=GetImgData(m_pImg,cn,w,h);
	CRect ImgRc(0,0,w,h);
	int Step=max(2.0,(MaxW-MinW)*0.08);
	for(int k=MinW;k<MaxW;k+=Step){
		for(int j=0;j<h;j+=Step){
			for(int i=0;i<w;i+=Step){
				MSG message; 
				while(PeekMessage (&message,NULL,0,0,PM_REMOVE)) 
				{ 
					TranslateMessage (&message); 
					DispatchMessage (&message); 
					if(message.message==WM_CLOSE){
						return;
					}
				} 
				CRect tmpRc(i,j,i+k,j+k);
				if(IsIn(ImgRc,tmpRc)==0){
					continue;
				}
				int SaveType=1;
				if(IsIn(tmpRc,minRc)==0){
					/*SaveType=0;
					if(Xiangjiaobi_InMinRect(tmpRc,minRc)<0.01 &&Xiangjiaobi_InMinRect(tmpRc,MaxRc)>0){
					}else*/
						continue;//不然太多
				}
				CRect tRcCt=tmpRc;
				tRcCt.DeflateRect(0.2*tRcCt.Width(),0.2*tRcCt.Height());
				if(IsIn(tRcCt,rcCt)==0){
					continue;
				}
				for(int a=-20;a<=20;a+=5){
					unsigned char *tmp=NULL;//CopyData(pData,w,h,i,j,k,k);
					if(a==0){
						tmp=CopyData(pData,w,h,i,j,k,k);
					}else{
						tmp=CopyAndRotateData(pData,w,h,i,j,k,k,(float)a);
					}
					if(!tmp){
						continue;
					}
					unsigned char *tmp1=new unsigned char[MIN_DET_SIZE*MIN_DET_SIZE];
					ScaleData(tmp,k,k,1,tmp1,MIN_DET_SIZE,MIN_DET_SIZE);
					delete []tmp;
					CImage *pImg=DataToImg(tmp1,1,MIN_DET_SIZE,MIN_DET_SIZE);
					if(SaveType==1){
						CStringA Str;
						zPicId++;
						if(zPicId%10000==0){
							Str.Format(GetDataDir()+"\\%s\\%d","玩手机",zPicId/10000);
							CreateDirectory(CString(Str),NULL);
						}
						Str.Format(GetDataDir()+"\\%s\\%d\\%d.jpg","玩手机",zPicId/10000,zPicId);
						pImg->Save(CString(Str));
					}
					/*else{
						CStringA Str;
						fPicId++;
						if(fPicId%10000==0){
							Str.Format(GetDataDir()+"\\%s\\%d","非玩手机",fPicId/10000);
							CreateDirectory(Str,NULL);
						}
						Str.Format(GetDataDir()+"\\%s\\%d\\%d.jpg","非玩手机",fPicId/10000,fPicId);
						pImg->Save(Str);
					}*/
					pImg->Destroy();
					delete pImg;
					delete []tmp1;
				}
				
			}
		}
	}
	CStringA Str;
	Str.Format("%d",zPicId);
	//CFile file;
	if(file.Open(CString(zName),CFile::modeCreate|CFile::modeWrite)){
		file.Write(Str,Str.GetLength());
		file.Close();
	}
	/*Str.Format("%d",fPicId);
	if(file.Open(fName,CFile::modeCreate|CFile::modeWrite)){
		file.Write(Str,Str.GetLength());
		file.Close();
	}*/
}
struct Line{
	CPoint pt1;
	CPoint pt2;
	Line(CPoint p1,CPoint p2){
		pt1=p1;
		pt2=p2;
	}
};

void CWisdomLabelDlg::OnBnClickedIdcreatedate3()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_Dbx.size()<=0)
		return;
	CDC *pDC=GetDC();
	unsigned char *pBiaoji=NULL;
	{
		vector<CPoint>BjPoint;
		vector<Line>LineList;
		for(int i=0;i<m_Dbx.size()-1;i++){
			LineList.push_back(Line(m_Dbx.at(i),m_Dbx.at(i+1)));
		}
		LineList.push_back(Line(m_Dbx.at(m_Dbx.size()-1),m_Dbx.at(0)));
		for(int k=0;k<LineList.size();k++){
			Line line=LineList.at(k);
			int x=line.pt2.x-line.pt1.x;
			int y=line.pt2.y-line.pt1.y;
			if(x==y && x==0){
				BjPoint.push_back(line.pt1);
			}else if(x==0){
				for(int j=0;j<=y;j++){
					BjPoint.push_back(CPoint (line.pt1.x,line.pt1.y+j));
				}
			}else if(y==0){
				for(int i=0;i<=x;i++){
					BjPoint.push_back(CPoint (line.pt1.x+i,line.pt1.y));
				}
			}
			else if(abs(x)>abs(y)){
				float k=(float)y/(float)x;
				if(x>0)
					for(int i=0;i<=x;i++){
						BjPoint.push_back(CPoint(line.pt1.x+i,k*(float)i+(float)line.pt1.y));
					}
				else
					for(int i=x;i<=0;i++){
						BjPoint.push_back(CPoint(line.pt1.x+i,k*(float)i+(float)line.pt1.y));
					}
			}else {
				float k=(float)x/(float)y;
				if(y>0)
					for(int j=0;j<=y;j++){
						BjPoint.push_back(CPoint(k*(float)j+(float)line.pt1.x,line.pt1.y+j));
					}
				else
					for(int j=y;j<=0;j++){
						BjPoint.push_back(CPoint(k*(float)j+(float)line.pt1.x,line.pt1.y+j));
					}
			}
		}
	
		//C2d1dCol Col(m_pImg);
		int cn=1;
		int w,h;
		unsigned char *pData=GetImgData(m_pImg,cn,w,h);
		pBiaoji=new unsigned char[w*h];
		for(int i=0;i<w*h;i++){
			pBiaoji[i]=0;
		}
		CRect Rc=DbxToRect(&m_Dbx);
		Rc.DeflateRect(-2,-2,-2,-2);
		for(int j=0;j<h;j++){
			for(int i=0;i<w;i++){
				int pos=j*w+i;
				if(!Rc.PtInRect(CPoint(i,j))){
					pBiaoji[pos]=255;
				}
			}
		}
		for(int i=0;i<BjPoint.size();i++){
			CPoint pt=BjPoint.at(i);
			int pos=pt.y*w+pt.x;
			pBiaoji[pos]=1;
		}
		vector<CPoint>AddPt;
		for(int j=0;j<h && AddPt.size()<1;j++){
			for(int i=0;i<w;i++){
				int pos=j*w+i;
				if(pBiaoji[pos]!=1&&pBiaoji[pos]!=255){
					pBiaoji[pos]=255;
					AddPt.push_back(CPoint(i,j));
					break;
				}
			}
		}
		for(;AddPt.size()>0;){
			//CPoint p8[8]={CPoint(-1,-1),CPoint(-1,0),CPoint(-1,-1),CPoint(0,-1),CPoint(1,-1),CPoint(1,0),CPoint(1,1),CPoint(0,1)};
			CPoint p8[4]={CPoint(-1,0),CPoint(0,-1),CPoint(1,0),CPoint(0,1)};
			CPoint pt=AddPt.front();
			AddPt.erase(AddPt.begin());
			for(int j=0;j<4;j++){
			
				CPoint pt1=pt+p8[j];
				if(pt1.x<0||pt1.x>=w||pt1.y<0||pt1.y>=h)
					continue;
				int pos=pt1.y*w+pt1.x;
				if(pBiaoji[pos]==1)
					continue;
				if(pBiaoji[pos]==255)
					continue;
				pBiaoji[pos]=255;
				AddPt.push_back(pt1);
			}
		}
		
		for(int i=0;i<BjPoint.size();i++){
			CPoint pt=ToTar(BjPoint.at(i));
			pDC->SetPixel(pt,RGB(255,255,0));
			
		}
		for(int j=0;j<h;j++){
			for(int i=0;i<w;i++){
				int pos=j*w+i;
				int v=pBiaoji[pos];
				if(pBiaoji[pos]<128){
					CPoint pt=ToTar(CPoint(i,j));
					pDC->SetPixel(pt,RGB(v,v,v));
				}
			}
		}
		delete []pData;
		//Sleep(10000);
		//delete []pBiaoji;
		
	}
	//return;
	srand(GetTickCount());
	CStringA zName=GetDataDir()+"\\zpicnum.txt";
	CStringA fName=GetDataDir()+"\\fpicnum.txt";
	//if(m_RectType=="玩手机"){
	//}
	////else if(m_RectType=="非玩手机"){
	////}
	//else{
	//	return;
	//}
	char cPicId[32]={0};
	CFile file;
	if(file.Open(CString(zName),CFile::modeRead)){
		int Size=file.Read(cPicId,32);
		file.Close();
		cPicId[Size]=0;
	}	
	int zPicId=atoi(cPicId);
	if(file.Open(CString(fName),CFile::modeRead)){
		int Size=file.Read(cPicId,32);
		file.Close();
		cPicId[Size]=0;
	}
	int fPicId=atoi(cPicId);
	//if(PicId<=0)
	//	return;
	CRect Rc=DbxToRect(&m_Dbx);
	float x=Rc.Width()*0.1;
	float y=Rc.Height()*0.1;
	CRect minRc(Rc.left+x,Rc.top+y,Rc.right-x,Rc.bottom-y);
	CRect MaxRc(Rc.left-2.5*x,Rc.top-2.5*y,Rc.right+2.5*x,Rc.bottom+2.5*y);
	int MaxW=max(MaxRc.Width(),MaxRc.Height());
	int MinW=max(minRc.Width(),minRc.Height());

	int cn=1;
	int w,h;
	
	unsigned char *pData=GetImgData(m_pImg,cn,w,h);
	CRect ImgRc(0,0,w,h);
	int Step1=max(1.0,(MaxW-MinW)*0.1);
	for(int k=MinW;k<MaxW;k+=Step1){
		int Step=max(1.0,0.1*(float)k);
		for(int j=0;j<h;j+=Step){
			for(int i=0;i<w;i+=Step){
				MSG message; 
				while(PeekMessage (&message,NULL,0,0,PM_REMOVE)) 
				{ 
					TranslateMessage (&message); 
					DispatchMessage (&message); 
					if(message.message==WM_CLOSE){
						return;
					}
				} 
				CRect tmpRc(i,j,i+k,j+k);
				
				CRect ReplaceRect(0,0,0,0);
				for(int k=0;k<1000;k++){
					int ii=rand()%w;
					int jj=rand()%w;
					CRect tRc(ii,jj,ii+k,jj+k);
					if(!IsIn(CRect(0,0,w,h),tRc))
						continue;
					CRect RcInsert;
					if(RcInsert.IntersectRect(tmpRc,tRc)){
					}else{
						ReplaceRect=tRc;
						break;
					}
				}
				
				if(IsIn(ImgRc,tmpRc)==0){
					continue;
				}
				//int SaveType=1;
				if(IsIn(tmpRc,minRc)==0){
					/*SaveType=0;
					if(Xiangjiaobi_InMinRect(tmpRc,minRc)<0.01 &&Xiangjiaobi_InMinRect(tmpRc,MaxRc)>0){
					}else*/
						continue;//不然太多
				}
				DrawRect1(pDC,tmpRc,RGB(255,0,255),PS_SOLID,1);
				DrawRect1(pDC,ReplaceRect,RGB(255,255,0),PS_SOLID,1);
				//Sleep(10000);
				CPoint cpt=minRc.CenterPoint()-tmpRc.TopLeft();
				for(int a=-20;a<=20;a+=5){
					unsigned char *tmp=NULL;//CopyData(pData,w,h,i,j,k,k);
					unsigned char *tmpBj=NULL;
					if(a==0){
						tmp=CopyData(pData,w,h,i,j,k,k);
						tmpBj=CopyData(pBiaoji,w,h,i,j,k,k);
					}else{
						tmp=CopyAndRotateData(pData,w,h,i,j,k,k,(float)a,cpt.x,cpt.y);
						tmpBj=CopyAndRotateData(pBiaoji,w,h,i,j,k,k,(float)a,cpt.x,cpt.y);
					}
					if(!tmp){
						continue;
					}

					unsigned char *tmp1=new unsigned char[MIN_DET_SIZE*MIN_DET_SIZE];
					unsigned char *tmpBj1=new unsigned char[MIN_DET_SIZE*MIN_DET_SIZE];
					ScaleData(tmp,k,k,1,tmp1,MIN_DET_SIZE,MIN_DET_SIZE);
					ScaleData(tmpBj,k,k,1,tmpBj1,MIN_DET_SIZE,MIN_DET_SIZE);
					CImage *pImg=DataToImg(tmp1,1,MIN_DET_SIZE,MIN_DET_SIZE);
					pImg->Draw(pDC->GetSafeHdc(),m_TarRect.left,m_TarRect.top);
					CStringA Str;
					zPicId++;
					if(zPicId%10000==0){
						Str.Format(GetDataDir()+"\\%s\\%d","玩手机",zPicId/10000);
						CreateDirectory(CString(Str),NULL);
					}
					Str.Format(GetDataDir()+"\\%s\\%d\\%d.jpg","玩手机",zPicId/10000,zPicId);
					pImg->Save(CString(Str));
					pImg->Destroy();

					if(ReplaceRect.Width()!=k){
						for(int jj=0;jj<k;jj++){
							for(int ii=0;ii<k;ii++){
								int pos=jj*k+ii;
								if(tmpBj[pos]<128){
									tmp[pos]=rand()%255;
								}
							}
						}
					}else{
						for(int jj=0;jj<k;jj++){
							for(int ii=0;ii<k;ii++){
								int pos=jj*k+ii;
								if(tmpBj[pos]<128){
									int iii=ii+ReplaceRect.left;
									int jjj=jj+ReplaceRect.top;
									tmp[pos]=pData[jjj*w+iii];
								}
							}
						}
						//for(int jj=0;jj<k;jj++){
						//	for(int ii=0;ii<k;ii++){
						//		int pos=jj*k+ii;
						//		if(tmpBj[pos]<128){
						//			CPoint pt=CPoint(ii,jj);
						//			pt.Offset(i,j);
						//			CPoint pt1=ToTar(pt);
						//			pDC->SetPixel(pt1,RGB(255,255,255));
						//		}
						//	}
						//}
					}
					//Sleep(200000);
					ScaleData(tmp,k,k,1,tmp1,MIN_DET_SIZE,MIN_DET_SIZE);
					ScaleData(tmpBj,k,k,1,tmpBj1,MIN_DET_SIZE,MIN_DET_SIZE);
					delete []tmp;
					delete []tmpBj;
					pImg=DataToImg(tmp1,1,MIN_DET_SIZE,MIN_DET_SIZE);
					fPicId++;
					if(fPicId%10000==0){
							Str.Format(GetDataDir()+"\\%s\\%d","非玩手机",fPicId/10000);
							CreateDirectory(CString(Str),NULL);
					}
					Str.Format(GetDataDir()+"\\%s\\%d\\%d.jpg","非玩手机",fPicId/10000,fPicId);
						pImg->Save(CString(Str));
					
					pImg->Destroy();
					delete pImg;
					delete []tmp1;
					delete []tmpBj1;
				}
				
			}
		}
	}
	ReleaseDC(pDC);
	delete []pData;
	delete []pBiaoji;
	CStringA Str;
	Str.Format("%d",zPicId);
	//CFile file;
	if(file.Open(CString(zName),CFile::modeCreate|CFile::modeWrite)){
		file.Write(Str,Str.GetLength());
		file.Close();
	}
	Str.Format("%d",fPicId);
	if(file.Open(CString(fName),CFile::modeCreate|CFile::modeWrite)){
		file.Write(Str,Str.GetLength());
		file.Close();
	}
}


void CWisdomLabelDlg::OnBnClickedIdcreatedate4()
{
	// TODO: 在此添加控件通知处理程序代码
	//srand(GetTickCount());
	{
		if(m_GraphRect.Width()<10 && m_GraphRect.Height()<10)
			return;
		CStringA zName=GetDataDir()+"\\zpicnum.txt";
		CStringA fName=GetDataDir()+"\\fpicnum.txt";
		
		char cPicId[32]={0};
		CFile file;
		if(file.Open(CString(zName),CFile::modeRead)){
			int Size=file.Read(cPicId,32);
			file.Close();
			cPicId[Size]=0;
		}	
		int zPicId=atoi(cPicId);
		int cn=1;
		int w,h;
		unsigned char *pData=GetImgData(m_pImg,cn,w,h);
		CDC *pDC=GetDC();
		CStringA sType="无";
		int ddd=m_GraphRect.Width()/10;
		int dd=0;
		//for(dd=-1;dd<=1;dd++)
		if(1){
			int dddd=dd*ddd;
			CRect RcZh=m_GraphRect;
			RcZh.InflateRect(dddd,dddd);
			int PyFw=RcZh.Width()/5;
			float pyStep=PyFw/5;
			for(int jj=-2;jj<=2;jj++){
				for(int ii=-2;ii<2;ii++){
					int i=ii*pyStep+RcZh.left;
					int j=jj*pyStep+RcZh.top;
					int k=RcZh.Width();
					for(int a=-10;a<=10;a+=5){
						//int a=rand()%80-40;
						MSG message; 
						while(PeekMessage (&message,NULL,0,0,PM_REMOVE)) 
						{ 
							TranslateMessage (&message); 
							DispatchMessage (&message); 
							if(message.message==WM_CLOSE){
								return;
							}
						} 

						unsigned char *tmp=NULL;//CopyData(pData,w,h,i,j,k,k);
					
		
						CPoint cpt(RcZh.Width()/2,RcZh.Height()/2);//RcZh.CenterPoint();
						float resp[14];
						vector<CPoint>resDbx;
						if(a==0){
							tmp=CopyData(pData,w,h,i,j,k,k);
						}else{
							tmp=CopyAndRotateData(pData,w,h,i,j,k,k,(float)a,cpt.x,cpt.y);
						}
						if(!tmp){
							continue;
						}

						CRect tRc=DbxToRect(&resDbx);
						resp[5*2]=tRc.TopLeft().x;
						resp[5*2+1]=tRc.TopLeft().y;
						resp[6*2]=tRc.BottomRight().x;
						resp[6*2+1]=tRc.BottomRight().y;
						unsigned char *tmp1=new unsigned char[MIN_DET_SIZE*MIN_DET_SIZE];
						//unsigned char *tmpg1=new unsigned char[MIN_DET_SIZE*MIN_DET_SIZE];
						//unsigned char *tmpBj1=new unsigned char[MIN_DET_SIZE*MIN_DET_SIZE];
						ScaleData(tmp,k,k,1,tmp1,MIN_DET_SIZE,MIN_DET_SIZE);
			
						float s=(float)MIN_DET_SIZE/(float)k;
			
						CImage *pImg=DataToImg(tmp1,1,MIN_DET_SIZE,MIN_DET_SIZE);

						pImg->Draw(pDC->GetSafeHdc(),m_TarRect.left,m_TarRect.top);

						int pos=m_PicPath.ReverseFind('\\');
						CStringA pname=m_PicPath.Mid(pos+1);
						pname=pname.Left(pname.GetLength()-4);
						zPicId++;	
						CStringA Str;
						{
							CFileStatus status;
							Str.Format(GetDataDir()+"\\%s",sType);
							if(CFile::GetStatus(CString(Str),status)==0){
								CreateDirectory(CString(Str),NULL);
							}
						}
						{
							CFileStatus status;
							Str.Format(GetDataDir()+"\\%s\\%s",sType,pname);
							if(CFile::GetStatus(CString(Str),status)==0){
								CreateDirectory(CString(Str),NULL);
							}
						}
						/*if(zPicId%10000==0){
						
							CreateDirectory(Str,NULL);
						}*/
						Str.Format(GetDataDir()+"\\%s\\%s\\%d.jpg",sType,pname,zPicId);
						pImg->Save(CString(Str));
						pImg->Destroy();
			
						if(0){
							Str.Format(GetDataDir()+"\\%s\\%s\\%d.jpg.p",sType,pname,zPicId);
							CFile f;
							if(f.Open(CString(Str),CFile::modeCreate|CFile::modeWrite|CFile::typeBinary)){
								f.Write(resp,sizeof(resp));
								f.Close();
							}
						}
		
						delete []tmp;
		
						//delete []tmpBj;
			
						delete pImg;
						delete []tmp1;
						//delete []tmpg1;
						//delete []tmpBj1;
					
					}
				}
			}
		}

		delete []pData;
		ReleaseDC(pDC);
		{
			CStringA Str;
			Str.Format("%d",zPicId);
			//CFile file;
			if(file.Open(CString(zName),CFile::modeCreate|CFile::modeWrite)){
				file.Write(Str,Str.GetLength());
				file.Close();
			}
		}
	}
	return;
	{
		if(m_GraphRect.Width()<10 && m_GraphRect.Height()<10)
			return;
		CStringA zName=GetDataDir()+"\\zpicnum.txt";
		CStringA fName=GetDataDir()+"\\fpicnum.txt";
		
		char cPicId[32]={0};
		CFile file;
		if(file.Open(CString(zName),CFile::modeRead)){
			int Size=file.Read(cPicId,32);
			file.Close();
			cPicId[Size]=0;
		}	
		int zPicId=atoi(cPicId);

		CDC *pDC=GetDC();
		CStringA sType="无";
		CRect RcZh=m_GraphRect;
		int i=RcZh.left;
		int j=RcZh.top;
		int k=RcZh.Width();
		for(int a=-40;a<=40;a++){
			//int a=rand()%80-40;
			MSG message; 
			while(PeekMessage (&message,NULL,0,0,PM_REMOVE)) 
			{ 
				TranslateMessage (&message); 
				DispatchMessage (&message); 
				if(message.message==WM_CLOSE){
					return;
				}
			} 
			int cn=1;
			int w,h;
			unsigned char *tmp=NULL;//CopyData(pData,w,h,i,j,k,k);
			unsigned char *pData=GetImgData(m_pImg,cn,w,h);
		
			CPoint cpt(RcZh.Width()/2,RcZh.Height()/2);//RcZh.CenterPoint();
			float resp[14];
			vector<CPoint>resDbx;
			if(a==0){
				tmp=CopyData(pData,w,h,i,j,k,k);
			}else{
				tmp=CopyAndRotateData(pData,w,h,i,j,k,k,(float)a,cpt.x,cpt.y);
			}
			if(!tmp){
				continue;
			}

			CRect tRc=DbxToRect(&resDbx);
			resp[5*2]=tRc.TopLeft().x;
			resp[5*2+1]=tRc.TopLeft().y;
			resp[6*2]=tRc.BottomRight().x;
			resp[6*2+1]=tRc.BottomRight().y;
			unsigned char *tmp1=new unsigned char[MIN_DET_SIZE*MIN_DET_SIZE];
			//unsigned char *tmpg1=new unsigned char[MIN_DET_SIZE*MIN_DET_SIZE];
			//unsigned char *tmpBj1=new unsigned char[MIN_DET_SIZE*MIN_DET_SIZE];
			ScaleData(tmp,k,k,1,tmp1,MIN_DET_SIZE,MIN_DET_SIZE);
			
			float s=(float)MIN_DET_SIZE/(float)k;
			
			CImage *pImg=DataToImg(tmp1,1,MIN_DET_SIZE,MIN_DET_SIZE);

			pImg->Draw(pDC->GetSafeHdc(),m_TarRect.left,m_TarRect.top);

			int pos=m_PicPath.ReverseFind('\\');
			CStringA pname=m_PicPath.Mid(pos+1);
			pname=pname.Left(pname.GetLength()-4);
			zPicId++;	
			CStringA Str;
			{
				CFileStatus status;
				Str.Format(GetDataDir()+"\\%s",sType);
				if(CFile::GetStatus(CString(Str),status)==0){
					CreateDirectory(CString(Str),NULL);
				}
			}
			{
				CFileStatus status;
				Str.Format(GetDataDir()+"\\%s\\%s",sType,pname);
				if(CFile::GetStatus(CString(Str),status)==0){
					CreateDirectory(CString(Str),NULL);
				}
			}
			/*if(zPicId%10000==0){
						
				CreateDirectory(Str,NULL);
			}*/
			Str.Format(GetDataDir()+"\\%s\\%s\\%d.jpg",sType,pname,zPicId);
			pImg->Save(CString(Str));
			pImg->Destroy();
			
			if(0){
				Str.Format(GetDataDir()+"\\%s\\%s\\%d.jpg.p",sType,pname,zPicId);
				CFile f;
				if(f.Open(CString(Str),CFile::modeCreate|CFile::modeWrite|CFile::typeBinary)){
					f.Write(resp,sizeof(resp));
					f.Close();
				}
			}
		
			delete []tmp;
		
			//delete []tmpBj;
			
			delete pImg;
			delete []tmp1;
			//delete []tmpg1;
			//delete []tmpBj1;
			delete []pData;
		}
		ReleaseDC(pDC);
		{
			CStringA Str;
			Str.Format("%d",zPicId);
			//CFile file;
			if(file.Open(CString(zName),CFile::modeCreate|CFile::modeWrite)){
				file.Write(Str,Str.GetLength());
				file.Close();
			}
		}
	}
	return;
	vector<CStringA > fList;
	CFileFind ff;
	int res=ff.FindFile(CString(m_FolderPath)+"\\*.*");
	while(res){
		res=ff.FindNextFile();
		if(ff.IsDots() || ff.IsDirectory())
			continue;
		CStringA FilePath=CStringA(ff.GetFilePath());
		//if(FilePath==m_PicPath)
		//	continue;
		CStringA FileName=CStringA(ff.GetFileName());
		CStringA FileTitle=CStringA(ff.GetFileTitle());
		CStringA hz=FilePath.Right(FileName.GetLength()-FileTitle.GetLength()-1);
		if(hz.CompareNoCase("jpg")==0
			||hz.CompareNoCase("bmp")==0
			||hz.CompareNoCase("png")==0){
				fList.push_back(CStringA(ff.GetFilePath()));
		}
	}

	if(m_Dbx.size()<=0)
		return;
	CDC *pDC=GetDC();
	unsigned char *pBiaoji=NULL;
	{
		vector<CPoint>BjPoint;
		vector<Line>LineList;
		for(int i=0;i<m_Dbx.size()-1;i++){
			LineList.push_back(Line(m_Dbx.at(i),m_Dbx.at(i+1)));
		}
		LineList.push_back(Line(m_Dbx.at(m_Dbx.size()-1),m_Dbx.at(0)));
		for(int k=0;k<LineList.size();k++){
			Line line=LineList.at(k);
			int x=line.pt2.x-line.pt1.x;
			int y=line.pt2.y-line.pt1.y;
			if(y==0 && x==0){
				BjPoint.push_back(line.pt1);
			}else if(x==0){
				if(y>=0){
					for(int j=0;j<=y;j++){
						BjPoint.push_back(CPoint (line.pt1.x,line.pt1.y+j));
					}
				}else{
					for(int j=0;j>=y;j--){
						BjPoint.push_back(CPoint (line.pt1.x,line.pt1.y+j));
					}
				}
			}else if(y==0){
				if(x>=0){
					for(int i=0;i<=x;i++){
						BjPoint.push_back(CPoint (line.pt1.x+i,line.pt1.y));
					}
				}else{
					for(int i=0;i>=x;i--){
						BjPoint.push_back(CPoint (line.pt1.x+i,line.pt1.y));
					}
				}
			}
			else if(abs(x)>abs(y)){
				float k=(float)y/(float)x;
				if(x>0)
					for(int i=0;i<=x;i++){
						BjPoint.push_back(CPoint(line.pt1.x+i,k*(float)i+(float)line.pt1.y));
					}
				else
					for(int i=x;i<=0;i++){
						BjPoint.push_back(CPoint(line.pt1.x+i,k*(float)i+(float)line.pt1.y));
					}
			}else {
				float k=(float)x/(float)y;
				if(y>0)
					for(int j=0;j<=y;j++){
						BjPoint.push_back(CPoint(k*(float)j+(float)line.pt1.x,line.pt1.y+j));
					}
				else
					for(int j=y;j<=0;j++){
						BjPoint.push_back(CPoint(k*(float)j+(float)line.pt1.x,line.pt1.y+j));
					}
			}
		}
	
		//C2d1dCol Col(m_pImg);
		int cn=1;
		int w,h;
		unsigned char *pData=GetImgData(m_pImg,cn,w,h);
		pBiaoji=new unsigned char[w*h];
		for(int i=0;i<w*h;i++){
			pBiaoji[i]=0;
		}
		CRect Rc=DbxToRect(&m_Dbx);
		Rc.DeflateRect(-2,-2,-2,-2);
		for(int j=0;j<h;j++){
			for(int i=0;i<w;i++){
				int pos=j*w+i;
				if(!Rc.PtInRect(CPoint(i,j))){
					pBiaoji[pos]=255;
				}
			}
		}
		for(int i=0;i<BjPoint.size();i++){
			CPoint pt=BjPoint.at(i);
			int pos=pt.y*w+pt.x;
			pBiaoji[pos]=1;
		}
		vector<CPoint>AddPt;
		for(int j=0;j<h && AddPt.size()<1;j++){
			for(int i=0;i<w;i++){
				int pos=j*w+i;
				if(pBiaoji[pos]!=1&&pBiaoji[pos]!=255){
					pBiaoji[pos]=255;
					AddPt.push_back(CPoint(i,j));
					break;
				}
			}
		}
		for(;AddPt.size()>0;){
			//CPoint p8[8]={CPoint(-1,-1),CPoint(-1,0),CPoint(-1,-1),CPoint(0,-1),CPoint(1,-1),CPoint(1,0),CPoint(1,1),CPoint(0,1)};
			CPoint p8[4]={CPoint(-1,0),CPoint(0,-1),CPoint(1,0),CPoint(0,1)};
			CPoint pt=AddPt.front();
			AddPt.erase(AddPt.begin());
			for(int j=0;j<4;j++){
			
				CPoint pt1=pt+p8[j];
				if(pt1.x<0||pt1.x>=w||pt1.y<0||pt1.y>=h)
					continue;
				int pos=pt1.y*w+pt1.x;
				if(pBiaoji[pos]==1)
					continue;
				if(pBiaoji[pos]==255)
					continue;
				pBiaoji[pos]=255;
				AddPt.push_back(pt1);
			}
		}
		
		for(int i=0;i<BjPoint.size();i++){
			CPoint pt=ToTar(BjPoint.at(i));
			pDC->SetPixel(pt,RGB(255,255,0));
		}
	/*	for(int j=0;j<h;j++){
			for(int i=0;i<w;i++){
				int pos=j*w+i;
				int v=pBiaoji[pos];
				if(pBiaoji[pos]<128){
					CPoint pt=ToTar(CPoint(i,j));
					pDC->SetPixel(pt,RGB(v,v,v));
				}
			}
		}*/
		delete []pData;
		//Sleep(10000);
		//delete []pBiaoji;
		
	}
	//return;
	CStringA sType;
	GetDlgItem(IDC_COMBO2)->GetWindowText(CString(sType));
	sType="无";
	//srand(GetTickCount());
	CStringA zName=GetDataDir()+"\\zpicnum.txt";
	CStringA fName=GetDataDir()+"\\fpicnum.txt";
	//if(m_RectType=="玩手机"){
	//}
	////else if(m_RectType=="非玩手机"){
	////}
	//else{
	//	return;
	//}
	char cPicId[32]={0};
	CFile file;
	if(file.Open(CString(zName),CFile::modeRead)){
		int Size=file.Read(cPicId,32);
		file.Close();
		cPicId[Size]=0;
	}	
	int zPicId=atoi(cPicId);
	//if(file.Open(fName,CFile::modeRead)){
	//	int Size=file.Read(cPicId,32);
	//	file.Close();
	//	cPicId[Size]=0;
	//}
	//int fPicId=atoi(cPicId);
	//if(PicId<=0)
	//	return;
	CRect Rc=ToZfx1(DbxToRect(&m_Dbx));
	CRect RcZh=Rc;
	float x=(float)Rc.Width()*0.05;
	float y=(float)Rc.Height()*0.05;
	CRect minRc(Rc.left+1*x,Rc.top+1*y,Rc.right-1*x,Rc.bottom-1*y);
	CRect MaxRc(Rc.left-1*x,Rc.top-1*y,Rc.right+1*x,Rc.bottom+1*y);
	int MaxW=max(MaxRc.Width(),MaxRc.Height());//(MaxRc.Width()+MaxRc.Height())/2;//
	int MinW=max(minRc.Width(),minRc.Height());///2;

	int gw=Rc.Width()*16/10;
	int gh=Rc.Height()*16/10;

	int cn=1;
	int w,h;
	
	unsigned char *pData=GetImgData(m_pImg,cn,w,h);
		/*for(int j=0;j<h;j++){
			for(int i=0;i<w;i++){
				int pos=j*w+i;
				if(pBiaoji[pos]>128){
					pData[pos]=128;
				}
			}
		}*/
	//unsigned char *pData1=new unsigned char[w*h*cn];
	//for(int k=0;k<100;k++){
	//	for(int j=0;j<h;j++){
	//		for(int i=0;i<w;i++){
	//			int pos=j*w+i;
	//			if(pBiaoji[pos]>128){
	//				int v=0;
	//				int n=0;
	//				for(int jj=-1;jj<=1;jj++){
	//					for(int ii=-1;ii<=1;ii++){
	//						int jjj=j+jj;
	//						int iii=i+ii;
	//						if(jjj<0 ||jjj>=h ||iii<0||iii>=w){
	//							continue;
	//						}
	//						int p=jjj*w+iii;
	//						v+=pData[p];
	//						n++;
	//					}
	//				}
	//				pData1[pos]=v/n;
	//			}else
	//				pData1[pos]=pData[pos];
	//		}
	//	}
	//	unsigned char *tmp=pData;
	//	pData=pData1;
	//	pData1=tmp;
	//}
	//delete []pData1;
	{
		int i=RcZh.left;
		int j=RcZh.top;
		int k=RcZh.Width();
		for(int jsq=0;jsq<27;jsq++){
			int a=rand()%80-40;
			MSG message; 
			while(PeekMessage (&message,NULL,0,0,PM_REMOVE)) 
			{ 
				TranslateMessage (&message); 
				DispatchMessage (&message); 
				if(message.message==WM_CLOSE){
					return;
				}
			} 
			unsigned char *tmp=NULL;//CopyData(pData,w,h,i,j,k,k);
			//unsigned char *tmpg=NULL;
			unsigned char *tmpBj=NULL;
			CPoint cpt(RcZh.Width()/2,RcZh.Height()/2);//RcZh.CenterPoint();
			float resp[14];
			vector<CPoint>resDbx;
			if(a==0){
				tmp=CopyData(pData,w,h,i,j,k,k);
				//tmpg=CopyData(pData,w,h,i-(gw-k)/2,j-(gh-k)/2,gw,gh);
				tmpBj=CopyData(pBiaoji,w,h,i,j,k,k);
				for(int p=0;p<5;p++){
					//RotatePt(m_Point[p].x-i,m_Point[p].y-j,-a,cpt.x,cpt.y,resp[p*2],resp[p*2+1]);
					resp[p*2]=m_Point[p].x-i;
					resp[p*2+1]=m_Point[p].y-j;
				}
				for(int p=0;p<m_Dbx.size();p++){
					CPoint pt=m_Dbx.at(p);
					float rx,ry;
					//RotatePt(pt.x-i,pt.y-j,-a,cpt.x,cpt.y,rx,ry);
					rx=pt.x-i;
					ry=pt.y-j;
					resDbx.push_back(CPoint(rx,ry));
				}
						
			}else{
				tmp=CopyAndRotateData(pData,w,h,i,j,k,k,(float)a,cpt.x,cpt.y);
				//tmpg=CopyAndRotateData(pData,w,h,i-(gw-k)/2,j-(gh-k)/2,gw,gh,(float)a,cpt.x,cpt.y);
				tmpBj=CopyAndRotateData(pBiaoji,w,h,i,j,k,k,(float)a,cpt.x,cpt.y);
						
				{

				}
				for(int p=0;p<5;p++){
					if(m_Point[0].x >0 && m_Point[0].y>0){
						RotatePt(m_Point[p].x-i,m_Point[p].y-j,-a,cpt.x,cpt.y,resp[p*2],resp[p*2+1]);
						resp[p*2]+=cpt.x;
						resp[p*2+1]+=cpt.y;
					}
					else{
						resp[p*2]=0;
						resp[p*2+1]=0;
					}
				}
				for(int p=0;p<m_Dbx.size();p++){
					CPoint pt=m_Dbx.at(p);
					float rx,ry;
					RotatePt(pt.x-i,pt.y-j,-a,cpt.x,cpt.y,rx,ry);
					rx+=cpt.x;
					ry+=cpt.y;
					resDbx.push_back(CPoint(rx,ry));
				}
			}
			if(!tmp){
				continue;
			}
			//if(a!=0)
			//{
			//	for(int r=0;r<10;r++){
			//		CStringA NewName=fList.at(rand()%fList.size());
			//		CImage NewImg;
			//		NewImg.Load(NewName);
			//		int nw=0;
			//		int nh=0;
			//		unsigned char *pNewData=GetImgData(&NewImg,1,nw,nh);
			//		if(nw<=k && nh<=k){
			//			delete []pNewData;
			//			NewImg.Destroy();
			//			continue;
			//		}
			//		int rx=rand()%(nw-k);
			//		int ry=rand()%(nh-k);
			//		
			//		//CRect Rcr(rx,ry,rx+k,ry+k);
			//		/*if(Rcr.IntersectRect(Rcr,RcZh))
			//			continue;*/
			//		for(int j=0;j<k;j++){
			//			for(int i=0;i<k;i++){
			//				if(tmpBj[j*k+i]>128)
			//					tmp[j*k+i]=pNewData[(j+ry)*nw+rx+i];//rand()%256;
			//			}
			//		}
			//		delete []pNewData;
			//		NewImg.Destroy();
			//		break;
			//	}

			//}
			CRect tRc=DbxToRect(&resDbx);
			resp[5*2]=tRc.TopLeft().x;
			resp[5*2+1]=tRc.TopLeft().y;
			resp[6*2]=tRc.BottomRight().x;
			resp[6*2+1]=tRc.BottomRight().y;
			unsigned char *tmp1=new unsigned char[MIN_DET_SIZE*MIN_DET_SIZE];
			unsigned char *tmpg1=new unsigned char[MIN_DET_SIZE*MIN_DET_SIZE];
			unsigned char *tmpBj1=new unsigned char[MIN_DET_SIZE*MIN_DET_SIZE];
			ScaleData(tmp,k,k,1,tmp1,MIN_DET_SIZE,MIN_DET_SIZE);
			//ScaleData(tmpg,gw,gh,1,tmpg1,MIN_DET_SIZE,MIN_DET_SIZE);
			ScaleData(tmpBj,k,k,1,tmpBj1,MIN_DET_SIZE,MIN_DET_SIZE);
			float s=(float)MIN_DET_SIZE/(float)k;
			for(int p=0;p<7;p++){
				resp[p*2]*=s;
				resp[p*2+1]*=s;
			}
			CImage *pImg=DataToImg(tmp1,1,MIN_DET_SIZE,MIN_DET_SIZE);
			//CImage *pImgBj=DataToImg(tmpBj1,1,MIN_DET_SIZE,MIN_DET_SIZE);
			//CImage *pImgg=DataToImg(tmpg1,1,MIN_DET_SIZE,MIN_DET_SIZE);
			pImg->Draw(pDC->GetSafeHdc(),m_TarRect.left,m_TarRect.top);
			for(int p=0;p<7;p++){
				/*CStringA Str;
				Str.Format("%d[%d,%d]",p,(int)resp[p*2],(int)resp[p*2+1]);
				pDC->TextOut(resp[p*2]+m_TarRect.left,resp[p*2+1]+m_TarRect.top,Str);*/
				pDC->SetPixel(CPoint(resp[p*2]+m_TarRect.left,resp[p*2+1]+m_TarRect.top),RGB(255,0,0));
						pDC->SetPixel(CPoint(resp[p*2]+m_TarRect.left+1,resp[p*2+1]+m_TarRect.top),RGB(255,255,0));
						pDC->SetPixel(CPoint(resp[p*2]+m_TarRect.left,resp[p*2+1]+m_TarRect.top+1),RGB(255,0,0));
						pDC->SetPixel(CPoint(resp[p*2]+m_TarRect.left+1,resp[p*2+1]+m_TarRect.top+1),RGB(255,255,0));
			}
			for(int p=0;p<7;p++){
				resp[p*2]/=(float)MIN_DET_SIZE;
				resp[p*2+1]/=(float)MIN_DET_SIZE;
			}
	/*		MSG message; 
		while(PeekMessage (&message,NULL,0,0,PM_REMOVE)) 
		{ 
			TranslateMessage (&message); 
			DispatchMessage (&message); 
			if(message.message==WM_CLOSE){
				return;
			}
		} */
			//if(a==0){
			//	for(int p=0;p<7;p++){
			//	/*CStringA Str;
			//	Str.Format("%d[%d,%d]",p,(int)resp[p*2],(int)resp[p*2+1]);
			//	pDC->TextOut(resp[p*2]+m_TarRect.left,resp[p*2+1]+m_TarRect.top,Str);*/
			//		pDC->SetPixel(CPoint(resp[p*2]+m_TarRect.left,resp[p*2+1]+m_TarRect.top),RGB(255,255,0));
			//		pDC->SetPixel(CPoint(resp[p*2]+m_TarRect.left+1,resp[p*2+1]+m_TarRect.top),RGB(255,255,0));
			//		pDC->SetPixel(CPoint(resp[p*2]+m_TarRect.left,resp[p*2+1]+m_TarRect.top+1),RGB(255,255,0));
			//		pDC->SetPixel(CPoint(resp[p*2]+m_TarRect.left+1,resp[p*2+1]+m_TarRect.top+1),RGB(255,255,0));
			//	}
			//	//Sleep(5000);
			//}
			//Sleep(1000);
			int pos=m_PicPath.ReverseFind('\\');
			CStringA pname=m_PicPath.Mid(pos+1);
			pname=pname.Left(pname.GetLength()-4);
			zPicId++;	
			CStringA Str;
			{
				CFileStatus status;
				Str.Format(GetDataDir()+"\\%s",sType);
				if(CFile::GetStatus(CString(Str),status)==0){
					CreateDirectory(CString(Str),NULL);
				}
			}
			{
				CFileStatus status;
				Str.Format(GetDataDir()+"\\%s\\%s",sType,pname);
				if(CFile::GetStatus(CString(Str),status)==0){
					CreateDirectory(CString(Str),NULL);
				}
			}
			/*if(zPicId%10000==0){
						
				CreateDirectory(Str,NULL);
			}*/
			Str.Format(GetDataDir()+"\\%s\\%s\\%d.jpg",sType,pname,zPicId);
			pImg->Save(CString(Str));
			pImg->Destroy();
			/*Str.Format(GetDataDir()+"\\%s\\%s\\%d.jpg_map_.jpg",sType,pname,zPicId);
			pImgBj->Save(Str);
			pImgBj->Destroy();*/
			//Str.Format(GetDataDir()+"\\%s\\%d\\%d_guoda.jpg",sType,zPicId/10000,zPicId);
			//pImgg->Save(Str);
			//pImgg->Destroy();
			//if(m_Point[0].x >0 && m_Point[0].y>0){
			if(0){
				Str.Format(GetDataDir()+"\\%s\\%s\\%d.jpg.p",sType,pname,zPicId);
				CFile f;
				if(f.Open(CString(Str),CFile::modeCreate|CFile::modeWrite|CFile::typeBinary)){
					f.Write(resp,sizeof(resp));
					f.Close();
				}
			}
			//}

			//if(ReplaceRect.Width()!=k){
			//	for(int jj=0;jj<k;jj++){
			//		for(int ii=0;ii<k;ii++){
			//			int pos=jj*k+ii;
			//			if(tmpBj[pos]<128){
			//				tmp[pos]=rand()%255;
			//			}
			//		}
			//	}
			//}else{
			//	for(int jj=0;jj<k;jj++){
			//		for(int ii=0;ii<k;ii++){
			//			int pos=jj*k+ii;
			//			if(tmpBj[pos]<128){
			//				int iii=ii+ReplaceRect.left;
			//				int jjj=jj+ReplaceRect.top;
			//				tmp[pos]=pData[jjj*w+iii];
			//			}
			//		}
			//	}
			//	//for(int jj=0;jj<k;jj++){
			//	//	for(int ii=0;ii<k;ii++){
			//	//		int pos=jj*k+ii;
			//	//		if(tmpBj[pos]<128){
			//	//			CPoint pt=CPoint(ii,jj);
			//	//			pt.Offset(i,j);
			//	//			CPoint pt1=ToTar(pt);
			//	//			pDC->SetPixel(pt1,RGB(255,255,255));
			//	//		}
			//	//	}
			//	//}
			//}
			//Sleep(200000);
			//ScaleData(tmp,k,k,1,tmp1,MIN_DET_SIZE,MIN_DET_SIZE);
			//ScaleData(tmpBj,k,k,1,tmpBj1,MIN_DET_SIZE,MIN_DET_SIZE);
			delete []tmp;
			//delete []tmpg;
			delete []tmpBj;
			//pImg=DataToImg(tmp1,1,MIN_DET_SIZE,MIN_DET_SIZE);
			/*fPicId++;
			if(fPicId%10000==0){
					Str.Format(GetDataDir()+"\\%s\\%d","非玩手机",fPicId/10000);
					CreateDirectory(Str,NULL);
			}
			Str.Format(GetDataDir()+"\\%s\\%d\\%d.jpg","非玩手机",fPicId/10000,fPicId);
				pImg->Save(Str);
					
			pImg->Destroy();*/
			delete pImg;
			//delete pImgBj;
			//delete pImgg;
			delete []tmp1;
			delete []tmpg1;
			delete []tmpBj1;
		}

	}
	return;
	CRect ImgRc(0,0,w,h);
	int Step1=max(1.0,(MaxW-MinW)*0.2);
	for(int k=MinW;k<MaxW;k+=Step1){
		int Step=max(1.0,0.2*(float)k);
		for(int j=-k;j<h;j+=Step){
			for(int i=-k;i<w;i+=Step){
				MSG message; 
				while(PeekMessage (&message,NULL,0,0,PM_REMOVE)) 
				{ 
					TranslateMessage (&message); 
					DispatchMessage (&message); 
					if(message.message==WM_CLOSE){
						return;
					}
				} 
				CRect tmpRc(i,j,i+k,j+k);
				
				//CRect ReplaceRect(0,0,0,0);
				/*for(int i=0;i<1000;i++){
					int ii=rand()%w;
					int jj=rand()%w;
					CRect tRc(ii,jj,ii+k,jj+k);
					if(!IsIn(CRect(0,0,w,h),tRc))
						continue;
					CRect RcInsert;
					if(RcInsert.IntersectRect(tmpRc,tRc)){
					}else{
						//ReplaceRect=tRc;
						break;
					}
				}*/
				
			/*	if(IsIn(ImgRc,tmpRc)==0){
					continue;
				}*/
				//int SaveType=1;
				if(IsIn(tmpRc,minRc)==0){
					/*SaveType=0;
					if(Xiangjiaobi_InMinRect(tmpRc,minRc)<0.01 &&Xiangjiaobi_InMinRect(tmpRc,MaxRc)>0){
					}else*/
						continue;//不然太多
				}
				CPoint pt=tmpRc.CenterPoint()-minRc.CenterPoint();
				if(abs(pt.x)>tmpRc.Width()/8)
					continue;
				if(abs(pt.y)>tmpRc.Height()/8)
					continue;
				DrawRect1(pDC,tmpRc,RGB(255,0,255),PS_SOLID,1);
				//DrawRect1(pDC,ReplaceRect,RGB(255,255,0),PS_SOLID,1);
				//Sleep(10000);
				CPoint cpt=minRc.CenterPoint()-tmpRc.TopLeft();
				for(int a=-21;a<=21;a+=3){
					unsigned char *tmp=NULL;//CopyData(pData,w,h,i,j,k,k);
					//unsigned char *tmpg=NULL;
					unsigned char *tmpBj=NULL;
					float resp[14];
					vector<CPoint>resDbx;
					if(a==0){
						tmp=CopyData(pData,w,h,i,j,k,k);
						//tmpg=CopyData(pData,w,h,i-(gw-k)/2,j-(gh-k)/2,gw,gh);
						tmpBj=CopyData(pBiaoji,w,h,i,j,k,k);
						for(int p=0;p<5;p++){
							//RotatePt(m_Point[p].x-i,m_Point[p].y-j,-a,cpt.x,cpt.y,resp[p*2],resp[p*2+1]);
							resp[p*2]=m_Point[p].x-i;
							resp[p*2+1]=m_Point[p].y-j;
						}
						for(int p=0;p<m_Dbx.size();p++){
							CPoint pt=m_Dbx.at(p);
							float rx,ry;
							//RotatePt(pt.x-i,pt.y-j,-a,cpt.x,cpt.y,rx,ry);
							rx=pt.x-i;
							ry=pt.y-j;
							resDbx.push_back(CPoint(rx,ry));
						}
						
					}else{
						tmp=CopyAndRotateData(pData,w,h,i,j,k,k,(float)a,cpt.x,cpt.y);
						//tmpg=CopyAndRotateData(pData,w,h,i-(gw-k)/2,j-(gh-k)/2,gw,gh,(float)a,cpt.x,cpt.y);
						tmpBj=CopyAndRotateData(pBiaoji,w,h,i,j,k,k,(float)a,cpt.x,cpt.y);
						
						for(int p=0;p<5;p++){
							if(m_Point[0].x >0 && m_Point[0].y>0){
								RotatePt(m_Point[p].x-i,m_Point[p].y-j,-a,cpt.x,cpt.y,resp[p*2],resp[p*2+1]);
								resp[p*2]+=cpt.x;
								resp[p*2+1]+=cpt.y;
							}
							else{
								resp[p*2]=0;
								resp[p*2+1]=0;
							}
						}
						for(int p=0;p<m_Dbx.size();p++){
							CPoint pt=m_Dbx.at(p);
							float rx,ry;
							RotatePt(pt.x-i,pt.y-j,-a,cpt.x,cpt.y,rx,ry);
							rx+=cpt.x;
							ry+=cpt.y;
							resDbx.push_back(CPoint(rx,ry));
						}
					}
					if(!tmp){
						continue;
					}
					CRect tRc=DbxToRect(&resDbx);
					resp[5*2]=tRc.TopLeft().x;
					resp[5*2+1]=tRc.TopLeft().y;
					resp[6*2]=tRc.BottomRight().x;
					resp[6*2+1]=tRc.BottomRight().y;
					unsigned char *tmp1=new unsigned char[MIN_DET_SIZE*MIN_DET_SIZE];
					unsigned char *tmpg1=new unsigned char[MIN_DET_SIZE*MIN_DET_SIZE];
					unsigned char *tmpBj1=new unsigned char[MIN_DET_SIZE*MIN_DET_SIZE];
					ScaleData(tmp,k,k,1,tmp1,MIN_DET_SIZE,MIN_DET_SIZE);
					//ScaleData(tmpg,gw,gh,1,tmpg1,MIN_DET_SIZE,MIN_DET_SIZE);
					ScaleData(tmpBj,k,k,1,tmpBj1,MIN_DET_SIZE,MIN_DET_SIZE);
					float s=(float)MIN_DET_SIZE/(float)k;
					for(int p=0;p<7;p++){
						resp[p*2]*=s;
						resp[p*2+1]*=s;
					}
					CImage *pImg=DataToImg(tmp1,1,MIN_DET_SIZE,MIN_DET_SIZE);
					//CImage *pImgBj=DataToImg(tmpBj1,1,MIN_DET_SIZE,MIN_DET_SIZE);
					//CImage *pImgg=DataToImg(tmpg1,1,MIN_DET_SIZE,MIN_DET_SIZE);
					pImg->Draw(pDC->GetSafeHdc(),m_TarRect.left,m_TarRect.top);
					for(int p=0;p<7;p++){
						/*CStringA Str;
						Str.Format("%d[%d,%d]",p,(int)resp[p*2],(int)resp[p*2+1]);
						pDC->TextOut(resp[p*2]+m_TarRect.left,resp[p*2+1]+m_TarRect.top,Str);*/
						pDC->SetPixel(CPoint(resp[p*2]+m_TarRect.left,resp[p*2+1]+m_TarRect.top),RGB(255,0,0));
								pDC->SetPixel(CPoint(resp[p*2]+m_TarRect.left+1,resp[p*2+1]+m_TarRect.top),RGB(255,255,0));
								pDC->SetPixel(CPoint(resp[p*2]+m_TarRect.left,resp[p*2+1]+m_TarRect.top+1),RGB(255,0,0));
								pDC->SetPixel(CPoint(resp[p*2]+m_TarRect.left+1,resp[p*2+1]+m_TarRect.top+1),RGB(255,255,0));
					}
					for(int p=0;p<7;p++){
						resp[p*2]/=(float)MIN_DET_SIZE;
						resp[p*2+1]/=(float)MIN_DET_SIZE;
					}
			/*		MSG message; 
				while(PeekMessage (&message,NULL,0,0,PM_REMOVE)) 
				{ 
					TranslateMessage (&message); 
					DispatchMessage (&message); 
					if(message.message==WM_CLOSE){
						return;
					}
				} */
					//if(a==0){
					//	for(int p=0;p<7;p++){
					//	/*CStringA Str;
					//	Str.Format("%d[%d,%d]",p,(int)resp[p*2],(int)resp[p*2+1]);
					//	pDC->TextOut(resp[p*2]+m_TarRect.left,resp[p*2+1]+m_TarRect.top,Str);*/
					//		pDC->SetPixel(CPoint(resp[p*2]+m_TarRect.left,resp[p*2+1]+m_TarRect.top),RGB(255,255,0));
					//		pDC->SetPixel(CPoint(resp[p*2]+m_TarRect.left+1,resp[p*2+1]+m_TarRect.top),RGB(255,255,0));
					//		pDC->SetPixel(CPoint(resp[p*2]+m_TarRect.left,resp[p*2+1]+m_TarRect.top+1),RGB(255,255,0));
					//		pDC->SetPixel(CPoint(resp[p*2]+m_TarRect.left+1,resp[p*2+1]+m_TarRect.top+1),RGB(255,255,0));
					//	}
					//	//Sleep(5000);
					//}
					//Sleep(1000);
					int pos=m_PicPath.ReverseFind('\\');
					CStringA pname=m_PicPath.Mid(pos+1);
					pname=pname.Left(pname.GetLength()-4);
					zPicId++;	
					CStringA Str;
					{
						CFileStatus status;
						Str.Format(GetDataDir()+"\\%s",sType);
						if(CFile::GetStatus(CString(Str),status)==0){
							CreateDirectory(CString(Str),NULL);
						}
					}
					{
						CFileStatus status;
						Str.Format(GetDataDir()+"\\%s\\%s",sType,pname);
						if(CFile::GetStatus(CString(Str),status)==0){
							CreateDirectory(CString(Str),NULL);
						}
					}
					/*if(zPicId%10000==0){
						
						CreateDirectory(Str,NULL);
					}*/
					Str.Format(GetDataDir()+"\\%s\\%s\\%d.jpg",sType,pname,zPicId);
					pImg->Save(CString(Str));
					pImg->Destroy();
					/*Str.Format(GetDataDir()+"\\%s\\%s\\%d.jpg_map_.jpg",sType,pname,zPicId);
					pImgBj->Save(Str);
					pImgBj->Destroy();*/
					//Str.Format(GetDataDir()+"\\%s\\%d\\%d_guoda.jpg",sType,zPicId/10000,zPicId);
					//pImgg->Save(Str);
					//pImgg->Destroy();
					//if(m_Point[0].x >0 && m_Point[0].y>0){
					if(0){
						Str.Format(GetDataDir()+"\\%s\\%s\\%d.jpg.p",sType,pname,zPicId);
						CFile f;
						if(f.Open(CString(Str),CFile::modeCreate|CFile::modeWrite|CFile::typeBinary)){
							f.Write(resp,sizeof(resp));
							f.Close();
						}
					}
					//}

					//if(ReplaceRect.Width()!=k){
					//	for(int jj=0;jj<k;jj++){
					//		for(int ii=0;ii<k;ii++){
					//			int pos=jj*k+ii;
					//			if(tmpBj[pos]<128){
					//				tmp[pos]=rand()%255;
					//			}
					//		}
					//	}
					//}else{
					//	for(int jj=0;jj<k;jj++){
					//		for(int ii=0;ii<k;ii++){
					//			int pos=jj*k+ii;
					//			if(tmpBj[pos]<128){
					//				int iii=ii+ReplaceRect.left;
					//				int jjj=jj+ReplaceRect.top;
					//				tmp[pos]=pData[jjj*w+iii];
					//			}
					//		}
					//	}
					//	//for(int jj=0;jj<k;jj++){
					//	//	for(int ii=0;ii<k;ii++){
					//	//		int pos=jj*k+ii;
					//	//		if(tmpBj[pos]<128){
					//	//			CPoint pt=CPoint(ii,jj);
					//	//			pt.Offset(i,j);
					//	//			CPoint pt1=ToTar(pt);
					//	//			pDC->SetPixel(pt1,RGB(255,255,255));
					//	//		}
					//	//	}
					//	//}
					//}
					//Sleep(200000);
					//ScaleData(tmp,k,k,1,tmp1,MIN_DET_SIZE,MIN_DET_SIZE);
					//ScaleData(tmpBj,k,k,1,tmpBj1,MIN_DET_SIZE,MIN_DET_SIZE);
					delete []tmp;
					//delete []tmpg;
					delete []tmpBj;
					//pImg=DataToImg(tmp1,1,MIN_DET_SIZE,MIN_DET_SIZE);
					/*fPicId++;
					if(fPicId%10000==0){
							Str.Format(GetDataDir()+"\\%s\\%d","非玩手机",fPicId/10000);
							CreateDirectory(Str,NULL);
					}
					Str.Format(GetDataDir()+"\\%s\\%d\\%d.jpg","非玩手机",fPicId/10000,fPicId);
						pImg->Save(Str);
					
					pImg->Destroy();*/
					delete pImg;
					//delete pImgBj;
					//delete pImgg;
					delete []tmp1;
					delete []tmpg1;
					delete []tmpBj1;
				}
				
			}
		}
	}
	ReleaseDC(pDC);
	delete []pData;
	delete []pBiaoji;
	CStringA Str;
	Str.Format("%d",zPicId);
	//CFile file;
	if(file.Open(CString(zName),CFile::modeCreate|CFile::modeWrite)){
		file.Write(Str,Str.GetLength());
		file.Close();
	}
	/*Str.Format("%d",fPicId);
	if(file.Open(fName,CFile::modeCreate|CFile::modeWrite)){
		file.Write(Str,Str.GetLength());
		file.Close();
	}*/
}


void CWisdomLabelDlg::OnBnClickedIdcreatedate5()
{
	// TODO: 在此添加控件通知处理程序代码
	//for(
	vector<Line>LineList;
	
	for(int db=0;db<m_LkList.size();db++){
			Lk lk=m_LkList.at(db);
			if(lk.LkType==TYPEDBC){
				m_Dbx.clear();
				for(int i=0;i<lk.Pts.size();i++){
					m_Dbx.push_back(lk.Pts.at(i));
				}
				for(int i=0;i<m_Dbx.size()-1;i++){
					LineList.push_back(Line(m_Dbx.at(i),m_Dbx.at(i+1)));
				}
				LineList.push_back(Line(m_Dbx.at(m_Dbx.size()-1),m_Dbx.at(0)));
				//break;
			}
			
	}
	if(m_Dbx.size()<=0)
		return;
	CStringA sType=GetWndTxtA(GetDlgItem(IDC_COMBO2));
	//->GetWindowText(sType);
	sType="无";
	CreateDirectory(CString(m_FolderPath)+"\\crop",NULL);

	//CreateDirectory(GetDataDir()+"\\crop",NULL);
	CDC *pDC=GetDC();
	unsigned char *pBiaoji=NULL;
	{
		vector<CPoint>BjPoint;
		

		for(int k=0;k<LineList.size();k++){
			Line line=LineList.at(k);
			int x=line.pt2.x-line.pt1.x;
			int y=line.pt2.y-line.pt1.y;
			if(y==0 && x==0){
				BjPoint.push_back(line.pt1);
			}else if(x==0){
				if(y>=0){
					for(int j=0;j<=y;j++){
						BjPoint.push_back(CPoint (line.pt1.x,line.pt1.y+j));
					}
				}else{
					for(int j=0;j>=y;j--){
						BjPoint.push_back(CPoint (line.pt1.x,line.pt1.y+j));
					}
				}
			}else if(y==0){
				if(x>=0){
					for(int i=0;i<=x;i++){
						BjPoint.push_back(CPoint (line.pt1.x+i,line.pt1.y));
					}
				}else{
					for(int i=0;i>=x;i--){
						BjPoint.push_back(CPoint (line.pt1.x+i,line.pt1.y));
					}
				}
			}
			else if(abs(x)>abs(y)){
				float k=(float)y/(float)x;
				if(x>0)
					for(int i=0;i<=x;i++){
						BjPoint.push_back(CPoint(line.pt1.x+i,k*(float)i+(float)line.pt1.y));
					}
				else
					for(int i=x;i<=0;i++){
						BjPoint.push_back(CPoint(line.pt1.x+i,k*(float)i+(float)line.pt1.y));
					}
			}else {
				float k=(float)x/(float)y;
				if(y>0)
					for(int j=0;j<=y;j++){
						BjPoint.push_back(CPoint(k*(float)j+(float)line.pt1.x,line.pt1.y+j));
					}
				else
					for(int j=y;j<=0;j++){
						BjPoint.push_back(CPoint(k*(float)j+(float)line.pt1.x,line.pt1.y+j));
					}
			}
		}
	
		//C2d1dCol Col(m_pImg);
		int cn=3;
		int w,h;
		unsigned char *pData=GetImgData(m_pImg,cn,w,h);
		pBiaoji=new unsigned char[w*h];
		for(int i=0;i<w*h;i++){
			pBiaoji[i]=0;
		}	
		for(int i=0;i<BjPoint.size();i++){
			CPoint pt=BjPoint.at(i);
			int pos=pt.y*w+pt.x;
			pBiaoji[pos]=1;
		}
		/*CRect Rc=DbxToRect(&m_Dbx);
		Rc.DeflateRect(-2,-2,-2,-2);
		for(int j=0;j<h;j++){
			for(int i=0;i<w;i++){
				int pos=j*w+i;
				if(!Rc.PtInRect(CPoint(i,j))){
					pBiaoji[pos]=255;
				}
			}
		}
	
		vector<CPoint>AddPt;
		//pBiaoji[0]=255;
		//AddPt.push_back(CPoint(0,0));
		for(int j=0;j<h && AddPt.size()<1;j++){
			for(int i=0;i<w;i++){
				int pos=j*w+i;
				if(pBiaoji[pos]!=1&&pBiaoji[pos]!=255){
					pBiaoji[pos]=255;
					AddPt.push_back(CPoint(i,j));
					break;
				}
			}
		}*/
		list<CPoint>AddPt;
		pBiaoji[0]=255;
		AddPt.push_back(CPoint(0,0));
		for(;AddPt.size()>0;){
			//CPoint p8[8]={CPoint(-1,-1),CPoint(-1,0),CPoint(-1,-1),CPoint(0,-1),CPoint(1,-1),CPoint(1,0),CPoint(1,1),CPoint(0,1)};
			CPoint p8[4]={CPoint(-1,0),CPoint(0,-1),CPoint(1,0),CPoint(0,1)};
			CPoint pt=AddPt.front();
			AddPt.erase(AddPt.begin());
			for(int j=0;j<4;j++){
			
				CPoint pt1=pt+p8[j];
				if(pt1.x<0||pt1.x>=w||pt1.y<0||pt1.y>=h)
					continue;
				int pos=pt1.y*w+pt1.x;
				if(pBiaoji[pos]==1)
					continue;
				if(pBiaoji[pos]==255)
					continue;
				pBiaoji[pos]=255;
				AddPt.push_back(pt1);
				CPoint pt2=ToTar(pt1);
				//pDC->SetPixel(pt2,RGB(255,0,0));
			}
		}
		
		for(int i=0;i<BjPoint.size();i++){
			CPoint pt=ToTar(BjPoint.at(i));
			pDC->SetPixel(pt,RGB(255,255,0));
		}
		for(int c=0;c<cn;c++)
		for(int j=0;j<h;j++){
			for(int i=0;i<w;i++){
				int pos=j*w+i;//c*w*h+
				int v=pBiaoji[pos];
				if(pBiaoji[pos]<128){
					
					pData[c*w*h+j*w+i]=128;//rand()%256;
					CPoint pt=ToTar(CPoint(i,j));
					pDC->SetPixel(pt,RGB(v,v,v));
				}
			}
		}
		m_pImg->Destroy();
		delete m_pImg;
		m_pImg=DataToImg(pData,cn,w,h);
		saveImg(m_PicPath,m_pImg);
		/*CImage *pImg=DataToImg(pData,1,w,h);
		int pos=m_PicPath.ReverseFind('\\');
		if(pos!=-1){
			CStringA Name1=m_PicPath.Mid(pos+1);
			CStringA Name;
			
			//Name.Format(GetDataDir()+"\\crop\\crop_%s.jpg",Name1);
			Name.Format("%s\\crop\\crop_%s.jpg",m_FolderPath,Name1);
			pImg->Save(CString(Name));
		}
		
		delete []pData;
		delete pImg;*/
		//Sleep(10000);
		//delete []pBiaoji;
		
	}

	ReleaseDC(pDC);
	delete []pBiaoji;
	m_Dbx.clear();
}


void CWisdomLabelDlg::OnBnClickedIdcreatedate6()
{
	// TODO: 在此添加控件通知处理程序代码
	OnBnClickedOpenpic5();
	CStringA StartName=m_PicPath;
	while(1){
		OnBnClickedButton8();
		OnBnClickedOpenpic5();
		if(m_PicPath==StartName)
			break;
	}
}


void CWisdomLabelDlg::OnBnClickedOpenpic5()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_pImgListCtrl.GetItemCount()==0)
		return;
	CStringA PicName1;
	int Size=m_pImgListCtrl.GetItemCount();
	for(int j=0;j<Size;j++){

		int id=j;//GetSelID();
		//if(id>=m_pImgListCtrl.GetItemCount())
		//	break;
		CStringA PicName=CStringA(m_pImgListCtrl.GetItemText(id,1));
		CStringA CropName=PicName.Left(PicName.ReverseFind('.'))+".xml";
		CFileStatus status;
		if(CFile::GetStatus(CString(CropName),status)==1){
			PicName1=PicName;
			break;
		}
	}
	if(PicName1.IsEmpty()){
		AfxMessageBoxA("没找到");
		return;
	}
	OpenNewPic(PicName1);
}


void CWisdomLabelDlg::OnBnClickedOpenpic6()
{
	// TODO: 在此添加控件通知处理程序代码
	int id=GetSelID();
	DeleteFile(CString(m_PicPath));
	DeleteFile(CString(GetXmlFileName()));
	m_pImgListCtrl.DeleteItem(id);
	if(m_pImgListCtrl.GetItemCount()>0)
		m_pImgListCtrl.SetSelectItem(id);
	
}


void CWisdomLabelDlg::OnBnClickedIdcreatedate7()
{
	 //TODO: 在此添加控件通知处理程序代码
	{
		unsigned int t=GetTickCount();
		for(int i=0;i<100000000;i++){
			float a=3.332339*2.02983;
		}
		unsigned int tt=GetTickCount()-t;
		CStringA Str;
		Str.Format("%u",tt);
		AfxMessageBoxA(Str);
	}
	{
		unsigned int t=GetTickCount();
		for(int i=0;i<100000000;i++){
			int a=302*93672;
		}
		unsigned int tt=GetTickCount()-t;
		CStringA Str;
		Str.Format("%u",tt);
		AfxMessageBoxA(Str);
	}
}


void CWisdomLabelDlg::OnBnClickedIdcreatedate8()
{
	// TODO: 在此添加控件通知处理程序代码
		CFileDialog Dlg(TRUE,FALSE,FALSE,FALSE,FALSE,FALSE);
	if(!Dlg.DoModal()==IDOK)
		return;
	CStringA Qz="sp_";
	int picn=0;
	//OpenNewPic(Dlg.GetPathName());
	char Buf[64000];
	CFileFind ff;
	CStringA FirstName;
	int res=ff.FindFile(Dlg.GetFolderPath()+"\\*.*");
	CStringA All;
	while(res){
		res=ff.FindNextFile();
		if(ff.IsDots() || ff.IsDirectory())
			continue;
		CStringA FilePath=CStringA(ff.GetFilePath());
		
		if(FirstName ==FilePath)
			break;
		//if(FilePath==m_PicPath)
		//	continue;
		CStringA FileName=CStringA(ff.GetFileName());
		CStringA FileTitle=CStringA(ff.GetFileTitle());
		CStringA hz=FilePath.Right(FileName.GetLength()-FileTitle.GetLength()-1);
		if(hz.CompareNoCase("mp4")==0){
			CStringA newName;
			//ffmpeg -i output.mp4 -r 30 -f image2 ff-%05d.jpeg
			newName.Format("ffmpeg -i \"%s\" -r 15 -f image2 %d-%s.jpg",ff.GetFilePath(),picn++,"%05d");
			All+=newName+"\r\n";
		}
	}
	//GetDlgItem(IDC_EDIT1)->SetWindowText(All);
	CFile file;
	if(file.Open(CString("e:\\test88.txt"),CFile::modeCreate|CFile::modeWrite)){
		file.Write(All,All.GetLength());
		file.Close();
	}
}


void CWisdomLabelDlg::OnBnClickedIdcreatedate9()
{
	// TODO: 在此添加控件通知处理程序代码
	OnBnClickedOpenpic5();
	CStringA StartName=m_PicPath;
	while(1){
		//OnBnClickedIdcreatedate4();
		//OnBnClickedIdcreatedate5();
		OnBnClickedIdcreatedate13();
		OnBnClickedOpenpic5();
		if(m_PicPath==StartName)
			break;
	}
}


void CWisdomLabelDlg::OnBnClickedOpenpic7()
{
	// TODO: 在此添加控件通知处理程序代码

	if(m_pImgListCtrl.GetItemCount()<=0)
		return;
	CStringA PicName1;
	int id1=-1;
	int id=GetSelID();
	int Size=m_pImgListCtrl.GetItemCount();
	for(int j=id+1;j<Size;j++){
		
		CStringA PicName=CStringA(m_pImgListCtrl.GetItemText(j,1));
		CStringA CropName=PicName+".facerect";
		CFileStatus status;
		if(CFile::GetStatus(CString(CropName),status)==1){

			CFile f;
			if(!f.Open(CString(CropName),CFile::modeRead))
				break;
			char Buf[256];
			int Size=f.Read(Buf,255);f.Close();
			//f.Close();
			Buf[Size]=0;
			CStringA Str[20];
			Str[0]=CStringA(Buf);
			for(int i=1;i<20;i++){
				int n=Str[i-1].Find(" ");
				if(n!=-1){
					Str[i]=Str[i-1].Mid(n+1);
					Str[i-1]=Str[i-1].Left(n);
				}else
					break;
			}
		
			CPoint pt[5];
			for(int i=0;i<5;i++){
				pt[i].x=atoi(Str[5+i*2]);
				pt[i].y=atoi(Str[5+i*2+1]);
			}
			if(pt[0].x>0 || pt[0].y>0 || pt[1].x>0 || pt[1].y>0){
				id1=j;
				PicName1=PicName;
				break;
			}

			
		}
	}
	if(id1>0){
		AfxMessageBoxA("没找到");
		return;
	}
	m_pImgListCtrl.SetSelectItem(id1);
	//OpenNewPic(PicName1);


}

struct FInfo{
	CStringA FileFolder;
	CStringA FilePath;
	CStringA FileTitle;
	CStringA Hz;
	CStringA FileName;
};
void CWisdomLabelDlg::OnBnClickedOpenpic8()
{
	// TODO: 在此添加控件通知处理程序代码
	/*DeleteFile(CString(m_PicPath+".facerect"));
	DeleteFile(CString(m_PicPath+".dbc"));
	DeleteFile(CString(m_PicPath+".lk"));
	DeleteFile(CString(this->GetXmlFileName()));
	m_pImgListCtrl.SetItemText(GetSelID(),2,CString(""));
	OpenNewPic(m_PicPath);*/
	vector<FInfo> files;
	CMTextDlg dlg;
	dlg.Title="xml转yolo设置";
	dlg.Size=2;
	

	dlg.Titles[0]="请输入标注的类型,多个用,隔开:";
	dlg.Titles[1]="请设置新标签保存的目录:";
	dlg.bLl[1]=2;
	dlg.TextWidth=800;
	//dlg.Titles[2]="没有以上标签的:";
	if(dlg.DoModal()!=IDOK)
		return;
	//int picn=0;//
	//OpenNewPic(Dlg.GetPathName());
	vector<CStringA>Labels=SplitStr(dlg.Texts[0],",");
	map<CStringA,int>LabMap;
	for(int k=0;k<Labels.size();k++){
		CStringA ss=Labels.at(k);
		map<CStringA,int>::iterator it=LabMap.find(ss);
		if(it!=LabMap.end()){
			AfxMessageBoxA("有重叠的标签");
			//break;
		}
		LabMap[ss]=k;
	}
	char Buf[64000];
	CFileFind ff;
	//CStringA FirstName;
	int res=ff.FindFile(CString(m_FolderPath+"\\*.*"));
	while(res){
		res=ff.FindNextFile();
		if(ff.IsDots() || ff.IsDirectory())
			continue;
		CStringA FilePath=CStringA(ff.GetFilePath());
		
		FInfo fi;
		fi.FileName=CStringA(ff.GetFileName());
		fi.FileTitle=CStringA(ff.GetFileTitle());	
		fi.FileFolder=m_FolderPath;
		fi.FilePath=CStringA(ff.GetFilePath());
		/*if("浙LD1930"==FileTitle){
			int a=3;
		}*/
		fi.Hz=FilePath.Right(fi.FileName.GetLength()-fi.FileTitle.GetLength()-1);
		
		if(fi.Hz.CompareNoCase("jpg")==0 ||fi.Hz.CompareNoCase("jpeg")==0
			||fi.Hz.CompareNoCase("JPEG")==0)
		{
				files.push_back(fi);
			
			/*	FilePath=Dlg.GetFolderPath()+"\\"+FileTitle+".xml";
				newName.Format("%s\\%s%d.xml",Dlg.GetFolderPath(),Qz,picn++);
				rename(FilePath,newName);*/
		}else{
				//DeleteFile(FilePath);
		}
		
	}
	ff.Close();
	for(int i=0;i<files.size();i++){
		FInfo fi=files.at(i);
		//picn++;
		//CStringA newName;
		////CStringA newName2;
		//while(1)
		//{
		//	newName.Format("%s\\%s%d.jpg",fi.FileFolder,Qz,picn);
		//	newName2.Format("%s%d.jpg",Qz,picn);
		//	CFileStatus status;
		//	if(CFile::GetStatus(CString(newName),status)){
		//		picn++;
		//	}else{
		//		break;
		//	}
		//}
		CStringA FilePath1,FilePath2;//=this->GetXmlFileName();
		FilePath1.Format("%s\\%s.xml",m_LabelPath,fi.FileTitle);
		FilePath2.Format("%s\\%s.txt",dlg.Texts[1],fi.FileTitle);
		
		int bOk=1;
		vector<Lk> Lks;
		CFileStatus status;
		if(CFile::GetStatus(CString(FilePath1),status))
		{
			OpenXml(FilePath1,&Lks);
			/*if(!DeleteFile(CString(FilePath1))){
				AfxMessageBoxA(FilePath1+"重命名失败,请检查是否被其他程序占用");
				bOk=0;
			}*/
		}
		if(Lks.size()>0){
			CImage img;
			img.Load(CString(fi.FilePath));
			if(!img.IsNull()){
				CFile labfile;
				if(labfile.Open(CString(FilePath2),CFile::modeCreate|CFile::modeWrite)){
					for(int k=0;k<Lks.size();k++){
						Lk lk=Lks.at(k);
						map<CStringA,int>::iterator it=LabMap.find(lk.DxType);
						if(it!=LabMap.end()){
							int itype=it->second;
							CPoint pt0=lk.Pts.at(0);
							CPoint pt1=lk.Pts.at(1);
							float x=(pt0.x+pt1.x)/2.0;
							float y=(pt0.y+pt1.y)/2.0;
							float w=pt1.x-pt0.x;
							float h=pt1.y-pt0.y;
							x/=(float)img.GetWidth();
							y/=(float)img.GetHeight();
							w/=(float)img.GetWidth();
							h/=(float)img.GetHeight();
							CStringA Str;
							Str.Format("%d %0.6f %0.6f %0.6f %0.6f\n",itype,x,y,w,h);
							labfile.Write(Str,Str.GetLength());
						}
					}
					labfile.Close();
				}
				
				//SaveXml(newName1,newName2,newName,CStringA(Dlg.GetFolderPath()),img.GetWidth(),img.GetHeight(),img.GetBPP()/8,&Lks);
				img.Destroy();
			}
		}
	}
}


void CWisdomLabelDlg::OnBnClickedOpenpic9()
{
	// TODO: 在此添加控件通知处理程序代码
	//vector<CStringA>tList;
	//while(m_PicList.GetCount()>0){
	//	tList.push_back(m_PicList.RemoveHead());
	//}
	//srand(GetTickCount());
	//random_shuffle(tList.begin(),tList.end());
	//for(int i=0;i<tList.size();i++){
	//	m_PicList.AddTail(tList.at(i));
	//}
}


void CWisdomLabelDlg::OnBnClickedIdcreatedate10()
{
	// TODO: 在此添加控件通知处理程序代码
	CFile file;
	if(!file.Open(CString("phone.bat"),CFile::modeCreate|CFile::modeWrite))
		return;
	CFileDialog Dlg(TRUE,FALSE,FALSE,FALSE,FALSE,FALSE);
	if(!Dlg.DoModal()==IDOK)
		return;
	
	CFileFind ff;
	int res=ff.FindFile(Dlg.GetFolderPath()+"\\*.*");
	while(res){
		res=ff.FindNextFile();
		if(ff.IsDots() || ff.IsDirectory())
			continue;
		CStringA FilePath=CStringA(ff.GetFilePath());

		CStringA FileName=CStringA(ff.GetFileName());
		CStringA FileTitle=CStringA(ff.GetFileTitle());
		CStringA hz=FilePath.Right(FileName.GetLength()-FileTitle.GetLength()-1);
		if(hz.CompareNoCase("jpg")==0
			||hz.CompareNoCase("bmp")==0
			||hz.CompareNoCase("png")==0){
				CStringA Str;
				Str.Format("testmtcnn.exe ./models %s 16 0.1\r\n",ff.GetFilePath());
				file.Write(Str,Str.GetLength());
		}
		//CStringA hz1=hz;
	}
	file.Close();
}


void CWisdomLabelDlg::OnBnClickedDbx2()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_DbxMin.size()<=0)
		return;
	CStringA sType;
	GetDlgItem(IDC_COMBO2)->GetWindowText(CString(sType));
	sType="无";
	CreateDirectory(CString("E:\\testData33\\非无"),NULL);
	CDC *pDC=GetDC();
	unsigned char *pBiaoji=NULL;
	{
		vector<CPoint>BjPoint;
		vector<Line>LineList;
		for(int i=0;i<m_DbxMin.size()-1;i++){
			LineList.push_back(Line(m_DbxMin.at(i),m_DbxMin.at(i+1)));
		}
		LineList.push_back(Line(m_DbxMin.at(m_DbxMin.size()-1),m_DbxMin.at(0)));
		for(int k=0;k<LineList.size();k++){
			Line line=LineList.at(k);
			int x=line.pt2.x-line.pt1.x;
			int y=line.pt2.y-line.pt1.y;
			if(y==0 && x==0){
				BjPoint.push_back(line.pt1);
			}else if(x==0){
				if(y>=0){
					for(int j=0;j<=y;j++){
						BjPoint.push_back(CPoint (line.pt1.x,line.pt1.y+j));
					}
				}else{
					for(int j=0;j>=y;j--){
						BjPoint.push_back(CPoint (line.pt1.x,line.pt1.y+j));
					}
				}
			}else if(y==0){
				if(x>=0){
					for(int i=0;i<=x;i++){
						BjPoint.push_back(CPoint (line.pt1.x+i,line.pt1.y));
					}
				}else{
					for(int i=0;i>=x;i--){
						BjPoint.push_back(CPoint (line.pt1.x+i,line.pt1.y));
					}
				}
			}
			else if(abs(x)>abs(y)){
				float k=(float)y/(float)x;
				if(x>0)
					for(int i=0;i<=x;i++){
						BjPoint.push_back(CPoint(line.pt1.x+i,k*(float)i+(float)line.pt1.y));
					}
				else
					for(int i=x;i<=0;i++){
						BjPoint.push_back(CPoint(line.pt1.x+i,k*(float)i+(float)line.pt1.y));
					}
			}else {
				float k=(float)x/(float)y;
				if(y>0)
					for(int j=0;j<=y;j++){
						BjPoint.push_back(CPoint(k*(float)j+(float)line.pt1.x,line.pt1.y+j));
					}
				else
					for(int j=y;j<=0;j++){
						BjPoint.push_back(CPoint(k*(float)j+(float)line.pt1.x,line.pt1.y+j));
					}
			}
		}
	
		//C2d1dCol Col(m_pImg);
		int cn=1;
		int w,h;
		unsigned char *pData=GetImgData(m_pImg,cn,w,h);
		pBiaoji=new unsigned char[w*h];
		for(int i=0;i<w*h;i++){
			pBiaoji[i]=0;
		}
		CRect Rc=DbxToRect(&m_DbxMin);
		Rc.DeflateRect(-2,-2,-2,-2);
		for(int j=0;j<h;j++){
			for(int i=0;i<w;i++){
				int pos=j*w+i;
				if(!Rc.PtInRect(CPoint(i,j))){
					pBiaoji[pos]=255;
				}
			}
		}
		for(int i=0;i<BjPoint.size();i++){
			CPoint pt=BjPoint.at(i);
			int pos=pt.y*w+pt.x;
			pBiaoji[pos]=1;
		}
		vector<CPoint>AddPt;
		for(int j=0;j<h && AddPt.size()<1;j++){
			for(int i=0;i<w;i++){
				int pos=j*w+i;
				if(pBiaoji[pos]!=1&&pBiaoji[pos]!=255){
					pBiaoji[pos]=255;
					AddPt.push_back(CPoint(i,j));
					break;
				}
			}
		}
		for(;AddPt.size()>0;){
			//CPoint p8[8]={CPoint(-1,-1),CPoint(-1,0),CPoint(-1,-1),CPoint(0,-1),CPoint(1,-1),CPoint(1,0),CPoint(1,1),CPoint(0,1)};
			CPoint p8[4]={CPoint(-1,0),CPoint(0,-1),CPoint(1,0),CPoint(0,1)};
			CPoint pt=AddPt.front();
			AddPt.erase(AddPt.begin());
			for(int j=0;j<4;j++){
			
				CPoint pt1=pt+p8[j];
				if(pt1.x<0||pt1.x>=w||pt1.y<0||pt1.y>=h)
					continue;
				int pos=pt1.y*w+pt1.x;
				if(pBiaoji[pos]==1)
					continue;
				if(pBiaoji[pos]==255)
					continue;
				pBiaoji[pos]=255;
				AddPt.push_back(pt1);
			}
		}
		
		for(int i=0;i<BjPoint.size();i++){
			CPoint pt=ToTar(BjPoint.at(i));
			pDC->SetPixel(pt,RGB(255,255,0));
		}
		for(int j=0;j<h;j++){
			for(int i=0;i<w;i++){
				int pos=j*w+i;
				int v=pBiaoji[pos];
				if(pBiaoji[pos]<128){
					CPoint pt=ToTar(CPoint(i,j));
					pData[j*w+i]=rand()%256;
					pDC->SetPixel(pt,RGB(v,v,v));
				}
			}
		}
		CImage *pImg=DataToImg(pData,1,w,h);
	/*	int pos=m_PicPath.ReverseFind('\\');
		if(pos!=-1){*/
		/*	CStringA Name1=m_PicPath.Mid(pos+1);
			CStringA Name;
			Name.Format("E:\\testData33\\非%s\\crop_%s",sType,Name1);*/
		DeleteFile(CString(m_PicPath));
		CStringA OldName=m_PicPath;
		m_PicPath=m_PicPath.Left(m_PicPath.GetLength()-3);
		m_PicPath+="jpg";
			pImg->Save(CString(m_PicPath));
			if(OldName!=m_PicPath){
				MoveFile(CString(OldName+".facerect"),CString(m_PicPath+".facerect"));
				MoveFile(CString(OldName+".dbc"),CString(m_PicPath+".dbc"));
			}
			OpenNewPic(m_PicPath);
			int id=GetSelID();
			m_pImgListCtrl.SetItemText(id,1,CString(m_PicPath));
			m_pImgListCtrl.SetSelectItem(-1);
			m_pImgListCtrl.SetSelectItem(id);
			
		//}
		
		delete []pData;
		delete pImg;
		//Sleep(10000);
		//delete []pBiaoji;
		
	}

	ReleaseDC(pDC);
	delete []pBiaoji;
}


void CWisdomLabelDlg::OnBnClickedOpenpic10()
{
	// TODO: 在此添加控件通知处理程序代码
	
	if(m_PicPath.Right(4)==".jpg"){
		CStringA Str;
		for(int i=0;i<100;i++){
			Str.Format("%s%d.jpg",m_PicPath.Left(m_PicPath.GetLength()-4),i);
			CFileStatus Status;
			if(CFile::GetStatus(CString(Str),Status))
				continue;
			CopyFile(CString(m_PicPath),CString(Str),0);
			int j=m_pImgListCtrl.InsertItem(GetSelID()+1,CString(""));
			m_pImgListCtrl.SetItemText(j,1,CString(Str));
			//m_PicList.AddTail(Str);
			break;
		}
	}else	if(m_PicPath.Right(4)==".png"){
		CStringA Str;
		for(int i=0;i<100;i++){
			Str.Format("%s%d.png",m_PicPath.Left(m_PicPath.GetLength()-4),i);
			CFileStatus Status;
			if(CFile::GetStatus(CString(Str),Status))
				continue;
			CopyFile(CString(m_PicPath),CString(Str),0);
			int j=m_pImgListCtrl.InsertItem(GetSelID()+1,CString(""));
			m_pImgListCtrl.SetItemText(j,1,CString(Str));
			//m_PicList.AddTail(Str);
			break;
		}
	}
}


void CWisdomLabelDlg::OnBnClickedIdcreatedate11()
{
	// TODO: 在此添加控件通知处理程序代码
	CStringA sType="非无";
	int pos=m_PicPath.ReverseFind('\\');
	CStringA pname=m_PicPath.Mid(pos+1);
	pname=pname.Left(pname.GetLength()-4);
	CStringA Str;
	{
		CFileStatus status;
		Str.Format(GetDataDir()+"\\%s",sType);
		if(CFile::GetStatus(CString(Str),status)==0){
			CreateDirectory(CString(Str),NULL);
		}
	}
	{
		CFileStatus status;
		Str.Format(GetDataDir()+"\\%s\\%s",sType,pname);
		if(CFile::GetStatus(CString(Str),status)==0){
			CreateDirectory(CString(Str),NULL);
		}
	}
			

	CRect Rc=DbxToRect(&m_Dbx);
	int cn=1;
	int w,h;
	unsigned char *pData=GetImgData(m_pImg,cn,w,h);
	int mw=min(w,h);
	int j=0;
	for(int i=0;i<10000;i++){
		int nw=max(40,rand()%mw);
		int x=rand()%(w-nw);
		int y=rand()%(h-nw);
		CRect newRc(x,y,x+nw,y+nw);
		CRect RcI;
		if(RcI.IntersectRect(Rc,newRc)){
		
		}else{
			RcI=CRect(0,0,0,0);
		}
		float ei=RcI.Width()*RcI.Height();
		float eu=Rc.Width()*Rc.Height()+newRc.Width()*newRc.Height();
		if(ei/eu<0.2){
			unsigned char * tmp=CopyData(pData,w,h,x,y,nw,nw);
			CImage *pImg=DataToImg(tmp,1,nw,nw);
			Str.Format(GetDataDir()+"\\%s\\%s\\%d_%0.3f.jpg",sType,pname,j,ei/eu);
			pImg->Save(CString(Str));
			pImg->Destroy();
			j++;
			if(j>50)
				break;
		}
	}
	delete []pData;
}



int Auto=0;


void CWisdomLabelDlg::OnBnClickedIdcreatedate12()
{
	// TODO: 在此添加控件通知处理程序代码
	CStringA Str;
	Str.Format("testmtcnn.exe D:/models %s 16 0.1",m_PicPath);
	if(Auto==0)
		WinExec(Str,SW_SHOW);
	else
		WinExec(Str,SW_HIDE);
	Sleep(500);
	CStdioFile file;
	if(file.Open(CString("test.txt"),CFile::modeRead)){
		CDC *pDC=GetDC();
		CPen pen3(PS_SOLID,2,RGB(255,255,0));
		CPen pen2(PS_SOLID,4,RGB(255,0,255));
		CPen pen1(PS_SOLID,2,RGB(0,255,255));
		CPen *pOldPen=pDC->SelectObject(&pen3);
		CStringA ss;
		while(file.ReadString(CString(ss))){
			ss.Trim();
			ss.Replace("\r","");
			ss.Replace("\n","");
			vector<CStringA>ws;
			SplitStr(&ws,ss);
			//if(ws.at(1)!="phone")
			//	continue;
			int lx=atoi(ws.at(0));
			if(lx==2){
				pDC->SelectObject(&pen2);
			}else if(lx==3){
				pDC->SelectObject(&pen3);
			}else if(lx==1){
				pDC->SelectObject(&pen1);
			}
			CRect Rc(atoi(ws.at(4)),atoi(ws.at(5)),atoi(ws.at(6)),atoi(ws.at(7)));
			pDC->MoveTo(ToTar(Rc.TopLeft()));
			pDC->LineTo(ToTar(BottomLeft(Rc)));
			pDC->LineTo(ToTar(Rc.BottomRight()));
			pDC->LineTo(ToTar(TopRight(Rc)));
			pDC->LineTo(ToTar(Rc.TopLeft()));
			CRect Rc1=ToTar(Rc);
			float fs=atof(ws.at(2));
			fs*=100;
			CStringA sfs;
			sfs.Format("%d",(int)fs);
			if(atoi(ws.at(0))==2){
				pDC->TextOut(Rc1.left,Rc1.top,CString(sfs));
			}
			else{
				pDC->TextOut(Rc1.left,Rc1.bottom,CString(sfs));
			}
		}
		pDC->SelectObject(pOldPen);
		ReleaseDC(pDC);
		file.Close();
	}
}


void CWisdomLabelDlg::OnBnClickedIdcreatedate13()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_Dbx.size()<=0){
		if(m_GraphRect.left>=0 && m_GraphRect.right>0){
			m_Dbx.push_back(m_GraphRect.TopLeft());
			m_Dbx.push_back(BottomLeft(m_GraphRect));
			m_Dbx.push_back(m_GraphRect.BottomRight());
			m_Dbx.push_back(TopRight(m_GraphRect));
		}else
			return;
	}
	CStringA sType="非无";
	int pos=m_PicPath.ReverseFind('\\');
	CStringA pname=m_PicPath.Mid(pos+1);
	pname=pname.Left(pname.GetLength()-4);
	CStringA Str;
	
	{
		CFileStatus status;
		Str.Format(GetDataDir()+"\\%s",sType);
		if(CFile::GetStatus(CString(Str),status)==0){
			CreateDirectory(CString(Str),NULL);
		}
	}
	{
		CFileStatus status;
		Str.Format(GetDataDir()+"\\%s\\%s",sType,pname);
		if(CFile::GetStatus(CString(Str),status)==0){
			CreateDirectory(CString(Str),NULL);
		}
	}

	int cn=1;
	int w,h;
	int rwh=66;
	unsigned char *pData=GetImgData(m_pImg,cn,w,h);
	unsigned char * tmp2=new unsigned char[rwh*rwh];
	unsigned char *pBiaoji=new unsigned char[w*h];
	int pBiaojiMj=0;
	for(int i=0;i<w*h;i++){
		pBiaoji[i]=0;
	}
	{
		vector<CPoint>BjPoint;
		vector<Line>LineList;
		for(int i=0;i<m_Dbx.size()-1;i++){
			LineList.push_back(Line(m_Dbx.at(i),m_Dbx.at(i+1)));
		}
		LineList.push_back(Line(m_Dbx.at(m_Dbx.size()-1),m_Dbx.at(0)));
		for(int k=0;k<LineList.size();k++){
			Line line=LineList.at(k);
			int x=line.pt2.x-line.pt1.x;
			int y=line.pt2.y-line.pt1.y;
			if(y==0 && x==0){
				BjPoint.push_back(line.pt1);
			}else if(x==0){
				if(y>=0){
					for(int j=0;j<=y;j++){
						BjPoint.push_back(CPoint (line.pt1.x,line.pt1.y+j));
					}
				}else{
					for(int j=0;j>=y;j--){
						BjPoint.push_back(CPoint (line.pt1.x,line.pt1.y+j));
					}
				}
			}else if(y==0){
				if(x>=0){
					for(int i=0;i<=x;i++){
						BjPoint.push_back(CPoint (line.pt1.x+i,line.pt1.y));
					}
				}else{
					for(int i=0;i>=x;i--){
						BjPoint.push_back(CPoint (line.pt1.x+i,line.pt1.y));
					}
				}
			}
			else if(abs(x)>abs(y)){
				float k=(float)y/(float)x;
				if(x>0)
					for(int i=0;i<=x;i++){
						BjPoint.push_back(CPoint(line.pt1.x+i,k*(float)i+(float)line.pt1.y));
					}
				else
					for(int i=x;i<=0;i++){
						BjPoint.push_back(CPoint(line.pt1.x+i,k*(float)i+(float)line.pt1.y));
					}
			}else {
				float k=(float)x/(float)y;
				if(y>0)
					for(int j=0;j<=y;j++){
						BjPoint.push_back(CPoint(k*(float)j+(float)line.pt1.x,line.pt1.y+j));
					}
				else
					for(int j=y;j<=0;j++){
						BjPoint.push_back(CPoint(k*(float)j+(float)line.pt1.x,line.pt1.y+j));
					}
			}
		}
	
		//C2d1dCol Col(m_pImg);
		
		CRect Rc=DbxToRect(&m_Dbx);
		Rc.DeflateRect(-2,-2,-2,-2);
		for(int j=0;j<h;j++){
			for(int i=0;i<w;i++){
				int pos=j*w+i;
				if(!Rc.PtInRect(CPoint(i,j))){
					pBiaoji[pos]=255;
				}
			}
		}
		for(int i=0;i<BjPoint.size();i++){
			CPoint pt=BjPoint.at(i);
			if(pt.x<0||pt.x>=w)
				continue;
			if(pt.y<0||pt.y>=h)
				continue;
			int pos=pt.y*w+pt.x;
			pBiaoji[pos]=1;
		}
		vector<CPoint>AddPt;
		for(int j=0;j<h && AddPt.size()<1;j++){
			for(int i=0;i<w;i++){
				int pos=j*w+i;
				if(pBiaoji[pos]!=1&&pBiaoji[pos]!=255){
					pBiaoji[pos]=255;
					AddPt.push_back(CPoint(i,j));
					break;
				}
			}
		}
		for(;AddPt.size()>0;){
			//CPoint p8[8]={CPoint(-1,-1),CPoint(-1,0),CPoint(-1,-1),CPoint(0,-1),CPoint(1,-1),CPoint(1,0),CPoint(1,1),CPoint(0,1)};
			CPoint p8[4]={CPoint(-1,0),CPoint(0,-1),CPoint(1,0),CPoint(0,1)};
			CPoint pt=AddPt.front();
			AddPt.erase(AddPt.begin());
			for(int j=0;j<4;j++){
			
				CPoint pt1=pt+p8[j];
				if(pt1.x<0||pt1.x>=w||pt1.y<0||pt1.y>=h)
					continue;
				int pos=pt1.y*w+pt1.x;
				if(pBiaoji[pos]==1)
					continue;
				if(pBiaoji[pos]==255)
					continue;
				pBiaoji[pos]=255;
				AddPt.push_back(pt1);
			}
		}
		
		//for(int i=0;i<BjPoint.size();i++){
		//	CPoint pt=ToTar(BjPoint.at(i));
		//	pDC->SetPixel(pt,RGB(255,255,0));
		//}

		//CDC *pDC=GetDC();
		//for(int j=0;j<h;j++){
		//	for(int i=0;i<w;i++){
		//		int pos=j*w+i;
		//		int v=pBiaoji[pos];
		//		if(pBiaoji[pos]<128){
		//			CPoint pt=ToTar(CPoint(i,j));
		//			pData[j*w+i]=255;//rand()%256;
		//			pDC->SetPixel(pt,RGB(v,v,v));
		//		}
		//	}
		//}
		//ReleaseDC(pDC);
		//CImage *pImg=DataToImg(pData,1,w,h);
		//int pos=m_PicPath.ReverseFind('\\');
		//if(pos!=-1){
		//	CStringA Name1=m_PicPath.Mid(pos+1);
		//	CStringA Name;
		//	
		//	Name.Format(GetDataDir()+"\\crop\\crop_%s.jpg",Name1);
		//	//Name.Format("%s\\crop\\crop_%s.jpg",m_FolderPath,Name1);
		//	pImg->Save(Name);
		//}
		//
		//delete []pData;
		//delete pImg;
		//Sleep(10000);
		//delete []pBiaoji;
		
	}
	CDC *pDC=GetDC();
	for(int j=0;j<h;j++){
		for(int i=0;i<w;i++){
			int pos=j*w+i;
			int v=pBiaoji[pos];
			if(pBiaoji[pos]<128){
				pBiaojiMj++;
				//CPoint pt=ToTar(CPoint(i,j));
				//pData[j*w+i]=rand()%256;
				//pDC->SetPixel(pt,RGB(v,v,v));
			}
		}
	}
	int j=0;
	int mw=min(w,h);
	for(int i=0;i<100000;i++){
		if(j%20==0)
			ReDrawItem();
		MSG message; 
		while(PeekMessage (&message,NULL,0,0,PM_REMOVE)) 
		{ 
			TranslateMessage (&message); 
			DispatchMessage (&message); 
			if(message.message==WM_CLOSE){
				return;
			}
		} 
		int nw=rand()%mw;
		nw=nw/2*2;
		if(nw<10||nw>=mw){
			i--;
			continue;
		}
		int x=rand()%(w-nw);
		int y=rand()%(h-nw);
		if(x<0||x>=w-nw||y<0||y>=h-nw){
			i--;
			continue;
		}
	/*	x=366;
		y=198;
		nw=216;*/
		CRect newRc(x,y,x+nw,y+nw);
		int ei=0;
		for(int yy=newRc.top;yy<newRc.bottom;yy++){
			for(int xx=newRc.left;xx<newRc.right;xx++){
				int pos=yy*w+xx;
				//int v=pBiaoji[pos];
				if(pBiaoji[pos]<128){
					//pData[yy*w+xx]=255;
					ei++;
				}
			}
		}
		float eu=pBiaojiMj+newRc.Width()*newRc.Height();
		float es=newRc.Width()*newRc.Height();
		if(ei*100/eu<15 ){//&& ei*100/es<20
			DrawRect1(pDC,ToZfx1(CRect(x,y,x+nw,y+nw)),RGB(rand()%256,rand()%256,rand()%256),PS_DASH,1);
			unsigned char * tmp1=CopyData(pData,w,h,x,y,nw,nw);
			ScaleData(tmp1,nw,nw,1,tmp2,rwh,rwh);
			CImage *pImg=DataToImg(tmp2,1,rwh,rwh);
			if(pImg==NULL)
				continue;
			//pImg->Draw(pDC->GetSafeHdc(),m_TarRect.left,m_TarRect.top);
			
			CStringA Str;
			Str.Format(GetDataDir()+"\\%s\\%s\\%0.3f_%d.jpg",sType,pname,(float)ei/(float)eu,j);
			pImg->Save(CString(Str));
			pImg->Destroy();
			delete pImg;
			delete []tmp1;
			j++;
			if(j>=200)
				break;
		}
		//break;
	}
	ReleaseDC(pDC);
	delete []pData;
	delete []tmp2;
	delete []pBiaoji;
}


CStringA CWisdomLabelDlg::GetDataDir(void)
{
	CStringA Str;
	GetDlgItem(IDC_EDIT1)->GetWindowText(CString(Str));
	CreateDirectory(CString(Str),NULL);
	CreateDirectory(CString(Str)+"\\无",NULL);
	CreateDirectory(CString(Str)+"\\非无",NULL);
	return Str;
	return CStringA();
}

CStringA LcHz="_gdlc.jpg";
CStringA LcName="";
void CWisdomLabelDlg::OnBnClickedOpenpic11()
{
	// TODO: 在此添加控件通知处理程序代码
	CStringA dName=m_PicName;
	if(dName.Right(4)==".txt")
		dName=dName.Left(dName.GetLength()-4)+".jpg";
	if(dName.Right(4)==".jpg")
		dName=dName.Left(dName.GetLength()-4)+ LcHz;
	CFileDialog Dlg(FALSE,NULL,NULL,OFN_CREATEPROMPT | OFN_PATHMUSTEXIST ,NULL);
	CStringA defaultFileName = dName;  
	//Dlg.m_ofn.lpstrInitialDir=m_PicPath.GetBuffer(MAX_PATH);
	m_PicPath.ReleaseBuffer();
	Dlg.m_ofn.lpstrFile = CString(defaultFileName).GetBuffer(MAX_PATH);  
	Dlg.m_ofn.nMaxFile = MAX_PATH;  
	if(Dlg.DoModal()==IDOK){
		CStringA FileName=CStringA(Dlg.GetPathName());
		CFileStatus status;
		if(CFile::GetStatus(CString(FileName),status)){
			AfxMessageBoxA("文件已经存在");
			return;
		}
		saveImg(m_PicPath,m_pImg);
		CStringA FileName1=m_LabelPath+"\\"+CStringA(Dlg.GetFileTitle());//FileName.Left(FileName.ReverseFind('.'));
		FileName1+=".xml";
		SaveXml(FileName1,CStringA(Dlg.GetFileName()),CStringA(FileName),CStringA(Dlg.GetFolderPath()),m_pImg->GetWidth(),m_pImg->GetHeight(),m_pImg->GetBPP()/8,&m_LkList);
		//CStringA j1Str=m_pImgListCtrl.GetItemText(GetSelID()+1,
		//CFileStatus status;
		//if(!CFile::GetStatus(FileName,status)){
			int j=m_pImgListCtrl.InsertItem(GetSelID()+1,Dlg.GetFileName());
			m_pImgListCtrl.SetItemText(j,1,CString(FileName));
			m_pImgListCtrl.SetSelectItem(j);
			//AfxMessageBoxA("已存在");
			//return;
		//}

		//OpenNewPic(Dlg.GetPathName());
		//AfxMessageBoxA("另存成功");
		//CopyFile(m_PicPath,Dlg.GetPathName(),0);
	}

}


void CWisdomLabelDlg::OnBnClickedOpenpic12()
{
	// TODO: 在此添加控件通知处理程序代码
		CFileDialog Dlg(TRUE,FALSE,FALSE,FALSE,FALSE,FALSE);
	if(!Dlg.DoModal()==IDOK)
		return;
	OpenNewPic(CStringA(Dlg.GetPathName()));
	m_FolderPath=Dlg.GetFolderPath();
	//m_PicList.RemoveAll();
	//CStringA PicDir=PicDir;//Dlg.GetFolderPath();
	CFileFind ff;
	int res=ff.FindFile(Dlg.GetFolderPath()+"\\*.*");
	while(res){
		res=ff.FindNextFile();
		if(ff.IsDots() || ff.IsDirectory())
			continue;
		CStringA FilePath=CStringA(ff.GetFilePath());
		//if(FilePath==m_PicPath)
		//	continue;
		CStringA FileName=CStringA(ff.GetFileName());
		CStringA FileTitle=CStringA(ff.GetFileTitle());
		CStringA hz=FilePath.Right(FileName.GetLength()-FileTitle.GetLength()-1);
		if(hz.CompareNoCase("jpg")==0
			||hz.CompareNoCase("bmp")==0
			||hz.CompareNoCase("png")==0){
				int j=m_pImgListCtrl.InsertItem(m_pImgListCtrl.GetItemCount(),CString(FileName));
				m_pImgListCtrl.SetItemText(j,1,CString(FilePath));
				//m_PicList.AddTail(FilePath);
		}
		//CStringA hz1=hz;
	}
}


void CWisdomLabelDlg::OnBnClickedIdcreatedate14()
{
	// TODO: 在此添加控件通知处理程序代码
	CStringA newPath=this->m_PicPath;//
	if(newPath.Find("_mohu_")==-1){
		newPath=this->m_FolderPath+"\\"+this->m_PicTitle+"_mohu_.jpg";
		int j=m_pImgListCtrl.InsertItem(m_pImgListCtrl.GetItemCount(),CString(m_PicName));
		m_pImgListCtrl.SetItemText(j,1,CString(newPath));
		//m_PicList.AddHead(newPath);
	}
	m_pImg->Save(CString(newPath));
	
}


void CWisdomLabelDlg::OnBnClickedIdcreatedate15()
{
	// TODO: 在此添加控件通知处理程序代码
	OnBnClickedOpenpic2();
	CStringA StartName=m_PicPath;
	while(1){
		//OnBnClickedIdcreatedate4();
		//OnBnClickedIdcreatedate5();
		OnBnClickedIdcreatedate14();
		OnBnClickedOpenpic2();
		if(m_PicPath==StartName)
			break;
	}
}


void CWisdomLabelDlg::OnBnClickedIdcreatedate16()
{
	// TODO: 在此添加控件通知处理程序代码
	OnBnClickedOpenpic5();
	CStringA StartName=m_PicPath;
	while(1){
		//OnBnClickedIdcreatedate4();
		//OnBnClickedIdcreatedate5();
		OnBnClickedIdcreatedate5();
		OnBnClickedOpenpic5();
		if(m_PicPath==StartName)
			break;
	}
}


void CWisdomLabelDlg::OnBnClickedIdcreatedate17()
{
	// TODO: 在此添加控件通知处理程序代码
	OnBnClickedOpenpic2();
	CStringA StartName=m_PicPath;
	Auto=1;
	while(1){
		//OnBnClickedIdcreatedate4();
		//OnBnClickedIdcreatedate5();
		MSG message; 
		while(PeekMessage (&message,NULL,0,0,PM_REMOVE)) 
		{ 
			TranslateMessage (&message); 
			DispatchMessage (&message); 
			if(message.message==WM_CLOSE){
				return;
			}
		} 
		OnBnClickedIdcreatedate12();
		OnBnClickedOpenpic2();
		if(m_PicPath==StartName)
			break;
		Sleep(50);
	}
	Auto=0;
}


void CWisdomLabelDlg::OnBnClickedIdcreatedate19()
{
	// TODO: 在此添加控件通知处理程序代码
	int cn=1;
	int w,h;
	int kw=3;
	int kh=3;
	float k0[3][3]={
		{1,1,1},
		{0,0,0},
		{-1,-1,-1}
	};
	float k1[3][3]={
		{-1,0,1},
		{-1,0,1},
		{-1,0,1}
	};
	unsigned char *pData=GetImgData(m_pImg,cn,w,h);
	unsigned int *pData1=new unsigned int[w*h];
	memset(pData1,128,w*h);
	for(int j=1;j<h-1;j++){
		for(int i=1;i<w-1;i++){
			int v=0;
			for(int kj=0;kj<kh;kj++){
				for(int ki=0;ki<kw;ki++){
					int jj=j+kj;
					int ii=i+ki;
					int pos=jj*w+ii;
					v+=pData[pos]*k0[kj][ki]+pData[pos]*k1[kj][ki];
				}
			}
			//v/=16;
			int pos=j*w+i+w+1;
			if(abs(v)<30)
				pData1[pos]=128;
			else
				pData1[pos]=abs(v)+128;
		}
	}
	m_pImg->Destroy();
	delete m_pImg;
	m_pImg=DataToImg(pData1,cn,w,h);
	delete []pData;
	delete []pData1;
	ReDrawItem();
}


void CWisdomLabelDlg::OnBnClickedIdcreatedate20()
{
	// TODO: 在此添加控件通知处理程序代码
}


void CWisdomLabelDlg::OnFgTpX()
{
	// TODO: 在此添加控件通知处理程序代码
	m_MouseState=FGPICX;
}


void CWisdomLabelDlg::OnFgTpY()
{
	// TODO: 在此添加控件通知处理程序代码
	m_MouseState=FGPICY;
}


void CWisdomLabelDlg::OnBnClickedClk()
{
	// TODO: 在此添加控件通知处理程序代码
	//m_TmpDbx.clear();
	//m_OldDbx=m_Dbx;
	//m_Dbx.clear();
	m_TmpLk[0].LkType=TYPESPLINE;
	m_TmpLk[1].LkType=TYPESPLINE;
	m_TmpLk[0].Pts.clear();
	m_TmpLk[1].Pts.clear();
	m_MouseState=SETLK;
}

void sort1(float*a,int len){
	int i,j,t;
	for(i=0;i<len;i++){
		for(j=0;j<len-i-1;j++){
			if(a[j]>a[j+1]){
				t=a[j];
				a[j]=a[j+1];
				a[j+1]=t;
			}
		}
	}
}
void CWisdomLabelDlg::OnBnClickedRuihua()
{
	// TODO: 在此添加控件通知处理程序代码
	int cn=3;
	int w,h;
	int kw=3;
	int kh=3;
	float k0[3][3]={
		{-0.1,-0.1,-0.1},
		{-0.1,1.8,-0.1},
		{-0.1,-0.1,-0.1}
	};
	//float k1[3][3]={
	//	{-1,0,1},
	//	{-1,0,1},
	//	{-1,0,1}
	//};
	unsigned char *pData=GetImgData(m_pImg,cn,w,h);
	unsigned char *pData1=new unsigned char[w*h*cn];
	//memset(pData1,128,w*h*cn);
	memcpy(pData1,pData,w*h*cn);
	for(int c=0;c<cn;c++){
		for(int j=1;j<h-1;j++){
			for(int i=1;i<w-1;i++){
				/*float tmp[9]={0};
				for(int kj=0;kj<kh;kj++){
					for(int ki=0;ki<kw;ki++){
						int jj=j+kj-1;
						int ii=i+ki-1;
						int pos=c*w*h+jj*w+ii;
						tmp[kj*kw+ki]=pData[pos];
					}
				}
				//v/=16;
				//sort(tmp.begin(),tmp.end());
				sort1(tmp,9);*/
				int pos=c*w*h+j*w+i;
				//if(tmp[0]==pData[pos]||tmp[8]==pData[pos])
				//	continue;
				float v=0;
				for(int kj=0;kj<kh;kj++){
					for(int ki=0;ki<kw;ki++){
						int jj=j+kj-1;
						int ii=i+ki-1;
						int pos=c*w*h+jj*w+ii;
						v+=(float)pData[pos]*k0[kj][ki];//+pData[pos]*k1[kj][ki];
					}
				}
				//v/=16;
				
				/*if(abs(v)<30)
					pData1[pos]=128;
				else*/
					pData1[pos]=UcXz(0.5+v);//abs(v)+128;
			}
		}
	}
	m_pImg->Destroy();
	delete m_pImg;
	m_pImg=DataToImg(pData1,cn,w,h);
	delete []pData;
	delete []pData1;
	ReDrawItem();
}


void CWisdomLabelDlg::OnBnClickedZzlb()
{
	// TODO: 在此添加控件通知处理程序代码
	int cn=3;
	int w,h;
	int kw=3;
	int kh=3;
	//float k0[3][3]={
	//	{-0.1,-0.1,-0.1},
	//	{-0.1,1.8,-0.1},
	//	{-0.1,-0.1,-0.1}
	//};
	//float k1[3][3]={
	//	{-1,0,1},
	//	{-1,0,1},
	//	{-1,0,1}
	//};
	unsigned char *pData=GetImgData(m_pImg,cn,w,h);
	unsigned char *pData1=new unsigned char[w*h*cn];
	memcpy(pData1,pData,w*h*cn);
	//memset(pData1,128,w*h*cn);
	for(int j=0;j<h-2;j++){
		for(int i=0;i<w-2;i++){
			for(int k=0;k<cn;k++){
				//vector<unsigned char> tmp;
				float tmp[9]={0};
				for(int kj=0;kj<kh;kj++){
					for(int ki=0;ki<kw;ki++){
						int jj=j+kj;
						int ii=i+ki;
						int pos=k*w*h+jj*w+ii;
						tmp[kj*kw+ki]=pData[pos];
					}
				}
				//v/=16;
				//sort(tmp.begin(),tmp.end());
				sort1(tmp,9);
				int pos=k*w*h+j*w+i+w+1;
				/*if(abs(v)<30)
					pData1[pos]=128;
				else*/
				pData1[pos]=tmp[4];//.at(tmp.size()/2);//abs(v)+128;
			}
		}
	}
	m_pImg->Destroy();
	delete m_pImg;
	m_pImg=DataToImg(pData1,cn,w,h);
	delete []pData;
	delete []pData1;
	ReDrawItem();
}


void CWisdomLabelDlg::OnBnClickedZzlb2()
{
	// TODO: 在此添加控件通知处理程序代码
	int cn=3;
	int w,h;
	int kw=3;
	int kh=3;
	//float k0[3][3]={
	//	{-0.1,-0.1,-0.1},
	//	{-0.1,1.8,-0.1},
	//	{-0.1,-0.1,-0.1}
	//};
	//float k1[3][3]={
	//	{-1,0,1},
	//	{-1,0,1},
	//	{-1,0,1}
	//};
	unsigned char *pData=GetImgData(m_pImg,cn,w,h);
	unsigned char *pData1=new unsigned char[w*h*cn];
	memcpy(pData1,pData,w*h*cn);
	//memset(pData1,128,w*h);
	for(int c=0;c<cn;c++)
	for(int j=1;j<h-1;j++){
		for(int i=1;i<w-1;i++){
			float v=0;
			//vector<unsigned char> tmp;
			int jh=kh/2;
			int ih=kw/2;
			for(int kj=0;kj<kh;kj++){
				for(int ki=0;ki<kw;ki++){
					int jj=j+kj-jh;
					int ii=i+ki-ih;
					int pos=c*w*h+jj*w+ii;
					v+=pData[pos];
				}
			}
			//v/=16;
			//sort(tmp.begin(),tmp.end());
			int pos=c*w*h+j*w+i;
			/*if(abs(v)<30)
				pData1[pos]=128;
			else*/
			pData1[pos]=v/(kh*kw);
		}
	}
	m_pImg->Destroy();
	delete m_pImg;
	m_pImg=DataToImg(pData1,cn,w,h);
	delete []pData;
	delete []pData1;
	ReDrawItem();
}


void CWisdomLabelDlg::OnBnClickedJuan()
{
	// TODO: 在此添加控件通知处理程序代码
	m_MouseState=JUAN;
}


void CWisdomLabelDlg::OnBnClickedQuan()
{
	// TODO: 在此添加控件通知处理程序代码
		if(m_DbxMin.size()<=0)
		return;
	CStringA sType=GetWndTxtA(GetDlgItem(IDC_COMBO2));
	//->GetWindowText(sType);
	sType="无";
	CreateDirectory(CString("E:\\testData33\\非无"),NULL);
	CDC *pDC=GetDC();
	unsigned char *pBiaoji=NULL;
	{
		vector<CPoint>BjPoint;
		vector<Line>LineList;
		for(int i=0;i<m_DbxMin.size()-1;i++){
			LineList.push_back(Line(m_DbxMin.at(i),m_DbxMin.at(i+1)));
		}
		LineList.push_back(Line(m_DbxMin.at(m_DbxMin.size()-1),m_DbxMin.at(0)));
		for(int k=0;k<LineList.size();k++){
			Line line=LineList.at(k);
			int x=line.pt2.x-line.pt1.x;
			int y=line.pt2.y-line.pt1.y;
			if(y==0 && x==0){
				BjPoint.push_back(line.pt1);
			}else if(x==0){
				if(y>=0){
					for(int j=0;j<=y;j++){
						BjPoint.push_back(CPoint (line.pt1.x,line.pt1.y+j));
					}
				}else{
					for(int j=0;j>=y;j--){
						BjPoint.push_back(CPoint (line.pt1.x,line.pt1.y+j));
					}
				}
			}else if(y==0){
				if(x>=0){
					for(int i=0;i<=x;i++){
						BjPoint.push_back(CPoint (line.pt1.x+i,line.pt1.y));
					}
				}else{
					for(int i=0;i>=x;i--){
						BjPoint.push_back(CPoint (line.pt1.x+i,line.pt1.y));
					}
				}
			}
			else if(abs(x)>abs(y)){
				float k=(float)y/(float)x;
				if(x>0)
					for(int i=0;i<=x;i++){
						BjPoint.push_back(CPoint(line.pt1.x+i,k*(float)i+(float)line.pt1.y));
					}
				else
					for(int i=x;i<=0;i++){
						BjPoint.push_back(CPoint(line.pt1.x+i,k*(float)i+(float)line.pt1.y));
					}
			}else {
				float k=(float)x/(float)y;
				if(y>0)
					for(int j=0;j<=y;j++){
						BjPoint.push_back(CPoint(k*(float)j+(float)line.pt1.x,line.pt1.y+j));
					}
				else
					for(int j=y;j<=0;j++){
						BjPoint.push_back(CPoint(k*(float)j+(float)line.pt1.x,line.pt1.y+j));
					}
			}
		}
	
		//C2d1dCol Col(m_pImg);
		int cn=1;
		int w,h;
		unsigned char *pData=GetImgData(m_pImg,cn,w,h);
		pBiaoji=new unsigned char[w*h];
		for(int i=0;i<w*h;i++){
			pBiaoji[i]=0;
		}
		CRect Rc=DbxToRect(&m_DbxMin);
		Rc.DeflateRect(-2,-2,-2,-2);
		for(int j=0;j<h;j++){
			for(int i=0;i<w;i++){
				int pos=j*w+i;
				if(!Rc.PtInRect(CPoint(i,j))){
					pBiaoji[pos]=255;
				}
			}
		}
		for(int i=0;i<BjPoint.size();i++){
			CPoint pt=BjPoint.at(i);
			int pos=pt.y*w+pt.x;
			pBiaoji[pos]=1;
		}
		vector<CPoint>AddPt;
		for(int j=0;j<h && AddPt.size()<1;j++){
			for(int i=0;i<w;i++){
				int pos=j*w+i;
				if(pBiaoji[pos]!=1&&pBiaoji[pos]!=255){
					pBiaoji[pos]=255;
					AddPt.push_back(CPoint(i,j));
					break;
				}
			}
		}
		for(;AddPt.size()>0;){
			//CPoint p8[8]={CPoint(-1,-1),CPoint(-1,0),CPoint(-1,-1),CPoint(0,-1),CPoint(1,-1),CPoint(1,0),CPoint(1,1),CPoint(0,1)};
			CPoint p8[4]={CPoint(-1,0),CPoint(0,-1),CPoint(1,0),CPoint(0,1)};
			CPoint pt=AddPt.front();
			AddPt.erase(AddPt.begin());
			for(int j=0;j<4;j++){
			
				CPoint pt1=pt+p8[j];
				if(pt1.x<0||pt1.x>=w||pt1.y<0||pt1.y>=h)
					continue;
				int pos=pt1.y*w+pt1.x;
				if(pBiaoji[pos]==1)
					continue;
				if(pBiaoji[pos]==255)
					continue;
				pBiaoji[pos]=255;
				AddPt.push_back(pt1);
			}
		}
		
		for(int i=0;i<BjPoint.size();i++){
			CPoint pt=ToTar(BjPoint.at(i));
			//pDC->SetPixel(pt,RGB(255,255,0));
		}
		for(int j=0;j<h;j++){
			for(int i=0;i<w;i++){
				int pos=j*w+i;
				int v=pBiaoji[pos];
				if(pBiaoji[pos]<128){
					CPoint pt=ToTar(CPoint(i,j));
					COLORREF col=m_pImg->GetPixel(i,j);
					COLORREF col1=RGB(GetRValue(col)-1,GetGValue(col)-1,GetBValue(col)-1);
					m_pImg->SetPixel(i,j,col1);
					//pData[j*w+i]-=5;
					//m_pImg->SetPixel*/
					pData[j*w+i]=rand()%256;
					pDC->SetPixel(pt,RGB(v,v,v));
				}
			}
		}
	//	CImage *pImg=DataToImg(pData,1,w,h);
	///*	int pos=m_PicPath.ReverseFind('\\');
	//	if(pos!=-1){*/
	//	/*	CStringA Name1=m_PicPath.Mid(pos+1);
	//		CStringA Name;
	//		Name.Format("E:\\testData33\\非%s\\crop_%s",sType,Name1);*/
	//	DeleteFile(m_PicPath);
	//	CStringA OldName=m_PicPath;
	//	//m_PicPath=m_PicPath.Left(m_PicPath.GetLength()-3);
	//	//m_PicPath+="png";
	//		pImg->Save(m_PicPath);
	//		if(OldName!=m_PicPath){
	//			MoveFile(OldName+".facerect",m_PicPath+".facerect");
	//			MoveFile(OldName+".dbc",m_PicPath+".dbc");
	//		}
	//		OpenNewPic(m_PicPath);
	//		
	//	//}
		
		delete []pData;
		//delete pImg;
		//Sleep(10000);
		//delete []pBiaoji;
		
	}

	ReleaseDC(pDC);
	delete []pBiaoji;
	//ReDrawItem();
}


void CWisdomLabelDlg::OnBnClickedCdisbiao()
{
	// TODO: 在此添加控件通知处理程序代码
	m_DrawLk=!m_DrawLk;
	if(m_DrawLk){
		//GetDlgItem(IDCDISBIAO)->SetWindowText("隐藏标注");
	}else{
		//GetDlgItem(IDCDISBIAO)->SetWindowText("显示标注");
	}
	ReDrawItem();
}


void CWisdomLabelDlg::OnBnClickedQuan2()
{
	// TODO: 在此添加控件通知处理程序代码
			int cn=1;
	int w,h;
	int kw=3;
	int kh=3;
	//float k0[3][3]={
	//	{-0.1,-0.1,-0.1},
	//	{-0.1,1.8,-0.1},
	//	{-0.1,-0.1,-0.1}
	//};
	//float k1[3][3]={
	//	{-1,0,1},
	//	{-1,0,1},
	//	{-1,0,1}
	//};
	unsigned char *pData=GetImgData(m_pImg,cn,w,h);
	//unsigned char *pData1=new unsigned char[w*h];
	//memset(pData1,128,w*h);
	for(int j=1;j<h-1;j++){
		for(int i=1;i<w-1;i++){
			int pos=j*w+i+w+1;
			if(pData[pos]>240){
					int v=0;
					int n=0;
				//vector<unsigned char> tmp;
				for(int kj=0;kj<kh;kj++){
					for(int ki=0;ki<kw;ki++){
						int jj=j+kj;
						int ii=i+ki;
						int pos1=jj*w+ii;
						if(pData[pos1]>240)
							continue;
						v+=pData[pos1];
						n++;
					}
				}
				pData[pos]=v/n;
			}
			//v/=16;
			//sort(tmp.begin(),tmp.end());
			//int pos=j*w+i+w+1;
			/*if(abs(v)<30)
				pData1[pos]=128;
			else*/
			
		}
	}
	m_pImg->Destroy();
	delete m_pImg;
	m_pImg=DataToImg(pData,cn,w,h);
	delete []pData;
	//delete []pData1;
			DeleteFile(CString(m_PicPath));
		CStringA OldName=m_PicPath;
		CStringA OldName1=this->m_FolderPath+"\\"+m_PicTitle;
		m_PicPath=m_PicPath.Left(m_PicPath.GetLength()-3);
		m_PicPath+="jpg";
			m_pImg->Save(CString(m_PicPath));
			if(OldName!=m_PicPath){
				MoveFile(CString(OldName+".facerect"),CString(m_PicPath+".facerect"));
				MoveFile(CString(OldName+".dbc"),CString(m_PicPath+".dbc"));
				MoveFile(CString(OldName1+".xml"),CString(m_PicPath+".xml"));
			}
			//OpenNewPic(m_PicPath);
			m_PicPath="";
			int id=GetSelID();
			m_pImgListCtrl.SetSelectItem(-1);
			m_pImgListCtrl.SetSelectItem(id);
			//m_PicList.RemoveHead();
			//m_PicList.AddHead(m_PicPath);
	ReDrawItem();
}


void CWisdomLabelDlg::OnBnClickedClk2()
{
	// TODO: 在此添加控件通知处理程序代码
	m_TmpLk[0].Pts.clear();
	m_TmpLk[1].Pts.clear();
	m_PtId=-1;
	m_MouseState=SELECTDX;
}


void CWisdomLabelDlg::OnBnClickedIdcreatedate21()
{
	// TODO: 在此添加控件通知处理程序代码
	{
		if(m_GraphRect.Width()<10 && m_GraphRect.Height()<10)
			return;
		CStringA zName=GetDataDir()+"\\zpicnum.txt";
		CStringA fName=GetDataDir()+"\\fpicnum.txt";
		
		char cPicId[32]={0};
		CFile file;
		if(file.Open(CString(zName),CFile::modeRead)){
			int Size=file.Read(cPicId,32);
			file.Close();
			cPicId[Size]=0;
		}	
		int zPicId=atoi(cPicId);
		int cn=1;
		int w,h;
		CBitmap Bit;
		CImage Img;//=NULL;
		if(m_LkList.size()>0){
			int w=m_pImg->GetWidth();
			int h=m_pImg->GetHeight();
			CDC *pDC=GetDC();
			Bit.CreateCompatibleBitmap(pDC,w,h);
			CDC memDC;
			memDC.CreateCompatibleDC(pDC);
			CBitmap *pOldBit=memDC.SelectObject(&Bit);
			CPen pen1(PS_SOLID,4,RGB(100,100,100));
			CPen *pOldPen=memDC.SelectObject(&pen1);
			memDC.FillRect(CRect(0,0,w,h),&CBrush(RGB(128,128,128)));
			for(int j=0;j<m_LkList.size();j++){
				Lk lk=m_LkList.at(j);
				if(lk.Pts.size()<=0){
					continue;
				}
				CPoint pt=ToTar(lk.Pts.at(0));
				//memDC.MoveTo(pt);
				for(int i=0;i<lk.Pts.size();i++){
					CPoint pt=lk.Pts.at(i);
					/*CRect Rc(CPoint(pt.x-3,pt.y-3),CPoint(pt.x+3,pt.y+3));
					if(m_MouseState==SELECTDX ||m_MouseState==GAIDIAN)
						memDC.Ellipse(&Rc);*/
					if(i==0)
						memDC.MoveTo(pt);
					else
						memDC.LineTo(pt);
				}
				//memDC.LineTo(pt);
			}
			memDC.SelectObject(pOldBit);
			memDC.SelectObject(pOldPen);
			//Img.Attach((HBITMAP)Bit.m_hObject);
			ReleaseDC(pDC);

		}else
			return;
		//if(Img.IsNull())
		//	return;
	
		unsigned char *pData=GetBitmapData(&Bit,cn,w,h);
		//Img.Detach();
		//Img.Destroy();
		Bit.DeleteObject();

		CDC *pDC=GetDC();
		CStringA sType="无";
		int ddd=m_GraphRect.Width()/10;
		int dd=0;
		//for(dd=-1;dd<=1;dd++)
		if(1){
			int dddd=dd*ddd;
			CRect RcZh=m_GraphRect;
			RcZh.InflateRect(dddd,dddd);
			int PyFw=RcZh.Width()/5;
			float pyStep=PyFw/5;
			for(int jj=-2;jj<=2;jj++){
				for(int ii=-2;ii<2;ii++){
					int i=ii*pyStep+RcZh.left;
					int j=jj*pyStep+RcZh.top;
					int k=RcZh.Width();
					for(int a=-10;a<=10;a+=5){
						//int a=rand()%80-40;
						MSG message; 
						while(PeekMessage (&message,NULL,0,0,PM_REMOVE)) 
						{ 
							TranslateMessage (&message); 
							DispatchMessage (&message); 
							if(message.message==WM_CLOSE){
								return;
							}
						} 

						unsigned char *tmp=NULL;//CopyData(pData,w,h,i,j,k,k);
					
		
						CPoint cpt(RcZh.Width()/2,RcZh.Height()/2);//RcZh.CenterPoint();
						float resp[14];
						vector<CPoint>resDbx;
						if(a==0){
							tmp=CopyData(pData,w,h,i,j,k,k);
						}else{
							tmp=CopyAndRotateData(pData,w,h,i,j,k,k,(float)a,cpt.x,cpt.y);
						}
						if(!tmp){
							continue;
						}

						CRect tRc=DbxToRect(&resDbx);
						resp[5*2]=tRc.TopLeft().x;
						resp[5*2+1]=tRc.TopLeft().y;
						resp[6*2]=tRc.BottomRight().x;
						resp[6*2+1]=tRc.BottomRight().y;
						unsigned char *tmp1=new unsigned char[MIN_DET_SIZE*MIN_DET_SIZE];
						//unsigned char *tmpg1=new unsigned char[MIN_DET_SIZE*MIN_DET_SIZE];
						//unsigned char *tmpBj1=new unsigned char[MIN_DET_SIZE*MIN_DET_SIZE];
						ScaleData(tmp,k,k,1,tmp1,MIN_DET_SIZE,MIN_DET_SIZE);
			
						float s=(float)MIN_DET_SIZE/(float)k;
			
						CImage *pImg=DataToImg(tmp1,1,MIN_DET_SIZE,MIN_DET_SIZE);

						pImg->Draw(pDC->GetSafeHdc(),m_TarRect.left,m_TarRect.top);

						int pos=m_PicPath.ReverseFind('\\');
						CStringA pname=m_PicPath.Mid(pos+1);
						pname=pname.Left(pname.GetLength()-4);
						zPicId++;	
						CStringA Str;
						{
							CFileStatus status;
							Str.Format(GetDataDir()+"\\%s",sType);
							if(CFile::GetStatus(CString(Str),status)==0){
								CreateDirectory(CString(Str),NULL);
							}
						}
						{
							CFileStatus status;
							Str.Format(GetDataDir()+"\\%s\\%s_lk",sType,pname);
							if(CFile::GetStatus(CString(Str),status)==0){
								CreateDirectory(CString(Str),NULL);
							}
						}
						/*if(zPicId%10000==0){
						
							CreateDirectory(Str,NULL);
						}*/
						Str.Format(GetDataDir()+"\\%s\\%s_lk\\%d.png",sType,pname,zPicId);
						pImg->Save(CString(Str));
						pImg->Destroy();
			
						if(0){
							Str.Format(GetDataDir()+"\\%s\\%s\\%d.jpg.p",sType,pname,zPicId);
							CFile f;
							if(f.Open(CString(Str),CFile::modeCreate|CFile::modeWrite|CFile::typeBinary)){
								f.Write(resp,sizeof(resp));
								f.Close();
							}
						}
		
						delete []tmp;
		
						//delete []tmpBj;
			
						delete pImg;
						delete []tmp1;
						//delete []tmpg1;
						//delete []tmpBj1;
					
					}
				}
			}
		}

		delete []pData;
		ReleaseDC(pDC);
		{
			CStringA Str;
			Str.Format("%d",zPicId);
			//CFile file;
			if(file.Open(CString(zName),CFile::modeCreate|CFile::modeWrite)){
				file.Write(Str,Str.GetLength());
				file.Close();
			}
		}
	}
	return;
}


void CWisdomLabelDlg::OnBnClickedOpenpic13()
{
	// TODO: 在此添加控件通知处理程序代码
	CFileDialog Dlg(TRUE,NULL,NULL,NULL,NULL,NULL);
	char fileName[MAX_PATH];
	strncpy(fileName,m_PicPath,MAX_PATH);
	Dlg.m_ofn.lpstrFile=CString(fileName).GetBuffer();
	Dlg.m_ofn.nMaxFile=MAX_PATH;
	if(Dlg.DoModal()==IDOK){
		m_pImg->Save(Dlg.GetPathName());
	}
}

struct PicAndDxN{
	int PicN;
	int DxN;
	CStringA lastPicName;
	PicAndDxN(){
		PicN=0;
		DxN=0;
	}
};
void CWisdomLabelDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	CMTextDlg Dlg1;
	Dlg1.Size=1;
	Dlg1.Title="请选择标注目录";
	Dlg1.Titles[0]="请输入标注目录:";
	Dlg1.bLl[0]=2;
	Dlg1.TextWidth=800;
	CFile fii;
	CStringA HisDir=GetModuleDir()+"\\cfg\\hisdir.txt";
	if(fii.Open(CString(HisDir),CFile::modeRead)){
		char Buf[4096];
		int size=fii.Read(Buf,4095);
		Buf[size]=0;
		vector<CStringA> List=SplitStr(CStringA(Buf),"\r\n");
		CStringA ss;
		for(int k=0;k<List.size();k++){
			ss=List.at(k);
		//while(fi.ReadString(ss)){
			ss.Replace("\r","");
			ss.Replace("\n","");
			Dlg1.Datas[0].AddTail(CString(ss));
		}
		fii.Close();
	}
	if(Dlg1.DoModal()!=IDOK)
		return;
	
	
	
	map<CStringA ,PicAndDxN> DxAndPicN;
	PicAndDxN tpdn;
	//OpenNewPic(Dlg.GetPathName());
	char Buf[64000];
	CFileFind ff;
	int res=ff.FindFile(CString(Dlg1.Texts[0]+"\\*.*"));
	while(res){
		res=ff.FindNextFile();
		if(ff.IsDots() || ff.IsDirectory())
			continue;
		CStringA FilePath=CStringA(ff.GetFilePath());
		//if(FilePath==m_PicPath)
		//	continue;
		CStringA FileName=CStringA(ff.GetFileName());
		CStringA FileTitle=CStringA(ff.GetFileTitle());
		CStringA hz=FilePath.Right(FileName.GetLength()-FileTitle.GetLength()-1);
		if(hz.CompareNoCase("xml")==0){
			CFile fi;
			if(!fi.Open(CString(FilePath),CFile::modeRead))
				continue;
			int Size=fi.Read(Buf,64000);
			fi.Close();
			tpdn.PicN++;
			Buf[Size]=0;
			CStringA Str(Buf);
			int pos=0;
			CStringA Fs="<object>";
			while((pos=Str.Find(Fs))!=-1){
				pos+=Fs.GetLength();
				Str=Str.Mid(pos);
				CStringA Fs1="<name>";
				pos=Str.Find(Fs1);
				if(pos!=-1){
					pos+=Fs1.GetLength();
					Str=Str.Mid(pos);
					int pos2=Str.Find("</name>");
					if(pos2!=-1){
						CStringA Name=Str.Left(pos2);
						if(!Name.IsEmpty()){
							map<CStringA ,PicAndDxN>::iterator it=DxAndPicN.find(Name);
							if(it!=DxAndPicN.end()){
								it->second.DxN++;
								if(it->second.lastPicName!=FilePath){
									it->second.lastPicName=FilePath;
									it->second.PicN++;
								}
							}else{
								PicAndDxN pdn;
								pdn.DxN=1;
								pdn.PicN=1;
								pdn.lastPicName=FilePath;
								DxAndPicN[Name]=pdn;
							}
						}
					}
				}
			}
		}
		//CStringA hz1=hz;
	}
	
	CStringA Res;
	for(map<CStringA ,PicAndDxN>::iterator it=DxAndPicN.begin();it!=DxAndPicN.end();it++){
		CStringA Str;
		Str.Format("'%s': (图片数%d,标签数%d),\r\n",it->first,it->second.PicN,it->second.DxN);
		Res+=Str;
		tpdn.DxN+=it->second.DxN;
		//tpdn.PicN+=
	}

	CFile fi;
	if(fi.Open(CString("label_num.txt"),CFile::modeCreate|CFile::modeWrite)){
		fi.Write(Res,Res.GetLength());
		fi.Close();
	}
	CStringA Str;
	Str.Format("'%s': (图片数%d,标签数%d),\r\n","total",tpdn.PicN,tpdn.DxN);
	Res+=Str;
	Res+="结果保存至 label_num.txt";
	CTextDlg Dlg2;
	Dlg2.m_Text=Res;
	Dlg2.DoModal();
	//AfxMessageBoxA(Res);
}


void CWisdomLabelDlg::OnBnClickedCsetpoint3()
{
	// TODO: 在此添加控件通知处理程序代码
	/*CFileDialog Dlg(FALSE,NULL,NULL,NULL,NULL,NULL);
	if(Dlg.DoModal()!=IDOK){
		return;
	}
	int w=1280;
	int h=720;
	int c=3;
	
	CFile fi;
	if(!fi.Open(Dlg.GetPathName(),CFile::modeRead|CFile::typeBinary))
		return;
	unsigned char *pData=new unsigned char[w*h*c];
	fi.Read(pData,w*h*c);
	if(m_pImg){
		if(!m_pImg->IsNull())
			m_pImg->Destroy();
		delete m_pImg;
	}
	m_pImg=new CImage();
	m_pImg->Create(w,h,c*8,0);*/
	if(!m_pImg||m_pImg->IsNull())
		return;
	
	static BOOL bSave=0;
	if(!m_IsPCl){
		CMTextDlg dlg;

		dlg.Title="红蓝互换设置";
		dlg.Size=1;
		
		dlg.Titles[0]="是否保存(0：不保存，1：保存):";
		
		dlg.Texts[0]=IntToStr(bSave);
		if(dlg.DoModal()!=IDOK)
			return;

		
		bSave=atoi(dlg.Texts[0]);
	}
	int h=m_pImg->GetHeight();
	int w=m_pImg->GetWidth();
	for(int j=0;j<h;j++){
		for(int i=0;i<w;i++){
			unsigned char *b=(unsigned char *)m_pImg->GetPixelAddress(i,j);
			unsigned char *g=b+1;
			unsigned char *r=g+1;
			unsigned char tmp=*r;
			*r=*b;
			*b=tmp;
			/*if(c==1){
				*b=*g=*r=pData[j*w+i];
			}else if(c==3){
				int pos1=j*w*c+i*c;
				int pos2=pos1+1;
				int pos3=pos2+1;
				*r=pData[pos1];
				*g=pData[pos2];
				*b=pData[pos3];
			}*/
		}
	}
//	delete []pData;
//	fi.Close();
	ReDrawItem();
	if(bSave)
		saveImg(m_PicPath,m_pImg);
}


void CWisdomLabelDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	CFileDialog Dlg(TRUE,FALSE,FALSE,FALSE,FALSE,FALSE);
	if(!Dlg.DoModal()==IDOK)
		return;
	map<CStringA ,PicAndDxN> DxAndPicN;
	PicAndDxN tpdn;
	//OpenNewPic(Dlg.GetPathName());
	char Buf[64000];
	CFileFind ff;
	int res=ff.FindFile(Dlg.GetFolderPath()+"\\*.*");
	while(res){
		res=ff.FindNextFile();
		if(ff.IsDots() || ff.IsDirectory())
			continue;
		CStringA FilePath=CStringA(ff.GetFilePath());
		//if(FilePath==m_PicPath)
		//	continue;
		CStringA FileName=CStringA(ff.GetFileName());
		CStringA FileTitle=CStringA(ff.GetFileTitle());
		CStringA hz=FilePath.Right(FileName.GetLength()-FileTitle.GetLength()-1);
		if(hz.CompareNoCase("xml")==0){
			CFile fi;
			if(!fi.Open(CString(FilePath),CFile::modeRead))
				continue;
			int Size=fi.Read(Buf,64000);
			fi.Close();
			tpdn.PicN++;
			Buf[Size]=0;
			CStringA Str(Buf);
			CStringA StrBf=Str;
			CStringA JpgPath=FilePath;
			JpgPath.Replace("Annotations","JPEGImages");
			JpgPath=JpgPath.Left(JpgPath.GetLength()-4);
			JpgPath+=".jpg";
			{
				//<filename>2007_000027.jpg</filename>
				/*int pos=0;
				CStringA Fs="<filename>";
				if((pos=Str.Find(Fs))!=-1){
				}*/
			}
			int bCopy=0;
			int pos=0;
			CStringA Fs="<object>";
			while((pos=Str.Find(Fs))!=-1){
				pos+=Fs.GetLength();
				Str=Str.Mid(pos);
				CStringA Fs1="<name>";
				pos=Str.Find(Fs1);
				if(pos!=-1){
					pos+=Fs1.GetLength();
					Str=Str.Mid(pos);
					int pos2=Str.Find("</name>");
					if(pos2!=-1){
						CStringA Name=Str.Left(pos2);

						if(Name=="car" || Name=="bus" ||Name=="person"){
							CStringA CarPath=JpgPath;
							CarPath.Replace("JPEGImages","car");
							CopyFile(CString(JpgPath),CString(CarPath),1);
							bCopy=1;
							break;
						}
					}
				}
			}
			if(bCopy){
				StrBf.Replace("<name>car</name>","<name>vehicle</name>");
				StrBf.Replace("<name>bus</name>","<name>vehicle</name>");
				CStringA CarPath=FilePath;
				CarPath.Replace("Annotations","car");
				CFile f;
				if(f.Open(CString(CarPath),CFile::modeCreate|CFile::modeWrite)){
					f.Write(StrBf,StrBf.GetLength());
					f.Close();
				}
			}
		}
		//vehicle
		//CStringA hz1=hz;
	}
	
	CStringA Res;
	for(map<CStringA ,PicAndDxN>::iterator it=DxAndPicN.begin();it!=DxAndPicN.end();it++){
		CStringA Str;
		Str.Format("'%s': (%d, %d),\r\n",it->first,it->second.PicN,it->second.DxN);
		Res+=Str;
		tpdn.DxN+=it->second.DxN;
		//tpdn.PicN+=
	}
	CStringA Str;
	Str.Format("'%s': (%d, %d),\r\n","total",tpdn.PicN,tpdn.DxN);
	Res+=Str;
	AfxMessageBoxA(Res);
	CFile fi;
	if(fi.Open(CString("dxn.txt"),CFile::modeCreate|CFile::modeWrite)){
		fi.Write(Res,Res.GetLength());
		fi.Close();
	}
}


void CWisdomLabelDlg::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码
	vector<FInfo> files;
	/*CFileDialog Dlg(TRUE,FALSE,FALSE,FALSE,FALSE,FALSE);
	if(Dlg.DoModal()!=IDOK)
		return;*/
	CMTextDlg dlg;

	dlg.Title="前缀设置";
	dlg.Size=2;
	dlg.Titles[0]="请输入前缀:";
	dlg.Titles[1]="是否完全重命名:";
	dlg.Texts[1]="0";
	if(dlg.DoModal()!=IDOK)
		return;
	//lg.Texts[0]
	CStringA Qz=dlg.Texts[0];
	int picn=0;//
	//OpenNewPic(Dlg.GetPathName());
	char Buf[64000];
	CFileFind ff;
	//CStringA FirstName;
	int res=ff.FindFile(CString(m_FolderPath+"\\*.*"));
	while(res){
		res=ff.FindNextFile();
		if(ff.IsDots() || ff.IsDirectory())
			continue;
		CStringA FilePath=CStringA(ff.GetFilePath());
		
		FInfo fi;
		fi.FileName=CStringA(ff.GetFileName());
		fi.FileTitle=CStringA(ff.GetFileTitle());	
		fi.FileFolder=m_FolderPath;
		fi.FilePath=CStringA(ff.GetFilePath());
		/*if("浙LD1930"==FileTitle){
			int a=3;
		}*/
		fi.Hz=FilePath.Right(fi.FileName.GetLength()-fi.FileTitle.GetLength()-1);
		
		if(fi.Hz.CompareNoCase("jpg")==0 ||fi.Hz.CompareNoCase("jpeg")==0
			||fi.Hz.CompareNoCase("JPEG")==0)
		{
				files.push_back(fi);
			
			/*	FilePath=Dlg.GetFolderPath()+"\\"+FileTitle+".xml";
				newName.Format("%s\\%s%d.xml",Dlg.GetFolderPath(),Qz,picn++);
				rename(FilePath,newName);*/
		}else{
				//DeleteFile(FilePath);
		}
		
	}
	ff.Close();
	for(int i=0;i<files.size();i++){
		FInfo fi=files.at(i);
		
		CStringA newName;
		CStringA newName2;
		while(1)
		{
			if(dlg.Texts[1]=="1"){
				newName.Format("%s\\%s%05d.jpg",fi.FileFolder,Qz,picn);
				newName2.Format("%s%05d.jpg",Qz,picn);
			}else{
				newName.Format("%s\\%s%s.jpg",fi.FileFolder,Qz,fi.FileTitle);
				newName2.Format("%s%s.jpg",Qz,fi.FileTitle);
			}
			CFileStatus status;
			if(CFile::GetStatus(CString(newName),status)){
				picn++;
			}else{
				break;
			}
		}
		CStringA FilePath1;//=this->GetXmlFileName();
		FilePath1.Format("%s\\%s.xml",m_LabelPath,fi.FileTitle);
		
		int bOk=1;
		vector<Lk> Lks;
		CFileStatus status;
		if(CFile::GetStatus(CString(FilePath1),status))
		{
			OpenXml(FilePath1,&Lks);
			if(!DeleteFile(CString(FilePath1))){
				AfxMessageBoxA(FilePath1+"重命名失败,请检查是否被其他程序占用");
				bOk=0;
			}
		}
		if(bOk){
			if(rename(fi.FilePath,newName)==0){
				
				CStringA newName1;
				if(dlg.Texts[1]=="1"){
					newName1.Format("%s\\%s%05d.xml",m_LabelPath,Qz,picn);
				}else{
					newName1.Format("%s\\%s%s.xml",m_LabelPath,Qz,fi.FileTitle);
				}
				if(Lks.size()>0){
					CImage img;
					img.Load(CString(newName));
					SaveXml(newName1,newName2,newName,m_LabelPath,img.GetWidth(),img.GetHeight(),img.GetBPP()/8,&Lks);
					img.Destroy();
				}
				//rename(FilePath1,newName);
			}else{
				AfxMessageBoxA(fi.FilePath+"重命名失败,请检查是否被其他程序占用");
			}
		}
		picn++;
	}
	//OnBnClickedIdcreatedate22();
}


void CWisdomLabelDlg::OnBnClickedButton4()
{
	// TODO: 在此添加控件通知处理程序代码
	CFileDialog Dlg(TRUE,FALSE,FALSE,FALSE,FALSE,FALSE);
	if(!Dlg.DoModal()==IDOK)
		return;
	map<CStringA ,PicAndDxN> DxAndPicN;
	PicAndDxN tpdn;
	//OpenNewPic(Dlg.GetPathName());
	char Buf[64000];
	CFileFind ff;
	int res=ff.FindFile(Dlg.GetFolderPath()+"\\*.*");
	while(res){
		res=ff.FindNextFile();
		if(ff.IsDots() || ff.IsDirectory())
			continue;
		CStringA FilePath=CStringA(ff.GetFilePath());
		//if(FilePath==m_PicPath)
		//	continue;
		CStringA FileName=CStringA(ff.GetFileName());
		CStringA FileTitle=CStringA(ff.GetFileTitle());
		CStringA hz=FilePath.Right(FileName.GetLength()-FileTitle.GetLength()-1);
		if(hz.CompareNoCase("xml")==0){
			CFile fi;
			if(!fi.Open(CString(FilePath),CFile::modeRead))
				continue;
			int Size=fi.Read(Buf,64000);
			fi.Close();
			tpdn.PicN++;
			Buf[Size]=0;
			CStringA Str(Buf);
			CStringA JpgPath=FilePath;
			JpgPath.Replace("\\Annotations\\","\\hstplate\\");
			JpgPath=JpgPath.Left(JpgPath.GetLength()-4);
			JpgPath+=".jpg";
			CFileStatus status;
			if(!CFile::GetStatus(CString(JpgPath),status)){
				continue;
			}
			{
				//<filename>2007_000027.jpg</filename>
				/*int pos=0;
				CStringA Fs="<filename>";
				if((pos=Str.Find(Fs))!=-1){
				}*/
			}
			int pos=0;
			CStringA Fs="<object>";
			while((pos=Str.Find(Fs))!=-1){
				pos+=Fs.GetLength();
				Str=Str.Mid(pos);
				CStringA Fs1="<name>";
				pos=Str.Find(Fs1);
				if(pos!=-1){
					pos+=Fs1.GetLength();
					Str=Str.Mid(pos);
					int pos2=Str.Find("</name>");
					if(pos2!=-1){
						CStringA Name=Str.Left(pos2);

						//if(Name=="car" || Name=="bus"){
							CStringA CarPath=JpgPath;
							CarPath.Replace("\\hstplate\\","\\car\\");
							AfxMessageBoxA(JpgPath+","+CarPath);
							CopyFile(CString(JpgPath),CString(CarPath),1);
							AfxMessageBoxA("y");
							break;
						//}
					}
				}
			}
		}
		//CStringA hz1=hz;
	}
	
	CStringA Res;
	for(map<CStringA ,PicAndDxN>::iterator it=DxAndPicN.begin();it!=DxAndPicN.end();it++){
		CStringA Str;
		Str.Format("'%s': (%d, %d),\r\n",it->first,it->second.PicN,it->second.DxN);
		Res+=Str;
		tpdn.DxN+=it->second.DxN;
		//tpdn.PicN+=
	}
	CStringA Str;
	Str.Format("'%s': (%d, %d),\r\n","total",tpdn.PicN,tpdn.DxN);
	Res+=Str;
	AfxMessageBoxA(Res);
	CFile fi;
	if(fi.Open(CString("dxn.txt"),CFile::modeCreate|CFile::modeWrite)){
		fi.Write(Res,Res.GetLength());
		fi.Close();
	}
}


void CWisdomLabelDlg::OnBnClickedButton5()
{
	// TODO: 在此添加控件通知处理程序代码
	CFileDialog Dlg(TRUE,FALSE,FALSE,FALSE,FALSE,FALSE);
	if(!Dlg.DoModal()==IDOK)
		return;
	map<CStringA ,PicAndDxN> DxAndPicN;
	PicAndDxN tpdn;
	//OpenNewPic(Dlg.GetPathName());
	char Buf[64000];
	CFileFind ff;
	int res=ff.FindFile(Dlg.GetFolderPath()+"\\*.*");
	while(res){
		res=ff.FindNextFile();
		if(ff.IsDots() || ff.IsDirectory())
			continue;
		CStringA FilePath=CStringA(ff.GetFilePath());
		//if(FilePath==m_PicPath)
		//	continue;
		CStringA FileName=CStringA(ff.GetFileName());
		CStringA FileTitle=CStringA(ff.GetFileTitle());
		CStringA hz=FilePath.Right(FileName.GetLength()-FileTitle.GetLength()-1);
		if(hz.CompareNoCase("xml")==0){
			CFile fi;
			if(!fi.Open(CString(FilePath),CFile::modeRead))
				continue;
			int Size=fi.Read(Buf,64000);
			fi.Close();
			tpdn.PicN++;
			Buf[Size]=0;
			CStringA Str(Buf);
			CStringA JpgPath=FilePath;
			JpgPath.Replace("Annotations","JPEGImages");
			JpgPath=JpgPath.Left(JpgPath.GetLength()-4);
			JpgPath+=".jpg";
			{
				//<filename>2007_000027.jpg</filename>
				/*int pos=0;
				CStringA Fs="<filename>";
				if((pos=Str.Find(Fs))!=-1){
				}*/
			}
			int pos=0;
			CStringA Fs="<object>";
			while((pos=Str.Find(Fs))!=-1){
				pos+=Fs.GetLength();
				Str=Str.Mid(pos);
				CStringA Fs1="<name>";
				pos=Str.Find(Fs1);
				if(pos!=-1){
					pos+=Fs1.GetLength();
					Str=Str.Mid(pos);
					int pos2=Str.Find("</name>");
					if(pos2!=-1){
						CStringA Name=Str.Left(pos2);

						if(Name=="palte"){
							AfxMessageBoxA(JpgPath);
							return;
							/*CStringA CarPath=JpgPath;
							CarPath.Replace("JPEGImages","car");
							CopyFile(JpgPath,CarPath,1);
							break;*/
						}
					}
				}
			}
		}
		//CStringA hz1=hz;
	}
	
	CStringA Res;
	for(map<CStringA ,PicAndDxN>::iterator it=DxAndPicN.begin();it!=DxAndPicN.end();it++){
		CStringA Str;
		Str.Format("'%s': (%d, %d),\r\n",it->first,it->second.PicN,it->second.DxN);
		Res+=Str;
		tpdn.DxN+=it->second.DxN;
		//tpdn.PicN+=
	}
	CStringA Str;
	Str.Format("'%s': (%d, %d),\r\n","total",tpdn.PicN,tpdn.DxN);
	Res+=Str;
	AfxMessageBoxA(Res);
	CFile fi;
	if(fi.Open(CString("dxn.txt"),CFile::modeCreate|CFile::modeWrite)){
		fi.Write(Res,Res.GetLength());
		fi.Close();
	}
}


void CWisdomLabelDlg::OnBnClickedOpenpic14()
{
	// TODO: 在此添加控件通知处理程序代码
	m_TmpLk[0].Pts.clear();
	m_TmpLk[1].Pts.clear();
	m_MouseState=DELETEDX;
}


void CWisdomLabelDlg::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	AfxMessageBoxA(IntToStr(nChar));
	CDialogEx::OnChar(nChar, nRepCnt, nFlags);
}


void CWisdomLabelDlg::OnBnClickedButton6()
{
	// TODO: 在此添加控件通知处理程序代码
	//CFileDialog Dlg(TRUE,NULL,NULL,NULL,NULL,NULL);
	//char fileName[MAX_PATH];
	//strncpy(fileName,m_PicPath,MAX_PATH);
	//Dlg.m_ofn.lpstrFile=(char *)fileName;
	//Dlg.m_ofn.nMaxFile=MAX_PATH;
	//if(Dlg.DoModal()==IDOK){
		m_pImg->Save(CString(m_PicPath));
		SaveXml(GetXmlFileName(),m_PicName,m_PicPath,m_FolderPath,m_pImg->GetWidth(),m_pImg->GetHeight(),m_pImg->GetBPP()/8,&m_LkList);
		AfxMessageBoxA("保存成功");
	//}
}


void CWisdomLabelDlg::OnBnClickedQuan3()
{
	// TODO: 在此添加控件通知处理程序代码
	int cn=1;
	int w,h;
	int kw=3;
	int kh=3;
	//float k0[3][3]={
	//	{-0.1,-0.1,-0.1},
	//	{-0.1,1.8,-0.1},
	//	{-0.1,-0.1,-0.1}
	//};
	//float k1[3][3]={
	//	{-1,0,1},
	//	{-1,0,1},
	//	{-1,0,1}
	//};
	unsigned char *pData=GetImgData(m_pImg,cn,w,h);
	//unsigned char *pData1=new unsigned char[w*h];
	//memset(pData1,128,w*h);
	//for(int j=1;j<h-1;j++){
	//	for(int i=1;i<w-1;i++){
	//		int pos=j*w+i+w+1;
	//		if(pData[pos]>240){
	//				int v=0;
	//				int n=0;
	//			//vector<unsigned char> tmp;
	//			for(int kj=0;kj<kh;kj++){
	//				for(int ki=0;ki<kw;ki++){
	//					int jj=j+kj;
	//					int ii=i+ki;
	//					int pos1=jj*w+ii;
	//					if(pData[pos1]>240)
	//						continue;
	//					v+=pData[pos1];
	//					n++;
	//				}
	//			}
	//			pData[pos]=v/n;
	//		}
	//		//v/=16;
	//		//sort(tmp.begin(),tmp.end());
	//		//int pos=j*w+i+w+1;
	//		/*if(abs(v)<30)
	//			pData1[pos]=128;
	//		else*/
	//		
	//	}
	//}
	m_pImg->Destroy();
	delete m_pImg;
	m_pImg=DataToImg(pData,cn,w,h);
	delete []pData;
	//delete []pData1;
	//DeleteFile(m_PicPath);
	//CStringA OldName=m_PicPath;
	//CStringA OldName1=this->m_FolderPath+"\\"+m_PicTitle;
	//m_PicPath=m_PicPath.Left(m_PicPath.GetLength()-3);
	//m_PicPath+="jpg";
	//m_pImg->Save(m_PicPath);
	//if(OldName!=m_PicPath){
	//	MoveFile(OldName+".facerect",m_PicPath+".facerect");
	//	MoveFile(OldName+".dbc",m_PicPath+".dbc");
	//	MoveFile(OldName1+".xml",m_PicPath+".xml");
	//}
	//OpenNewPic(m_PicPath);
	//m_PicList.RemoveHead();
	//m_PicList.AddHead(m_PicPath);
	ReDrawItem();
}


void CWisdomLabelDlg::OnBnClickedCdisbiao2()
{
	// TODO: 在此添加控件通知处理程序代码
	CColorDialog Dlg;
	if(Dlg.DoModal()==IDOK){
		m_TextColor=Dlg.GetColor();
		ReDrawItem();
	}
}


void CWisdomLabelDlg::OnBnClickedButton7()
{
	// TODO: 在此添加控件通知处理程序代码
	srand(GetTickCount());
	unsigned char sz[256];
	for(int i=0;i<256;i++){
		sz[i]=i;
	}
	for(int i=0;i<10000;i++){
		int a=rand()%256;
		int b=rand()%256;
		int tmp=sz[a];
		sz[a]=sz[b];
		sz[b]=tmp;
	}
	CStringA All;
	for(int i=0;i<256;i++){
		CStringA Str;
		Str.Format("%d,",sz[i]);
		All+=Str;
	}
	All+="\r\n";
	for(int i=0;i<256;i++){
		CStringA Str;
		int j=0;
		for(int k=0;k<256;k++){
			if(sz[k]==i){
				j=k;
				break;
			}
		}
		Str.Format("%d,",j);
		All+=Str;
	}
	GetDlgItem(IDC_EDIT1)->SetWindowText(CString(All));
}


void CWisdomLabelDlg::SaveXml(CStringA xmlName,CStringA picName,CStringA picPath,CStringA picDir,int w,int h,int cn,vector<Lk >*lks)
{
	for(vector<Lk>::iterator it=lks->begin();it!=lks->end();it++){
		if(it->LkType==TYPERECT){
			int left=it->Pts.at(0).x;
			int top=it->Pts.at(0).y;
			int right=it->Pts.at(1).x;
			int bottom=it->Pts.at(1).y;
			int bTh=0;
			if(left>right){
				int tmp=left;
				left=right;
				right=tmp;
				bTh=1;
			}
			if(bottom<top){
				int tmp=top;
				top=bottom;
				bottom=tmp;
				bTh=1;
			}
			if(bTh){
				vector<CPoint >tmpPts=it->Pts;
				it->Pts.clear();
				it->Pts.push_back(CPoint(left,top));
				it->Pts.push_back(CPoint(right,bottom));
				if(tmpPts.size()>2){
					it->Pts.insert(it->Pts.end(),tmpPts.begin()+2,tmpPts.end());
				}
			}
		}
		//for(vector<Lk>::iterator itp=it->Pts.begin();
	}
	if(lks->size()>0)
	{
		//AfxMessageBoxA(m_PicTitle);
		if(xmlName==GetXmlFileName())
			m_pImgListCtrl.SetItemText(GetSelID(),2,CString("已标注"));
		CFile f;
		CStringA Path=xmlName;//this->GetXmlFileName();
		//AfxMessageBoxA(Path);
		if(!f.Open(CString(Path),CFile::modeCreate|CFile::modeWrite))
			return;
		//AfxMessageBoxA(Path);
		CStringA Str="<annotation>\r\n";
		Str+="<folder>"+picDir.Right(picDir.GetLength()-picDir.ReverseFind('\\')-1)+"</folder>\r\n";
		Str+="<filename>"+picName+"</filename>\r\n";
		Str+="<path>"+picPath+"</path>\r\n";
		Str+="<source>\r\n<database>Unknown</database>\r\n</source>\r\n";
		
		Str+="<size>\r\n<width>"+IntToStr(w)+"</width>\r\n";
		Str+="<height>"+IntToStr(h)+"</height>\r\n";
		//AfxMessageBoxA(Str);
		Str+="<depth>"+IntToStr(cn)+"</depth>\r\n</size>\r\n";
		Str+="<segmented>0</segmented>\r\n";

;//m_PicPath;
		//Str.Replace(" ","");
		for(int j=0;j<lks->size();j++){
			Lk lk=lks->at(j);
			CStringA LkType="none";
			if(lk.LkType==TYPERECT)
				LkType="bndbox";
			else if(lk.LkType==TYPESPLINE)
				LkType="spline";
			else if(lk.LkType==TYPEDBC){
				LkType="polygon";
			}else if(lk.LkType==TYPEPOINT){
				LkType="point";
			}
			/*if(lk.LkType !=TYPERECT)
				continue;*/
			CStringA DxType=lk.DxType;
			if(!m_LabMap1[DxType].IsEmpty())
				DxType=m_LabMap1[DxType];
			Str+="<object>\r\n";
			Str+="<name>"+DxType+"</name>\r\n";
			Str+="<iscopy>"+IntToStr(lk.isCopy)+"</iscopy>\r\n";
			Str+="<pose>Unspecified</pose>\r\n";
			Str+="<truncated>0</truncated>\r\n";
			Str+="<difficult>0</difficult>\r\n";
			Str+="<"+LkType+">\r\n";
		
			if(lk.LkType==TYPERECT){
				int left=lk.Pts.at(0).x;
				int top=lk.Pts.at(0).y;
				int right=lk.Pts.at(1).x;
				int bottom=lk.Pts.at(1).y;
				if(left>right){
					int tmp=left;
					left=right;
					right=tmp;
				}
				if(bottom<top){
					int tmp=top;
					top=bottom;
					bottom=tmp;
				}
				Str+="<xmin>"+IntToStr(left)+"</xmin>\r\n";
				Str+="<ymin>"+IntToStr(top)+"</ymin>\r\n";
				Str+="<xmax>"+IntToStr(right)+"</xmax>\r\n";
				Str+="<ymax>"+IntToStr(bottom)+"</ymax>\r\n";
			}else{
				for(int i=0;i<lk.Pts.size();i++){
					Str+="<value>"+IntToStr(lk.Pts.at(i).x)+"</value>\r\n";
					Str+="<value>"+IntToStr(lk.Pts.at(i).y)+"</value>\r\n";
				}
			}
			Str+="</"+LkType+">\r\n";
			Str+="</object>\r\n";
		}
		Str+="</annotation>";
		f.Write(Str,Str.GetLength());
		f.Close();
	}else{
	}
}


void CWisdomLabelDlg::OnBnClickedButton8()
{
	// TODO: 在此添加控件通知处理程序代码
	int cn=3;
	int w,h;
	unsigned char *pData=GetImgData(m_pImg,cn,w,h);
	for(int c=0;c<cn;c++){
		int cpos=c*w*h;
		for(int j=0;j<h;j++){
			for(int i=0;i<w/2;i++){
				int im=w-1-i;
				int tmp=pData[cpos+j*w+i];
				pData[cpos+j*w+i]=pData[cpos+j*w+im];
				pData[cpos+j*w+im]=tmp;
			}
		}
	}
	m_pImg->Destroy();
	delete m_pImg;
	m_pImg=DataToImg(pData,cn,w,h);
	delete []pData;
	if(m_LkList.size()>0)
	{
		for(vector<Lk >::iterator it=m_LkList.begin();it!=m_LkList.end();it++){
			for(vector<CPoint>::iterator it1=it->Pts.begin();it1!=it->Pts.end();it1++){
				it1->x=w-1-it1->x;
			}
		}
	}
	//m_PicPath=m_PicPath+"mir.jpg";
	m_pImg->Save(CString(m_PicPath));
	//OnBnClickedSaveAnnotation();
	ReDrawItem();
}


void CWisdomLabelDlg::OnBnClickedIdcreatedate22()
{
	// TODO: 在此添加控件通知处理程序代码
	int id=GetSelID();
	for(int i=0;i<m_pImgListCtrl.GetItemCount();i++){
		OpenNewPic(CStringA(m_pImgListCtrl.GetItemText(i,1)));
		SaveXml(this->GetXmlFileName(),m_PicName,m_PicPath,m_FolderPath,m_pImg->GetWidth(),m_pImg->GetHeight(),m_pImg->GetBPP()/8,&m_LkList);
	}
	m_pImgListCtrl.SetSelectItem(id);
}


void CWisdomLabelDlg::LoadLabel(CStringA lablePath)
{
}


void CWisdomLabelDlg::OnBnClickedClk3()
{
	// TODO: 在此添加控件通知处理程序代码
	m_TmpLk[0].LkType=TYPEDBC;
	m_TmpLk[1].LkType=TYPEDBC;
	m_TmpLk[0].Pts.clear();
	m_TmpLk[1].Pts.clear();
	m_MouseState=SETLK;
}


void CWisdomLabelDlg::OnBnClickedCsetpoint4()
{
	// TODO: 在此添加控件通知处理程序代码
	m_MouseState=MOHU1;
}


void CWisdomLabelDlg::ReDrawItem(void)
{
	CRect Rc;
	GetClientRect(&Rc);
	Rc.left+=m_LeftWidth;
	Rc.right-=m_RightWidth;
	Rc.bottom-=26;
	InvalidateRect(Rc);
	UpdateWindow();
}


void CWisdomLabelDlg::OnBnClickedOpenpicDir()
{
	// TODO: 在此添加控件通知处理程序代码
	CMTextDlg Dlg1;
	Dlg1.Size=1;
	Dlg1.Title="请选择要标注的图片的目录";
	Dlg1.SaveTitle="hisimgdir";
	Dlg1.Titles[0]="请输入图片目录:";
	Dlg1.Texts[0]=m_FolderPath;
	Dlg1.bLl[0]=2;
	Dlg1.bSave[0]=1;
	Dlg1.TextWidth=800;
	//CFile fi;
	//CStringA HisDir=GetModuleDir()+"\\cfg\\hisdir.txt";
	//if(fi.Open(CString(HisDir),CFile::modeRead)){
	//	char Buf[4096];
	//	int size=fi.Read(Buf,4095);
	//	Buf[size]=0;
	//	vector<CStringA> List=SplitStr(CStringA(Buf),"\r\n");
	//	CStringA ss;
	//	for(int k=0;k<List.size();k++){
	//		ss=List.at(k);
	//	//while(fi.ReadString(ss)){
	//		ss.Replace("\r","");
	//		ss.Replace("\n","");
	//		Dlg1.Datas[0].AddTail(CString(ss));
	//	}
	//	fi.Close();
	//}
	if(Dlg1.DoModal()!=IDOK)
		return;
	OpenDir(Dlg1.Texts[0]);
	/*return;
	CFolderPickerDialog  Dlg(NULL,0,this,0);
	if(!Dlg.DoModal()==IDOK)
		return;
	OpenDir(Dlg.GetFolderPath());*/
	
}


void CWisdomLabelDlg::OnBnClickedButton9()
{
	// TODO: 在此添加控件通知处理程序代码
}


void CWisdomLabelDlg::UpdateImgFileList(void)
{
	/*m_pImgListCtrl.SetRedraw(FALSE);
	m_pImgListCtrl.DeleteAllItems();
	POSITION pos=m_PicList.GetHeadPosition();
	while(pos){
		CStringA Str=m_PicList.GetNext(pos);
		m_pImgListCtrl.InsertItem(m_pImgListCtrl.GetItemCount(),Str);
	}
	m_pImgListCtrl.SetRedraw();*/
}


void CWisdomLabelDlg::OnBnClickedCheck5()
{
	// TODO: 在此添加控件通知处理程序代码
	this->ReDrawItem();
}


void CWisdomLabelDlg::OnBnClickedCheck6()
{
	// TODO: 在此添加控件通知处理程序代码
	this->ReDrawItem();
}


void CWisdomLabelDlg::OnBnClickedIdcreatedate24()
{
	// TODO: 在此添加控件通知处理程序代码
	m_SrcRect=FRect(0,0,m_pImg->GetWidth(),m_pImg->GetHeight());
	ReDrawItem();
}


void CWisdomLabelDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
		CPoint ptSrc=ToSrc(point);
	if(m_MouseState==SETFACERECTFIRSTPOINT){
		//m_MouseState=GETFACERECTSECONDPOINT;
		m_GraphRect.left=ptSrc.x;//(float)(point.x-m_TarRect.left)*m_DrawScale;;
		m_GraphRect.top=ptSrc.y;//(float)(point.y-m_TarRect.top)*m_DrawScale;
		if(m_GraphRect.left<0)
			m_GraphRect.left=0;
		if(m_GraphRect.top<0)
			m_GraphRect.top=0;
		//m_GraphRect.right=point.x;
		//m_GraphRect.bottom=point.y;
		//m_GraphRect=ToZfx(m_GraphRect);
		if(m_GraphRect.left<0){
			m_GraphRect.OffsetRect(0-m_GraphRect.left,0);
		}if(m_GraphRect.right>=m_pImg->GetWidth()){
			m_GraphRect.OffsetRect(m_pImg->GetWidth()-m_GraphRect.right+1,0);
		}if(m_GraphRect.top<0){
			m_GraphRect.OffsetRect(0,0-m_GraphRect.top);
		}if(m_GraphRect.bottom>=m_pImg->GetHeight()){
			m_GraphRect.OffsetRect(0,m_pImg->GetHeight()-m_GraphRect.bottom+1);
		}
		m_TmpGraphRect=CRect(-10,-10,-10,-10);
		m_MouseState=SETFACERECTSECONDPOINT;
		//OnBnClickedSaveAnnotation();
		ReDrawItem();
	}else if(m_MouseState==SETFACERECTSECONDPOINT){
		if(m_CmdState==CMDNONE){

			static CStringA DxType="";
			CStringA tmp=GetWndTxtA(&m_MrBzBtn);
			
			if(tmp.IsEmpty()){
				CDxLxDlg dlg;
				dlg.m_MainDlg=this;
				dlg.m_LabMap=&m_LabMap;
				dlg.m_LabMap1=&m_LabMap1;
				dlg.m_DxType=DxType;
				if(dlg.DoModal()==IDOK){
					DxType=dlg.m_DxType;
				}
			}else
				DxType=tmp;
			{
					Lk lk;
				lk.LkType=TYPERECT;
				lk.DxType=DxType;
				lk.Pts.push_back(m_TmpGraphRect.TopLeft());
				lk.Pts.push_back(m_TmpGraphRect.BottomRight());
				m_LkList.push_back(lk);
				map<CStringA,CBitmap *>::iterator it=m_LabMap.find(DxType);
				if(it==m_LabMap.end()){
					AddLab(DxType);
				}
			}
			//OnBnClickedSaveAnnotation();
			//ReDrawItem();

			//m_MouseState=GETFACERECTFIRSTPOINT;
			//m_GraphRect.right=ptSrc.x;//(float)(point.x-m_TarRect.left)*m_DrawScale;
			//m_GraphRect.bottom=ptSrc.y;//(float)(point.y-m_TarRect.top)*m_DrawScale;
			//if(!m_pImg->IsNull()){
			//	if(m_GraphRect.right>m_pImg->GetWidth()-1)
			//		m_GraphRect.right=m_pImg->GetWidth()-1;
			//	if(m_GraphRect.bottom>m_pImg->GetHeight()-1)
			//		m_GraphRect.bottom=m_pImg->GetHeight()-1;
			//}
			////m_GraphRect=ToZfx(m_GraphRect);
			//if(m_GraphRect.left<0){
			//	m_GraphRect.OffsetRect(0-m_GraphRect.left,0);
			//}if(m_GraphRect.right>=m_pImg->GetWidth()){
			//	m_GraphRect.OffsetRect(m_pImg->GetWidth()-m_GraphRect.right+1,0);
			//}if(m_GraphRect.top<0){
			//	m_GraphRect.OffsetRect(0,0-m_GraphRect.top);
			//}if(m_GraphRect.bottom>=m_pImg->GetHeight()){
			//	m_GraphRect.OffsetRect(0,m_pImg->GetHeight()-m_GraphRect.bottom+1);
			//}
			//m_TmpGraphRect=CRect(-10,-10,-10,-10);
			m_MouseState=SETFACERECTFIRSTPOINT;
			m_TmpGraphRect=CRect(-10,-10,-10,-10);
			ReDrawItem();
			OnBnClickedSaveAnnotation();
		}else if(m_CmdState==CMDDELETEPIXEL){
			if(AfxMessageBoxA("您真的要删除这个区域的颜色吗？删除后将不能还原",MB_YESNO)==IDYES){
				DeletePixel(m_pImg,m_TmpGraphRect);
				m_pImg->Save(CString(m_PicPath));
				
			}
			m_MouseState=SETFACERECTFIRSTPOINT;
				m_TmpGraphRect=CRect(-10,-10,-10,-10);
				ReDrawItem();
		}
		
	}else if(m_MouseState==SETFACECENTER){
		//m_GraphRect=m_TmpGraphRect;
		static CStringA DxType="";
		CDxLxDlg dlg;
		dlg.m_MainDlg=this;
		dlg.m_LabMap=&m_LabMap;
		dlg.m_LabMap1=&m_LabMap1;
		dlg.m_DxType=DxType;
		if(dlg.DoModal()==IDOK){
			Lk lk;
			lk.LkType=TYPERECT;
			DxType=lk.DxType=dlg.m_DxType;
			lk.Pts.push_back(m_TmpGraphRect.TopLeft());
			lk.Pts.push_back(m_TmpGraphRect.BottomRight());
			m_LkList.push_back(lk);
			OnBnClickedSaveAnnotation();
			ReDrawItem();
			map<CStringA,CBitmap *>::iterator it=m_LabMap.find(DxType);
			if(it==m_LabMap.end()){
				AddLab(DxType);
			}
		}
	}
	//else if(m_MouseState==SET1POINT
	//	||m_MouseState==SET1POINT+1
	//	||m_MouseState==SET1POINT+2
	//	||m_MouseState==SET1POINT+3
	//	||m_MouseState==SET1POINT+4){
	//	m_Point[m_MouseState-SET1POINT].x=ptSrc.x;//(point.x-m_TarRect.left)*m_DrawScale;
	//	m_Point[m_MouseState-SET1POINT].y=ptSrc.y;//(point.y-m_TarRect.top)*m_DrawScale;
	//	m_MouseState++;
	//	if(m_MouseState>SET1POINT+4){
	//		m_MouseState=MOUSESTATENONE;
	//	}
	//	OnBnClickedSaveAnnotation();
	//	ReDrawItem();
	//}
	else if(m_MouseState==SETPOINT){
		//CPoint newPt(ptSrc);
		Lk lk;
		lk.LkType=TYPEPOINT;
		lk.Pts.push_back(ptSrc);
		m_LkList.push_back(lk);
		OnBnClickedSaveAnnotation();
		ReDrawItem();
	}
	else if(m_MouseState==SETDBX){
		CPoint newPt(ptSrc);
		if(newPt.x<1)
			newPt.x=1;
		if(newPt.y<=1)
			newPt.y=1;
		if(newPt.x>=m_pImg->GetWidth()-1)
			newPt.x=m_pImg->GetWidth()-2;
		if(newPt.y>=m_pImg->GetHeight()-1)
			newPt.y=m_pImg->GetHeight()-2;
		m_Dbx.push_back(newPt);
		ReDrawItem();
	}else if(m_MouseState==SETDBXMIN){
		CPoint newPt(ptSrc);
		if(newPt.x<1)
			newPt.x=1;
		if(newPt.y<=1)
			newPt.y=1;
		if(newPt.x>=m_pImg->GetWidth()-1)
			newPt.x=m_pImg->GetWidth()-2;
		if(newPt.y>=m_pImg->GetHeight()-1)
			newPt.y=m_pImg->GetHeight()-2;
		m_DbxMin.push_back(newPt);
		ReDrawItem();
	}
	else if(m_MouseState==SETLK){
		CPoint newPt(ptSrc);
		if(newPt.x<1)
			newPt.x=1;
		if(newPt.y<=1)
			newPt.y=1;
		if(newPt.x>=m_pImg->GetWidth()-1)
			newPt.x=m_pImg->GetWidth()-2;
		if(newPt.y>=m_pImg->GetHeight()-1)
			newPt.y=m_pImg->GetHeight()-2;
		m_TmpLk[0].Pts.push_back(newPt);
		ReDrawItem();
	}
	else if(m_MouseState==FGPICX){
		int cn=1;
		int w,h;
		unsigned char *pData=GetImgData(m_pImg,cn,w,h);
		unsigned char *pData1=CopyData(pData,w,h,0,0,ptSrc.x,h);
		unsigned char *pData2=CopyData(pData,w,h,ptSrc.x,0,w-ptSrc.x,h);
		CImage *pImg1=DataToImg(pData1,1,ptSrc.x,h);
		CImage *pImg2=DataToImg(pData2,1,w-ptSrc.x,h);
		pImg1->Save(CString(m_PicPath));
		CStringA Str=m_PicPath.Left(m_PicPath.GetLength()-4);
		Str+="1.jpg";
		pImg2->Save(CString(Str));
		int j=m_pImgListCtrl.InsertItem(GetSelID(),CString(""));
		m_pImgListCtrl.SetSelectItem(j);
		//m_PicList.AddHead(Str);
		pImg1->Destroy();
		pImg2->Destroy();
		delete []pData;
		delete []pData1;
		delete []pData2;
		delete pImg1;
		delete pImg2;
		OpenNewPic(m_PicPath);

	}
	else if(m_MouseState==FGPICY){
		int cn=1;
		int w,h;
		unsigned char *pData=GetImgData(m_pImg,cn,w,h);
		unsigned char *pData1=CopyData(pData,w,h,0,0,w,ptSrc.y);
		unsigned char *pData2=CopyData(pData,w,h,0,ptSrc.y,w,h-ptSrc.y);
		CImage *pImg1=DataToImg(pData1,1,w,ptSrc.y);
		CImage *pImg2=DataToImg(pData2,1,w,h-ptSrc.y);
		pImg1->Save(CString(m_PicPath));
		CStringA Str=m_PicPath.Left(m_PicPath.GetLength()-4);
		Str+="1.jpg";
		pImg2->Save(CString(Str));
		//m_PicList.AddHead(Str);
		int j=m_pImgListCtrl.InsertItem(GetSelID(),CString(""));
		m_pImgListCtrl.SetSelectItem(j);
		pImg1->Destroy();
		pImg2->Destroy();
		delete []pData;
		delete []pData1;
		delete []pData2;
		delete pImg1;
		delete pImg2;
		OpenNewPic(m_PicPath);

	}
	else if(m_MouseState==JUAN){
		COLORREF col=m_pImg->GetPixel(ptSrc.x,ptSrc.y);
		COLORREF col1=RGB(GetRValue(col)-5,GetGValue(col)-5,GetBValue(col)-5);
		m_pImg->SetPixel(ptSrc.x,ptSrc.y,col1);
		ReDrawItem();
	}else if(m_MouseState==SELECTDX){
		//BOOL bSel=0;
		/*if(m_LkList.size()>0){
			double MinDis=100000000;
			Lk MinLk;
			int MinIDLk=-1;
			int MinIDPt=-1;
			int MinIDLine=-1;
			for(int j=0;j<m_LkList.size();j++){
				Lk lk=m_LkList.at(j);
				if(lk.LkType!=TYPERECT)
					continue;
				for(int i=0;i<lk.Pts.size();i++){
					CPoint pt=ToTar(lk.Pts.at(i));
					int LineID=-1;
					double Dis=DisPt2Lk1(ptSrc,lk,LineID);
					if(Dis<MinDis){
						MinDis=Dis;
						MinIDLk=j;
						MinIDPt=i;
						MinLk=lk;
						MinIDLine=LineID;
					}
				}
			}
			for(int j=0;j<m_LkList.size();j++){
				Lk lk=m_LkList.at(j);
				if(lk.LkType==TYPERECT)
					continue;
				for(int i=0;i<lk.Pts.size();i++){
					CPoint pt=lk.Pts.at(i);
					double Dis=sqrt(pow((float)(pt.x-ptSrc.x),2)+pow((float)(pt.y-ptSrc.y),2));
					if(Dis<MinDis){
						MinDis=Dis;
						MinIDLk=j;
						MinIDPt=i;
						MinLk=lk;
					}
				}
			}
			//MinDis=sqrt(MinDis);
			MinDis/=m_DrawScale;
			if(MinIDLk!=-1 && MinDis<=m_SectPtW/2){
				Lk lk=m_LkList.at(MinIDLk);
				m_TmpLk[1]=lk;
				m_LkList.erase(m_LkList.begin()+MinIDLk);
				m_PtId=MinIDPt;
				m_SelLineID=MinIDLine;
				m_CurPt.x=-1;
				m_MouseState=GAIDIAN;
				ReDrawItem();
			}else{
				m_CurPt.x=-100;
				m_CurLk.Pts.clear();
			}
		}*/
			
		if(m_SelLkID>=0){
				Lk lk=m_LkList.at(m_SelLkID);
				m_TmpLk[1]=lk;
				m_LkList.erase(m_LkList.begin()+m_SelLkID);
				m_PtId=m_SelPtID;
				m_SelLineID=m_SelLineID;
				m_CurPt.x=-1;
				m_MouseState=GAIDIAN;
				ReDrawItem();
		}else{
				m_CurPt.x=-100;
				m_CurLk.Pts.clear();
		}
		/*for(int j=0;j<m_LkList.size() &&m_TmpLk[1].Pts.size()<=0;j++){
			Lk lk=m_LkList.at(j);
			for(int i=0;i<lk.Pts.size();i++){
				CPoint pt=ToTar(lk.Pts.at(i));
				CRect Rc(CPoint(pt.x-3,pt.y-3),CPoint(pt.x+3,pt.y+3));
				if(Rc.PtInRect(point)){
					m_TmpLk[1]=lk;
					m_LkList.erase(m_LkList.begin()+j);
					m_PtId=i;
					m_MouseState=GAIDIAN;
					ReDrawItem();
					break;
				}
			}
		}*/
	}
	else if(m_MouseState==GAIBIOQIAN){
		if(m_LkList.size()>0){
			double MinDis=100000000;
			Lk MinLk;
			int MinIDJ=-1;
			int MinIDI=-1;
			for(int j=0;j<m_LkList.size();j++){
				Lk lk=m_LkList.at(j);
					int LinkId=-1;
					int Dis=DisPt2Lk(ptSrc,lk,LinkId);//(pt.x-point.x)*(pt.x-point.x)+(pt.y-point.y)*(pt.y-point.y);
					if(Dis<MinDis){
						MinDis=Dis;
						MinIDJ=j;
						//MinIDI=i;
						MinLk=lk;
					}
				
			}
			MinDis/=m_DrawScale;
			if(MinIDJ!=-1 && MinDis<=m_SectPtW/2){
				Lk lk=m_LkList.at(MinIDJ);
				CDxLxDlg dlg;
				dlg.m_MainDlg=this;
				dlg.m_LabMap=&m_LabMap;
				dlg.m_LabMap1=&m_LabMap1;
				dlg.m_DxType=lk.DxType;
				if(dlg.DoModal()==IDOK){
					//lk.DxType=dlg.m_DxType;
					m_LkList[MinIDJ].DxType=dlg.m_DxType;
					OnBnClickedSaveAnnotation();
					/*m_TmpLk[1]=lk;
					m_LkList.erase(m_LkList.begin()+j);
					m_PtId=i;
					m_MouseState=GAIDIAN;*/
					m_CurLk.Pts.clear();
					ReDrawItem();
					map<CStringA,CBitmap *>::iterator it=m_LabMap.find(dlg.m_DxType);
					if(it==m_LabMap.end()){
						AddLab(dlg.m_DxType);
					}
					//break;
				}
				
			}else{
				m_CurPt.x=-100;
				m_CurLk.Pts.clear();
			}
		}
	}else if(m_MouseState==ERZHIHUA){
		if(m_LkList.size()>0){
			double MinDis=100000000;
			Lk MinLk;
			int MinIDJ=-1;
			int MinIDI=-1;
			for(int j=0;j<m_LkList.size();j++){
				Lk lk=m_LkList.at(j);
					int LineID=-1;
					int Dis=DisPt2Lk(ptSrc,lk,LineID);//(pt.x-point.x)*(pt.x-point.x)+(pt.y-point.y)*(pt.y-point.y);
					if(Dis<MinDis){
						MinDis=Dis;
						MinIDJ=j;
						//MinIDI=i;
						MinLk=lk;
					}
				
			}
			MinDis/=m_DrawScale;
			if(MinIDJ!=-1 && MinDis<=m_SectPtW/2){
				//m_LkList.erase(m_LkList.begin()+MinIDJ);
				int cn=1;
				int w,h;
				unsigned char *pData=GetImgData(m_pImg,cn,w,h);
				//for(int i=0;i<m_LkList.size();i++){
					Lk lk=m_LkList.at(MinIDJ);
					if(lk.LkType==TYPERECT&&lk.Pts.size()>1)
					{
						CRect rc(lk.Pts.at(0),lk.Pts.at(1));
						shuipinjunzhi(pData,w,h,rc.left,rc.top,rc.right,rc.bottom);
						TwoValue(pData,w,h,rc.left,rc.top,rc.right,rc.bottom,20);
					}
				//}
	
				m_pImg->Destroy();
				delete m_pImg;
				m_pImg=DataToImg(pData,cn,w,h);
				delete []pData;
				//ReDrawItem();


				m_CurPt.x=-100;
				//OnBnClickedSaveAnnotation();
				m_CurLk.Pts.clear();
				ReDrawItem();
			}else{
				m_CurPt.x=-100;
				m_CurLk.Pts.clear();
			}
		}
	}
	else if(m_MouseState==DELETEDX){
		if(m_LkList.size()>0){
			double MinDis=100000000;
			Lk MinLk;
			int MinIDJ=-1;
			int MinIDI=-1;
			for(int j=0;j<m_LkList.size();j++){
				Lk lk=m_LkList.at(j);
					int LineID=-1;
					int Dis=DisPt2Lk(ptSrc,lk,LineID);//(pt.x-point.x)*(pt.x-point.x)+(pt.y-point.y)*(pt.y-point.y);
					if(Dis<MinDis){
						MinDis=Dis;
						MinIDJ=j;
						//MinIDI=i;
						MinLk=lk;
					}
				
			}
			MinDis/=m_DrawScale;
			if(MinIDJ!=-1 && MinDis<=m_SectPtW/2){
				m_LkList.erase(m_LkList.begin()+MinIDJ);
				m_CurPt.x=-100;
				OnBnClickedSaveAnnotation();
				m_CurLk.Pts.clear();
				ReDrawItem();
			}else{
				m_CurPt.x=-100;
				m_CurLk.Pts.clear();
			}
		}
	}
	else if(m_MouseState==GAIDIAN){
		
		m_SelLineID=-1;
		m_LkList.push_back(m_TmpLk[1]);
		m_TmpLk[1].Pts.clear();
		m_TmpLk[0].Pts.clear();
		m_MouseState=SELECTDX;
		OnBnClickedSaveAnnotation();
		ReDrawItem();
	}else if(m_MouseState==MOHU1){
		m_MouseState=MOHU2;
		SetTimer(1,100,NULL);
	}
	else if(m_MouseState==MOHU2){
		KillTimer(1);
		m_MouseState=MOHU1;
	}else if(m_MouseState == SETCOPYIMGPOS){
		if(m_CropImg&&!m_CropImg->IsNull()){
			
			m_CropNewPos=ptSrc;
			float cScale=(float)m_pImg->GetWidth()/(float)m_CopyOrWidth;
			if(m_pImg->GetWidth()==m_CopyOrWidth){
					Fugai(m_pImg,m_CropImg,m_CropMb,ptSrc.x,ptSrc.y);
			}
			else{
					
					
					int ow,oh;
					unsigned char *od=GetImgData(m_CropImg,3,ow,oh);
					int tw=(float)ow*cScale;
					int th=(float)oh*cScale;
					unsigned char *d=new unsigned char[3*tw*th];
					float *mb=new float[tw*th];
					ScaleData(m_CropMb,ow,oh,1,mb,tw,th);
					ScaleData(od,ow, oh ,3,d, tw, th );
					CImage *tImg=DataToImg(d,3,tw,th);
					Fugai(m_pImg,tImg,mb,ptSrc.x,ptSrc.y);
					tImg->Destroy();
					delete tImg;
					delete []od;
					delete []d;
			}
			//Fugai(m_pImg,m_CropImg,ptSrc.x,ptSrc.y);
			//复制标注
			m_pImg->Save(CString(m_PicPath));
			for(int j=0;j<m_FzList.size();j++){
				Lk lk=m_FzList.at(j);
				CPoint ofpt=m_CropNewPos;//-m_CropPos;
				vector<CPoint >::iterator it=lk.Pts.begin();
				for(;it!=lk.Pts.end();it++){
					it->x=cScale*it->x;
					it->y=cScale*it->y;
					it->Offset(ofpt);
					
				}
				m_LkList.push_back(lk);
			}
			OnBnClickedSaveAnnotation();
			if(m_CmdState==0){
				
				//m_MouseState=SETCOPYBZDX;
			}
			
			ReDrawItem();
		}
	}
	else if(m_MouseState == SELCROPDBC){
		if(m_LkList.size()>0){
			double MinDis=100000000;
			Lk MinLk;
			int MinIDJ=-1;
			int MinIDI=-1;
			for(int j=0;j<m_LkList.size();j++){
				Lk lk=m_LkList.at(j);
					int LineID=-1;
					int Dis=DisPt2Lk(ptSrc,lk,LineID);//(pt.x-point.x)*(pt.x-point.x)+(pt.y-point.y)*(pt.y-point.y);
					if(Dis<MinDis){
						MinDis=Dis;
						MinIDJ=j;
						//MinIDI=i;
						MinLk=lk;
					}
				
			}
			MinDis/=m_DrawScale;
			if(MinIDJ!=-1 && MinDis<=m_SectPtW/2){
				
				Lk lk=m_LkList.at(MinIDJ);
				
				m_CropLkList.push_back(lk);
				
				m_CurPt.x=-100;
				OnBnClickedSaveAnnotation();
				m_CurLk.Pts.clear();
				ReDrawItem();
			}else{
				m_CurPt.x=-100;
				m_CurLk.Pts.clear();
			}
		}
	}
	else if(m_MouseState == SETCOPYBZDX){
		if(m_LkList.size()>0){
			double MinDis=100000000;
			Lk MinLk;
			int MinIDJ=-1;
			int MinIDI=-1;
			for(int j=0;j<m_LkList.size();j++){
				Lk lk=m_LkList.at(j);
					int LineID=-1;
					int Dis=DisPt2Lk(ptSrc,lk,LineID);//(pt.x-point.x)*(pt.x-point.x)+(pt.y-point.y)*(pt.y-point.y);
					if(Dis<MinDis){
						MinDis=Dis;
						MinIDJ=j;
						//MinIDI=i;
						MinLk=lk;
					}
				
			}
			MinDis/=m_DrawScale;
			if(MinIDJ!=-1 && MinDis<=m_SectPtW/2){
				
				Lk lk=m_LkList.at(MinIDJ);
				for(vector<CPoint>::iterator it=lk.Pts.begin();it!=lk.Pts.end();it++){
					it->x-=m_CropPos.x;
					it->y-=m_CropPos.y;
				}
				m_FzList.push_back(lk);
				
				m_CurPt.x=-100;
				OnBnClickedSaveAnnotation();
				m_CurLk.Pts.clear();
				ReDrawItem();
			}else{
				m_CurPt.x=-100;
				m_CurLk.Pts.clear();
			}
		}
	}
	CDialogEx::OnLButtonDown(nFlags, point);
}
vector<CStringA> GetObjs(CStringA Str,CStringA BeginStr,CStringA EndStr){
	vector<CStringA> Objs;
	int pos=0;
	while((pos=Str.Find(BeginStr,pos))!=-1){
				pos+=BeginStr.GetLength();
				int PosStart=pos;
				pos=Str.Find(EndStr,pos);
				CStringA newStr=Str.Mid(PosStart).Left(pos-PosStart);
				Objs.push_back(newStr);
				pos+=EndStr.GetLength();
	}
	return Objs;
}

CStringA GetObj(CStringA Str,CStringA BeginStr,CStringA EndStr){
	int pos=0;
	if((pos=Str.Find(BeginStr,pos))!=-1){
				pos+=BeginStr.GetLength();
				int PosStart=pos;
				pos=Str.Find(EndStr,pos);
				CStringA newStr=Str.Mid(PosStart).Left(pos-PosStart);
				newStr.Trim();
				return newStr;
				//pos+=EndStr.GetLength();
	}
	return "";
}
void CWisdomLabelDlg::OpenXml(CStringA xmlName,vector<Lk >*lks)
{
	CStringA xmlPath=xmlName;//GetXmlFileName();
	CFile fi;
	if(!fi.Open(CString(xmlPath),CFile::modeRead))
		return;
	char Buf[64000];
	int Size=fi.Read(Buf,64000);
	fi.Close();
	Buf[Size]=0;
	CStringA Str(Buf);

	vector<CStringA> Objs=GetObjs(Str,"<object>","</object>");
	for(int i=0;i<Objs.size();i++){
		CStringA Name=GetObj(Objs.at(i),"<name>","</name>");
		if(!m_LabMap2[Name].IsEmpty()){
			Name=m_LabMap2[Name];
		}
		CStringA bndbox=GetObj(Objs.at(i),"<bndbox>","</bndbox>");
		CStringA iscopy=GetObj(Objs.at(i),"<iscopy>","</iscopy>");
		CStringA spline=GetObj(Objs.at(i),"<spline>","</spline>");
		CStringA polygon=GetObj(Objs.at(i),"<polygon>","</polygon>");
		CStringA point=GetObj(Objs.at(i),"<point>","</point>");
		if(!bndbox.IsEmpty()){
			CStringA xmin=GetObj(bndbox,"<xmin>","</xmin>");
			CStringA ymin=GetObj(bndbox,"<ymin>","</ymin>");
			CStringA xmax=GetObj(bndbox,"<xmax>","</xmax>");
			CStringA ymax=GetObj(bndbox,"<ymax>","</ymax>");
			Lk lk;
			lk.DxType=Name;
			lk.LkType=TYPERECT;
			lk.isCopy=atoi(iscopy);
			lk.Pts.push_back(CPoint(atoi(xmin),atoi(ymin)));
			lk.Pts.push_back(CPoint(atoi(xmax),atoi(ymax)));
			lks->push_back(lk);
		}else if(!spline.IsEmpty()){
			vector<CStringA> values=GetObjs(spline,"<value>","</value>");
			Lk lk;
			lk.DxType=Name;
			lk.isCopy=atoi(iscopy);
			lk.LkType=TYPESPLINE;
			for(int j=0;j<values.size();j+=2){
				lk.Pts.push_back(CPoint(atoi(values.at(j)),atoi(values.at(j+1))));
			}
			lks->push_back(lk);
		}else if(!polygon.IsEmpty()){
			vector<CStringA> values=GetObjs(polygon,"<value>","</value>");
			Lk lk;
			lk.DxType=Name;
			lk.isCopy=atoi(iscopy);
			lk.LkType=TYPEDBC;
			for(int j=0;j<values.size();j+=2){
				lk.Pts.push_back(CPoint(atoi(values.at(j)),atoi(values.at(j+1))));
			}
			lks->push_back(lk);
		}else if(!point.IsEmpty()){
			vector<CStringA> values=GetObjs(point,"<value>","</value>");
			Lk lk;
			lk.DxType=Name;
			lk.isCopy=atoi(iscopy);
			lk.LkType=TYPEPOINT;
			for(int j=0;j<values.size();j+=2){
				lk.Pts.push_back(CPoint(atoi(values.at(j)),atoi(values.at(j+1))));
			}
			lks->push_back(lk);
		}
		//bndbox
	}

}


CStringA CWisdomLabelDlg::GetXmlFileName(void)
{
	//AfxMessageBoxA(this->m_LabelPath+"\\"+this->m_PicTitle+".xml");
	if(this->m_LabelPath=="")
		return this->m_PicTitle+".xml";
	return this->m_LabelPath+"\\"+this->m_PicTitle+".xml";
}


void CWisdomLabelDlg::OnBnClickedOpenpic17()
{
	// TODO: 在此添加控件通知处理程序代码
	int id=GetSelID();
	m_pImgListCtrl.DeleteItem(id);
	m_pImgListCtrl.SetSelectItem(id);
	//OpenNewPic(m_PicList.GetHead());
}


void CWisdomLabelDlg::OnBnClickedIdcreatedate2()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_pImg==NULL)
		return;
	static float bs=1.25;
	static BOOL bSave=0;
	if(!m_IsPCl){
		CMTextDlg dlg;

		dlg.Title="亮度设置";
		dlg.Size=2;
		dlg.Titles[0]="请输入亮度倍率:";
		dlg.Titles[1]="是否保存(0：不保存，1：保存):";
		dlg.Texts[0]=FloatToStr(bs);
		dlg.Texts[1]=IntToStr(bSave);
		if(dlg.DoModal()!=IDOK)
			return;

		bs=atof(dlg.Texts[0]);
		bSave=atoi(dlg.Texts[1]);
	}
	int c=3;
	int w,h;
	unsigned char *data=GetImgData(m_pImg,c,w,h);
	for(int cn=0;cn<c;cn++){
		int posc=cn*w*h;
		for(int j=0;j<h;j++){
			int posy=posc+j*w;
			for(int i=0;i<w;i++){
				int pos=posy+i;
				float v=data[pos];
				v*=bs;
				if(v>254)
					v=254;
				data[pos]=0.5+v;
			}
		}
	}
	m_pImg->Destroy();
	delete m_pImg;
	m_pImg=DataToImg(data,c,w,h);
	ReDrawItem();
	delete []data;
	if(bSave)
		saveImg(m_PicPath,m_pImg);
	//if(bSave)
		//Save
	//LcHz="_ldlc.jpg";
}


void CWisdomLabelDlg::OnBnClickedIdcreatedate18()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_pImg==NULL)
		return;
	static float bs=0.8;
	static BOOL bSave=0;
	if(!m_IsPCl){
		CMTextDlg dlg;

		dlg.Title="亮度设置";
		dlg.Size=2;
		dlg.Titles[0]="请输入亮度倍率:";
		dlg.Titles[1]="是否保存(0：不保存，1：保存):";
		dlg.Texts[0]=FloatToStr(bs);
		dlg.Texts[1]=IntToStr(bSave);
		if(dlg.DoModal()!=IDOK)
			return;

		bs=atof(dlg.Texts[0]);
		bSave=atoi(dlg.Texts[1]);
	}
	int c=3;
	int w,h;
	unsigned char *data=GetImgData(m_pImg,c,w,h);
	for(int cn=0;cn<c;cn++){
		int posc=cn*w*h;
		for(int j=0;j<h;j++){
			int posy=posc+j*w;
			for(int i=0;i<w;i++){
				int pos=posy+i;
				float v=data[pos];
				v*=bs;
				if(v>254)
					v=254;
				data[pos]=0.5+v;
			}
		}
	}
	m_pImg->Destroy();
	delete m_pImg;
	m_pImg=DataToImg(data,c,w,h);
	ReDrawItem();
	delete []data;
	if(bSave)
		saveImg(m_PicPath,m_pImg);
}

 #include "Video2JpgSetting.h"
void CWisdomLabelDlg::OnBnClickedVideoToOnePic()
{
	// TODO: 在此添加控件通知处理程序代码
//	CVideo2JpgSetting Dlg;
//	Dlg.DoModal();
//	AfxMessageBoxA(Dlg.m_VideoDir);
	/*CFileDialog dlg(TRUE,CString("avi"),NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,CString("(*.avi;*.mp4)|*.avi;*.mp4||")); 
	dlg.m_ofn.lpstrTitle=CString("请选择视频文件");
	if(dlg.DoModal() != IDOK)
		return;*/
	CMTextDlg pDlg;
	pDlg.Size=3;
	pDlg.Title="请选择时间";
	pDlg.Titles[0]="请选择视频路径:";
	pDlg.bLl[0]=1;
	pDlg.Titles[1]="请选择时间(秒):";
	pDlg.Texts[1]="1";
	pDlg.Titles[2]="请输入图片名称:";
	pDlg.Texts[2]="picname";
	if(pDlg.DoModal()!=IDOK)
		return;
	CStringA strPath=GetModuleDir()+"\\ffmpeg\\";
	CStringA OutPath=GetModuleDir()+"\\"+CStringA(pDlg.Texts[2])+".jpg";//tmp.jpg";
		//ffmpeg -i test.asf -y -f image2 -t 0.001 -s 352x240 a.jpg
	//ffmpeg -i \"%s\" -r 15 -f image2 %d-%s.jpg
	CStringA cmdLine=strPath+"ffmpeg.exe -i "+CStringA(pDlg.Texts[0])+" -y -f image2 -t "+CStringA(pDlg.Texts[1])+" "+OutPath;
	//CStringA cmdLine=strPath+"ffmpeg.exe -i "+dlg.GetPathName()+" -y -f image2 -t 0.001 "+OutPath;
	STARTUPINFO si={sizeof(si)};
	PROCESS_INFORMATION pif;
		si.dwFlags=STARTF_USESHOWWINDOW;
		si.wShowWindow=FALSE;//TRUE表示显示创建的进程的窗口
		BOOL bRet=::CreateProcess(
						NULL,//TEXT("c://programfiles//internetexplorer//iexplore.exe"),
						CString(cmdLine).GetBuffer(0),//cmdline,//在Unicode版本中此参数不能为常量字符串，因为此参数会被修改
						NULL,
						NULL,
						FALSE,
						CREATE_NEW_CONSOLE,
						NULL,
						CString(strPath),
						&si,
						&pif);
	
	if(bRet)
	{
		WaitForSingleObject( pif.hProcess, INFINITE );
		::CloseHandle(pif.hThread);
		::CloseHandle(pif.hProcess);
	}
	else
	{
		//int error=GetLastError();
		AfxMessageBoxA("获取视频的图片失败");
		return;
	}
	OpenNewPic(OutPath);
} 
/*****
功能：扩大或裁剪图片
函数名：CreateNewdat
pOldData:原始数据
w:原始宽度
h:原始高度
nw:新的宽度
nh:新的高度
返回：新的数据
******/
//unsigned char *CreateNewdat(unsigned char *pOldData,int w,int h,int nw,int nh){
//
//}
void CWisdomLabelDlg::OnBnClickedButton12()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_pImg==NULL)
		return;
	CPicEnlargeDlg Dlg;
	if(Dlg.DoModal()!=IDOK)
		return;

	int w=0;
	int h=0;
	unsigned char *pOldData=GetImgData(m_pImg,3,w,h);
	int nw=w+Dlg.m_LeftEnlarge+Dlg.m_RightEnlarge;
	int nh=h+Dlg.m_TopEnlarge+Dlg.m_BottomEnlarge;
	unsigned char *pNewData=new unsigned char[nw*nh*3];
	memset(pNewData,128,nw*nh*3);
	/*for(int j=
	int a=3;*/
	for(int k=0;k<3;k++){
		for(int j=0;j<h;j++){
			for(int i=0;i<w;i++){
				int pos=w*h*k+j*w+i;
				int jj=j+Dlg.m_TopEnlarge;
				int ii=i+Dlg.m_LeftEnlarge;
				if(jj<0||jj>=nh)
					continue;
				if(ii<0||ii>=nw)
					continue;
				int posnew=nw*nh*k+jj*nw+ii;
				pNewData[posnew]=pOldData[pos];
			}
		}
	}

	m_pImg->Destroy();
	m_pImg=DataToImg(pNewData,3,nw,nh);
	m_SrcRect=FRect(0,0,m_pImg->GetWidth(),m_pImg->GetHeight());
	ReDrawItem();
	m_ChangePic=1;
}


void CWisdomLabelDlg::OnBnClickedButton13()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_pImg==NULL)
		return;
	CScaleEnlargeDlg Dlg;
	if(Dlg.DoModal()!=IDOK)
		return;
	
	int w=0;
	int h=0;
	unsigned char *pOldData=GetImgData(m_pImg,3,w,h);

	int nw=w*Dlg.m_XScale/100.0;
	int nh=h*Dlg.m_YScale/100.0;
	int left=(nw-w)/2;
	int top=(nh-h)/2;
	unsigned char *pNewData=new unsigned char[nw*nh*3];
	memset(pNewData,128,nw*nh*3);
	/*for(int j=
	int a=3;*/
	for(int k=0;k<3;k++){
		for(int j=0;j<h;j++){
			for(int i=0;i<w;i++){
				int pos=w*h*k+j*w+i;
				int jj=j+top;
				int ii=i+left;
				if(jj<0||jj>=nh)
					continue;
				if(ii<0||ii>=nw)
					continue;
				int posnew=nw*nh*k+jj*nw+ii;
				pNewData[posnew]=pOldData[pos];
			}
		}
	}

	m_pImg->Destroy();
	m_pImg=DataToImg(pNewData,3,nw,nh);
	m_SrcRect=FRect(0,0,m_pImg->GetWidth(),m_pImg->GetHeight());
	for(int i=0;i<m_LkList.size();i++){
			Lk lk=m_LkList.at(i);
			for(int j=0;j<lk.Pts.size();j++){
				CPoint pt=lk.Pts.at(j);
				/*pt.x*=100.0/(float)Dlg.m_XScale;
				pt.y*=100.0/(float)Dlg.m_YScale;*/
				pt.Offset(left,top);
				vector<Lk>::iterator it0=m_LkList.begin()+i;
				vector<CPoint >::iterator it1=it0->Pts.begin()+j;
				it1->x=pt.x;
				it1->y=pt.y;
			}
	}
	ReDrawItem();
	delete []pOldData;
	delete []pNewData;
	//m_ChangePic=1;
	LcHz="_gdlc.jpg";
	OnBnClickedOpenpic11();
}


void CWisdomLabelDlg::OnBnClickedButton15()
{
	// TODO: 在此添加控件通知处理程序代码
	CRotateSettingDlg Dlg;
	if(Dlg.DoModal()!=IDOK)
		return;
	int a=Dlg.m_Angle;
}



void CWisdomLabelDlg::OnBnClickedQuan4()
{
	// TODO: 在此添加控件通知处理程序代码
	int t=0;
	for(int j=0;j<m_pImgListCtrl.GetItemCount();j++){

		int id=j;//GetSelID();
		//if(id>=m_pImgListCtrl.GetItemCount())
		//	break;
		CStringA PicName=CStringA(m_pImgListCtrl.GetItemText(id,1));
		int w,h,c;
		unsigned char *data=loadImage(m_PicPath,&w,&h,&c);
		if(data)
			free(data);
		else{
			CImage img;
			img.Load(CString(PicName));
			img.Save(CString(PicName));
			//m_pImg->Save(m_PicPath);//AfxMessageBoxA(m_PicPath);
			t++;
		}
	}
	AfxMessageBoxA(IntToStr(t));
}


void CWisdomLabelDlg::OnBnClickedQuan5()
{
	// TODO: 在此添加控件通知处理程序代码
	CLabeCropDlg Dlg;
	Dlg.m_Scale=200;
	if(Dlg.DoModal()!=IDOK)
		return;
	OnBnClickedOpenpic5();
	CStringA StartName=m_PicPath;
	while(1){
		
		for(int i=0;i<m_LkList.size();i++){
			Lk lk=m_LkList.at(i);
			if(lk.DxType != CStringA(Dlg.m_Label))
				continue;
			CRect Rc(lk.Pts.at(0),lk.Pts.at(1));
			int nw=Rc.Width()*Dlg.m_Scale/100;
			int nh=Rc.Height()*Dlg.m_Scale/100;
			int xOffset=(nw-Rc.Width())/2;
			int yOffset=(nh-Rc.Height())/2;
			Rc.left-=xOffset;
			Rc.right+=xOffset;
			Rc.top-=yOffset;
			Rc.bottom+=yOffset;
			if(Rc.Width()>Rc.Height()){
				int y=(Rc.Width()-Rc.Height())/2;
				Rc.top-=y;
				Rc.bottom+=y;
				
			}else if(Rc.Width()<Rc.Height()){
				int x=(-Rc.Width()+Rc.Height())/2;
				Rc.left-=x;
				Rc.right+=x;
			
			}	
			if(Rc.left<0)
					Rc.left=0;
			if(Rc.right>=m_pImg->GetWidth())
					Rc.right=m_pImg->GetWidth()-1;
			if(Rc.top<0)
					Rc.top=0;
			if(Rc.bottom>=m_pImg->GetHeight())
					Rc.bottom=m_pImg->GetHeight()-1;
			int w,h;
			unsigned char *data=GetImgData(m_pImg,3,w,h);
			unsigned char *data1=CopyData3(data,w,h,3,Rc.left,Rc.top,Rc.Width(),Rc.Height());
			CImage *pImg=DataToImg(data1,3,Rc.Width(),Rc.Height());
			delete []data;
			delete []data1;
			static int id=0;
			
			vector<Lk >tmpLkList;
			int k=0;
			for(int j=0;j<m_LkList.size();j++){
				/*if(j == i)
					k++;*/
				Lk lkj=m_LkList.at(j);
				//if(lkj.DxType==lk.DxType){
				//	//k++;
				//	continue;
				//}
				CRect Rcj(lkj.Pts.at(0),lkj.Pts.at(1));
				CRect Rcis;

				if(Rcis.IntersectRect(Rc,Rcj)){
					if(Rcis.Width()*Rcis.Height()*100/(Rcj.Width()*Rcj.Height())>40){
						Lk lkn=lkj;
						lkn.Pts.clear();
						for(int k=0;k<lkj.Pts.size();k++){
							CPoint pt=lkj.Pts.at(k);
							pt.Offset(-Rc.left,-Rc.top);
							lkn.Pts.push_back(pt);
						
						}
						tmpLkList.push_back(lkn);
						if(lkj.DxType==lk.DxType){
							k++;
						}
					}
				}
			}
			if((Dlg.m_OnlyHasChild &&tmpLkList.size()>k)
				||!Dlg.m_OnlyHasChild){
				CStringA Str,Str1;
				Str.Format("%s\\%d.jpg",Dlg.m_Path,id);
				Str1.Format("",Dlg.m_Path.Right(Dlg.m_Path.GetLength()-Dlg.m_Path.ReverseFind('\\')-1));
				pImg->Save(CString(Str));
				
				
				CStringA StrXml;
				StrXml.Format("%s\\%d.xml",Dlg.m_Path,id++);
				SaveXml(StrXml,Str1,Str,CStringA(Dlg.m_Path),pImg->GetWidth(),pImg->GetHeight(),pImg->GetBPP()/8,&tmpLkList);
				pImg->Destroy();
				delete pImg;
			}
		
		}


		OnBnClickedOpenpic5();
		if(m_PicPath==StartName)
			break;
	}

}


void CWisdomLabelDlg::OnBnClickedIdcreatedate25()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_AutoBzDxName.IsEmpty()||m_AutoBzPicSize.IsEmpty()
		||m_AutoBzModelP.IsEmpty()||m_AutoBzModelB.IsEmpty()){
			AfxMessageBoxA("请先设置自动标注参数");
	}

	CStringA strPath=GetModuleDir()+"\\ffmpeg\\";
	//.\a 512 .\model\model2 .\model\model3  phone,smoke,face,sideface,openmouth,closeeye
	CStringA cmdLine=strPath+"autobz.exe "+m_FolderPath+" "+m_AutoBzSizeType+" " +m_AutoBzPicSize +" "+m_AutoBzModelP+" "+m_AutoBzModelB+" "+m_AutoBzDxName;
	//CStringA cmdLine=strPath+"ffmpeg.exe -i "+dlg.GetPathName()+" -y -f image2 -t 0.001 "+OutPath;
	CTextDlg dlg2;
	dlg2.m_Text=cmdLine;
	dlg2.DoModal();
	STARTUPINFO si={sizeof(si)};
	PROCESS_INFORMATION pif;
		si.dwFlags=STARTF_USESHOWWINDOW;
		si.wShowWindow=TRUE;//TRUE表示显示创建的进程的窗口
		BOOL bRet=::CreateProcess(
						NULL,//TEXT("c://programfiles//internetexplorer//iexplore.exe"),
						CString(cmdLine).GetBuffer(0),//cmdline,//在Unicode版本中此参数不能为常量字符串，因为此参数会被修改
						NULL,
						NULL,
						FALSE,
						CREATE_NEW_CONSOLE,
						NULL,
						CString(strPath),
						&si,
						&pif);
	
	if(bRet)
	{
		WaitForSingleObject( pif.hProcess, INFINITE );
		::CloseHandle(pif.hThread);
		::CloseHandle(pif.hProcess);
	}
	else
	{
		//int error=GetLastError();
		AfxMessageBoxA("自动标注失败");
		return;
	}
	//OpenDir(dlg1.GetPathName());

}


void CWisdomLabelDlg::OnBnClickedQuan6()
{
	// TODO: 在此添加控件通知处理程序代码
	CStringA xmlPath=GetXmlFileName();
	CFile fi;
	if(!fi.Open(CString(xmlPath),CFile::modeRead))
		return;
	char Buf[64000];
	int Size=fi.Read(Buf,64000);
	fi.Close();
	Buf[Size]=0;
	CStringA Str(Buf);
	CTextDlg Dlg;
	Dlg.m_Text=Str;
	Dlg.DoModal();
}


void CWisdomLabelDlg::InitLab(void)
{
	m_LabMap.clear();
	m_LabMap1.clear();
	m_LabMap2.clear();
	CStdioFile fi;
	if(fi.Open(CString(GetModuleDir()+"\\cfg\\itemsname.txt"),CFile::modeRead)){
		CString ss;
		while(fi.ReadString(ss)){
			ss.Trim();
			AddLab(CStringA(ss));
			//m_FyTjBox.AddString(ss);
		}
		fi.Close();
	}
	m_BlSxXBtn.SetWindowText(CString("全部"));
}


void CWisdomLabelDlg::OnBnClickedIdcreatedate27()
{
	// TODO: 在此添加控件通知处理程序代码
	/*CFolderPickerDialog  Dlg(NULL,0,this,0);// Dlg(TRUE,NULL,NULL,NULL,NULL,NULL);
	Dlg.m_ofn.lpstrTitle="请选择标注目录";
	if(Dlg.DoModal()!=IDOK)
		return;
	CStringA BzDir=Dlg.GetPathName();*/
	CStringA BzDir=m_LabelPath;
	CMTextDlg dlg;
	dlg.Title="标签替换";
	dlg.Size=3;
	dlg.Titles[0]="请输入旧标签:";
	dlg.Titles[1]="请输入新标签:";
	dlg.Titles[2]="请输入标注目录:";
	dlg.Texts[2]=BzDir;
	dlg.bLl[2]=2;
	dlg.TextWidth=800;
	if(dlg.DoModal()!=IDOK)
		return;
	BzDir=dlg.Texts[2];
	CStringA biaoqian=dlg.Texts[0];
	CStringA  newbiaoqian=dlg.Texts[1];
		
	char Buf[64000];
	CFileFind ff;
	int res=ff.FindFile(CString(BzDir)+"\\*.*");
	while(res){
		res=ff.FindNextFile();
		if(ff.IsDots() || ff.IsDirectory())
			continue;
		CStringA FilePath=CStringA(ff.GetFilePath());
		//if(FilePath==m_PicPath)
		//	continue;
		CStringA FileName=CStringA(ff.GetFileName());
		CStringA FileTitle=CStringA(ff.GetFileTitle());
		CStringA hz=FilePath.Right(FileName.GetLength()-FileTitle.GetLength()-1);
		if(hz.CompareNoCase("xml")==0){
			CFile fi;
			if(!fi.Open(CString(FilePath),CFile::modeRead))
				continue;
			int Size=fi.Read(Buf,64000);
			fi.Close();
			
			Buf[Size]=0;
			CStringA Str(Buf);
			CStringA StrBf=Str;
			
			int bFind=0;
			int pos=0;
			CStringA Fs="<object>";
			while((pos=Str.Find(Fs))!=-1){
				pos+=Fs.GetLength();
				Str=Str.Mid(pos);
				CStringA Fs1="<name>";
				pos=Str.Find(Fs1);
				if(pos!=-1){
					pos+=Fs1.GetLength();
					Str=Str.Mid(pos);
					int pos2=Str.Find("</name>");
					if(pos2!=-1){
						CStringA Name=Str.Left(pos2);

						if(Name==biaoqian){
						
							bFind=1;
							break;
						}
					}
				}
			}
			if(bFind){
				StrBf.Replace("<name>"+biaoqian+"</name>","<name>"+newbiaoqian+"</name>");
				
				
				CFile f;
				if(f.Open(CString(FilePath),CFile::modeCreate|CFile::modeWrite)){
					f.Write(StrBf,StrBf.GetLength());
					f.Close();
				}
			}
		}
		//vehicle
		//CStringA hz1=hz;
	}
}
 

void CWisdomLabelDlg::OnBnClickedIdcreatedate28()
{
	// TODO: 在此添加控件通知处理程序代码
	CFileDialog Dlg(TRUE,NULL,NULL,NULL,NULL,NULL);
	if(Dlg.DoModal()!=IDOK){
		return;
	}
	CMTextDlg dlg;
	dlg.Title="输入尺寸";
	dlg.Size=3;
	dlg.Titles[0]="请输入宽度:";
	dlg.Titles[1]="请输入高度:";
	dlg.Titles[2]="请选择类型:";
	dlg.Datas[2].AddTail(_T("yuv420p"));
	dlg.Datas[2].AddTail(_T("yuv420sp"));
	if(dlg.DoModal()!=IDOK)
		return;
	int w=atoi(dlg.Texts[0]);
	int h=atoi(dlg.Texts[1]);
	CStringA Type=dlg.Texts[2];
	unsigned char *data=new unsigned char[w*h*3];
	unsigned char *rgb=new unsigned char[w*h*3];
	
	CFile f;
	if(f.Open(Dlg.GetPathName(),CFile::modeRead|CFile::typeBinary)){
		f.Read(data,w*h*3/2);
	/*	memcpy(data+w*h,data,w*h);
		memcpy(data+w*h*2,data,w*h);*/
		f.Close();
	}
	//ConvertYUV2RGB(data,data+w*h,data+w*h*5/4,rgb,w,h);
	unsigned char *R=rgb;
	unsigned char *G=R+w*h;
	unsigned char *B=G+w*h;
	
	if(Type=="yuv420sp"){
		unsigned char *y=data;
		unsigned char *u=y+w*h;
		unsigned char *v=u+w*h/4;
		for(int j=0;j<h;j++){
			for(int i=0;i<w;i++){
				v=data+w*h+(j/2*w+i);
				u=v+1;//w*h/4;

				// R = Y + (1.370705 * (V-128))
				// G = Y - (0.698001 * (V-128)) - (0.337633 * (U-128))
				// B = Y + (1.732446 * (U-128))
				// R = Y + (1.370705 * (V-128))
				// G = Y - (0.698001 * (V-128)) - (0.337633 * (U-128))
				// B = Y + (1.732446 * (U-128))
				*R = UcXz((float)*y + 1.370705 * ((float)*v-128.0)); 
				*G = UcXz((float)*y - 0.337633  * ((float)*u-128.0) - 0.698001*((float)*v-128.0));
				*B = UcXz((float)*y + 1.732446 * ((float)*u-128.0));
			
				y++;
				R++;
				G++;
				B++;
			}
		}

	}else{
		unsigned char *y=data;
		unsigned char *u=y+w*h;
		unsigned char *v=u+w*h/4;
		for(int j=0;j<h;j++){
			for(int i=0;i<w;i++){
				u=data+w*h+(j/2*w/2+i/2);
				v=u+w*h/4;

				// R = Y + (1.370705 * (V-128))
				// G = Y - (0.698001 * (V-128)) - (0.337633 * (U-128))
				// B = Y + (1.732446 * (U-128))
				// R = Y + (1.370705 * (V-128))
				// G = Y - (0.698001 * (V-128)) - (0.337633 * (U-128))
				// B = Y + (1.732446 * (U-128))
				*R = UcXz((float)*y + 1.370705 * ((float)*v-128.0)); 
				*G = UcXz((float)*y - 0.337633  * ((float)*u-128.0) - 0.698001*((float)*v-128.0));
				*B = UcXz((float)*y + 1.732446 * ((float)*u-128.0));
			
				y++;
				R++;
				G++;
				B++;
			}
		}
	}
	
	if(m_pImg&&!m_pImg->IsNull())
		m_pImg->Destroy();
	if(m_pImg)
		delete m_pImg;
	if(m_ie.m_hWnd)
		m_ie.ShowWindow(SW_HIDE);
	m_pImg=DataToImg(rgb,3,w,h);
	m_SrcRect=FRect(0,0,m_pImg->GetWidth(),m_pImg->GetHeight());
	delete []data;
	delete []rgb;

	ReDrawItem();
	m_PicName=Dlg.GetFileName();
}


void CWisdomLabelDlg::OnBnClickedIdcreatedate29()
{
	// TODO: 在此添加控件通知处理程序代码
	int h=1080;
	int w=1920;
	CMTextDlg dlg;
	dlg.Title="输入尺寸";
	dlg.Size=3;
	dlg.Titles[0]="请输入宽度:";
	dlg.Titles[1]="请输入高度:";
	dlg.Titles[2]="请选择目录:";
	dlg.Texts[0]="1920";
	dlg.Texts[1]="1080";
	dlg.bLl[2]=1;
	dlg.TextWidth=800;
	if(dlg.DoModal()!=IDOK)
		return;

	vector<CStringA>fNames;
	CFileFind ff;
	int res=ff.FindFile(CString(dlg.Texts[2]+"\\*.*"));
	while(res){
		res=ff.FindNextFile();
		if(ff.IsDots() || ff.IsDirectory())
			continue;
		CStringA FilePath=CStringA(ff.GetFilePath());
		fNames.push_back(FilePath);
	}
	ff.Close();
	w=atoi(dlg.Texts[0]);
	h=atoi(dlg.Texts[1]);

	unsigned char *data=new unsigned char[w*h*3];
	for(int i=0;i<fNames.size();i++){
		CFile f;
		CStringA fName=fNames.at(i);
		if(f.Open(CString(fName),CFile::modeRead|CFile::typeBinary)){
			f.Read(data,w*h);
			memcpy(data+w*h,data,w*h);
			memcpy(data+w*h*2,data,w*h);
			f.Close();
		}
		if(m_pImg&&!m_pImg->IsNull())
			m_pImg->Destroy();
		if(m_pImg)
			delete m_pImg;
		if(m_ie.m_hWnd)
			m_ie.ShowWindow(SW_HIDE);
		m_pImg=DataToImg(data,3,w,h);
		m_SrcRect=FRect(0,0,m_pImg->GetWidth(),m_pImg->GetHeight());
		CStringA newName;
		newName=fName+".jpg";
		m_pImg->Save(CString(newName));
	
	}
	delete []data;
	ReDrawItem();
	//m_PicName=Dlg.GetFileName();
}


void CWisdomLabelDlg::OnBnClickedIdcreatedate30()
{
	// TODO: 在此添加控件通知处理程序代码
	CMTextDlg dlg;
	dlg.Title="标签缩放";
	dlg.Size=3;
	dlg.Titles[0]="请输入(选择)标签:";
	for(map<CStringA,CBitmap *>::iterator it=m_LabMap.begin();it!=m_LabMap.end();it++){
		dlg.Datas[0].AddTail(CString(it->first));
	}
	dlg.Texts[1]="100";
	dlg.Titles[1]="请输入X向缩放百分比:";
	dlg.Texts[2]="100";
	dlg.Titles[2]="请输入Y向缩放百分比:";
	if(dlg.DoModal()!=IDOK)
		return;

	OnBnClickedOpenpic5();
	CStringA StartName=m_PicPath;
	while(1){
		int bFind=0;
		for(int i=0;i<m_LkList.size();i++){
			Lk lk=m_LkList.at(i);
			if(lk.DxType == dlg.Texts[0]){//Dlg.m_Label1
				vector<Lk >::iterator it=m_LkList.begin()+i;
				int sx=atoi(dlg.Texts[1]);
				int sy=atoi(dlg.Texts[2]);
				CRect Rc(it->Pts.at(0),it->Pts.at(1));
				int nw=Rc.Width()*sx/100;
				int nh=Rc.Height()*sy/100;
				int xf=(nw-Rc.Width())/2;
				int yf=(nh-Rc.Height())/2;
				Rc.InflateRect(-xf,-yf,xf,yf);
				it->Pts.clear();
				it->Pts.push_back(Rc.TopLeft());
				it->Pts.push_back(Rc.BottomRight());
				//it->DxType=Dlg.m_Text2;//Dlg.m_Label2;
				bFind=1;
			}
		}
		if(bFind){
			SaveXml(this->GetXmlFileName(),m_PicName,m_PicPath,m_FolderPath,m_pImg->GetWidth(),m_pImg->GetHeight(),m_pImg->GetBPP()/8,&m_LkList);
		}


		OnBnClickedOpenpic5();
		if(m_PicPath==StartName)
			break;
	}
}


void CWisdomLabelDlg::OnBnClickedButton17()
{
	// TODO: 在此添加控件通知处理程序代码
		CWnd *pBtn=GetDlgItem(IDCROPFACE);
	CStringA Text;
	pBtn->GetWindowText(CString(Text));
	//if(Text=="框脸"){
		m_MouseState=SETFACERECTFIRSTPOINT;
		//pBtn->SetWindowText("停止框脸");
	//}else{
	//	m_MouseState=MOUSESTATENONE;
	//	pBtn->SetWindowText("框脸");
	//}
		m_CmdState=CMDDELETEPIXEL;
}


void CWisdomLabelDlg::OnBnClickedIdcreatedate31()
{
	// TODO: 在此添加控件通知处理程序代码
	CMTextDlg dlg;
	dlg.Title="前缀";
	dlg.Size=1;

	dlg.Titles[0]="前缀:";
	if(dlg.DoModal()!=IDOK)
		return;
	////for(map<CStringA,CBitmap *>::iterator it=m_LabMap.begin();it!=m_LabMap.end();it++){
	////	dlg.Datas[0].AddTail(it->first);
	////}
	////dlg.Texts[1]="100";
	////dlg.Titles[1]="请输入X向缩放百分比:";
	////dlg.Texts[2]="100";
	////dlg.Titles[2]="请输入Y向缩放百分比:";
	//if(dlg.DoModal()!=IDOK)
	//	return;
	CFolderPickerDialog  Dlg(NULL,0,this,0);
	if(!Dlg.DoModal()==IDOK)
		return;
	CStringA FolderPath=CStringA(Dlg.GetFolderPath());
	OnBnClickedOpenpic5();
	CStringA StartName=m_PicPath;
	while(1){
		
		for(int i=0;i<m_LkList.size();i++){
			Lk lk=m_LkList.at(i);
			
			CRect Rc(lk.Pts.at(0),lk.Pts.at(1));
			
			int w,h;
			unsigned char *data=GetImgData(m_pImg,3,w,h);
			unsigned char *data1=CopyData3(data,w,h,3,Rc.left,Rc.top,Rc.Width(),Rc.Height());
			CImage *pImg=DataToImg(data1,3,Rc.Width(),Rc.Height());
			delete []data;
			delete []data1;
			CStringA Str;
			Str.Format("%s\\%s",FolderPath,lk.DxType);
			CFileStatus Status;
			if(!CFile::GetStatus(CString(Str),Status))
				::CreateDirectory(CString(Str),NULL);
			static int id=0;
			Str.Format("%s\\%s_%d.jpg",Str,dlg.Texts[0],id++);
			pImg->Save(CString(Str));
			pImg->Destroy();
			delete pImg;
			
		}


		OnBnClickedOpenpic5();
		if(m_PicPath==StartName)
			break;
	}

}


void CWisdomLabelDlg::OnBnClickedIdcreatedate32()
{
	// TODO: 在此添加控件通知处理程序代码
	CMTextDlg dlg;
	dlg.Title="带标签保存";
	dlg.Size=1;

	dlg.Titles[0]="后缀:";
	if(dlg.DoModal()!=IDOK)
		return;
	OnBnClickedOpenpic2();
	CStringA StartName=m_PicPath;
	while(1){
				CPaintDC dc(this); 
			CRect Rc(0,0,m_pImg->GetWidth(),m_pImg->GetHeight());
		//GetClientRect(&Rc);
		CDC memDC;
		memDC.CreateCompatibleDC(&dc);
		CBitmap bit;
		bit.CreateCompatibleBitmap(&dc,Rc.Width(),Rc.Height());
		CBitmap *pOldBit=memDC.SelectObject(&bit);
		
		//memDC.FillRect(&Rc,&CBrush(RGB(240,240,240)));
		if(m_pImg&&!m_pImg->IsNull()&&Rc.Width()>m_LeftWidth+m_RightWidth+10)
		{
			//CImage *cImg=ImageCopy(m_pImg);
			CRect Rc1=Rc;
			m_DrawScale=1;
		
			
			CRect Rc2=m_SrcRect.ToCRect();

			m_TarRect=Rc1;
			
			memDC.SetStretchBltMode(HALFTONE);
			
		
			m_pImg->Draw(memDC.GetSafeHdc(),Rc1,Rc2);
			

			if(m_DrawLk){
				//DrawRect1(&memDC,ToZfx1(m_GraphRect),RGB(0,255,0),PS_DASH,1);

				
				/*CPen pen(PS_DASH,1,RGB(255,255,0));
				CPen *pOldPen=memDC.SelectObject(&pen);*/
				
				for(int j=0;j<m_LkList.size();j++){
					
					Lk lk=m_LkList.at(j);

					if(lk.Pts.size()<=0){
						continue;
					}
					memDC.SetTextColor(RGB(0,0,0));
					DrawLk1(&memDC,&lk,dlg.Texts[0]);
					vector<Lk >::iterator itlk=m_LkList.begin()+j;
					itlk->BqPicRc=lk.BqPicRc;
					itlk->BqRc=lk.BqRc;
					CPoint pt=ToTar(lk.Pts.at(0));
					//memDC.MoveTo(pt);
					
				}
				
				//memDC.SelectObject(pOldPen);
			}
		}
		//dc.BitBlt(0,0,Rc.Width(),Rc.Height(),&memDC,0,0,SRCCOPY);
		CImage tmpImg;
		tmpImg.Attach((HBITMAP)bit.m_hObject);
		tmpImg.Save(CString(m_PicPath+"_lab.jpg"));
		tmpImg.Detach();
		memDC.SelectObject(pOldBit);
		memDC.DeleteDC();
		bit.DeleteObject();

		OnBnClickedOpenpic2();
		if(m_PicPath==StartName)
			break;
	}
	
}


void CWisdomLabelDlg::OnBnClickedIdcreatedate33()
{
	// TODO: 在此添加控件通知处理程序代码

	vector<Lk >LkList;
	if(LkList.size()<=0)
		LkList=m_LkList;
	CStringA StartName=m_PicPath;
	OnBnClickedOpenpic2();
	
	while(1){
		vector<Lk >tList=LkList;
		for(vector<Lk >::iterator it=tList.begin();it!=tList.end();it++){
			CRect Rc=CRect(it->Pts.at(0),it->Pts.at(1));
			int tw=Rc.Width()/40;
			int th=Rc.Height()/20;
			for(vector<CPoint>::iterator itp=it->Pts.begin();itp!=it->Pts.end();itp++){
				itp->x+=tw*(rand()%11 -5)/5;
				itp->y+=th*(rand()%11 -5)/5;
			}
		}
		m_LkList.insert(m_LkList.end(),tList.begin(),tList.end());
		SaveXml(this->GetXmlFileName(),m_PicName,m_PicPath,m_FolderPath,m_pImg->GetWidth(),m_pImg->GetHeight(),m_pImg->GetBPP()/8,&m_LkList);
		OnBnClickedOpenpic2();
		if(m_PicPath==StartName)
			break;
	}
}


void CWisdomLabelDlg::OnBnClickedIdcreatedate34()
{
	// TODO: 在此添加控件通知处理程序代码
	CMTextDlg dlg;
	dlg.Title="帧率";
	dlg.Size=1;

	dlg.Titles[0]="帧率:";
	dlg.Texts[0]="25";
	if(dlg.DoModal()!=IDOK)
		return;
	int fps=atoi(dlg.Texts[0]);
	if(fps==0)
		fps=25;

	CFolderPickerDialog  Dlg(NULL,0,this,0);
	if(!Dlg.DoModal()==IDOK)
		return;
	CStringA FolderPath=CStringA(Dlg.GetFolderPath());
	//OnBnClickedOpenpic5();
	CStringA StartName=m_PicPath;
	char detFileName[256];
    sprintf(detFileName, "%s\\detinfo.txt", FolderPath);
    FILE *detFile = fopen(detFileName, "wb");
	int FrameId=0;

	while(1){
		int FrameTime=FrameId*1000/fps;
		FrameId++;
		static int time = -1;
        if((int)FrameTime/1000>time)
        {
            time = FrameTime / 1000;
            static char buff[256];
            static int jpgid = 0;
            for (int j = 0; j < m_LkList.size(); j++) {
				Lk lk=m_LkList.at(j);
                char Lab[256] = { 0 };
				strcpy(Lab,lk.DxType);

				CRect Rc(lk.Pts.at(0),lk.Pts.at(1));
                sprintf(buff, "%s\\%05d.jpg", FolderPath,  jpgid);

				int w,h;
				unsigned char *data=GetImgData(m_pImg,3,w,h);
				unsigned char *data1=CopyData3(data,w,h,3,Rc.left,Rc.top,Rc.Width(),Rc.Height());
				CImage *pImg=DataToImg(data1,3,Rc.Width(),Rc.Height());
				delete []data;
				delete []data1;
								
				pImg->Save(CString(buff));
				pImg->Destroy();
				delete pImg;
                                
                                    
                sprintf(buff, "%05d.jpg", jpgid++);
                char buf1[1024] = { 0 };
                sprintf(buf1, "    {\"time\":%d,\"type\":\"%s\",\"event\":\"%s\",\"file\":\"%s\"},\r\n", time, Lab,"", buff);
                fwrite(buf1, 1, strlen(buf1), detFile);

            }
        }





		OnBnClickedOpenpic2();
		if(m_PicPath==StartName)
			break;
	}
	 if (detFile)
        fclose(detFile);
}


void CWisdomLabelDlg::OnBnClickedIdcreatedate35()
{
	// TODO: 在此添加控件通知处理程序代码
		vector<FInfo> files;
	CFileDialog Dlg(TRUE,FALSE,FALSE,FALSE,FALSE,FALSE);
	if(!Dlg.DoModal()==IDOK)
		return;
	//CStringA Qz="66_";
	//int picn=0;//
	//OpenNewPic(Dlg.GetPathName());
	char Buf[64000];
	CFileFind ff;
	//CStringA FirstName;
	int res=ff.FindFile(Dlg.GetFolderPath()+"\\*.*");
	while(res){
		res=ff.FindNextFile();
		if(ff.IsDots() || ff.IsDirectory())
			continue;
		CStringA FilePath=CStringA(ff.GetFilePath());
		
		FInfo fi;
		fi.FileName=CStringA(ff.GetFileName());
		fi.FileTitle=CStringA(ff.GetFileTitle());	
		fi.FileFolder=CStringA(Dlg.GetFolderPath());
		fi.FilePath=CStringA(ff.GetFilePath());
		/*if("浙LD1930"==FileTitle){
			int a=3;
		}*/
		fi.Hz=FilePath.Right(fi.FileName.GetLength()-fi.FileTitle.GetLength()-1);
		
		if(fi.Hz.CompareNoCase("png")==0)
		{
				files.push_back(fi);
			
			/*	FilePath=Dlg.GetFolderPath()+"\\"+FileTitle+".xml";
				newName.Format("%s\\%s%d.xml",Dlg.GetFolderPath(),Qz,picn++);
				rename(FilePath,newName);*/
		}else{
				//DeleteFile(FilePath);
		}
		
	}
	ff.Close();
	for(int i=0;i<files.size();i++){
		FInfo fi=files.at(i);
		//picn++;
		CStringA newName;
		//while(1)
		//{
			newName.Format("%s\\%s.jpg",Dlg.GetFolderPath(),fi.FileTitle);
			CImage img;
			img.Load(CString(fi.FilePath));
			saveImg(m_PicPath,&img);
			//img.SaveJpeg(CString(newName),95);
			//img.SaveJpeg(newName,90);
//			img.SaveJpg();
			img.Destroy();
			//CFileStatus status;
			//if(CFile::GetStatus(newName,status)){
			//	picn++;
			//}else{
			//	//break;
			//}
		//}
		//CStringA FilePath1;//=this->GetXmlFileName();
		//FilePath1.Format("%s\\%s.xml",Dlg.GetFolderPath(),fi.FileTitle);
		
		//int bOk=1;
		//vector<Lk> Lks;
		//CFileStatus status;
		//if(CFile::GetStatus(FilePath1,status))
		//{
		//	OpenXml(FilePath1,&Lks);
		//	if(!DeleteFile(FilePath1)){
		//		AfxMessageBoxA(FilePath1+"重命名失败,请检查是否被其他程序占用");
		//		bOk=0;
		//	}
		//}
		//if(bOk){
		//	if(rename(fi.FilePath,newName)==0){
		//		newName.Format("%s\\%s%d.xml",Dlg.GetFolderPath(),Qz,picn);
		//		if(Lks.size()>0){
		//			SaveXml(newName,&Lks);
		//		}
		//		//rename(FilePath1,newName);
		//	}else{
		//		AfxMessageBoxA(fi.FilePath+"重命名失败,请检查是否被其他程序占用");
		//	}
		//}
	}
}


int CWisdomLabelDlg::GetSelID(void)
{
	return m_pImgListCtrl.GetSelectItem();
	//for(int i=0;i<m_pImgListCtrl.GetItemCount();i++){
	//	if(m_pImgListCtrl.GetItemState(i,LVIS_SELECTED)==LVIS_SELECTED){
	//		return i;
	//	}
	//}
	return 0;
}


void CWisdomLabelDlg::OnHdnItemchangedList3(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	//AfxMessageBoxA("y");
}


void CWisdomLabelDlg::OnLvnItemchangedList3(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	int id=GetSelID();
	if(m_pImgListCtrl.GetItemState(id,LVIS_SELECTED)!=LVIS_SELECTED){
		for(int i=0;i<m_pImgListCtrl.GetItemCount();i++){
			if(m_pImgListCtrl.GetItemState(i,LVIS_SELECTED)==LVIS_SELECTED){
				id=m_pImgListCtrl.m_SelID=i;
			}
		}	
	}
	CStringA Str;
	Str.Format("%d",id);
	//AfxMessageBoxA(Str);
	if(id<0)
		return;
	
	
	static int oldPos=-1;
	static int oldCount=-1;
	if(id!=oldPos || oldCount!=m_pImgListCtrl.GetItemCount()){
		OpenNewPic(CStringA(m_pImgListCtrl.GetItemText(id,1)));
		if(GetForegroundWindow()==this)
			m_pImgListCtrl.EnsureVisible(id, FALSE);
		/*m_pImgListCtrl.SetRedraw(0);
		//m_pImgListCtrl.SetScrollPos(1,id);
		//m_pImgListCtrl.SendMessage(WM_VSCROLL , MAKEWPARAM(SB_THUMBPOSITION, 0) ,0);
		int pos1=m_pImgListCtrl.GetScrollPos(1);
		if(pos1<id){
			while(m_pImgListCtrl.GetScrollPos(1)+100<id){
				m_pImgListCtrl.SendMessage(WM_VSCROLL , MAKEWPARAM(SB_PAGEDOWN, 0) ,0);
				
			}
		}else{
			while(m_pImgListCtrl.GetScrollPos(1)-100>id){
				m_pImgListCtrl.SendMessage(WM_VSCROLL , MAKEWPARAM(SB_PAGEUP, 0) ,0);
				
			}
		}
		pos1=m_pImgListCtrl.GetScrollPos(1);
		if(pos1<id){
			while(pos1<id-20){
				m_pImgListCtrl.PostMessage(WM_VSCROLL , MAKEWPARAM(SB_LINEDOWN, 0) ,0);
				pos1++;
			}
		}else{
			while(pos1>id-20){
				m_pImgListCtrl.PostMessage(WM_VSCROLL , MAKEWPARAM(SB_LINEUP, 0) ,0);
				pos1--;
			}
		}
		m_pImgListCtrl.SetRedraw(1);*/
		m_pImgListCtrl.Invalidate();
		oldCount=m_pImgListCtrl.GetItemCount();
		if(id!=oldPos)
			oldPos=id;
		if(m_BackOFront)
			m_BackOFront=0;
		else
			m_BackList.push_back(id);
	}
	
	//m_pImgListCtrl.SetRedraw(1);
	//m_pImgListCtrl.SendMessage(WM_VSCROLL , MAKEWPARAM(SB_THUMBPOSITION, id) ,0);
	//AfxMessageBoxA("y");
}


void CWisdomLabelDlg::OnNMClickList3(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	//AfxMessageBoxA("yy");

}


void CWisdomLabelDlg::OnBnClickedButton14()
{
	// TODO: 在此添加控件通知处理程序代码
}


void CWisdomLabelDlg::OnBnClickedButton11()
{
	// TODO: 在此添加控件通知处理程序代码	
	if(m_pImg==NULL)
		return;
	static int width=1920;
	static int height=1080;
	static int bSave=0;
	if(!m_IsPCl){
		CMTextDlg dlg;
		dlg.Title="图片扩裁到指定尺寸";
		dlg.Size=3;
		dlg.Titles[0]="请输入新的宽度:";
		dlg.Titles[1]="请输入新的高度:";
		dlg.Titles[2]="是否保存(0：不保存，1：保存):";
		dlg.Texts[0]=IntToStr(width);
		dlg.Texts[1]=IntToStr(height);
		dlg.Texts[2]=IntToStr(bSave);
		if(dlg.DoModal()!=IDOK)
			return;
		width=atoi(dlg.Texts[0]);
		height=atoi(dlg.Texts[1]);
		bSave=atoi(dlg.Texts[2]);
	}


	
	int w=0;
	int h=0;
	unsigned char *pOldData=GetImgData(m_pImg,3,w,h);

	int nw=width;
	int nh=height;
	int left=(nw-w)/2;
	int top=(nh-h)/2;
	unsigned char *pNewData=new unsigned char[nw*nh*3];
	memset(pNewData,128,nw*nh*3);
	/*for(int j=
	int a=3;*/
	for(int k=0;k<3;k++){
		for(int j=0;j<h;j++){
			for(int i=0;i<w;i++){
				int pos=w*h*k+j*w+i;
				int jj=j+top;
				int ii=i+left;
				if(jj<0||jj>=nh)
					continue;
				if(ii<0||ii>=nw)
					continue;
				int posnew=nw*nh*k+jj*nw+ii;
				pNewData[posnew]=pOldData[pos];
			}
		}
	}

	m_pImg->Destroy();
	m_pImg=DataToImg(pNewData,3,nw,nh);
	m_SrcRect=FRect(0,0,m_pImg->GetWidth(),m_pImg->GetHeight());
	for(int i=0;i<m_LkList.size();i++){
			Lk lk=m_LkList.at(i);
			for(int j=0;j<lk.Pts.size();j++){
				CPoint pt=lk.Pts.at(j);
				/*pt.x*=100.0/(float)Dlg.m_XScale;
				pt.y*=100.0/(float)Dlg.m_YScale;*/
				pt.Offset(left,top);
				vector<Lk>::iterator it0=m_LkList.begin()+i;
				vector<CPoint >::iterator it1=it0->Pts.begin()+j;
				it1->x=pt.x;
				it1->y=pt.y;
			}
	}
	ReDrawItem();
	delete []pOldData;
	delete []pNewData;
	if(bSave)
		saveImg(m_PicPath,m_pImg);
}


void CWisdomLabelDlg::OnBnClickedIdcreatedate36()
{
	// TODO: 在此添加控件通知处理程序代码
	vector<Line>LineList;
	for(int db=0;db<m_LkList.size();db++){
			Lk lk=m_LkList.at(db);
			if(lk.LkType==TYPEDBC){
				m_Dbx.clear();
				for(int i=0;i<lk.Pts.size();i++){
					m_Dbx.push_back(lk.Pts.at(i));
				}
				for(int i=0;i<m_Dbx.size()-1;i++){
					LineList.push_back(Line(m_Dbx.at(i),m_Dbx.at(i+1)));
				}
				LineList.push_back(Line(m_Dbx.at(m_Dbx.size()-1),m_Dbx.at(0)));
				//break;
			}
			
	}
	if(m_Dbx.size()<=0)
		return;
	CStringA sType=GetWndTxtA(GetDlgItem(IDC_COMBO2));
	//->GetWindowText(sType);
	sType="无";
	CreateDirectory(CString(m_FolderPath)+"\\crop",NULL);

	//CreateDirectory(GetDataDir()+"\\crop",NULL);
	CDC *pDC=GetDC();
	unsigned char *pBiaoji=NULL;
	{
		vector<CPoint>BjPoint;
		

		for(int k=0;k<LineList.size();k++){
			Line line=LineList.at(k);
			int x=line.pt2.x-line.pt1.x;
			int y=line.pt2.y-line.pt1.y;
			if(y==0 && x==0){
				BjPoint.push_back(line.pt1);
			}else if(x==0){
				if(y>=0){
					for(int j=0;j<=y;j++){
						BjPoint.push_back(CPoint (line.pt1.x,line.pt1.y+j));
					}
				}else{
					for(int j=0;j>=y;j--){
						BjPoint.push_back(CPoint (line.pt1.x,line.pt1.y+j));
					}
				}
			}else if(y==0){
				if(x>=0){
					for(int i=0;i<=x;i++){
						BjPoint.push_back(CPoint (line.pt1.x+i,line.pt1.y));
					}
				}else{
					for(int i=0;i>=x;i--){
						BjPoint.push_back(CPoint (line.pt1.x+i,line.pt1.y));
					}
				}
			}
			else if(abs(x)>abs(y)){
				float k=(float)y/(float)x;
				if(x>0)
					for(int i=0;i<=x;i++){
						BjPoint.push_back(CPoint(line.pt1.x+i,k*(float)i+(float)line.pt1.y));
					}
				else
					for(int i=x;i<=0;i++){
						BjPoint.push_back(CPoint(line.pt1.x+i,k*(float)i+(float)line.pt1.y));
					}
			}else {
				float k=(float)x/(float)y;
				if(y>0)
					for(int j=0;j<=y;j++){
						BjPoint.push_back(CPoint(k*(float)j+(float)line.pt1.x,line.pt1.y+j));
					}
				else
					for(int j=y;j<=0;j++){
						BjPoint.push_back(CPoint(k*(float)j+(float)line.pt1.x,line.pt1.y+j));
					}
			}
		}
	
		//C2d1dCol Col(m_pImg);
		int cn=1;
		int w,h;
		unsigned char *pData=GetImgData(m_pImg,cn,w,h);
		pBiaoji=new unsigned char[w*h];
		for(int i=0;i<w*h;i++){
			pBiaoji[i]=0;
		}	
		for(int i=0;i<BjPoint.size();i++){
			CPoint pt=BjPoint.at(i);
			int pos=pt.y*w+pt.x;
			pBiaoji[pos]=1;
		}
		/*CRect Rc=DbxToRect(&m_Dbx);
		Rc.DeflateRect(-2,-2,-2,-2);
		for(int j=0;j<h;j++){
			for(int i=0;i<w;i++){
				int pos=j*w+i;
				if(!Rc.PtInRect(CPoint(i,j))){
					pBiaoji[pos]=255;
				}
			}
		}
	
		vector<CPoint>AddPt;
		//pBiaoji[0]=255;
		//AddPt.push_back(CPoint(0,0));
		for(int j=0;j<h && AddPt.size()<1;j++){
			for(int i=0;i<w;i++){
				int pos=j*w+i;
				if(pBiaoji[pos]!=1&&pBiaoji[pos]!=255){
					pBiaoji[pos]=255;
					AddPt.push_back(CPoint(i,j));
					break;
				}
			}
		}*/
		list<CPoint>AddPt;
		pBiaoji[0]=255;
		AddPt.push_back(CPoint(0,0));
		for(;AddPt.size()>0;){
			//CPoint p8[8]={CPoint(-1,-1),CPoint(-1,0),CPoint(-1,-1),CPoint(0,-1),CPoint(1,-1),CPoint(1,0),CPoint(1,1),CPoint(0,1)};
			CPoint p8[4]={CPoint(-1,0),CPoint(0,-1),CPoint(1,0),CPoint(0,1)};
			CPoint pt=AddPt.front();
			AddPt.erase(AddPt.begin());
			for(int j=0;j<4;j++){
			
				CPoint pt1=pt+p8[j];
				if(pt1.x<0||pt1.x>=w||pt1.y<0||pt1.y>=h)
					continue;
				int pos=pt1.y*w+pt1.x;
				if(pBiaoji[pos]==1)
					continue;
				if(pBiaoji[pos]==255)
					continue;
				pBiaoji[pos]=255;
				AddPt.push_back(pt1);
				CPoint pt2=ToTar(pt1);
				//pDC->SetPixel(pt2,RGB(255,0,0));
			}
		}
		
		for(int i=0;i<BjPoint.size();i++){
			CPoint pt=ToTar(BjPoint.at(i));
			pDC->SetPixel(pt,RGB(255,255,0));
		}
		int ndn=1;
		int times=0;
		while(ndn){
			ndn=0;
			vector<CPoint> tpList;
			for(int j=0;j<h;j++){
				for(int i=0;i<w;i++){
					int pos=j*w+i;
					if(pBiaoji[pos]>128){
						int n=0;
						CPoint p8[8]={CPoint(-1,1),CPoint(-1,0),CPoint(-1,-1),CPoint(0,-1),CPoint(1,-1),CPoint(1,0),CPoint(1,1),CPoint(0,1)};
						for(int p=0;p<8;p++){
							int jj=j+p8[p].y;
							int ii=i+p8[p].x;
							if(ii<0||jj<0||ii>=w||jj>=h)
								continue;
							int pos1=jj*w+ii;
							if(pBiaoji[pos1]<128){
								n++;
								break;
							}
						}
						if(n>0){
							tpList.push_back(CPoint(i,j));
						}
					}
				}
			}
			ndn=tpList.size();
			if(ndn<=0)
				break;
			times++;
			if(times>3){
				for(int k=0;k<tpList.size();k++){
					CPoint tpt=tpList.at(k);
					int j=tpt.y;
					int i=tpt.x;
					int pos=j*w+i;

					float v=0;
					int n=0;
					for(int jj=j-2;jj<=j+2;jj++){
						for(int ii=i-2;ii<=i+2;ii++){
							if(ii<0||jj<0||ii>=w||jj>=h)
							continue;
							int pos1=jj*w+ii;
							if(pBiaoji[pos1]<128){
								v+=pData[pos1];
								n++;
							}
						}
					}
					/*CPoint p8[8]={CPoint(-1,1),CPoint(-1,0),CPoint(-1,-1),CPoint(0,-1),CPoint(1,-1),CPoint(1,0),CPoint(1,1),CPoint(0,1)};
					for(int p=0;p<8;p++){
						int jj=j+p8[p].y;
						int ii=i+p8[p].x;
						if(ii<0||jj<0||ii>=w||jj>=h)
							continue;
						int pos1=jj*w+ii;
						if(pBiaoji[pos1]<128){
							v+=pData[pos1];
							n++;
						}
					}*/
					if(n>0){
						v/=(float)n;
						pData[pos]=0.5+v;//rand()%256;
						//CPoint pt=ToTar(CPoint(i,j));
						//pDC->SetPixel(pt,RGB(v,v,v));
					}
				}
			}
			for(int k=0;k<tpList.size();k++){
				CPoint tpt=tpList.at(k);
				int j=tpt.y;
				int i=tpt.x;
				int pos=j*w+i;
				pBiaoji[pos]=2;
			}
		}
		/*for(int j=0;j<h;j++){
			for(int i=0;i<w;i++){
				int pos=j*w+i;
				
				if(pBiaoji[pos]>128){
					float v=0;
					int n=0;
					CPoint p8[8]={CPoint(-1,1),CPoint(-1,0),CPoint(-1,-1),CPoint(0,-1),CPoint(1,-1),CPoint(1,0),CPoint(1,1),CPoint(0,1)};
					for(int p=0;p<8;p++){
						int jj=j+p8[p].y;
						int ii=i+p8[p].x;
						if(ii<0||jj<0||ii>=w||jj>=h)
							continue;
						int pos1=jj*w+ii;
						if(pBiaoji[pos1]<128){
							v+=pData[pos1];
							n++;
						}
					}
					if(n>0){
						v/=(float)n;
						pData[pos]=0.5+v;//rand()%256;
						pBiaoji[pos]=2;
						CPoint pt=ToTar(CPoint(i,j));
						//pDC->SetPixel(pt,RGB(v,v,v));
					}
				}
			}
		}
		for(int j=h-1;j>=0;j--){
			for(int i=w-1;i>=0;i--){
				int pos=j*w+i;
				
				if(pBiaoji[pos]>128){
					float v=0;
					int n=0;
					CPoint p8[8]={CPoint(-1,1),CPoint(-1,0),CPoint(-1,-1),CPoint(0,-1),CPoint(1,-1),CPoint(1,0),CPoint(1,1),CPoint(0,1)};
					for(int p=0;p<8;p++){
						int jj=j+p8[p].y;
						int ii=i+p8[p].x;
						if(ii<0||jj<0||ii>=w||jj>=h)
							continue;
						int pos1=jj*w+ii;
						if(pBiaoji[pos1]<128){
							v+=pData[pos1];
							n++;
						}
					}
					if(n>0){
						v/=(float)n;
						pData[pos]=0.5+v;//rand()%256;
						pBiaoji[pos]=2;
						CPoint pt=ToTar(CPoint(i,j));
						//pDC->SetPixel(pt,RGB(v,v,v));
					}
				}
			}
		}*/
		if(m_pImg)
			m_pImg->Destroy();
		delete m_pImg;
		m_pImg=DataToImg(pData,1,w,h);

		
		delete []pData;
		ReDrawItem();
		//delete pImg;
		//Sleep(10000);
		//delete []pBiaoji;
		
	}

	ReleaseDC(pDC);
	delete []pBiaoji;
	m_Dbx.clear();
	/*LcHz="_qddy.jpg";
	OnBnClickedOpenpic11();
	m_MouseState=DELETEDX;
	LcHz="_gdlc.jpg";*/
}


void CWisdomLabelDlg::OnBnClickedOpenAnnotationsDir()
{
	// TODO: 在此添加控件通知处理程序代码
	/*CFolderPickerDialog  Dlg(NULL,0,this,0);
	if(!Dlg.DoModal()==IDOK)
		return;*/
	CMTextDlg Dlg1;
	Dlg1.Size=1;
	Dlg1.SaveTitle="biaozhu";
	Dlg1.Title="请选择标注目录";
	Dlg1.Titles[0]="请输入标注目录:";
	Dlg1.bLl[0]=2;
	Dlg1.bSave[0]=1;
	Dlg1.Texts[0]=m_LabelPath;
	Dlg1.TextWidth=800;
	if(Dlg1.DoModal()!=IDOK)
		return;
	m_LabelPath=Dlg1.Texts[0];
	this->OpenXml(GetXmlFileName(),&this->m_LkList);
	ReDrawItem();
	AddToHisDir(m_LabelPath);
}


void CWisdomLabelDlg::OnBnClickedIdcreatedate37()
{
	// TODO: 在此添加控件通知处理程序代码
	m_FzList=m_LkList;
}


void CWisdomLabelDlg::OnBnClickedIdcreatedate38()
{
	// TODO: 在此添加控件通知处理程序代码
	m_LkList=m_FzList;
	SaveXml(GetXmlFileName(),m_PicName,m_PicPath,m_FolderPath,m_pImg->GetWidth(),m_pImg->GetHeight(),m_pImg->GetBPP()/8,&m_LkList);
	ReDrawItem();
}


void CWisdomLabelDlg::OnBnClickedIdcreatedate39()
{
	// TODO: 在此添加控件通知处理程序代码
		vector<Line>LineList;
	for(int db=0;db<m_LkList.size();db++){
			Lk lk=m_LkList.at(db);
			if(lk.LkType==TYPEDBC){
				m_Dbx.clear();
				for(int i=0;i<lk.Pts.size();i++){
					m_Dbx.push_back(lk.Pts.at(i));
				}
				for(int i=0;i<m_Dbx.size()-1;i++){
					LineList.push_back(Line(m_Dbx.at(i),m_Dbx.at(i+1)));
				}
				LineList.push_back(Line(m_Dbx.at(m_Dbx.size()-1),m_Dbx.at(0)));
				//break;
			}
			
	}
	if(m_Dbx.size()<=0)
		return;
	CStringA sType=GetWndTxtA(GetDlgItem(IDC_COMBO2));
	//->GetWindowText(sType);
	sType="无";
	CreateDirectory(CString(m_FolderPath+"\\crop"),NULL);

	//CreateDirectory(GetDataDir()+"\\crop",NULL);
	CDC *pDC=GetDC();
	unsigned char *pBiaoji=NULL;
	{
		vector<CPoint>BjPoint;
		

		for(int k=0;k<LineList.size();k++){
			Line line=LineList.at(k);
			int x=line.pt2.x-line.pt1.x;
			int y=line.pt2.y-line.pt1.y;
			if(y==0 && x==0){
				BjPoint.push_back(line.pt1);
			}else if(x==0){
				if(y>=0){
					for(int j=0;j<=y;j++){
						BjPoint.push_back(CPoint (line.pt1.x,line.pt1.y+j));
					}
				}else{
					for(int j=0;j>=y;j--){
						BjPoint.push_back(CPoint (line.pt1.x,line.pt1.y+j));
					}
				}
			}else if(y==0){
				if(x>=0){
					for(int i=0;i<=x;i++){
						BjPoint.push_back(CPoint (line.pt1.x+i,line.pt1.y));
					}
				}else{
					for(int i=0;i>=x;i--){
						BjPoint.push_back(CPoint (line.pt1.x+i,line.pt1.y));
					}
				}
			}
			else if(abs(x)>abs(y)){
				float k=(float)y/(float)x;
				if(x>0)
					for(int i=0;i<=x;i++){
						BjPoint.push_back(CPoint(line.pt1.x+i,k*(float)i+(float)line.pt1.y));
					}
				else
					for(int i=x;i<=0;i++){
						BjPoint.push_back(CPoint(line.pt1.x+i,k*(float)i+(float)line.pt1.y));
					}
			}else {
				float k=(float)x/(float)y;
				if(y>0)
					for(int j=0;j<=y;j++){
						BjPoint.push_back(CPoint(k*(float)j+(float)line.pt1.x,line.pt1.y+j));
					}
				else
					for(int j=y;j<=0;j++){
						BjPoint.push_back(CPoint(k*(float)j+(float)line.pt1.x,line.pt1.y+j));
					}
			}
		}
	
		//C2d1dCol Col(m_pImg);
		int cn=1;
		int w,h;
		unsigned char *pData=GetImgData(m_pImg,cn,w,h);
		pBiaoji=new unsigned char[w*h];
		for(int i=0;i<w*h;i++){
			pBiaoji[i]=0;
		}	
		for(int i=0;i<BjPoint.size();i++){
			CPoint pt=BjPoint.at(i);
			int pos=pt.y*w+pt.x;
			pBiaoji[pos]=1;
		}
		/*CRect Rc=DbxToRect(&m_Dbx);
		Rc.DeflateRect(-2,-2,-2,-2);
		for(int j=0;j<h;j++){
			for(int i=0;i<w;i++){
				int pos=j*w+i;
				if(!Rc.PtInRect(CPoint(i,j))){
					pBiaoji[pos]=255;
				}
			}
		}
	
		vector<CPoint>AddPt;
		//pBiaoji[0]=255;
		//AddPt.push_back(CPoint(0,0));
		for(int j=0;j<h && AddPt.size()<1;j++){
			for(int i=0;i<w;i++){
				int pos=j*w+i;
				if(pBiaoji[pos]!=1&&pBiaoji[pos]!=255){
					pBiaoji[pos]=255;
					AddPt.push_back(CPoint(i,j));
					break;
				}
			}
		}*/
		list<CPoint>AddPt;
		pBiaoji[0]=255;
		AddPt.push_back(CPoint(0,0));
		for(;AddPt.size()>0;){
			//CPoint p8[8]={CPoint(-1,-1),CPoint(-1,0),CPoint(-1,-1),CPoint(0,-1),CPoint(1,-1),CPoint(1,0),CPoint(1,1),CPoint(0,1)};
			CPoint p8[4]={CPoint(-1,0),CPoint(0,-1),CPoint(1,0),CPoint(0,1)};
			CPoint pt=AddPt.front();
			AddPt.erase(AddPt.begin());
			for(int j=0;j<4;j++){
			
				CPoint pt1=pt+p8[j];
				if(pt1.x<0||pt1.x>=w||pt1.y<0||pt1.y>=h)
					continue;
				int pos=pt1.y*w+pt1.x;
				if(pBiaoji[pos]==1)
					continue;
				if(pBiaoji[pos]==255)
					continue;
				pBiaoji[pos]=255;
				AddPt.push_back(pt1);
				CPoint pt2=ToTar(pt1);
				//pDC->SetPixel(pt2,RGB(255,0,0));
			}
		}
		
		for(int i=0;i<BjPoint.size();i++){
			CPoint pt=ToTar(BjPoint.at(i));
			pDC->SetPixel(pt,RGB(255,255,0));
		}
		for(int j=0;j<h;j++){
			for(int i=0;i<w;i++){
				int pos=j*w+i;
				if(pBiaoji[pos]<128){
					pData[pos]=128;
				}
			}
		}
		
		if(m_pImg)
			m_pImg->Destroy();
		delete m_pImg;
		m_pImg=DataToImg(pData,1,w,h);

		
		delete []pData;
		ReDrawItem();
		//delete pImg;
		//Sleep(10000);
		//delete []pBiaoji;
		
	}

	ReleaseDC(pDC);
	delete []pBiaoji;
	m_Dbx.clear();
	LcHz="_qdz.jpg";
	OnBnClickedOpenpic11();
	m_MouseState=DELETEDX;
	LcHz="_gdlc.jpg";
}


void CWisdomLabelDlg::OnBnClickedIdcreatedate40()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_pImg==NULL)
		return;
	int c=3;
	int w,h;
	float zv;
	int n=0;
	unsigned char *data=GetImgData(m_pImg,c,w,h);
	for(int cn=0;cn<c;cn++){
		int posc=cn*w*h;
		for(int j=0;j<h;j++){
			int posy=posc+j*w;
			for(int i=0;i<w;i++){
				int pos=posy+i;
				float v=data[pos];
				zv+=v;
				n++;
			}
		}
	}
	zv/=(float)n;
	for(int cn=0;cn<c;cn++){
		int posc=cn*w*h;
		for(int j=0;j<h;j++){
			int posy=posc+j*w;
			for(int i=0;i<w;i++){
				int pos=posy+i;
				float v=data[pos];
				float vv=v-zv;
				vv*=1.25;
				v=zv+vv;
				if(v>254)
					v=254;
				if(v<1)
					v=1;
				data[pos]=0.5+v;
			}
		}
	}
	m_pImg->Destroy();
	delete m_pImg;
	m_pImg=DataToImg(data,c,w,h);
	ReDrawItem();
	delete []data;
	LcHz="_dbdlc.jpg";
}


void CWisdomLabelDlg::OnBnClickedIdcreatedate41()
{
	// TODO: 在此添加控件通知处理程序代码
		if(m_pImg==NULL)
		return;
	int c=3;
	int w,h;
	float zv;
	int n=0;
	unsigned char *data=GetImgData(m_pImg,c,w,h);
	for(int cn=0;cn<c;cn++){
		int posc=cn*w*h;
		for(int j=0;j<h;j++){
			int posy=posc+j*w;
			for(int i=0;i<w;i++){
				int pos=posy+i;
				float v=data[pos];
				zv+=v;
				n++;
			}
		}
	}
	zv/=(float)n;
	for(int cn=0;cn<c;cn++){
		int posc=cn*w*h;
		for(int j=0;j<h;j++){
			int posy=posc+j*w;
			for(int i=0;i<w;i++){
				int pos=posy+i;
				float v=data[pos];
				float vv=v-zv;
				vv*=0.8;
				v=zv+vv;
				if(v>254)
					v=254;
				if(v<1)
					v=1;
				data[pos]=0.5+v;
			}
		}
	}
	m_pImg->Destroy();
	delete m_pImg;
	m_pImg=DataToImg(data,c,w,h);
	ReDrawItem();
	delete []data;
	LcHz="_dbd.jpg";
}


void CWisdomLabelDlg::OnBnClickedIdcreatedate42()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_CropLkList.size()<=0){
		m_MouseState=SELCROPDBC;
		return;
	}
	int maxx=-100;
	int maxy=maxx;
	int minx=1000000000;
	int miny=minx;
	vector<Line>LineList;
	for(int db=0;db<m_CropLkList.size();db++){
			Lk lk=m_CropLkList.at(db);
			m_Dbx.clear();
			if(lk.LkType==TYPEDBC){
				
				for(int i=0;i<lk.Pts.size();i++){
					CPoint pt=lk.Pts.at(i);
					m_Dbx.push_back(pt);
					
				}
				
				//break;
			}else if(lk.LkType==TYPERECT){
				CRect rc(lk.Pts.at(0),lk.Pts.at(1));
				m_Dbx.push_back(rc.TopLeft());
				m_Dbx.push_back(TopRight(rc));
				m_Dbx.push_back(rc.BottomRight());
				m_Dbx.push_back(BottomLeft(rc));
			}
			if(m_Dbx.size()>2){
				for(int i=0;i<m_Dbx.size();i++)
				{
					CPoint pt=m_Dbx.at(i);
					if(pt.x>maxx)
						maxx=pt.x;
					if(pt.y>maxy)
						maxy=pt.y;
					if(pt.x<minx)
						minx=pt.x;
					if(pt.y<miny)
						miny=pt.y;
				}
				for(int i=0;i<m_Dbx.size()-1;i++){
						LineList.push_back(Line(m_Dbx.at(i),m_Dbx.at(i+1)));

				}
				LineList.push_back(Line(m_Dbx.at(m_Dbx.size()-1),m_Dbx.at(0)));
			}
			
	}
	if(LineList.size()<=0)
		return;
	CStringA sType=GetWndTxtA(GetDlgItem(IDC_COMBO2));
	//->GetWindowText(sType);
	sType="无";
	CreateDirectory(CString(m_FolderPath+"\\crop"),NULL);

	//CreateDirectory(GetDataDir()+"\\crop",NULL);
	CDC *pDC=GetDC();
	unsigned char *pBiaoji=NULL;
	{
		vector<CPoint>BjPoint;
		

		for(int k=0;k<LineList.size();k++){
			Line line=LineList.at(k);
			int x=line.pt2.x-line.pt1.x;
			int y=line.pt2.y-line.pt1.y;
			if(y==0 && x==0){
				BjPoint.push_back(line.pt1);
			}else if(x==0){
				if(y>=0){
					for(int j=0;j<=y;j++){
						BjPoint.push_back(CPoint (line.pt1.x,line.pt1.y+j));
					}
				}else{
					for(int j=0;j>=y;j--){
						BjPoint.push_back(CPoint (line.pt1.x,line.pt1.y+j));
					}
				}
			}else if(y==0){
				if(x>=0){
					for(int i=0;i<=x;i++){
						BjPoint.push_back(CPoint (line.pt1.x+i,line.pt1.y));
					}
				}else{
					for(int i=0;i>=x;i--){
						BjPoint.push_back(CPoint (line.pt1.x+i,line.pt1.y));
					}
				}
			}
			else if(abs(x)>abs(y)){
				float k=(float)y/(float)x;
				if(x>0)
					for(int i=0;i<=x;i++){
						BjPoint.push_back(CPoint(line.pt1.x+i,k*(float)i+(float)line.pt1.y));
					}
				else
					for(int i=x;i<=0;i++){
						BjPoint.push_back(CPoint(line.pt1.x+i,k*(float)i+(float)line.pt1.y));
					}
			}else {
				float k=(float)x/(float)y;
				if(y>0)
					for(int j=0;j<=y;j++){
						BjPoint.push_back(CPoint(k*(float)j+(float)line.pt1.x,line.pt1.y+j));
					}
				else
					for(int j=y;j<=0;j++){
						BjPoint.push_back(CPoint(k*(float)j+(float)line.pt1.x,line.pt1.y+j));
					}
			}
		}
	
		//C2d1dCol Col(m_pImg);
		int cn=3;
		int w,h;
		unsigned char *pData=GetImgData(m_pImg,cn,w,h);
		pBiaoji=new unsigned char[w*h];
		for(int i=0;i<w*h;i++){
			pBiaoji[i]=0;
		}	
		for(int i=0;i<BjPoint.size();i++){
			CPoint pt=BjPoint.at(i);
			int pos=pt.y*w+pt.x;
			pBiaoji[pos]=1;
		}
		/*CRect Rc=DbxToRect(&m_Dbx);
		Rc.DeflateRect(-2,-2,-2,-2);
		for(int j=0;j<h;j++){
			for(int i=0;i<w;i++){
				int pos=j*w+i;
				if(!Rc.PtInRect(CPoint(i,j))){
					pBiaoji[pos]=255;
				}
			}
		}
	
		vector<CPoint>AddPt;
		//pBiaoji[0]=255;
		//AddPt.push_back(CPoint(0,0));
		for(int j=0;j<h && AddPt.size()<1;j++){
			for(int i=0;i<w;i++){
				int pos=j*w+i;
				if(pBiaoji[pos]!=1&&pBiaoji[pos]!=255){
					pBiaoji[pos]=255;
					AddPt.push_back(CPoint(i,j));
					break;
				}
			}
		}*/
		list<CPoint>AddPt;
		pBiaoji[0]=255;
		AddPt.push_back(CPoint(0,0));
		for(;AddPt.size()>0;){
			//CPoint p8[8]={CPoint(-1,-1),CPoint(-1,0),CPoint(-1,-1),CPoint(0,-1),CPoint(1,-1),CPoint(1,0),CPoint(1,1),CPoint(0,1)};
			CPoint p8[4]={CPoint(-1,0),CPoint(0,-1),CPoint(1,0),CPoint(0,1)};
			CPoint pt=AddPt.front();
			AddPt.erase(AddPt.begin());
			for(int j=0;j<4;j++){
			
				CPoint pt1=pt+p8[j];
				if(pt1.x<0||pt1.x>=w||pt1.y<0||pt1.y>=h)
					continue;
				int pos=pt1.y*w+pt1.x;
				if(pBiaoji[pos]==1)
					continue;
				if(pBiaoji[pos]==255)
					continue;
				pBiaoji[pos]=255;
				AddPt.push_back(pt1);
				CPoint pt2=ToTar(pt1);
				//pDC->SetPixel(pt2,RGB(255,0,0));
			}
		}
		
		for(int i=0;i<BjPoint.size();i++){
			CPoint pt=ToTar(BjPoint.at(i));
			pDC->SetPixel(pt,RGB(255,255,0));
		}
		CRect rc(minx,miny,maxx,maxy);
		float xs=(float)m_pImg->GetWidth()/416.0;
		float ys=(float)m_pImg->GetWidth()/416.0;
		rc.InflateRect(1,1);
		if(m_CropMb)
			delete []m_CropMb;
		m_CropMb=new float[rc.Width()*rc.Height()];
		for(int j=rc.top;j<rc.bottom;j++){
			for(int i=rc.left;i<rc.right;i++){
				int pos=j*w+i;
				int pos1=(j-rc.top)*rc.Width()+i-rc.left;
				if(pBiaoji[pos]>128){
					m_CropMb[pos1]=0;
				}else 
					m_CropMb[pos1]=1;
			}
		}
		/*int ndn=1;
		int times=0;
		while(ndn){
			ndn=0;
			vector<CPoint> tpList;
			for(int j=rc.top;j<rc.bottom;j++){
				for(int i=rc.left;i<rc.right;i++){
					int pos=j*w+i;
					if(pBiaoji[pos]>128){
						int n=0;
						CPoint p8[8]={CPoint(-1,1),CPoint(-1,0),CPoint(-1,-1),CPoint(0,-1),CPoint(1,-1),CPoint(1,0),CPoint(1,1),CPoint(0,1)};
						for(int p=0;p<8;p++){
							int jj=j+p8[p].y;
							int ii=i+p8[p].x;
							if(ii<0||jj<0||ii>=w||jj>=h)
								continue;
							int pos1=jj*w+ii;
							if(pBiaoji[pos1]<128){
								n++;
								break;
							}
						}
						if(n>0){
							tpList.push_back(CPoint(i,j));
						}
					}
				}
			}
			ndn=tpList.size();
			if(ndn<=0)
				break;
			times++;
			if(times>3){
				for(int k=0;k<tpList.size();k++){
					CPoint tpt=tpList.at(k);
					int j=tpt.y;
					int i=tpt.x;
					int pos=j*w+i;

					float v=0;
					int n=0;
					for(int jj=j-2;jj<=j+2;jj++){
						for(int ii=i-2;ii<=i+2;ii++){
							if(ii<0||jj<0||ii>=w||jj>=h)
							continue;
							int pos1=jj*w+ii;
							if(pBiaoji[pos1]<128){
								v+=pData[pos1];
								n++;
							}
						}
					}
				
					if(n>0){
						v/=(float)n;
						pData[pos]=0.5+v;//rand()%256;
						//CPoint pt=ToTar(CPoint(i,j));
						//pDC->SetPixel(pt,RGB(v,v,v));
					}
				}
			}
			for(int k=0;k<tpList.size();k++){
				CPoint tpt=tpList.at(k);
				int j=tpt.y;
				int i=tpt.x;
				int pos=j*w+i;
				pBiaoji[pos]=2;
			}
		}*/
		/*for(int j=0;j<h;j++){
			for(int i=0;i<w;i++){
				int pos=j*w+i;
				
				if(pBiaoji[pos]>128){
					float v=0;
					int n=0;
					CPoint p8[8]={CPoint(-1,1),CPoint(-1,0),CPoint(-1,-1),CPoint(0,-1),CPoint(1,-1),CPoint(1,0),CPoint(1,1),CPoint(0,1)};
					for(int p=0;p<8;p++){
						int jj=j+p8[p].y;
						int ii=i+p8[p].x;
						if(ii<0||jj<0||ii>=w||jj>=h)
							continue;
						int pos1=jj*w+ii;
						if(pBiaoji[pos1]<128){
							v+=pData[pos1];
							n++;
						}
					}
					if(n>0){
						v/=(float)n;
						pData[pos]=0.5+v;//rand()%256;
						pBiaoji[pos]=2;
						CPoint pt=ToTar(CPoint(i,j));
						//pDC->SetPixel(pt,RGB(v,v,v));
					}
				}
			}
		}
		for(int j=h-1;j>=0;j--){
			for(int i=w-1;i>=0;i--){
				int pos=j*w+i;
				
				if(pBiaoji[pos]>128){
					float v=0;
					int n=0;
					CPoint p8[8]={CPoint(-1,1),CPoint(-1,0),CPoint(-1,-1),CPoint(0,-1),CPoint(1,-1),CPoint(1,0),CPoint(1,1),CPoint(0,1)};
					for(int p=0;p<8;p++){
						int jj=j+p8[p].y;
						int ii=i+p8[p].x;
						if(ii<0||jj<0||ii>=w||jj>=h)
							continue;
						int pos1=jj*w+ii;
						if(pBiaoji[pos1]<128){
							v+=pData[pos1];
							n++;
						}
					}
					if(n>0){
						v/=(float)n;
						pData[pos]=0.5+v;//rand()%256;
						pBiaoji[pos]=2;
						CPoint pt=ToTar(CPoint(i,j));
						//pDC->SetPixel(pt,RGB(v,v,v));
					}
				}
			}
		}*/
		//if(m_pImg)
		//	m_pImg->Destroy();
		//delete m_pImg;
		//m_pImg=DataToImg(pData,1,w,h);
		unsigned char * cropData=CopyData(pData,w,h,rc.left,rc.top,rc.Width(),rc.Height(),cn);
		if(m_CropImg){
			m_CropImg->Destroy();
			delete m_CropImg;
		}
		m_CropImg=DataToImg(cropData,cn,rc.Width(),rc.Height());
		m_CropPos=rc.TopLeft();
		delete []cropData;
		delete []pData;
		//m_MouseState=SETCOPYIMGPOS;
		m_CropLkList.clear();
		m_MouseState=SETCOPYBZDX;
		m_FzList.clear();
		m_CmdState=0;
		m_CopyOrWidth=m_pImg->GetWidth();
		m_CopyOrHeight=m_pImg->GetHeight();
		ReDrawItem();
		
		//delete pImg;
		//Sleep(10000);
		//delete []pBiaoji;
		
	}

	ReleaseDC(pDC);
	delete []pBiaoji;
	m_Dbx.clear();

	//LcHz="_qddy.jpg";
	//OnBnClickedOpenpic11();
	//m_MouseState=DELETEDX;
	//LcHz="_gdlc.jpg";
}


void CWisdomLabelDlg::OnBnClickedIdcreatedate43()
{
	// TODO: 在此添加控件通知处理程序代码
	m_MouseState=SETCOPYBZDX;
	m_FzList.clear();
}


void CWisdomLabelDlg::OnBnClickedIdcreatedate44()
{
	// TODO: 在此添加控件通知处理程序代码
	int cn=3;
	int w,h;
	unsigned char *pData=GetImgData(m_pImg,cn,w,h);
	for(int db=0;db<m_LkList.size();db++){
		Lk lk=m_LkList.at(db);
		if(lk.LkType==TYPERECT && lk.DxType=="del"){
			CRect Rc(lk.Pts.at(0),lk.Pts.at(1));
			for(int c=0;c<cn;c++){
				for(int j=0;j<h;j++){
					for(int i=0;i<w;i++){
						CPoint pt(i,j);
						if(Rc.PtInRect(pt)){
							int pos=c*w*h+j*w+i;
							pData[pos]=128;
						}
					}
				}
			}
			m_LkList.erase(m_LkList.begin()+db);
			db--;
		}
	}
	m_pImg->Destroy();
	delete m_pImg;
	m_pImg=DataToImg(pData,cn,w,h);
	m_pImg->Save(CString(m_PicPath));
	ReDrawItem();
	
}


void CWisdomLabelDlg::OnBnClickedIdcreatedate45()
{
	// TODO: 在此添加控件通知处理程序代码
	m_MouseState=SETCOPYIMGPOS;
}


void CWisdomLabelDlg::OnBnClickedButton18()
{
	// TODO: 在此添加控件通知处理程序代码
	int n=m_pImgListCtrl.GetItemCount();
	int r=0;
	while(n>RAND_MAX){
		n-=RAND_MAX;
		r+=rand()%RAND_MAX;
	}
	if(n>0)
		r+=rand()%n;
	m_pImgListCtrl.SetSelectItem(r);
}


void CWisdomLabelDlg::OnBnClickedIdcreatedate46()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_FrontList.size()>0){
		int id=m_FrontList.back();
		m_FrontList.pop_back();
		m_BackList.push_back(id);
		m_BackOFront=1;
		id=m_FrontList.back();
		m_pImgListCtrl.SetSelectItem(id);
	}
}


void CWisdomLabelDlg::OnBnClickedIdcreatedate47()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_BackList.size()>0){
		int id=m_BackList.back();
		m_BackList.pop_back();
		m_FrontList.push_back(id);
		m_BackOFront=1;
		id=m_BackList.back();
		m_pImgListCtrl.SetSelectItem(id);
	}
}


void CWisdomLabelDlg::OnBnClickedButton19()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_CropImg&&!m_CropImg->IsNull()){
		int x=rand()%(m_pImg->GetWidth()-m_CropImg->GetWidth());
		int y=rand()%(m_pImg->GetHeight()-m_CropImg->GetHeight());
		CPoint pt(x,y);
		CPoint pt1=ToTar(pt);
		m_MousePos=m_CropMousePos=pt1;
		m_MouseState=SETCOPYIMGPOS;
		ReDrawItem();
		//ClientToScreen(&pt1);
		//SetCursorPos(pt1.x,pt1.y);
		
		//m_CropNewPos=ptSrc;
		//
		//if(m_pImg->GetWidth()==m_CopyOrWidth){
		//	//CRect Rc(x,y,m_CropImg->GetWidth()+x,m_CropImg->GetHeight()+y);
		//		Fugai(m_pImg,m_CropImg,m_CropMb,ptSrc.x,ptSrc.y);
		//}
		//else{
		//			
		//		float cScale=(float)m_pImg->GetWidth()/(float)m_CopyOrWidth;
		//		int ow,oh;
		//		unsigned char *od=GetImgData(m_CropImg,3,ow,oh);
		//		int tw=(float)ow*cScale;
		//		int th=(float)oh*cScale;
		//		unsigned char *d=new unsigned char[3*tw*th];
		//		float *mb=new float[tw*th];
		//		ScaleData(m_CropMb,ow,oh,1,mb,tw,th);
		//		ScaleData(od,ow, oh ,3,d, tw, th );
		//		CImage *tImg=DataToImg(d,3,tw,th);
		//		Fugai(m_pImg,tImg,mb,ptSrc.x,ptSrc.y);
		//		tImg->Destroy();
		//		delete tImg;
		//		delete []od;
		//		delete []d;
		//}
		////Fugai(m_pImg,m_CropImg,ptSrc.x,ptSrc.y);
		////复制标注
		//m_pImg->Save(m_PicPath);
	}
	
}


void CWisdomLabelDlg::OnBnClickedButton20()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_CropImg&&!m_CropImg->IsNull()){
		CPoint ptSrc=ToSrc(m_CropMousePos);
		
		
	
		
		
		m_CropNewPos=ptSrc;
		float cScale=(float)m_pImg->GetWidth()/(float)m_CopyOrWidth;
		if(m_pImg->GetWidth()==m_CopyOrWidth){
			//CRect Rc(x,y,m_CropImg->GetWidth()+x,m_CropImg->GetHeight()+y);
				Fugai(m_pImg,m_CropImg,m_CropMb,ptSrc.x,ptSrc.y);
		}
		else{
					
				
				int ow,oh;
				unsigned char *od=GetImgData(m_CropImg,3,ow,oh);
				int tw=(float)ow*cScale;
				int th=(float)oh*cScale;
				unsigned char *d=new unsigned char[3*tw*th];
				float *mb=new float[tw*th];
				ScaleData(m_CropMb,ow,oh,1,mb,tw,th);
				ScaleData(od,ow, oh ,3,d, tw, th );
				CImage *tImg=DataToImg(d,3,tw,th);
				Fugai(m_pImg,tImg,mb,ptSrc.x,ptSrc.y);
				tImg->Destroy();
				delete tImg;
				delete []od;
				delete []d;
		}
		//Fugai(m_pImg,m_CropImg,ptSrc.x,ptSrc.y);
		//复制标注
		m_pImg->Save(CString(m_PicPath));
		for(int j=0;j<m_FzList.size();j++){
			Lk lk=m_FzList.at(j);
			CPoint ofpt=m_CropNewPos;//-m_CropPos;
			vector<CPoint >::iterator it=lk.Pts.begin();
			for(;it!=lk.Pts.end();it++){
				it->x=cScale*it->x;
				it->y=cScale*it->y;
				it->Offset(ofpt);
			}
			m_LkList.push_back(lk);
		}
		OnBnClickedSaveAnnotation();
		ReDrawItem();
	}
}


void CWisdomLabelDlg::OnBnClickedButton22()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_CropImg&&!m_CropImg->IsNull()){
		shujing(m_CropImg,m_CropMb,&m_FzList);
	}
}


void CWisdomLabelDlg::OnBnClickedButton23()
{
	// TODO: 在此添加控件通知处理程序代码
	CStringA Str=GetWndTxtA(&m_FindNrBtn);
	//.GetWindowText(Str);
	if(Str.IsEmpty())
		return;
	for(int i=GetSelID()+1;i<m_pImgListCtrl.GetItemCount();i++){
		CStringA tmp=CStringA(m_pImgListCtrl.GetItemText(i,0));
		if(tmp.Find(Str)!=-1){
			m_pImgListCtrl.SetSelectItem(i);
			break;
		}
	}
}


void CWisdomLabelDlg::OnBnClickedButton24()
{
	// TODO: 在此添加控件通知处理程序代码
	vector<vector<CStringA>>List;
	for(int i=0;i<m_pImgListCtrl.GetItemCount();i++){
		vector<CStringA>tmpList;
		for(int j=0;j<4;j++){
			tmpList.push_back(CStringA(m_pImgListCtrl.GetItemText(i,j)));
		}
		List.push_back(tmpList);
	}
	for(int j=0;j<List.size()-1;j++){
		for(int i=0;i<List.size()-j-1;i++){
			if(List.at(i).at(3)>List.at(i+1).at(3)){
				CStringA s1=List[i].at(3);
				CStringA s2=List[i+1].at(3);
				swap(List[i],List[i+1]);
				CStringA s3=List[i].at(3);
				CStringA s4=List[i+1].at(3);
				CStringA s5=s3;
			}
		}
	}
	m_pImgListCtrl.SetRedraw(0);
	m_pImgListCtrl.DeleteAllItems();
	//return;
	for(int i=0;i<List.size();i++){
		vector<CStringA >tmpList=List.at(i);
		int j=m_pImgListCtrl.InsertItem(m_pImgListCtrl.GetItemCount(),CString(tmpList.at(0)));
		for(int k=1;k<4;k++){
			m_pImgListCtrl.SetItemText(j,k,CString(tmpList.at(k)));
		}
	}
	m_pImgListCtrl.SetRedraw(1);
}


void CWisdomLabelDlg::OnBnClickedButton25()
{
	// TODO: 在此添加控件通知处理程序代码
		vector<vector<CStringA>>List;
	for(int i=0;i<m_pImgListCtrl.GetItemCount();i++){
		vector<CStringA>tmpList;
		for(int j=0;j<4;j++){
			tmpList.push_back(CStringA(m_pImgListCtrl.GetItemText(i,j)));
		}
		List.push_back(tmpList);
	}
	for(int j=0;j<List.size()-1;j++){
		for(int i=0;i<List.size()-j-1;i++){
			if(List.at(i).at(0)>List.at(i+1).at(0)){
				CStringA s1=List[i].at(3);
				CStringA s2=List[i+1].at(3);
				swap(List[i],List[i+1]);
				CStringA s3=List[i].at(3);
				CStringA s4=List[i+1].at(3);
				CStringA s5=s3;
			}
		}
	}
	m_pImgListCtrl.SetRedraw(0);
	m_pImgListCtrl.DeleteAllItems();
	//return;
	for(int i=0;i<List.size();i++){
		vector<CStringA >tmpList=List.at(i);
		int j=m_pImgListCtrl.InsertItem(m_pImgListCtrl.GetItemCount(),CString(tmpList.at(0)));
		for(int k=1;k<4;k++){
			m_pImgListCtrl.SetItemText(j,k,CString(tmpList.at(k)));
		}
	}
	m_pImgListCtrl.SetRedraw(1);
}


void CWisdomLabelDlg::OnBnClickedCdisbiao3()
{
	// TODO: 在此添加控件通知处理程序代码
	int cn=3;
	int w,h;
	unsigned char *pData=GetImgData(m_pImg,cn,w,h);
	for(int i=0;i<m_LkList.size();i++){
		Lk lk=m_LkList.at(i);
		if(lk.DxType == "smoke"&& lk.isCopy==0&&lk.Pts.size()>1){
			CRect rc(lk.Pts.at(0),lk.Pts.at(1));
			int nw=rc.Width();
			int nh=rc.Height();
			unsigned char *nData=CopyData(pData,w,h,rc.left,rc.top,nw,nh,cn);
			int jj=0;
			for(int j=0;j<8&& jj<50;j++){
				int x=rand()%(w-nw);
				int y=rand()%(h-nh);
				CRect rcn(x,y,x+nw,y+nh);
				int bInsert=0;
				for(int k=0;k<m_LkList.size();k++){
					Lk tlk=m_LkList.at(k);
					if(tlk.LkType!=TYPERECT)
						continue;
					CRect rct(tlk.Pts.at(0),tlk.Pts.at(1));
					if(rct.IntersectRect(rct,rcn)){
						bInsert=1;
						break;
					}
				}
				if(bInsert){
					j--;
					jj++;
				}
				else{
					FugaiData(pData,w,h,nData,x,y,nw,nh,cn);
					Lk cLk;
					cLk.LkType=TYPERECT;
					cLk.isCopy=1;
					cLk.DxType="smoke";
					cLk.Pts.push_back(CPoint(x,y));
					cLk.Pts.push_back(rcn.BottomRight());
					m_LkList.push_back(cLk);
				}
			}
			delete []nData;
		}
	}
	m_pImg->Destroy();
	delete m_pImg;
	m_pImg=DataToImg(pData,cn,w,h);
	delete []pData;
	m_pImg->Save(CString(m_PicPath));
	SaveXml(GetXmlFileName(),m_PicName,m_PicPath,m_FolderPath,m_pImg->GetWidth(),m_pImg->GetHeight(),m_pImg->GetBPP()/8,&m_LkList);
	ReDrawItem();
}


void CWisdomLabelDlg::OnBnClickedButton26()
{
	// TODO: 在此添加控件通知处理程序代码
	CStringA Str=GetWndTxtA(&m_FindNrBtn);
	//.GetWindowText(Str);
	if(Str.IsEmpty())
		return;
	for(int i=0;i<m_pImgListCtrl.GetItemCount();i++){
		CStringA tmp=CStringA(m_pImgListCtrl.GetItemText(i,0));
		if(tmp.Find(Str)!=-1){
			m_pImgListCtrl.SetSelectItem(i);
			break;
		}
	}
}


void CWisdomLabelDlg::OnBnClickedCdisbiao4()
{
	// TODO: 在此添加控件通知处理程序代码
	CStringA path=m_ResPath+"\\res\\"+m_PicName;//m_FolderPath.Left(m_FolderPath.ReverseFind('\\'))
	//AfxMessageBoxA(path);
	m_pImg->Destroy();
	delete m_pImg;
	m_pImg=new CImage;
	m_pImg->Load(CString(path));
	//m_LkList.clear();
	//delete []pData;
	ReDrawItem();
}


void CWisdomLabelDlg::OnBnClickedCdisbiao5()
{
	// TODO: 在此添加控件通知处理程序代码
	//vector<Lk> tmpList=m_LkList;
	//m_LkList.clear();
	CStringA tmp=m_PicPath;
	m_PicPath="";
	OpenNewPic(tmp);
	/*if(tmpList.size()>0){
		m_LkList.insert(m_LkList.end(),tmpList.begin(),tmpList.end());
	}*/
}


void CWisdomLabelDlg::OnBnClickedCdisbiao6()
{
	// TODO: 在此添加控件通知处理程序代码
	int cn=1;
	int w,h;
	unsigned char *pData=GetImgData(m_pImg,cn,w,h);
	TwoValue(pData,w,h,0,0,w,h,200);
	m_pImg->Destroy();
	delete m_pImg;
	m_pImg=DataToImg(pData,cn,w,h);
	ReDrawItem();
}


void CWisdomLabelDlg::OnBnClickedCdisbiao7()
{
	// TODO: 在此添加控件通知处理程序代码
	m_MouseState=ERZHIHUA;
	
}


void CWisdomLabelDlg::OnBnClickedCdisbiao8()
{
	// TODO: 在此添加控件通知处理程序代码
	for(int i=0;i<m_pImgListCtrl.GetItemCount();i++){
		CStringA Str=CStringA(m_pImgListCtrl.GetItemText(i,1));
		CStringA OldName=Str;
		Str=Str.Left(Str.ReverseFind('.'));
		CStringA Str0=Str.Left(Str.GetLength()-3);
		CStringA Str1=Str.Right(3);
		if(Str1.Left(1)=="_"){
			Str1=Str1.Right(2);
			Str1="_0"+Str1;
		}

		Str=Str0+Str1;
		Str0=Str.Left(Str.GetLength()-2);
		Str1=Str.Right(2);
		if(Str1.Left(1)=="_"){
			Str1=Str1.Right(1);
			Str1="_00"+Str1;
		}
		Str=Str0+Str1+".jpg";
		rename(OldName,Str);
		//m_pImgListCtrl.SetItemText(i,0,Str);
	}
}


void CWisdomLabelDlg::OnBnClickedCdisbiao9()
{
	// TODO: 在此添加控件通知处理程序代码
	CStringA Str;
	CStringA XmlStr;
	if(m_TmpDir.IsEmpty()){
		Str=m_FolderPath+"\\tmp";
		
	}else{
		Str=m_TmpDir;
	}
	CreateDirectory(CString(Str),NULL);
	XmlStr=Str+"\\"+m_PicTitle+".xml";
	Str+="\\"+m_PicName;
	CopyFile(CString(m_PicPath),CString(Str),1);
	CopyFile(CString(GetXmlFileName()),CString(XmlStr),1);
	OnBnClickedOpenpic2();
	//OnBnClickedButton18();
}


void CWisdomLabelDlg::OnBnClickedIdcreatedate48()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_CropImg&&!m_CropImg->IsNull()){
		
		int xn=m_pImg->GetWidth()/m_CropImg->GetWidth();
		int yn=m_pImg->GetHeight()/m_CropImg->GetHeight();
		for(int j=0;j<yn;j++){
			for(int i=0;i<xn;i++){
				int x=i*m_CropImg->GetWidth();
				int y=j*m_CropImg->GetHeight();

				CPoint ptSrc=CPoint(x,y);
				Fugai(m_pImg,m_CropImg,m_CropMb,ptSrc.x,ptSrc.y);
			
			
				
				for(int j=0;j<m_FzList.size();j++){
					Lk lk=m_FzList.at(j);
					CPoint ofpt=ptSrc;//m_CropNewPos;//-m_CropPos;
					vector<CPoint >::iterator it=lk.Pts.begin();
					for(;it!=lk.Pts.end();it++){
						//it->x=cScale*it->x;
						//it->y=cScale*it->y;
						it->Offset(ofpt);
					
					}
					m_LkList.push_back(lk);
				}
				
				if(m_CmdState==0){
				
					//m_MouseState=SETCOPYBZDX;
				}

			}
		}
		m_pImg->Save(CString(m_PicPath));
		OnBnClickedSaveAnnotation();

		ReDrawItem();
	}
}


void CWisdomLabelDlg::OnBnClickedIdcreatedate49()
{
	// TODO: 在此添加控件通知处理程序代码
	static int emptyid=0;
	CStringA Str;
	while(1){
		Str.Format("%s\\empty%d.jpg",m_FolderPath,emptyid);
		CFileStatus status;
		if(CFile::GetStatus(CString(Str),status))
			emptyid++;
		else
			break;
	}
	int w=m_pImg->GetWidth();
	int h=m_pImg->GetHeight();
	int cn=3;
	unsigned char* pData=new unsigned char[w*h*cn];
	memset(pData,128,w*h*cn);
	CImage *tImg=DataToImg(pData,cn,w,h);
	tImg->Save(CString(Str));
	tImg->Destroy();
	delete tImg;
	//ReDrawItem();
	delete []pData;
	int j=m_pImgListCtrl.InsertItem(GetSelID()+1,CString("0"));
	m_pImgListCtrl.SetItemText(j,1,CString(Str));
	m_pImgListCtrl.SetSelectItem(j);
	m_EmptyID=j;
}


void CWisdomLabelDlg::OpenDir(CStringA Dir)
{
	//m_ResPath=m_FolderPath.Left(m_FolderPath.ReverseFind('\\'));
	m_LabelPath=m_FolderPath=Dir;
	
	//m_ResPath=m_FolderPath.Left(m_FolderPath.ReverseFind('\\'));

	//OpenNewPic(Dlg.GetPathName());
	//m_FolderPath=Dlg.GetFolderPath();
	//m_PicName=Dlg.GetFileName();
	
	//AfxMessageBoxA(m_PicTitle);
	m_pImgListCtrl.SetRedraw(0);
	m_pImgListCtrl.DeleteAllItems();
	//CStringA PicDir=PicDir;//Dlg.GetFolderPath();
	CFileFind ff;
	int res=ff.FindFile(CString(Dir+"\\*.*"));
	while(res){
		res=ff.FindNextFile();
		if(ff.IsDots() || ff.IsDirectory())
			continue;
		CStringA FilePath=CStringA(ff.GetFilePath());
		//if(FilePath==m_PicPath)
		//	continue;
		CStringA FileName=CStringA(ff.GetFileName());
		CStringA FileTitle=CStringA(ff.GetFileTitle());
		CStringA hz=FilePath.Right(FileName.GetLength()-FileTitle.GetLength()-1);
		if(hz.CompareNoCase("jpg")==0
			||hz.CompareNoCase("jpeg")==0
			||hz.CompareNoCase("bmp")==0
			||hz.CompareNoCase("png")==0){
				int j=m_pImgListCtrl.InsertItem(m_pImgListCtrl.GetItemCount(),CString(FileName));
				//m_pImgListCtrl.SetItemText(j,1,FileName);
				m_pImgListCtrl.SetItemText(j,1,CString(FilePath));
				/*CStringA XmlFile=m_LabelPath+"\\"+FileTitle+".xml";//GetXmlFileName();//
				if(FileTitle=="2-00207"){
					int debug=1;
				}
				CFileStatus state;
				if(CFile::GetStatus(CString(XmlFile),state)==1){
					m_pImgListCtrl.SetItemText(j,2,CString("已标注"));
				}
				CTime time;
				ff.GetLastWriteTime(time);
				CTime time1;
				time1=state.m_mtime;
				if(time1>time)
					time=time1;
				m_pImgListCtrl.SetItemText(j,3,time.Format("%Y-%m-%d %H:%M:%S"));*/
				//m_PicList.AddTail(FilePath);
		}
		//CStringA hz1=hz;
	}
	//UpdateImgFileList();
	m_MouseState=-1;
	m_pImgListCtrl.SetRedraw(1);
	m_pImgListCtrl.SetSelectItem(0);
	
	/*CFileStatus Status;
	if(CFile::GetStatus(HisDir,Status)!=1){
		CFile fi;
		fi.Open(HisDir,CFile::modeCreate);
		fi.Close();
	}*/
	//AddToHisDir(m_FolderPath);
}


void CWisdomLabelDlg::OnBnClickedOpenpic18()
{
	// TODO: 在此添加控件通知处理程序代码

	CMTextDlg dlg;

	dlg.Title="选择历史目录";
	dlg.Size=1;
	dlg.Titles[0]="请选择历史目录:";
	CStdioFile fi;
	CStringA HisDir=GetModuleDir()+"\\cfg\\hisdir.txt";
	if(fi.Open(CString(HisDir),CFile::modeRead)){
		CString ss;
		while(fi.ReadString(ss)){
			ss.Replace(_T("\r"),_T(""));
			ss.Replace(_T("\n"),_T(""));
			dlg.Datas[0].AddHead(CString(ss));
		}
		fi.Close();
	}
	if(dlg.DoModal()!=IDOK){
		return;
	}
	//OpenDir(dlg.Texts[0]);
}


void CWisdomLabelDlg::OnBnClickedRuihua2()
{
	// TODO: 在此添加控件通知处理程序代码
	int cn=3;
	int w,h;
	int kw=3;
	int kh=3;
	//float k0[3][3]={
	//	{-0.1,-0.1,-0.1},
	//	{-0.1,1.8,-0.1},
	//	{-0.1,-0.1,-0.1}
	//};
	//float k1[3][3]={
	//	{-1,0,1},
	//	{-1,0,1},
	//	{-1,0,1}
	//};
	unsigned char *pData=GetImgData(m_pImg,cn,w,h);
	unsigned char *pData1=new unsigned char[w*h*cn];
	//memset(pData1,128,w*h*cn);
	memcpy(pData1,pData,w*h*cn);
	for(int c=0;c<cn;c++){
		for(int j=1;j<h-1;j++){
			for(int i=1;i<w-1;i++){
				//float v=0;
				int pos=c*w*h+j*w+i;
				float curv=pData[pos];
				float minc=10000;
				float minv=0;
				for(int kj=0;kj<kh;kj++){
					for(int ki=0;ki<kw;ki++){
						if(kj==1 && ki==1)
							continue;
						int jj=j+kj-1;
						int ii=i+ki-1;
						int pos1=c*w*h+jj*w+ii;
					
						float v=(float)pData[pos1];//*k0[kj][ki];//+pData[pos]*k1[kj][ki];
						if(abs(v-curv)<abs(minc)){
							minv=v;
							minc=v-curv;
						}
					}
				}
				//v/=16;
				
				/*if(abs(v)<30)
					pData1[pos]=128;
				else*/
					pData1[pos]=(curv+minv)/2;//UcXz(0.5+v);//abs(v)+128;
			}
		}
	}
	m_pImg->Destroy();
	delete m_pImg;
	m_pImg=DataToImg(pData1,cn,w,h);
	delete []pData;
	delete []pData1;
	ReDrawItem();
}


void CWisdomLabelDlg::AddToHisDir(CStringA Dir)
{
	vector<CStringA> fList;
	fList.push_back(Dir);
	CFile fi;
	CStringA HisDir=GetModuleDir()+"\\cfg\\hisdir.txt";
	//;
	if(fi.Open(CString(HisDir),CFile::modeRead)){
		//CString ss;
		char Buf[4096];
		int size=fi.Read(Buf,4095);
		Buf[size]=0;
		vector<CStringA> List=SplitStr(CStringA(Buf),"\r\n");
		for(int k=0;k<List.size();k++){
			CStringA ss=List.at(k);
			//while(fi.ReadString(ss)){
			ss.Replace("\r","");
			ss.Replace("\n","");
			if(ss==Dir)
				continue;
			/*for(int i=0;i<fList.size();i++){
				CStringA ts=fList.at(i);
				if(ts==ss){
					fList.erase(fList.begin()+i);
					i--;
				}
			}*/
			fList.push_back(ss);
			//dlg.Datas[0].AddHead(ss);
		}
		fi.Close();
	}
	CFile fii;
	if(fii.Open(CString(HisDir),CFile::modeCreate|CFile::modeWrite)){
		for(int i=0;i<fList.size();i++){
			CStringA ts=fList.at(i);
			ts+="\r\n";
			//CString Str(ts);
			fii.Write(ts,ts.GetLength());
		}
		fii.Close();
	}
}


void CWisdomLabelDlg::OnBnClickedOpenpic19()
{
	// TODO: 在此添加控件通知处理程序代码
	CFolderPickerDialog  Dlg(NULL,0,this,0);
	if(!Dlg.DoModal()==IDOK)
		return;
	m_ResPath=Dlg.GetFolderPath();
	AddToHisDir(m_ResPath);
}


void CWisdomLabelDlg::OnBnClickedCdisbiao10()
{
	// TODO: 在此添加控件通知处理程序代码
	CStringA ss;
	CFileFind ff;
	int res=ff.FindFile(CString(m_LabelPath+"\\*.*"));
	while(res){
		res=ff.FindNextFile();
		if(ff.IsDots() || ff.IsDirectory())
			continue;
		CStringA FilePath=CStringA(ff.GetFilePath());
		//if(FilePath==m_PicPath)
		//	continue;
		CStringA FileName=CStringA(ff.GetFileName());
		CStringA FileTitle=CStringA(ff.GetFileTitle());
		CStringA hz=FilePath.Right(FileName.GetLength()-FileTitle.GetLength()-1);
		if(hz.CompareNoCase("xml")==0){
			CStringA Str=m_FolderPath+"\\"+FileTitle+".jpg";
			CFileStatus status;
			if(CFile::GetStatus(CString(Str),status)!=1){
				ss+=Str+"\r\n";
				DeleteFile(CString(FilePath));
			}
		}
	}
	CTextDlg dlg;
	dlg.m_Text=ss;
	dlg.DoModal();
}


void CWisdomLabelDlg::OnBnClickedCdisbiao11()
{
	// TODO: 在此添加控件通知处理程序代码
		CMTextDlg dlg;
	dlg.Title="输入尺寸";
	dlg.Size=1;
	
	dlg.Titles[0]="请选择类型:";
	dlg.Datas[0].AddTail(CString("yuv420p"));
	dlg.Datas[0].AddTail(CString("yuv420sp"));
	if(dlg.DoModal()!=IDOK)
		return;
	CStringA Type=dlg.Texts[0];
	int c=3;
	int w,h;
	unsigned char *Data=GetImgData(m_pImg,c,w,h);
	unsigned char *yuv=new unsigned char[w*h*c*3/2];
	unsigned char *R=Data;
	unsigned char *G=R+w*h;
	unsigned char *B=G+w*h;
	if(Type=="yuv420sp"){
		unsigned char *y=yuv;
		unsigned char *u=y+w*h;
		unsigned char *v=u+w*h/4;
		for(int j=0;j<h;j++){
			for(int i=0;i<w;i++){
				*y=UcXz(0.299*(*R)+0.587*(*G)+0.114*(*B));
				if(i%2==0 &&j%2==0){
					v=yuv+w*h+(j/2*w+i);
					u=v+1;
					*u=UcXz(-0.169* *R - 0.331* *G + 0.5  * *B + 128);;//0.493*((*B)-(*y));//0.493*((*B)－(*y));
					*v= UcXz(0.5 * *R - 0.419* *G + 0.081* *B + 128);//;//0,877*((*R)-(*y));//0.877(R－Y);
					//u++;
					//v++;
				}
				y++;
				R++;
				G++;
				B++;
			}
		}
		CFile f;
		if(f.Open(CString("d:\\2.yuv420sp"),CFile::modeCreate|CFile::modeWrite)){
			f.Write(yuv,w*h*c*3/2);
			f.Close();
		}
	}
	else{
		unsigned char *y=yuv;
		unsigned char *u=y+w*h;
		unsigned char *v=u+w*h/4;
		for(int j=0;j<h;j++){
			for(int i=0;i<w;i++){
				*y=UcXz(0.299*(*R)+0.587*(*G)+0.114*(*B));
				if(i%2==0 &&j%2==0){
					u=yuv+w*h+(j/2*w/2+i/2);
					v=u+w*h/4;
					*u=UcXz(-0.169* *R - 0.331* *G + 0.5  * *B + 128);;//0.493*((*B)-(*y));//0.493*((*B)－(*y));
					*v= UcXz(0.5 * *R - 0.419* *G + 0.081* *B + 128);//;//0,877*((*R)-(*y));//0.877(R－Y);
					//u++;
					//v++;
				}
				y++;
				R++;
				G++;
				B++;
			}
		}
		CFile f;
		if(f.Open(CString("d:\\2.yuv420p"),CFile::modeCreate|CFile::modeWrite)){
			f.Write(yuv,w*h*c*3/2);
			f.Close();
		}
	}
	//ConvertRGB2YUV(w,h,Data,yuv);

}


void CWisdomLabelDlg::OnBnClickedIdcreatedate50()
{
	// TODO: 在此添加控件通知处理程序代码
	int w,h;
	int cn=3;
	unsigned char *data=GetImgData(m_pImg,cn,w,h);
	unsigned char *row=new unsigned char[w];
	for(int c=0;c<cn;c++){
		for(int j=0;j<h/2;j++){
			memcpy(row,data+c*w*h+j*w,w);
			memcpy(data+c*w*h+j*w,data+c*w*h+(h-j-1)*w,w);
			memcpy(data+c*w*h+(h-j-1)*w,row,w);
		}
	}
	/*int cy=h/2;
	for(vector<Lk >::iterator it=m_LkList.begin();
		it!=m_LkList.end();it++){
		for(vector<CPoint>::iterator it1=it->Pts.begin();it1!=it->Pts.end();it1++){
			if(it1->y=cy+
		}
	}*/
	if(m_LkList.size()>0)
	{
		for(vector<Lk >::iterator it=m_LkList.begin();it!=m_LkList.end();it++){
			for(vector<CPoint>::iterator it1=it->Pts.begin();it1!=it->Pts.end();it1++){
				it1->y=h-1-it1->y;
			}
		}
	}
	m_pImg->Destroy();
	delete m_pImg;
	m_pImg=NULL;
	m_pImg=DataToImg(data,cn,w,h);
	
	saveImg(m_PicPath,m_pImg);
	SaveXml(this->GetXmlFileName(),m_PicName,m_PicPath,m_FolderPath,
			m_pImg->GetWidth(),m_pImg->GetHeight(),m_pImg->GetBPP()/8,&m_LkList);
	free(data);
	free(row);
	ReDrawItem();
}


void CWisdomLabelDlg::OnGetpicbylable()
{
	// TODO: 在此添加命令处理程序代码

	

	CMTextDlg dlg;
	dlg.Title="标签";
	dlg.Size=5;
	dlg.Titles[0]="请输入要提取的标签,多个用逗号隔开:";
	dlg.Titles[1]="请选择标注目录:";
	dlg.Titles[2]="请选择图片目录:";
	dlg.Titles[3]="请选择新的标注目录:";
	dlg.Titles[4]="请选择新的图片目录:";
	dlg.bLl[1]=2;
	dlg.bLl[2]=2;
	dlg.bLl[3]=2;
	dlg.bLl[4]=2;
	dlg.Texts[2]=m_FolderPath;
	dlg.Texts[1]=m_LabelPath;
	dlg.TextWidth=800;
	if(dlg.DoModal()!=IDOK)
		return;
	CStringA BzDir=dlg.Texts[1];


	CStringA PicDir=dlg.Texts[2];

	
	CStringA NewBzDir=dlg.Texts[3];


	CStringA NewPicDir=dlg.Texts[4];

	vector<CStringA> biaoqians=SplitStr(dlg.Texts[0],",");
		
	char Buf[64000];
	CFileFind ff;
	int res=ff.FindFile(CString(BzDir+"\\*.*"));
	while(res){
		res=ff.FindNextFile();
		if(ff.IsDots() || ff.IsDirectory())
			continue;
		CStringA FilePath=CStringA(ff.GetFilePath());
		//if(FilePath==m_PicPath)
		//	continue;
		CStringA FileName=CStringA(ff.GetFileName());
		CStringA FileTitle=CStringA(ff.GetFileTitle());
		CStringA hz=FilePath.Right(FileName.GetLength()-FileTitle.GetLength()-1);
		if(hz.CompareNoCase("xml")==0){
			CFile fi;
			if(!fi.Open(CString(FilePath),CFile::modeRead))
				continue;
			int Size=fi.Read(Buf,64000);
			fi.Close();
			
			Buf[Size]=0;
			CStringA Str(Buf);
			CStringA StrBf=Str;
			CStringA JpgPath=FilePath;
			JpgPath.Replace(BzDir,PicDir);
			JpgPath=JpgPath.Left(JpgPath.GetLength()-4);
			JpgPath+=".jpg";
			
			int bCopy=0;
			int pos=0;
			CStringA Fs="<object>";
			while((pos=Str.Find(Fs))!=-1){
				pos+=Fs.GetLength();
				Str=Str.Mid(pos);
				CStringA Fs1="<name>";
				pos=Str.Find(Fs1);
				if(pos!=-1){
					pos+=Fs1.GetLength();
					Str=Str.Mid(pos);
					int pos2=Str.Find("</name>");
					if(pos2!=-1){
						CStringA Name=Str.Left(pos2);
						int bFind=0;
						for(int k=0;k<biaoqians.size();k++){
							if(Name == biaoqians.at(k)){
								bFind=1;
								break;
							}
						}
						if(bFind){//Name==biaoqian
							CStringA CarPath=JpgPath;
							CarPath.Replace(PicDir,NewPicDir);
							CopyFile(CString(JpgPath),CString(CarPath),1);
							bCopy=1;
							break;
						}
					}
				}
			}
			if(bCopy){
				//StrBf.Replace("<name>car</name>","<name>vehicle</name>");
				//StrBf.Replace("<name>bus</name>","<name>vehicle</name>");
				CStringA CarPath=FilePath;
				CarPath.Replace(BzDir,NewBzDir);
				CopyFile(CString(FilePath),CString(CarPath),1);
				/*CFile f;
				if(f.Open(CarPath,CFile::modeCreate|CFile::modeWrite)){
					f.Write(StrBf,StrBf.GetLength());
					f.Close();
				}*/
			}
		}
		//vehicle
		//CStringA hz1=hz;
	}
	

}


void CWisdomLabelDlg::OnCheckname()
{
	// TODO: 在此添加命令处理程序代码
//<filename>"+picName+"</filename>
	CFolderPickerDialog  Dlg(NULL,0,this,0);// Dlg(TRUE,NULL,NULL,NULL,NULL,NULL);
	Dlg.m_ofn.lpstrTitle=CString("请选择标注目录");
	if(Dlg.DoModal()!=IDOK)
		return;
	CStringA BzDir=CStringA(Dlg.GetPathName());


	CStringA errNames;
	char Buf[64000];
	CFileFind ff;
	int res=ff.FindFile(CString(BzDir)+"\\*.*");
	while(res){
		res=ff.FindNextFile();
		if(ff.IsDots() || ff.IsDirectory())
			continue;
		CStringA FilePath=CStringA(ff.GetFilePath());
		//if(FilePath==m_PicPath)
		//	continue;
		CStringA FileName=CStringA(ff.GetFileName());
		CStringA FileTitle=CStringA(ff.GetFileTitle());
		CStringA hz=FilePath.Right(FileName.GetLength()-FileTitle.GetLength()-1);
		if(hz.CompareNoCase("xml")==0){
			CFile fi;
			if(!fi.Open(CString(FilePath),CFile::modeRead))
				continue;
			int Size=fi.Read(Buf,64000);
			fi.Close();
			
			Buf[Size]=0;
			CStringA Str(Buf);
			CStringA StrBf=Str;
			CStringA JpgPath="<filename>"+FileTitle+".jpg"+"</filename>";
			if(Str.Find(JpgPath)==-1){
				errNames+=JpgPath+";";
				//AfxMessageBoxA(JpgPath+"filename error!");
			}
			
			
		}
		//vehicle
		//CStringA hz1=hz;
	}
	CTextDlg tDlg;
	tDlg.m_Text=errNames;
	tDlg.DoModal();
}


void CWisdomLabelDlg::OnCheckbox()
{
	// TODO: 在此添加命令处理程序代码
		CFolderPickerDialog  Dlg(NULL,0,this,0);// Dlg(TRUE,NULL,NULL,NULL,NULL,NULL);
	Dlg.m_ofn.lpstrTitle=CString("请选择标注目录");
	if(Dlg.DoModal()!=IDOK)
		return;
	CStringA BzDir=CStringA(Dlg.GetPathName());

		Dlg.m_ofn.lpstrTitle=CString("请选择图片目录");
	if(Dlg.DoModal()!=IDOK)
		return;
	CStringA PicDir=CStringA(Dlg.GetPathName());


		
	//char Buf[64000];
	CStringA StrAll;
	CFileFind ff;
	int res=ff.FindFile(CString(BzDir+"\\*.*"));
	while(res){
		res=ff.FindNextFile();
		if(ff.IsDots() || ff.IsDirectory())
			continue;
		CStringA FilePath=CStringA(ff.GetFilePath());
		//if(FilePath==m_PicPath)
		//	continue;
		CStringA FileName=CStringA(ff.GetFileName());
		CStringA FileTitle=CStringA(ff.GetFileTitle());
		CStringA hz=FilePath.Right(FileName.GetLength()-FileTitle.GetLength()-1);
		if(hz.CompareNoCase("xml")==0){
			CStringA JpgPath=FilePath;
			JpgPath.Replace(BzDir,PicDir);
			JpgPath=JpgPath.Left(JpgPath.GetLength()-4);
			JpgPath+=".jpg";
			CImage img;
			img.Load(CString(JpgPath));
			if(img.IsNull()){
				AfxMessageBoxA("open "+JpgPath+" failed！");
				continue;
			}
			vector<Lk >lks;
			OpenXml(FilePath,&lks);
			for(int i=0;i<lks.size();i++){
				Lk lk=lks.at(i);
				for(int j=0;j<lk.Pts.size();j++){
					CPoint pt=lk.Pts.at(j);
					if(pt.x<0||pt.x>img.GetWidth()||pt.y<0||pt.y>img.GetHeight()){
						StrAll+=FileTitle+"\r\n";
						//AfxMessageBoxA(FilePath);
						break;
					}
				}
			}
			img.Destroy();
		}
	}
	CTextDlg dlg;
	dlg.m_Text=StrAll;
	dlg.DoModal();
}


LRESULT CWisdomLabelDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加专用代码和/或调用基类
	if(message==WM_MOUSEWHEEL){
		/*CPoint pt;
		GetCursorPos(&pt);
		ScreenToClient(&pt);
		CRect Rc;
		GetClientRect(&Rc);
		Rc.left+=m_LeftWidth;
		Rc.right-=m_RightWidth;
		if(Rc.PtInRect(pt)){
			
		}*/
	/*	int zDelta=pMsg->wParam>>16;
		UINT nFlags=pMsg->wParam-(zDelta<<16);
		CPoint pt(pMsg->lParam%65536,pMsg->lParam>>16);
		OnMouseWheel(nFlags,zDelta,pt);
		return TRUE;*/
	}
	return CDialogEx::DefWindowProc(message, wParam, lParam);
}


void CWisdomLabelDlg::OnBnClickedRuihua3()
{
	// TODO: 在此添加控件通知处理程序代码
		int cn=3;
	int w,h;
	int kw=3;
	int kh=3;
	//float k0[3][3]={
	//	{-0.1,-0.1,-0.1},
	//	{-0.1,1.8,-0.1},
	//	{-0.1,-0.1,-0.1}
	//};
	//float k1[3][3]={
	//	{-1,0,1},
	//	{-1,0,1},
	//	{-1,0,1}
	//};
	unsigned char *pData=GetImgData(m_pImg,cn,w,h);
	unsigned char *pData1=new unsigned char[w*h*cn];
	memcpy(pData1,pData,w*h*cn);
	//memset(pData1,128,w*h*cn);
	for(int j=0;j<h-2;j++){
		for(int i=0;i<w-2;i++){
			for(int k=0;k<cn;k++){
				//vector<unsigned char> tmp;
				float tmp[9]={0};
				for(int kj=0;kj<kh;kj++){
					for(int ki=0;ki<kw;ki++){
						int jj=j+kj;
						int ii=i+ki;
						int pos=k*w*h+jj*w+ii;
						tmp[kj*kw+ki]=pData[pos];
					}
				}
				//v/=16;
				//sort(tmp.begin(),tmp.end());
				sort1(tmp,9);
				int pos=k*w*h+j*w+i+w+1;
				int v1=abs(tmp[0]-pData[pos]);
				int v2=abs(tmp[8]-pData[pos]);
				/*if(abs(v)<30)
					pData1[pos]=128;
				else*/
				float bz=(float)v1/(float)v2;
				if(bz<0.5||bz>2){
					if(v1<v2)
						pData1[pos]=tmp[0];//.at(tmp.size()/2);//abs(v)+128;
					else
						pData1[pos]=tmp[8];
				}
			}
		}
	}
	m_pImg->Destroy();
	delete m_pImg;
	m_pImg=DataToImg(pData1,cn,w,h);
	delete []pData;
	delete []pData1;
	ReDrawItem();
}


void CWisdomLabelDlg::OnRuihua3()
{
	// TODO: 在此添加命令处理程序代码
	int cn=3;
	int w,h;
	int kw=3;
	int kh=3;
	float k0[3][3]={
		{-0.1,-0.1,-0.1},
		{-0.1,1.8,-0.1},
		{-0.1,-0.1,-0.1}
	};
	//float k1[3][3]={
	//	{-1,0,1},
	//	{-1,0,1},
	//	{-1,0,1}
	//};
	unsigned char *pData=GetImgData(m_pImg,cn,w,h);
	unsigned char *pData1=new unsigned char[w*h*cn];
	//memset(pData1,128,w*h*cn);
	memcpy(pData1,pData,w*h*cn);
	for(int c=0;c<cn;c++){
		for(int j=1;j<h-1;j++){
			for(int i=1;i<w-1;i++){
				int pos=c*w*h+j*w+i;
				float v=0;
				for(int kj=0;kj<kh;kj++){
					for(int ki=0;ki<kw;ki++){
						int jj=j+kj-1;
						int ii=i+ki-1;
						int pos1=c*w*h+jj*w+ii;
						v+=(float)abs(pData[pos1]*-pData[pos]);//+pData[pos]*k1[kj][ki];
					}
				}
				v/=8.0;

				float cz=0.5;

				for(int kj=0;kj<kh;kj++){
					for(int ki=0;ki<kw;ki++){
						if(kj==1&&ki==1){
							k0[kj][ki]=1+cz*8;
						}
						else
							k0[kj][ki]=0.0-cz;
					}
				}
				
				
				 v=0;
				for(int kj=0;kj<kh;kj++){
					for(int ki=0;ki<kw;ki++){
						int jj=j+kj-1;
						int ii=i+ki-1;
						int pos=c*w*h+jj*w+ii;
						v+=(float)pData[pos]*k0[kj][ki];//+pData[pos]*k1[kj][ki];
					}
				}
				//v/=16;
				
				/*if(abs(v)<30)
					pData1[pos]=128;
				else*/
					pData1[pos]=UcXz(0.5+v);//abs(v)+128;
			}
		}
	}
	m_pImg->Destroy();
	delete m_pImg;
	m_pImg=DataToImg(pData1,cn,w,h);
	delete []pData;
	delete []pData1;
	ReDrawItem();
}


void CWisdomLabelDlg::OnCreatepicmodel()
{
	// TODO: 在此添加命令处理程序代码
	CFont font;
	font.CreateFont(
	   18,                        // nHeight
	   0,                         // nWidth
	   0,                         // nEscapement
	   0,                         // nOrientation
	   FW_NORMAL,                 // nWeight
	   FALSE,                     // bItalic
	   FALSE,                     // bUnderline
	   0,                         // cStrikeOut
	   ANSI_CHARSET,              // nCharSet
	   OUT_DEFAULT_PRECIS,        // nOutPrecision
	   CLIP_DEFAULT_PRECIS,       // nClipPrecision
	   DEFAULT_QUALITY,           // nQuality
	   DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
	   _T("隶书")); 
	static const char* class_names[] = {"使用手机","抽烟","正脸","侧脸","打哈欠","闭眼睛"};
	CRect rc(0,0,72,140);
	COLORREF bkColor(RGB(2,2,2));
	COLORREF textColor(RGB(0,0,0));
	CDC *pDC=GetDC();
	CDC memDC;
	memDC.CreateCompatibleDC(pDC);
	CBitmap bit;
	bit.CreateCompatibleBitmap(pDC,rc.Width(),rc.Height());
	CBitmap *pOldBit=memDC.SelectObject(&bit);
	CFont *pOldFont=memDC.SelectObject(&font);
	memDC.SetTextColor(textColor);
	memDC.SetBkColor(bkColor);
	memDC.FillRect(&rc,&CBrush(bkColor));
	for(int i=0;i<6;i++){
		memDC.TextOut(1,i*20,CString(class_names[i]));
	}
	memDC.SelectObject(pOldBit);
	memDC.SelectObject(pOldFont);
	ReleaseDC(pDC);
	CImage img;
	img.Attach((HBITMAP)bit.m_hObject);
	img.Save(CString("d:\\model1.png"));
	img.Detach();
	bit.DeleteObject();
	font.DeleteObject();

}
#define MAXN 29
#define MHN 8
#define JI0 1
#define JI1 32
#define JI2 32*32
#define JI3 32*32*32
#define JI4 32*32*32*32
#define JI5 32*32*32*32*32
int gMhTjThreadRun=0;
UINT static mhhyThread(void *lp){
	float or[MAXN];
	float mid[MAXN];
	float res[MAXN];
	int *tj=new int[JI5];
	int *tjbf=new int[JI5];
	int *tjts=new int[JI5];
	memset(tj,0,JI5*sizeof(int));
	memset(tjts,0,JI5*sizeof(int));
	srand(GetTickCount());
	{
		CFile f;
		if(f.Open(CString("d:\\mhhytj.txt"),CFile::modeRead)){
			f.Read(tj,JI5*sizeof(int));
			f.Read(tjts,JI5*sizeof(int));
			f.Close();
		}
	
	
	}
	//unsigned int st=GetTickCount();
	for(int x=0;x<0xffff;x++){
		if(gMhTjThreadRun==0)
			break;
		for(int y=0;y<0xffffff;y++){
			if(gMhTjThreadRun==0)
				break;
			for(int j=0;j<MAXN;j++)
				or[j]=rand()%256;
			memcpy(res,or,sizeof(float)*MAXN);
			memcpy(mid,or,sizeof(float)*MAXN);
			for(int k=0;k<MHN;k++){
				for(int j=1;j<MAXN-1;j++){
					res[j]=(mid[j]+mid[j-1]+mid[j+1])/3;
				}
				memcpy(mid,res,sizeof(float)*MAXN);
			}
			//float jz=0;
			int start=MAXN/2-2;
			int end=MAXN/2+2;
			int midi=MAXN/2;
			int pos=res[start+0]/8*JI0
				+res[start+1]/8*JI1
				+res[start+2]/8*JI2
				+res[start+3]/8*JI3
				+res[start+4]/8*JI4;
			tj[pos]+=or[midi]-res[midi];
			tjts[pos]++;
			//unsigned int ct=GetTickCount();
		}
		memcpy(tjbf,tj,sizeof(int)*JI5);
		for(int i=0;i<JI5;i++){
			if(tjts[i]>0){
				if(tjts[i]>1){
					tjbf[i]/=tjts[i];
				}
			}
		}
		CFile f;
		if(f.Open(CString(CString("d:\\mhhy.txt")),CFile::modeCreate|CFile::modeWrite)){
			f.Write(tjbf,JI5*sizeof(int));
			f.Close();
		}
		if(f.Open(CString(CString("d:\\mhhytj.txt")),CFile::modeCreate|CFile::modeWrite)){
			f.Write(tj,JI5*sizeof(int));
			f.Write(tjts,JI5*sizeof(int));
			f.Close();
		}
		if(x==0)
			AfxMessageBoxA("x==0");
	}
	{
		CFile f;
		if(f.Open(CString("d:\\mhhytj.txt"),CFile::modeCreate|CFile::modeWrite)){
				f.Write(tj,JI5*sizeof(int));
				f.Write(tjts,JI5*sizeof(int));
				f.Close();
		}
		for(int i=0;i<JI5;i++){
			if(tjts[i]>0){
				if(tjts[i]>1){
					tj[i]/=tjts[i];
				}
			}
		}
	
		if(f.Open(CString("d:\\mhhy.txt"),CFile::modeCreate|CFile::modeWrite)){
			f.Write(tj,JI5*sizeof(int));
			f.Close();
		}
	}
	delete []tj;
	delete []tjts;
	delete []tjbf;
	gMhTjThreadRun=2;
	//AfxMessageBoxA("ok");
	return 1;
}
void CWisdomLabelDlg::OnMhhytj()
{
	// TODO: 在此添加命令处理程序代码
	if(gMhTjThreadRun==1)
		return;
	gMhTjThreadRun=1;
	AfxBeginThread((AFX_THREADPROC)mhhyThread,0);
}


void CWisdomLabelDlg::OnMhhy()
{
	// TODO: 在此添加命令处理程序代码
	int *tj=new int[JI5];
	//int *tjbf=new int[JI5];
	int *tjts=new int[JI5];
	memset(tj,0,JI5*sizeof(int));
	memset(tjts,0,JI5*sizeof(int));
	{
		CFile f;
		if(f.Open(CString("d:\\mhhytj.txt"),CFile::modeRead)){
			f.Read(tj,JI5*sizeof(int));
			f.Read(tjts,JI5*sizeof(int));
			f.Close();
		}
		for(int i=0;i<JI5;i++){
			if(tjts[i]>0){
				if(tjts[i]>1){
					tj[i]/=tjts[i];
				}
			}
		}
	}
	int cn=3;
	int w,h;
	unsigned char *pData=GetImgData(m_pImg,cn,w,h);
	unsigned char *pData1=new unsigned char[w*h*cn];
	//memset(pData1,128,w*h*cn);
	memcpy(pData1,pData,w*h*cn);
	for(int c=0;c<cn;c++){
		for(int j=2;j<h-2;j++){
			for(int i=2;i<w-2;i++){
				int pos=c*w*h+j*w+i;
				int v4=0;
				int v[5];
				for(int ki=-2;ki<=2;ki++){
					int jj=j;
					int ii=i+ki;
					int pos1=c*w*h+jj*w+ii;
					v[ki+2]=pData[pos1];
				}

				int pos2=v[0]/8*JI0
				+v[1]/8*JI1
				+v[2]/8*JI2
				+v[3]/8*JI3
				+v[4]/8*JI4;
				v4+=tj[pos2];

				for(int kj=-2;kj<=2;kj++){
					int jj=j+kj;
					int ii=i;//+ki;
					int pos1=c*w*h+jj*w+ii;
					v[kj+2]=pData[pos1];
				}

				pos2=v[0]/8*JI0
				+v[1]/8*JI1
				+v[2]/8*JI2
				+v[3]/8*JI3
				+v[4]/8*JI4;
				v4+=tj[pos2];
				pData[pos]=UcXz(v4/16+pData[pos]);
			}
		}
	}
	m_pImg->Destroy();
	delete m_pImg;
	m_pImg=DataToImg(pData,cn,w,h);
	delete []pData;
	delete []pData1;
	delete []tj;
	delete []tjts;
	
	ReDrawItem();
}


void CWisdomLabelDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(gMhTjThreadRun==1){
		return;
		/*gMhTjThreadRun=0;
		while(gMhTjThreadRun==2)
			break;*/
	}
	CDialogEx::OnClose();
}


void CWisdomLabelDlg::OnStopmhhytj()
{
	// TODO: 在此添加命令处理程序代码
	gMhTjThreadRun=0;
}


void CWisdomLabelDlg::OnOpenvideoframe()
{
	// TODO: 在此添加命令处理程序代码
	OnBnClickedVideoToOnePic();
}


void CWisdomLabelDlg::On32789()
{
	// TODO: 在此添加命令处理程序代码
	//int getCross(fLine line1, fLine line2,fPoint *res);
	fPoint resPt;
	int res;
	res=getCross(fLine(fPoint(1,3),fPoint(5,8)),fLine(fPoint(2,6),fPoint(4,7)),&resPt);
	res=getCross(fLine(fPoint(1,3),fPoint(1,8)),fLine(fPoint(2,6),fPoint(4,7)),&resPt);
	res=getCross(fLine(fPoint(1,3),fPoint(5,8)),fLine(fPoint(2,6),fPoint(2,7)),&resPt);
	res=getCross(fLine(fPoint(1,3),fPoint(5,8)),fLine(fPoint(2,6),fPoint(4,6)),&resPt);

	res=getCross(fLine(fPoint(1,3),fPoint(1,8)),fLine(fPoint(2,6),fPoint(2,7)),&resPt);
	res=getCross(fLine(fPoint(1,3),fPoint(5,3)),fLine(fPoint(2,6),fPoint(4,6)),&resPt);
	res=getCross(fLine(fPoint(1,3),fPoint(5,8)),fLine(fPoint(2,4),fPoint(6,9)),&resPt);
	int b=res;
}


void CWisdomLabelDlg::OnBnClickedCdisbiao12()
{
	// TODO: 在此添加控件通知处理程序代码
	CStringA Str;
	if(m_TmpDir.IsEmpty()){
		Str=m_FolderPath+"\\tmp";
	}else
		Str=m_TmpDir;
	CreateDirectory(CString(Str),NULL);
	CStringA Str1=Str;
	Str+="\\"+m_PicName;
	MoveFile(CString(m_PicPath),CString(Str));
	CStringA XmlStr=Str1+"\\"+m_PicTitle+".xml";
	Str+="\\"+m_PicName;
	//CopyFile(CString(m_PicPath),CString(Str),1);
	CopyFile(CString(GetXmlFileName()),CString(XmlStr),1);
	OnBnClickedOpenpic2();

}


void CWisdomLabelDlg::OnBnClickedIdcreatedate51()
{
	// TODO: 在此添加控件通知处理程序代码
	//CFileDialog dlg(TRUE,"avi",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"(*.avi;*.mp4)|*.avi;*.mp4||"); 
	//dlg.m_ofn.lpstrTitle="请选择视频文件";
	//if(dlg.DoModal() != IDOK)
	//	return;
	//CFolderPickerDialog  dlg1(NULL,0,this,0);
	//if(dlg1.DoModal()!=IDOK){
	//	return;
	//}
	CMTextDlg pDlg;
	pDlg.Size=4;
	pDlg.Title="视频转多个图片:";
	pDlg.Titles[0]="请输入视频文件地址:";
	pDlg.Titles[1]="请输入图片目录:";
	pDlg.bLl[0]=1;
	pDlg.bLl[1]=2;
	pDlg.Titles[2]="请输入帧率(1秒转几帧):";
	pDlg.Texts[2]="15";
	pDlg.Titles[3]="请输入前缀:";
	pDlg.TextWidth=800;
	if(pDlg.DoModal()!=IDOK)
		return;
	/*CMTextDlg pDlg;
	pDlg.Size=2;
	pDlg.Title="请选择时间";
	pDlg.Titles[0]="请选择时间(秒):";
	pDlg.Texts[0]="1";
	pDlg.Titles[1]="请输入摄像头ID:";
	pDlg.Texts[1]="0";
	if(pDlg.DoModal()!=IDOK)
		return;*/
	CStringA strPath=GetModuleDir()+"\\ffmpeg\\";
	//ffmpeg -i e:\wb\2.mp4 -r 1 -f image2 e:\wb\2-%05d.jpg
	//CStringA OutPath=GetModuleDir()+"\\"+CStringA(pDlg.Texts[1])+".jpg";//tmp.jpg";
		//ffmpeg -i test.asf -y -f image2 -t 0.001 -s 352x240 a.jpg
	//ffmpeg -i \"%s\" -r 15 -f image2 %d-%s.jpg
	CStringA cmdLine=strPath+"ffmpeg.exe -i \""+pDlg.Texts[0]+"\" -f image2 -r "+pDlg.Texts[2]+"  \""+pDlg.Texts[1]+"\\"+pDlg.Texts[3]+"%05d.jpg\"";
	//CStringA cmdLine=strPath+"ffmpeg.exe -i "+dlg.GetPathName()+" -y -f image2 -t 0.001 "+OutPath;
	CTextDlg dlg2;
	dlg2.m_Text=cmdLine;
	dlg2.DoModal();
	STARTUPINFO si={sizeof(si)};
	PROCESS_INFORMATION pif;
		si.dwFlags=STARTF_USESHOWWINDOW;
		si.wShowWindow=TRUE;//TRUE表示显示创建的进程的窗口
		BOOL bRet=::CreateProcess(
						NULL,//TEXT("c://programfiles//internetexplorer//iexplore.exe"),
						CString(cmdLine).GetBuffer(0),//cmdline,//在Unicode版本中此参数不能为常量字符串，因为此参数会被修改
						NULL,
						NULL,
						FALSE,
						CREATE_NEW_CONSOLE,
						NULL,
						CString(strPath),
						&si,
						&pif);
	
	if(bRet)
	{
		WaitForSingleObject( pif.hProcess, INFINITE );
		::CloseHandle(pif.hThread);
		::CloseHandle(pif.hProcess);
	}
	else
	{
		//int error=GetLastError();
		AfxMessageBoxA("获取视频的图片失败");
		return;
	}
	OpenDir(pDlg.Texts[1]);
}


void CWisdomLabelDlg::OnNextall()
{
	// TODO: 在此添加命令处理程序代码
	/*while(m_pImgListCtrl.GetItemCount()-1>GetSelID()){
		OnBnClickedOpenpic2();
		MSG message; 
		while(PeekMessage (&message,NULL,0,0,PM_REMOVE)) 
		{ 
			TranslateMessage (&message); 
			DispatchMessage (&message); 
			if(message.message==WM_CLOSE){
				return;
			}
		} 
	}*/
	SetTimer(0,200,NULL);
}


void CWisdomLabelDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(nIDEvent ==0){
		KillTimer(0);
		if(m_pImgListCtrl.GetItemCount()-1>GetSelID()){
			OnBnClickedOpenpic2();
			SetTimer(0,10,NULL);
		}
		
	}else if(nIDEvent==1){
		int w=m_pImg->GetWidth();
		int h=m_pImg->GetHeight();
		
		CPoint ptSrc=ToSrc(m_MousePos);
		if(ptSrc.x>1 && ptSrc.x<w-2 && ptSrc.y>1&&ptSrc.y<w-2){
			mohu2(m_pImg,ptSrc.x,ptSrc.y,m_SectPtW/m_DrawScale);
			ReDrawItem();
		}
	}
	else if(nIDEvent ==3){
		KillTimer(3);
		if(m_pImgListCtrl.GetItemCount()-1>GetSelID()&&PCmdList.size()>0){
			if(m_PTimes++>0)
				m_IsPCl=1;
			for(int i=0;i<PCmdList.size();i++){
				SendMessage(WM_COMMAND,MAKEWPARAM(PCmdList.at(i),BN_CLICKED),0);
			}
			//OnBnClickedOpenpic2();
			m_IsPCl=0;
			SetTimer(3,10,NULL);
		}
		
	}
	else if(nIDEvent ==10){
		if(m_PiCmd>=0){
			SendMessage(WM_COMMAND,MAKEWPARAM(m_PiCmd,BN_CLICKED),0);
		}else{
			KillTimer(10);
		}
	}
	CDialogEx::OnTimer(nIDEvent);
}


void CWisdomLabelDlg::OnBnClickedOpenpic20()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_pImgListCtrl.GetItemCount()<=0)
		return;
	if(m_ChangePic){
		if(AfxMessageBoxA("您已经修改了图片，是否要保存图片?",MB_YESNO)==IDYES){
			m_pImg->Save(CString(m_PicPath));
			SaveXml(GetXmlFileName(),m_PicName,m_PicPath,m_FolderPath,m_pImg->GetWidth(),m_pImg->GetHeight(),m_pImg->GetBPP()/8,&m_LkList);
			m_ChangePic=0;
		}
	}
	
		/*CStringA PicName=m_PicList.RemoveHead();
		m_PicList.AddTail(PicName);
		PicName=m_PicList.GetHead();*/
		CStringA sTmp=GetWndTxtA(&m_KdBtn);
			
		//.GetWindowText(CString(sTmp));
		int jg=atoi(sTmp);
		int id=GetSelID();
		if(id<m_pImgListCtrl.GetItemCount()-1){
			id+=jg;
			if(id>m_pImgListCtrl.GetItemCount()-1)
				id=m_pImgListCtrl.GetItemCount()-1;
			m_pImgListCtrl.SetSelectItem(id);
		}
		//OpenNewPic(PicName);
	
}


void CWisdomLabelDlg::OnBnClickedOpenpic21()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_pImgListCtrl.GetItemCount()<=0)
		return;
	if(m_ChangePic){
		if(AfxMessageBoxA("您已经修改了图片，是否要保存图片?",MB_YESNO)==IDYES){
			m_pImg->Save(CString(m_PicPath));
			SaveXml(GetXmlFileName(),m_PicName,m_PicPath,m_FolderPath,m_pImg->GetWidth(),m_pImg->GetHeight(),m_pImg->GetBPP()/8,&m_LkList);
			m_ChangePic=0;
		}
	}
	
		/*CStringA PicName=m_PicList.RemoveHead();
		m_PicList.AddTail(PicName);
		PicName=m_PicList.GetHead();*/
		CStringA sTmp=GetWndTxtA(&m_KdBtn);
		int jg=atoi(sTmp);
		int id=GetSelID();
		if(id>0){
			id-=jg;
			if(id<0)
				id=0;
			m_pImgListCtrl.SetSelectItem(id);
		}
		//OpenNewPic(PicName);
	
}


void CWisdomLabelDlg::OnFgjzlb()
{
	// TODO: 在此添加命令处理程序代码
	int cn=3;
	int w,h;
	int kw=3;
	int kh=3;
	
	unsigned char *pData=GetImgData(m_pImg,cn,w,h);
	unsigned char *pData1=new unsigned char[w*h*cn];
	unsigned char *pBj=new unsigned char[w*h*cn];
	memcpy(pData1,pData,w*h*cn);
	memset(pBj,0,w*h*cn);
	for(int c=0;c<cn;c++)
	for(int j=1;j<h-1;j++){
		for(int i=1;i<w-1;i++){
			int pos=c*w*h+j*w+i;
			float vmax=pData[pos];
			float vmin=vmax;
			//vector<unsigned char> tmp;
			int jh=kh/2;
			int ih=kw/2;
			/*if(i==1050 && j==1020){
				int a=3;
			}*/
			for(int kj=0;kj<kh;kj++){
				for(int ki=0;ki<kw;ki++){
					int jj=j+kj-jh;
					int ii=i+ki-ih;
					int pos1=c*w*h+jj*w+ii;
					if(pData[pos1]>vmax)
						vmax=pData[pos1];
					if(pData[pos1]<vmin)
						vmin=pData[pos1];
				}
			}
			if(vmin == pData[pos]){
				pBj[pos]=1;
				pData1[pos]=0;
			}
			if(vmax==pData[pos]){
				pBj[pos]=1;
				pData1[pos]=255;
			}
		}
	}
	//memset(pData1,128,w*h);
	/*for(int c=0;c<cn;c++)
	for(int j=1;j<h-1;j++){
		for(int i=1;i<w-1;i++){
			int pos=c*w*h+j*w+i;
			if(pBj[pos]==1)
				continue;
			float v=0;
			//vector<unsigned char> tmp;
			int jh=kh/2;
			int ih=kw/2;
			for(int kj=0;kj<kh;kj++){
				for(int ki=0;ki<kw;ki++){
					int jj=j+kj-jh;
					int ii=i+ki-ih;
					int pos1=c*w*h+jj*w+ii;
					v+=pData[pos1];
				}
			}
			
			pData1[pos]=v/(kh*kw);
		}
	}*/
	m_pImg->Destroy();
	delete m_pImg;
	m_pImg=DataToImg(pData1,cn,w,h);
	delete []pData;
	delete []pData1;
	delete []pBj;
	ReDrawItem();
}


void CWisdomLabelDlg::OnBnClickedCdisbiao13()
{
	// TODO: 在此添加控件通知处理程序代码
	::CreateDirectory(CString(m_FolderPath+"\\move"),NULL);
	vector<CStringA> List;
	CStringA ss;
	CFileFind ff;
	int res=ff.FindFile(CString(m_FolderPath+"\\*.*"));
	while(res){
		res=ff.FindNextFile();
		if(ff.IsDots() || ff.IsDirectory())
			continue;
		CStringA FilePath=CStringA(ff.GetFilePath());
		//if(FilePath==m_PicPath)
		//	continue;
		CStringA FileName=CStringA(ff.GetFileName());
		CStringA FileTitle=CStringA(ff.GetFileTitle());
		CStringA hz=FilePath.Right(FileName.GetLength()-FileTitle.GetLength()-1);
		if(hz.CompareNoCase("jpg")==0){
			CStringA Str=m_LabelPath+"\\"+FileTitle+".xml";
			CFileStatus status;
			if(CFile::GetStatus(CString(Str),status)!=1){
				//ss+=Str+"\r\n";
				List.push_back(FileName);
				//DeleteFile(FilePath);
			}
		}
	}
	//CTextDlg dlg;
	//dlg.m_Text=ss;
	//dlg.DoModal();
	for(int i=0;i<List.size();i++){
		CStringA Str=List.at(i);
		CStringA Name1=m_FolderPath+"\\"+Str;
		CStringA Name2=m_FolderPath+"\\move\\"+Str;
		MoveFile(CString(Name1),CString(Name2));
	}
}


void CWisdomLabelDlg::OnBnClickedCdisbiao14()
{
	// TODO: 在此添加控件通知处理程序代码
	CMTextDlg dlg;
	dlg.Title="移动数量设置";
	dlg.Size=2;
	dlg.Texts[0]="100";
	dlg.Texts[1]="0";
	dlg.Titles[0]="请输入移动多少张图片:";
	dlg.Titles[1]="请输入是否保留原图,1保留:";
	dlg.DoModal();
	::CreateDirectory(CString(m_FolderPath+"\\move"),NULL);
	vector<CStringA> List;
	CStringA ss;
	CFileFind ff;
	int res=ff.FindFile(CString(m_FolderPath)+"\\*.*");
	while(res){
		res=ff.FindNextFile();
		if(ff.IsDots() || ff.IsDirectory())
			continue;
		CStringA FilePath=CStringA(ff.GetFilePath());
		//if(FilePath==m_PicPath)
		//	continue;
		CStringA FileName=CStringA(ff.GetFileName());
		CStringA FileTitle=CStringA(ff.GetFileTitle());
		CStringA hz=FilePath.Right(FileName.GetLength()-FileTitle.GetLength()-1);
		if(hz.CompareNoCase("jpg")==0){
			CStringA Str=m_LabelPath+"\\"+FileTitle+".xml";
			CFileStatus status;
			if(CFile::GetStatus(CString(Str),status)!=1){
				//ss+=Str+"\r\n";
				List.push_back(FileName);
				//DeleteFile(FilePath);
			}
		}
	}
	//CTextDlg dlg;
	//dlg.m_Text=ss;
	//dlg.DoModal();
	srand(GetTickCount());
	random_shuffle(List.begin(),List.end());

	int n=atoi(dlg.Texts[0]);
	for(int i=0;i<List.size()&&i<n;i++){
		CStringA Str=List.at(i);
		CStringA Name1=m_FolderPath+"\\"+Str;
		CStringA Name2=m_FolderPath+"\\move\\"+Str;
		if(dlg.Texts[1]=="1"){
			CopyFile(CString(Name1),CString(Name2),1);
		}else
			MoveFile(CString(Name1),CString(Name2));
	}
}


void CWisdomLabelDlg::OnBnClickedIdcreatedate52()
{
	// TODO: 在此添加控件通知处理程序代码
	/*CMTextDlg dlg;
	dlg.Title="自动标注设置";
	dlg.Size=2;
	dlg.Texts[0]="512";
	dlg.Titles[0]="请输长边拉伸尺寸:";
	dlg.Titles[1]="请输入标注的类型,多个用,隔开:";
	if(dlg.DoModal()!=IDOK)
		return;
	CFileDialog mdlg(TRUE,NULL,NULL,NULL,NULL,NULL); 
	mdlg.m_ofn.lpstrTitle="请选择参数文件";
	if(mdlg.DoModal() != IDOK)
		return;
	CStringA pName=mdlg.GetPathName();
	mdlg.m_ofn.lpstrTitle="请选择算子文件";
	if(mdlg.DoModal() != IDOK)
		return;
	CStringA bName=mdlg.GetPathName();*/
	if(!m_IsPCl){
		CMTextDlg dlg;
		dlg.SaveTitle="autolabel";
		dlg.Title="自动标注设置";
		dlg.Size=5;
		dlg.Texts[0]=m_AutoBzSizeType;
		dlg.Texts[1]=m_AutoBzPicSize;
		dlg.Texts[2]=m_AutoBzDxName;
		dlg.Texts[3]=m_AutoBzModelP;
		dlg.Texts[4]=m_AutoBzModelB;
		dlg.Titles[0]="已作废,不用管";//请输拉伸类型(0:等比例,1:自适应)
		dlg.Titles[1]="请输入输入尺寸:";
		dlg.Titles[2]="请输入标注的类型,多个用,隔开:";
		dlg.Titles[3]="请输入pam模型位置:";
		dlg.Titles[4]="请输入bin模型位置:";
		dlg.bLl[3]=1;
		dlg.bLl[4]=1;
		dlg.bSave[2]=1;
		dlg.bSave[3]=1;
		dlg.bSave[4]=1;
		dlg.TextWidth=800;
		if(dlg.DoModal()!=IDOK)
			return;
		m_AutoBzDxName=dlg.Texts[2];
		m_AutoBzPicSize=dlg.Texts[1];
		//m_AutoBzSizeType=dlg.Texts[0];
		m_AutoBzModelP=dlg.Texts[3];
		m_AutoBzModelB=dlg.Texts[4];
	}

	CStringA strPath=GetModuleDir()+"\\ffmpeg\\";
	//.\a 512 .\model\model2 .\model\model3  phone,smoke,face,sideface,openmouth,closeeye
	CStringA cmdLine=strPath+"autobz.exe "+m_PicPath+" " +m_AutoBzSizeType+" " +m_AutoBzPicSize +" "+m_AutoBzModelP+" "+m_AutoBzModelB+" "+m_AutoBzDxName;
	//CStringA cmdLine=strPath+"ffmpeg.exe -i "+dlg.GetPathName()+" -y -f image2 -t 0.001 "+OutPath;
	CTextDlg dlg2;
	dlg2.m_Text=cmdLine;
	dlg2.DoModal();
	STARTUPINFO si={sizeof(si)};
	PROCESS_INFORMATION pif;
		si.dwFlags=STARTF_USESHOWWINDOW;
		si.wShowWindow=TRUE;//TRUE表示显示创建的进程的窗口
		BOOL bRet=::CreateProcess(
						NULL,//TEXT("c://programfiles//internetexplorer//iexplore.exe"),
						CString(cmdLine).GetBuffer(0),//cmdline,//在Unicode版本中此参数不能为常量字符串，因为此参数会被修改
						NULL,
						NULL,
						FALSE,
						CREATE_NEW_CONSOLE,
						NULL,
						CString(strPath),
						&si,
						&pif);
	
	if(bRet)
	{
		WaitForSingleObject( pif.hProcess, INFINITE );
		::CloseHandle(pif.hThread);
		::CloseHandle(pif.hProcess);
	}
	else
	{
		//int error=GetLastError();
		AfxMessageBoxA("自动标注失败");
		return;
	}
	Sleep(100);
	CStringA XmlName=m_FolderPath+"\\"+m_PicTitle+".xml";
	vector<Lk> newLks;
	OpenXml(XmlName,&newLks);
	if(newLks.size()>0){
		m_LkList.insert(m_LkList.end(),newLks.begin(),newLks.end());
		ReDrawItem();
		DeleteFile(CString(XmlName));
		SaveXml(GetXmlFileName(),m_PicName,m_PicPath,m_FolderPath,m_pImg->GetWidth(),m_pImg->GetHeight(),3,&m_LkList);
	}
}


void CWisdomLabelDlg::OnBnClickedIdcreatedate53()
{
	// TODO: 在此添加控件通知处理程序代码
	CMTextDlg dlg;
	dlg.Title="自动标注设置";
	dlg.Size=5;
	dlg.Texts[0]=m_AutoBzSizeType;
	dlg.Texts[1]=m_AutoBzPicSize;
	dlg.Texts[2]=m_AutoBzDxName;
	dlg.Texts[3]=m_AutoBzModelP;
	dlg.Texts[4]=m_AutoBzModelB;
	dlg.Titles[0]="请输拉伸类型(0:等比例,1:自适应)";
	dlg.Titles[1]="请输长边拉伸尺寸:";
	dlg.Titles[2]="请输入标注的类型,多个用,隔开:";
	dlg.Titles[3]="请输入pam模型位置:";
	dlg.Titles[4]="请输入bin模型位置:";
	dlg.bLl[3]=1;
	dlg.bLl[4]=1;
	dlg.TextWidth=800;
	if(dlg.DoModal()!=IDOK)
		return;
	m_AutoBzDxName=dlg.Texts[2];
	m_AutoBzPicSize=dlg.Texts[1];
	m_AutoBzSizeType=dlg.Texts[0];
	m_AutoBzModelP=dlg.Texts[3];
	m_AutoBzModelB=dlg.Texts[4];
	
	
}


void CWisdomLabelDlg::OnBnClickedIdcreatedate54()
{
	// TODO: 在此添加控件通知处理程序代码
	m_pImgListCtrl.SetSelectItem(m_EmptyID);
}


void CWisdomLabelDlg::OnBnClickedIdcreatedate55()
{
	// TODO: 在此添加控件通知处理程序代码
	CFileDialog mdlg(TRUE,NULL,NULL,NULL,NULL,NULL); 
	mdlg.m_ofn.lpstrTitle=CString("请选择参数文件");
	if(mdlg.DoModal() != IDOK)
		return;
	m_AutoBzModelP=mdlg.GetPathName();
	mdlg.m_ofn.lpstrTitle=CString("请选择算子文件");
	if(mdlg.DoModal() != IDOK)
		return;
	m_AutoBzModelB=mdlg.GetPathName();
}


void CWisdomLabelDlg::OnBnClickedButton21()
{
	// TODO: 在此添加控件通知处理程序代码
	int cn=3;
	int w,h;
	unsigned char *pData=GetImgData(m_pImg,cn,w,h);
	for(int c=0;c<cn;c++){
		int cpos=c*w*h;
		for(int j=0;j<h;j++){
			for(int i=0;i<w/2;i++){
				int im=w-1-i;
				int tmp=pData[cpos+j*w+i];
				pData[cpos+j*w+i]=pData[cpos+j*w+im];
				pData[cpos+j*w+im]=tmp;
			}
		}
	}
	m_pImg->Destroy();
	delete m_pImg;
	m_pImg=DataToImg(pData,cn,w,h);
	delete []pData;
	if(m_LkList.size()>0)
	{
		for(vector<Lk >::iterator it=m_LkList.begin();it!=m_LkList.end();it++){
			for(vector<CPoint>::iterator it1=it->Pts.begin();it1!=it->Pts.end();it1++){
				it1->x=w-1-it1->x;
			}
		}
	}
	//m_PicPath=m_PicPath+"mir.jpg";
	saveImg(m_PicPath,m_pImg);
	SaveXml(this->GetXmlFileName(),m_PicName,m_PicPath,m_FolderPath,
			m_pImg->GetWidth(),m_pImg->GetHeight(),m_pImg->GetBPP()/8,&m_LkList);
	//OnBnClickedSaveAnnotation();
	ReDrawItem();
}


void CWisdomLabelDlg::OnBnClickedButton28()
{
	// TODO: 在此添加控件通知处理程序代码
	CImage *pImg=r90(m_pImg);
	if(m_LkList.size()>0)
	{
		for(vector<Lk >::iterator it=m_LkList.begin();it!=m_LkList.end();it++){
			for(vector<CPoint>::iterator it1=it->Pts.begin();it1!=it->Pts.end();it1++){
				//int pos=c*w*h+i*w+(h-1-j);
				int x=it1->x;
				it1->x=it1->y;
				it1->y=m_pImg->GetWidth()-1-x;
				//it1->x=h-1-it1->x;
			}
		}
	}
	m_pImg->Destroy();
	delete m_pImg;
	m_pImg=pImg;
	m_SrcRect=FRect(0,0,m_pImg->GetWidth(),m_pImg->GetHeight());
	
	saveImg(m_PicPath,m_pImg);
	SaveXml(this->GetXmlFileName(),m_PicName,m_PicPath,m_FolderPath,
			m_pImg->GetWidth(),m_pImg->GetHeight(),m_pImg->GetBPP()/8,&m_LkList);
	ReDrawItem();
	
}


void CWisdomLabelDlg::OnBnClickedOpenpic22()
{
	// TODO: 在此添加控件通知处理程序代码
	CMTextDlg Dlg1;
	Dlg1.Size=4;
	Dlg1.Title="请选择筛选项";
	Dlg1.Titles[0]="请选择筛方法:";
	Dlg1.Texts[0]=m_BlSxXBtn.m_Title1;
	Dlg1.Datas[0].AddTail(CString("包含"));
	Dlg1.Datas[0].AddTail(CString("排除"));
	Dlg1.Titles[1]=CString("请选择筛选项:");
	Dlg1.Texts[1]=GetWndTxtA(&m_BlSxXBtn);
	//.GetWindowTextA();
	Dlg1.bLl[0]=0;
	Dlg1.Datas[1].AddTail(CString("全部"));
	Dlg1.Datas[1].AddTail(CString("已标注"));
	Dlg1.Datas[1].AddTail(CString("未标注"));
	Dlg1.Titles[2]=CString("请设置最小数量:");
	Dlg1.Titles[3]=CString("请设置最大数量:");
	Dlg1.Texts[2]=IntToStr(m_MinNum);
	Dlg1.Texts[3]=IntToStr(m_MaxNum);
	Dlg1.TextWidth=800;
	for(map<CStringA,CStringA>::iterator it=m_LabMap1.begin();it!=m_LabMap1.end();it++){
		Dlg1.Datas[1].AddTail(CString(it->first));
	}
	
	if(Dlg1.DoModal()!=IDOK)
		return;
	m_BlSxXBtn.m_Title1=Dlg1.Texts[0];
	//Dlg1.Texts[1]=GetWndTxtA(&m_BlSxXBtn);
	m_BlSxXBtn.SetWindowText(CString(Dlg1.Texts[1]));
	m_MinNum=atoi(Dlg1.Texts[2]);
	m_MaxNum=atoi(Dlg1.Texts[3]);
}


void CWisdomLabelDlg::OnBnClickedOpenpic23()
{
	// TODO: 在此添加控件通知处理程序代码
	CMTextDlg Dlg1;
	Dlg1.Size=1;
	Dlg1.Title="请选择筛选项";
	Dlg1.Titles[0]="请选择筛选项:";
	Dlg1.bLl[0]=0;
	Dlg1.Datas[0].AddTail(_T("1"));
	Dlg1.TextWidth=800;
	for(int i=1;i<101;i++)
		Dlg1.Datas[0].AddTail(IntToStrW(i));
	/*for(map<CStringA,CStringA>::iterator it=m_LabMap1.begin();it!=m_LabMap1.end();it++){
		Dlg1.Datas[0].AddTail(it->first);
	}*/
	
	if(Dlg1.DoModal()!=IDOK)
		return;
	m_KdBtn.SetWindowText(CString(Dlg1.Texts[0]));
}


void CWisdomLabelDlg::OnBnClickedOpenpic24()
{
	// TODO: 在此添加控件通知处理程序代码
	CMTextDlg Dlg1;
	Dlg1.Size=1;
	Dlg1.Title="请设置默认标注";
	Dlg1.Titles[0]="请选择或输入默认标注:";
	Dlg1.bLl[0]=0;
	Dlg1.TextWidth=800;
	//m_MrBzBtn.GetWindowText(CString(Dlg1.Texts[0]));
	//Dlg1.Datas[0].AddTail("全部");
	for(map<CStringA,CStringA>::iterator it=m_LabMap1.begin();it!=m_LabMap1.end();it++){
		Dlg1.Datas[0].AddTail(CString(it->first));
	}
	
	if(Dlg1.DoModal()!=IDOK)
		return;
	m_MrBzBtn.SetWindowText(CString(Dlg1.Texts[0]));
}


void CWisdomLabelDlg::OnBnClickedOpenpic25()
{
	// TODO: 在此添加控件通知处理程序代码
	CMTextDlg Dlg1;
	Dlg1.Size=1;
	Dlg1.Title="请设置指定目录";
	Dlg1.Titles[0]="请选择或输入指定目录:";
	Dlg1.bLl[0]=2;
	Dlg1.TextWidth=800;
	m_TmpDirBtn.GetWindowText(CString(m_TmpDir));
	//Dlg1.Texts[0]=m_TmpDir;
	////Dlg1.Datas[0].AddTail("全部");
	//for(map<CStringA,CStringA>::iterator it=m_LabMap1.begin();it!=m_LabMap1.end();it++){
	//	Dlg1.Datas[0].AddTail(it->first);
	//}
	
	if(Dlg1.DoModal()!=IDOK)
		return;
	m_TmpDir=Dlg1.Texts[0];
	m_TmpDirBtn.SetWindowText(CString(Dlg1.Texts[0]));
	
}


void CWisdomLabelDlg::OnBnClickedOpenpic26()
{
	// TODO: 在此添加控件通知处理程序代码
	CMTextDlg Dlg1;
	Dlg1.Size=1;
	Dlg1.Title="请设置查找的类型";
	Dlg1.Titles[0]="请输入查找的类型:";
	Dlg1.bLl[0]=0;
	Dlg1.Texts[0]=GetWndTxtA(&m_FindNrBtn);
	Dlg1.TextWidth=800;
	//.//GetWindowText(CString());
	//Dlg1.Datas[0].AddTail("全部");
	//for(map<CStringA,CStringA>::iterator it=m_LabMap1.begin();it!=m_LabMap1.end();it++){
	//	Dlg1.Datas[0].AddTail(CString(it->first));
	//}
	
	if(Dlg1.DoModal()!=IDOK)
		return;
	m_FindNrBtn.SetWindowText(CString(Dlg1.Texts[0]));
}


void CWisdomLabelDlg::OnBnClickedIdcreatedate56()
{
	// TODO: 在此添加控件通知处理程序代码
	CKjjDlg Dlg;
	Dlg.m_KjjMap=&m_KjjMap;
	if(Dlg.DoModal()==IDOK){

	}
}


void CWisdomLabelDlg::OnBnClickedIdcreatedate57()
{
	// TODO: 在此添加控件通知处理程序代码
	vector<FInfo> files;
	CFileDialog Dlg(TRUE,FALSE,FALSE,FALSE,FALSE,FALSE);
	if(!Dlg.DoModal()==IDOK)
		return;
	CMTextDlg dlg;

	dlg.Title="删除标签设置";
	dlg.Size=1;
	dlg.Titles[0]="请输入要删除的标签:";
	dlg.DoModal();
	//lg.Texts[0]
	CStringA Qz=dlg.Texts[0];
	int picn=0;//
	//OpenNewPic(Dlg.GetPathName());
	char Buf[64000];
	CFileFind ff;
	//CStringA FirstName;
	int res=ff.FindFile(Dlg.GetFolderPath()+"\\*.*");
	while(res){
		res=ff.FindNextFile();
		if(ff.IsDots() || ff.IsDirectory())
			continue;
		CStringA FilePath=CStringA(ff.GetFilePath());
		
		FInfo fi;
		fi.FileName=CStringA(ff.GetFileName());
		fi.FileTitle=CStringA(ff.GetFileTitle());	
		fi.FileFolder=CStringA(Dlg.GetFolderPath());
		fi.FilePath=CStringA(ff.GetFilePath());
		/*if("浙LD1930"==FileTitle){
			int a=3;
		}*/
		fi.Hz=FilePath.Right(fi.FileName.GetLength()-fi.FileTitle.GetLength()-1);
		
		if(fi.Hz.CompareNoCase("jpg")==0 ||fi.Hz.CompareNoCase("jpeg")==0
			||fi.Hz.CompareNoCase("JPEG")==0)
		{
				files.push_back(fi);
			
			/*	FilePath=Dlg.GetFolderPath()+"\\"+FileTitle+".xml";
				newName.Format("%s\\%s%d.xml",Dlg.GetFolderPath(),Qz,picn++);
				rename(FilePath,newName);*/
		}else{
				//DeleteFile(FilePath);
		}
		
	}
	ff.Close();
	for(int i=0;i<files.size();i++){
		FInfo fi=files.at(i);
		//picn++;
		//CStringA newName;
		//CStringA newName2;
		
		CStringA FilePath1;//=this->GetXmlFileName();
		FilePath1.Format("%s\\%s.xml",fi.FileFolder,fi.FileTitle);
		
		int bOk=0;
		vector<Lk> Lks;
		CFileStatus status;
		if(CFile::GetStatus(CString(FilePath1),status))
		{
			OpenXml(FilePath1,&Lks);
			int bDel=0;
			for(int i=0;i<Lks.size();i++){
				Lk lk=Lks.at(i);
				if(lk.DxType == dlg.Texts[0]){
					Lks.erase(Lks.begin()+i);
					i--;
					bDel=1;
				}
			}
			if(bDel){
				bOk=1;
					if(!DeleteFile(CString(FilePath1))){
						AfxMessageBoxA(FilePath1+"重命名失败,请检查是否被其他程序占用");
						bOk=0;
					}
			}
		}
		if(bOk){
			//if(rename(fi.FilePath,newName)==0){
				
				//CStringA newName1;
				//newName1.Format("%s\\%s%d.xml",Dlg.GetFolderPath(),Qz,picn);
			
			if(Lks.size()>0){
				CImage img;
				img.Load(CString(fi.FilePath));
				SaveXml(FilePath1,fi.FilePath,fi.FilePath,fi.FileFolder,img.GetWidth(),img.GetHeight(),img.GetBPP()/8,&Lks);
				img.Destroy();
			}
				//rename(FilePath1,newName);
			//}else{
				//AfxMessageBoxA(fi.FilePath+"重命名失败,请检查是否被其他程序占用");
			//}
		}
	}
}




void CWisdomLabelDlg::OnBnClickedOpenpic27()
{
	// TODO: 在此添加控件通知处理程序代码
}


void CWisdomLabelDlg::OnBnClickedIdcreatedate58()
{
	// TODO: 在此添加控件通知处理程序代码
	vector<FInfo> files;
	CFileDialog Dlg(TRUE,FALSE,FALSE,FALSE,FALSE,FALSE);
	if(!Dlg.DoModal()==IDOK)
		return;
	CMTextDlg dlg;

	dlg.Title="空标签设置";
	dlg.Size=1;
	dlg.Titles[0]="输入空标签名称:";
	dlg.DoModal();
	//lg.Texts[0]
	CStringA Qz=dlg.Texts[0];
	int picn=0;//
	//OpenNewPic(Dlg.GetPathName());
	char Buf[64000];
	CFileFind ff;
	//CStringA FirstName;
	int res=ff.FindFile(Dlg.GetFolderPath()+"\\*.*");
	while(res){
		res=ff.FindNextFile();
		if(ff.IsDots() || ff.IsDirectory())
			continue;
		CStringA FilePath=CStringA(ff.GetFilePath());
		
		FInfo fi;
		fi.FileName=CStringA(ff.GetFileName());
		fi.FileTitle=CStringA(ff.GetFileTitle());	
		fi.FileFolder=CStringA(Dlg.GetFolderPath());
		fi.FilePath=CStringA(ff.GetFilePath());
		/*if("浙LD1930"==FileTitle){
			int a=3;
		}*/
		fi.Hz=FilePath.Right(fi.FileName.GetLength()-fi.FileTitle.GetLength()-1);
		
		if(fi.Hz.CompareNoCase("jpg")==0 ||fi.Hz.CompareNoCase("jpeg")==0
			||fi.Hz.CompareNoCase("JPEG")==0)
		{
				files.push_back(fi);
			
			/*	FilePath=Dlg.GetFolderPath()+"\\"+FileTitle+".xml";
				newName.Format("%s\\%s%d.xml",Dlg.GetFolderPath(),Qz,picn++);
				rename(FilePath,newName);*/
		}else{
				//DeleteFile(FilePath);
		}
		
	}
	ff.Close();
	for(int i=0;i<files.size();i++){
		FInfo fi=files.at(i);
		//picn++;
		//CStringA newName;
		//CStringA newName2;
		
		CStringA FilePath1;//=this->GetXmlFileName();
		FilePath1.Format("%s\\%s.xml",fi.FileFolder,fi.FileTitle);
		
		int bOk=1;
		vector<Lk> Lks;
		CFileStatus status;
		if(CFile::GetStatus(CString(FilePath1),status))
		{
			OpenXml(FilePath1,&Lks);
		}
		Lk lk;
		lk.Pts.push_back(CPoint(10,10));
		lk.Pts.push_back(CPoint(100.100));
		lk.LkType=TYPERECT;
		lk.DxType=dlg.Texts[0];
		Lks.push_back(lk);
		if(1){
			//if(rename(fi.FilePath,newName)==0){
				
				//CStringA newName1;
				//newName1.Format("%s\\%s%d.xml",Dlg.GetFolderPath(),Qz,picn);
			
			if(Lks.size()>0){
				CImage img;
				img.Load(CString(fi.FilePath));
				SaveXml(FilePath1,fi.FilePath,fi.FilePath,fi.FileFolder,img.GetWidth(),img.GetHeight(),img.GetBPP()/8,&Lks);
				img.Destroy();
			}
				//rename(FilePath1,newName);
			//}else{
				//AfxMessageBoxA(fi.FilePath+"重命名失败,请检查是否被其他程序占用");
			//}
		}
	}
}

float IOU(CRect Rc1,CRect Rc2){
	CRect Rc;
	if(Rc.IntersectRect(Rc1,Rc2)){
		float jmj=(float)(Rc.Width()*Rc.Height());
		float ymj=(float)(Rc1.Width()*Rc1.Height()+Rc2.Width()*Rc2.Height())-jmj;
		return jmj/ymj;
	}
	return 0;
}
void CWisdomLabelDlg::OnHand2auto()
{
	// TODO: 在此添加命令处理程序代码
	CFile f;
	if(!f.Open(CString(m_FolderPath+"\\errlist.txt"),CFile::modeCreate|CFile::modeWrite)){
		return;
	}
	CMTextDlg dlg;

	dlg.Title="自动标注目录设置";
	dlg.Size=2;
	dlg.Titles[0]="请输入自动标注目录:";
	dlg.bLl[0]=2;
	dlg.Titles[1]="请输入判断的标注,多个以逗号隔开:";
	dlg.TextWidth=800;
	if(dlg.DoModal()!=IDOK)
		return;
	
	map<CStringA,int> bzmap=SplitStrToMap(dlg.Texts[1],",");
	vector<FInfo> files;
		char Buf[64000];
	CFileFind ff;
	//CStringA FirstName;
	int res=ff.FindFile(CString(m_FolderPath+"\\*.*"));
	while(res){
		res=ff.FindNextFile();
		if(ff.IsDots() || ff.IsDirectory())
			continue;
		CStringA FilePath=CStringA(ff.GetFilePath());
		
		FInfo fi;
		fi.FileName=CStringA(ff.GetFileName());
		fi.FileTitle=CStringA(ff.GetFileTitle());	
		fi.FileFolder=CStringA(m_FolderPath);
		fi.FilePath=CStringA(ff.GetFilePath());
		/*if("浙LD1930"==FileTitle){
			int a=3;
		}*/
		fi.Hz=FilePath.Right(fi.FileName.GetLength()-fi.FileTitle.GetLength()-1);
		
		if(fi.Hz.CompareNoCase("jpg")==0 ||fi.Hz.CompareNoCase("jpeg")==0
			||fi.Hz.CompareNoCase("JPEG")==0)
		{
				files.push_back(fi);
			
			/*	FilePath=Dlg.GetFolderPath()+"\\"+FileTitle+".xml";
				newName.Format("%s\\%s%d.xml",Dlg.GetFolderPath(),Qz,picn++);
				rename(FilePath,newName);*/
		}else{
				//DeleteFile(FilePath);
		}
		
	}
	ff.Close();
	for(int i=0;i<files.size();i++){
		FInfo fi=files.at(i);
		//picn++;
		//CStringA newName;
		//CStringA newName2;
		
		CStringA FilePath1;//=this->GetXmlFileName();
		CStringA FilePath2;
		FilePath1.Format("%s\\%s.xml",m_LabelPath,fi.FileTitle);
		FilePath2.Format("%s\\%s.xml",dlg.Texts[0],fi.FileTitle);
		int bOk=1;
		vector<Lk> Lks1,Lks2;
		CFileStatus status;
		if(CFile::GetStatus(CString(FilePath1),status))
		{
			OpenXml(FilePath1,&Lks1);
		}else{
			continue;
		}
		if(CFile::GetStatus(CString(FilePath2),status))
		{
			OpenXml(FilePath2,&Lks2);
		}else{
			continue;
		}

		for(int j=0;j<Lks1.size();j++){
			Lk lkj=Lks1.at(j);			
			if(lkj.LkType!=TYPERECT){
				Lks1.erase(Lks1.begin()+j);
				j--;
				continue;
			}
			map<CStringA,int>::iterator it= bzmap.find(lkj.DxType);

			if(it==bzmap.end()){
				Lks1.erase(Lks1.begin()+j);
				j--;
				continue;
			}
			
		}
		for(int k=0;k<Lks2.size();k++){
			Lk lkk=Lks2.at(k);
			if(lkk.LkType!=TYPERECT){
				Lks2.erase(Lks2.begin()+k);
				k--;
				continue;
			}
			map<CStringA,int>::iterator it= bzmap.find(lkk.DxType);
			if(it==bzmap.end()){
				Lks2.erase(Lks2.begin()+k);
				k--;
				continue;
			}
		}

		int bLb=0;
		int bWb=0;
		for(int j=0;j<Lks1.size();j++){
			int bFind=0;
			Lk lkj=Lks1.at(j);
			CRect Rcj(lkj.Pts.at(0),lkj.Pts.at(1));
			for(int k=0;k<Lks2.size();k++){
				Lk lkk=Lks2.at(k);
				CRect Rck(lkk.Pts.at(0),lkk.Pts.at(1));
				if(lkj.DxType ==lkk.DxType){
					float iou=IOU(Rck,Rcj);
					if(iou>0.3){
						bFind=1;
						break;
					}
				}
			}
			if(!bFind){
				bLb=1;
				break;
			}
		}
		for(int k=0;k<Lks2.size();k++){
			int bFind=0;
			Lk lkk=Lks2.at(k);
			CRect Rck(lkk.Pts.at(0),lkk.Pts.at(1));
			for(int j=0;j<Lks1.size();j++){
				Lk lkj=Lks1.at(j);
				CRect Rcj(lkj.Pts.at(0),lkj.Pts.at(1));
				if(lkj.DxType ==lkk.DxType){
					float iou=IOU(Rck,Rcj);
					if(iou>0.2){
						bFind=1;
						break;
					}
				}
			}
			if(!bFind){
				bWb=1;
				break;
			}
		}
		if(bLb||bWb){
			CStringA Str;
			Str.Format("%s %d %d\r\n",fi.FileName,bLb,bWb);
			f.Write(Str,Str.GetLength());
			CStringA FilePath3;//=this->GetXmlFileName();
			FilePath3.Format("%s\\%s",dlg.Texts[0],fi.FileName);
			CopyFile(CString(fi.FilePath),CString(FilePath3),1);
		}
	}
	f.Close();
}


void CWisdomLabelDlg::OnTestiou()
{
	// TODO: 在此添加命令处理程序代码
	{
		CRect Rc1(10,10,100,100);
		CRect Rc2=Rc1;
		float iou=IOU(Rc1,Rc2);
		float a=iou;
	}
	{
		CRect Rc1(10,10,100,100);
		CRect Rc2(80,80,90,90);
		float iou=IOU(Rc1,Rc2);
		float a=iou;
	}
	{
		CRect Rc1(10,10,100,100);
		CRect Rc2(80,80,120,120);
		float iou=IOU(Rc1,Rc2);
		float a=iou;
	}

}


void CWisdomLabelDlg::OnOpenlistbytxt()
{
	// TODO: 在此添加命令处理程序代码
}


void CWisdomLabelDlg::OnYolotoxml()
{
	// TODO: 在此添加命令处理程序代码
	vector<FInfo> files;
	CMTextDlg dlg;
	dlg.Title="yolo转xml设置";
	dlg.Size=3;
	

	dlg.Titles[0]="请输入标注的类型,多个用,隔开:";
	dlg.Titles[1]="请设置yolo标签保存的目录:";
	dlg.Titles[2]="请设置xml标签(新)保存的目录:";
	dlg.bLl[1]=2;
	dlg.bLl[2]=2;
	dlg.TextWidth=800;
	//dlg.Titles[2]="没有以上标签的:";
	if(dlg.DoModal()!=IDOK)
		return;
	//int picn=0;//
	//OpenNewPic(Dlg.GetPathName());
	vector<CStringA>Labels=SplitStr(dlg.Texts[0],",");
	//map<CStringA,int>LabMap;
	//for(int k=0;k<Labels.size();k++){
	//	CStringA ss=Labels.at(k);
	//	map<CStringA,int>::iterator it=LabMap.find(ss);
	//	if(it!=LabMap.end()){
	//		AfxMessageBoxA("有重叠的标签");
	//		//break;
	//	}
	//	LabMap[ss]=k;
	//}
	char Buf[64000];
	CFileFind ff;
	//CStringA FirstName;
	int res=ff.FindFile(CString(m_FolderPath+"\\*.*"));
	while(res){
		res=ff.FindNextFile();
		if(ff.IsDots() || ff.IsDirectory())
			continue;
		CStringA FilePath=CStringA(ff.GetFilePath());
		
		FInfo fi;
		fi.FileName=CStringA(ff.GetFileName());
		fi.FileTitle=CStringA(ff.GetFileTitle());	
		fi.FileFolder=m_FolderPath;
		fi.FilePath=CStringA(ff.GetFilePath());
		/*if("浙LD1930"==FileTitle){
			int a=3;
		}*/
		fi.Hz=FilePath.Right(fi.FileName.GetLength()-fi.FileTitle.GetLength()-1);
		
		if(fi.Hz.CompareNoCase("jpg")==0 ||fi.Hz.CompareNoCase("jpeg")==0
			||fi.Hz.CompareNoCase("JPEG")==0)
		{
				files.push_back(fi);
			
			/*	FilePath=Dlg.GetFolderPath()+"\\"+FileTitle+".xml";
				newName.Format("%s\\%s%d.xml",Dlg.GetFolderPath(),Qz,picn++);
				rename(FilePath,newName);*/
		}else{
				//DeleteFile(FilePath);
		}
		
	}
	ff.Close();
	for(int i=0;i<files.size();i++){
		FInfo fi=files.at(i);
		//picn++;
		//CStringA newName;
		////CStringA newName2;
		//while(1)
		//{
		//	newName.Format("%s\\%s%d.jpg",fi.FileFolder,Qz,picn);
		//	newName2.Format("%s%d.jpg",Qz,picn);
		//	CFileStatus status;
		//	if(CFile::GetStatus(CString(newName),status)){
		//		picn++;
		//	}else{
		//		break;
		//	}
		//}
		CStringA FilePath1,FilePath2;//=this->GetXmlFileName();
		FilePath1.Format("%s\\%s.xml",dlg.Texts[2],fi.FileTitle);
		FilePath2.Format("%s\\%s.txt",dlg.Texts[1],fi.FileTitle);
		
		int bOk=1;
		vector<Lk> Lks;
		CFileStatus status;
		if(CFile::GetStatus(CString(FilePath2),status))
		{
			CImage img;
			img.Load(CString(fi.FilePath));
			char Buf[64000];
			CFile f;
			if(f.Open(CString(FilePath2),CFile::modeRead)){
				int size=f.Read(Buf,64000-1);
				
				if(size>0)
				{
					Buf[size]=0;
					CStringA Str(Buf);
					vector<CStringA> List1=SplitStr(Str,"\n");
					for(int k=0;k<List1.size();k++){
						CStringA Strk=List1.at(k);
						Strk.Replace("  "," ");
						vector<CStringA>List2=SplitStr(Strk," ");
						Lk lk;
						if(List2.size()>4){
							CStringA tmp=List2.at(0);
							int type=atoi(tmp);
							if(type<Labels.size()){
								lk.DxType=Labels.at(atoi(List2.at(0)));
								lk.LkType=TYPERECT;
								float centerx=atof(List2.at(1))*(float)img.GetWidth();
								float centery=atof(List2.at(2))*(float)img.GetHeight();
								float w=atof(List2.at(3))*(float)img.GetWidth();
								float h=atof(List2.at(4))*(float)img.GetHeight();
								int left=centerx-w/2;
								int top=centery-h/2;
								int right=centerx+w/2;
								int bottom=centery+h/2;
								lk.Pts.push_back(CPoint(left,top));
								lk.Pts.push_back(CPoint(right,bottom));
							}
						}
						if(lk.Pts.size()>0)
							Lks.push_back(lk);
						
								
						
					}
				}
			}
			//OpenXml(FilePath1,&Lks);
			/*if(!DeleteFile(CString(FilePath1))){
				AfxMessageBoxA(FilePath1+"重命名失败,请检查是否被其他程序占用");
				bOk=0;
			}*/
			if(Lks.size()>0){
				
				if(!img.IsNull()){
					SaveXml(FilePath1,fi.FileName,fi.FileName,fi.FileFolder,img.GetWidth(),img.GetHeight(),3,&Lks);
				
				
					//SaveXml(newName1,newName2,newName,CStringA(Dlg.GetFolderPath()),img.GetWidth(),img.GetHeight(),img.GetBPP()/8,&Lks);
					
				}
			}
			img.Destroy();
		}
		
	}
}


void CWisdomLabelDlg::OnHeighterr()
{
	// TODO: 在此添加命令处理程序代码
	
	vector<FInfo> files;
	char Buf[64000];
	CFileFind ff;
	//CStringA FirstName;
	int res=ff.FindFile(CString(m_LabelPath)+"\\*.*");
	while(res){
		res=ff.FindNextFile();
		if(ff.IsDots() || ff.IsDirectory())
			continue;
		CStringA FilePath=CStringA(ff.GetFilePath());
		
		FInfo fi;
		fi.FileName=CStringA(ff.GetFileName());
		fi.FileTitle=CStringA(ff.GetFileTitle());	
		//fi.FileFolder=CStringA(Dlg.GetFolderPath());
		fi.FilePath=CStringA(ff.GetFilePath());
		/*if("浙LD1930"==FileTitle){
			int a=3;
		}*/
		fi.Hz=FilePath.Right(fi.FileName.GetLength()-fi.FileTitle.GetLength()-1);
		
		if(fi.Hz.CompareNoCase("xml")==0 )
		{
				files.push_back(fi);
			
			/*	FilePath=Dlg.GetFolderPath()+"\\"+FileTitle+".xml";
				newName.Format("%s\\%s%d.xml",Dlg.GetFolderPath(),Qz,picn++);
				rename(FilePath,newName);*/
		}else{
				//DeleteFile(FilePath);
		}
		
	}
	ff.Close();
	CString Strs;
	for(int i=0;i<files.size();i++){
		FInfo fi=files.at(i);
	
		vector<Lk>Lks;
		CFileStatus status;
		if(CFile::GetStatus(CString(fi.FilePath),status))
		{
			OpenXml(fi.FilePath,&Lks);
			for(int k=0;k<Lks.size();k++){
				Lk lk=Lks.at(k);
				if(lk.DxType=="person"){
					CRect Rc(lk.Pts.at(0),lk.Pts.at(1));
					if(Rc.Width()>Rc.Height()){
						//Strs+=fi.FileTitle+"\r\n";
					}
				}else if(lk.DxType=="vehicle"){
					CRect Rc(lk.Pts.at(0),lk.Pts.at(1));
					if(Rc.Width()*2<Rc.Height()){
						Strs+=fi.FileTitle+"\r\n";
					}
				}
			}
		}
	
	}
	CTextDlg dlg;
	dlg.m_Text=Strs;
	dlg.DoModal();
}


void CWisdomLabelDlg::OnFindsame()
{
	// TODO: 在此添加命令处理程序代码
	//for(int j=0;j<m_pImgListCtrl.GetItemCount();j++)
	{
		int j=GetSelID();
		CStringA StrPathj=m_PicPath;//CStringA(m_pImgListCtrl.GetItemText(j,1));
		CStringA StrTitlej=m_PicTitle;//CStringA(m_pImgListCtrl.GetItemText(j,0));
		CStringA StrFolderj=m_FolderPath;
		CImage imgj;
		imgj.Load(CString(StrPathj));
		int wj,hj,cn;
		cn=1;
		int minCz=0xffffffff;
		int minid=-1;
		unsigned char *Dataj=GetImgData(&imgj,cn,wj,hj);
		for(int i=0;i<m_pImgListCtrl.GetItemCount();i++){
			if(i==j)
				continue;
			CImage imgi;
			CStringA StrPathi=CStringA(m_pImgListCtrl.GetItemText(i,1));
			CStringA StrNamei=CStringA(m_pImgListCtrl.GetItemText(i,0));
			imgi.Load(CString(StrPathi));
			if(imgj.GetWidth()!= imgi.GetWidth() ||imgj.GetHeight()!=imgi.GetHeight()){
				imgi.Destroy();
				continue;
			}
			int wi,hi;
			int cz=0;
			unsigned char *Datai=GetImgData(&imgi,cn,wi,hi);
			for(int y=0;y<hj;y++){
				for(int x=0;x<wj;x++){
					int pos=x+y*wj;
					cz+=abs((int)Datai[pos]-(int)Dataj[pos]);
				}
			}
			if(cz<minCz){
				minCz=cz;
				minid=i;
			}

			imgi.Destroy();
			delete []Datai;
		}
		imgj.Destroy();
		delete []Dataj;
		if(minid!=-1){
			CStringA StrPathi=CStringA(m_pImgListCtrl.GetItemText(minid,1));
			CStringA NewPath=m_FolderPath+"\\nop-"+StrTitlej+".jpg";
			LcName="nop-"+StrTitlej+".jpg";
			//AfxMessageBoxA(LcName);
			CStringA NewXml=m_LabelPath+"\\"+StrTitlej+".xml";
			/*if(rename(StrPathi,NewPath)==0){
				vector<Lk >lks;
				OpenXml(m_LabelPath+"\\"+Srt
			}*/
			CTextDlg dlg;
			dlg.m_Text=NewPath+"\r\n"+StrPathi;
			dlg.DoModal();
			m_pImgListCtrl.SetSelectItem(minid);
			//AfxMessageBoxA(StrPathi);
		}
	}
}


void CWisdomLabelDlg::OnRename()
{
	// TODO: 在此添加命令处理程序代码
		CStringA dName=LcName;
		//AfxMessageBoxA(dName);
	/*if(dName.Right(4)==".txt")
		dName=dName.Left(dName.GetLength()-4)+".jpg";
	if(dName.Right(4)==".jpg")
		dName=dName.Left(dName.GetLength()-4)+ LcHz;*/
	
	CFileDialog Dlg(FALSE,NULL,NULL,OFN_CREATEPROMPT | OFN_PATHMUSTEXIST ,NULL);
	CString defaultFileName = CString(dName);  
	//Dlg.m_ofn.lpstrInitialDir=m_PicPath.GetBuffer(MAX_PATH);
	//m_PicPath.ReleaseBuffer();
	Dlg.m_ofn.lpstrFile =defaultFileName.GetBuffer(MAX_PATH);  
	Dlg.m_ofn.nMaxFile = MAX_PATH;  
	if(Dlg.DoModal()==IDOK){
		CStringA FileName=CStringA(Dlg.GetPathName());
		CFileStatus status;
		if(CFile::GetStatus(CString(FileName),status)){
			AfxMessageBoxA("文件已经存在");
			return;
		}
		if(rename(m_PicPath,FileName)==0){
			//m_pImg->SaveJpeg(CString(FileName),90);
			DeleteFile(CString(GetXmlFileName()));
			CStringA FileName1=m_LabelPath+"\\"+CStringA(Dlg.GetFileTitle());//FileName.Left(FileName.ReverseFind('.'));
			FileName1+=".xml";
			SaveXml(FileName1,CStringA(Dlg.GetFileName()),CStringA(FileName),CStringA(Dlg.GetFolderPath()),m_pImg->GetWidth(),m_pImg->GetHeight(),m_pImg->GetBPP()/8,&m_LkList);
			//CStringA j1Str=m_pImgListCtrl.GetItemText(GetSelID()+1,
			//CFileStatus status;
			//if(!CFile::GetStatus(FileName,status)){
			//int j=m_pImgListCtrl.InsertItem(GetSelID()+1,Dlg.GetFileName());
			int j=GetSelID();
			m_pImgListCtrl.SetItemText(j,1,CString(FileName));
			m_pImgListCtrl.SetItemText(j,0,Dlg.GetFileName());
			m_PicTitle=CStringA(Dlg.GetFileTitle());
			m_PicName=CStringA(Dlg.GetFileName());
			m_PicPath=CStringA(Dlg.GetPathName());
			//m_pImgListCtrl.SetSelectItem(j);
			//AfxMessageBoxA("已存在");
			//return;
		//}

		//OpenNewPic(Dlg.GetPathName());
		//AfxMessageBoxA("另存成功");
		//CopyFile(m_PicPath,Dlg.GetPathName(),0);
		}
		//m_pImg->SaveJpeg(CString(FileName),90);
		
	}
	defaultFileName.ReleaseBuffer();
}

struct xy{
	int w;
	int h;
	unsigned char data[64][64];
};
vector <xy> gxys;
void CWisdomLabelDlg::OnFindsame2()
{
	// TODO: 在此添加命令处理程序代码
	if(gxys.size()==0){
		for(int i=0;i<m_pImgListCtrl.GetItemCount();i++){
			
			CImage imgi;
			CStringA StrPathi=CStringA(m_pImgListCtrl.GetItemText(i,1));
			CStringA StrNamei=CStringA(m_pImgListCtrl.GetItemText(i,0));
			imgi.Load(CString(StrPathi));
			
			int wi,hi;
			int cn=1;
			int cz=0;
			unsigned char *Datai=GetImgData(&imgi,cn,wi,hi);
			int xg=wi/64;
			int yg=hi/64;
			xy xyi;
			for(int y=0;y<64;y++){
				for(int x=0;x<64;x++){
					int xx=x*xg;
					int yy=y*yg;
					int pos=xx+yy*wi;
					xyi.data[y][x]=Datai[pos];
					
				}
			}
			xyi.w=wi;
			xyi.h=hi;
			gxys.push_back(xyi);
			imgi.Destroy();
			delete []Datai;
		}
	}
	{
		int j=GetSelID();
		CStringA StrPathj=m_PicPath;//CStringA(m_pImgListCtrl.GetItemText(j,1));
		CStringA StrTitlej=m_PicTitle;//CStringA(m_pImgListCtrl.GetItemText(j,0));
		CStringA StrFolderj=m_FolderPath;

		int minCz=10000000;
		int minid=-1;
		xy xyj=gxys.at(j);
		for(int i=0;i<m_pImgListCtrl.GetItemCount();i++){
			if(i==j)
				continue;
			
			CStringA StrPathi=CStringA(m_pImgListCtrl.GetItemText(i,1));
			CStringA StrNamei=CStringA(m_pImgListCtrl.GetItemText(i,0));
			int cz=0;
			xy xyi=gxys.at(i);
			if(xyi.w!=xyj.w||xyi.h!=xyj.h)
				continue;
			for(int y=0;y<64;y++){
				for(int x=0;x<64;x++){
					cz+=abs((int)xyj.data[y][x]-(int)xyi.data[y][x]);
				}
			}
			if(cz<minCz){
				minCz=cz;
				minid=i;
			}
			
		}
		
		if(minid!=-1){
			CStringA StrPathi=CStringA(m_pImgListCtrl.GetItemText(minid,1));
			CStringA NewPath=m_FolderPath+"\\nop-"+StrTitlej+".jpg";
			CStringA NewXml=m_LabelPath+"\\"+StrTitlej+".xml";
			LcName="nop-"+StrTitlej+".jpg";
			/*if(rename(StrPathi,NewPath)==0){
				vector<Lk >lks;
				OpenXml(m_LabelPath+"\\"+Srt
			}*/
			//CTextDlg dlg;
			//dlg.m_Text=NewPath+"\r\n"+StrPathi;
			//dlg.DoModal();
			m_pImgListCtrl.SetSelectItem(minid);
			//AfxMessageBoxA(StrPathi);
		}
	}

}
vector<CStringA >NoPds;
vector<CStringA>PddxNames;
int sid=0;
void CWisdomLabelDlg::OnSearchpd()
{
	// TODO: 在此添加命令处理程序代码
	int AllOk=1;
	for(int i=sid;i<m_pImgListCtrl.GetItemCount();i++){
		//CImage imgi;
		CStringA StrPathi=CStringA(m_pImgListCtrl.GetItemText(i,1));
		CStringA StrNamei=CStringA(m_pImgListCtrl.GetItemText(i,0));
		//if(StrNamei.Left(4)=="nop-")
		//	continue;
		CStringA StrTitlei=StrNamei.Left(StrNamei.GetLength()-4);
		CStringA xmlpath=m_LabelPath+"\\"+StrTitlei+".xml";
		vector<Lk >lks;
		OpenXml(xmlpath,&lks);
		int bGz=0;
		for(int l=0;l<lks.size();l++){
			Lk lk=lks.at(l);
			for(int n=0;n<PddxNames.size();n++){
				if(lk.DxType==PddxNames.at(n)){
					bGz=1;
					break;
				}
			}
			if(bGz==1)
				break;
		}
		if(!bGz)
			continue;
		int bFind=0;
		CStringA wantj="nop-"+StrNamei;
		for(int j=0;j<m_pImgListCtrl.GetItemCount();j++){
			if(j==i)
				continue;
			CStringA StrNamej=CStringA(m_pImgListCtrl.GetItemText(j,0));
			if(StrNamej==wantj){
				bFind=1;
				break;
			}
		}
		if(bFind)
			continue;
		for(int p=0;p<NoPds.size();p++){
			if(NoPds.at(p) ==StrNamei){
				bFind=1;
				break;
			}
		}
		if(!bFind){
			m_pImgListCtrl.SetSelectItem(i);
			sid=i+1;
			AllOk=0;
			break;
		}
		//for(int j=0;j<
	}
	if(AllOk){
		sid=0;
		AfxMessageBoxA("re start");
	}
}


void CWisdomLabelDlg::OnNopd()
{
	// TODO: 在此添加命令处理程序代码
	NoPds.push_back(m_PicName);
}


void CWisdomLabelDlg::OnSizeto()
{
	// TODO: 在此添加命令处理程序代码
	CMTextDlg dlg;
	dlg.Title="图片扩充到指定尺寸";
	dlg.Size=2;
	dlg.Titles[0]="请输入新的宽度:";
	dlg.Titles[1]="请输入新的高度:";
	dlg.Texts[0]="1920";
	dlg.Texts[1]="1080";
	if(dlg.DoModal()!=IDOK)
		return;
	for(int a=0;a<m_pImgListCtrl.GetItemCount();a++){
		CString ImgPath=m_pImgListCtrl.GetItemText(a,1);
		CImage img;
		img.Load(ImgPath);
		int w=0;
		int h=0;
		unsigned char *pOldData=GetImgData(&img,3,w,h);
		int nw=atoi(dlg.Texts[0]);
		int nh=atoi(dlg.Texts[1]);
		int left=(nw-w)/2;
		int top=(nh-h)/2;
		unsigned char *pNewData=new unsigned char[nw*nh*3];
		memset(pNewData,128,nw*nh*3);

		for(int k=0;k<3;k++){
			for(int j=0;j<h;j++){
				for(int i=0;i<w;i++){
					int pos=w*h*k+j*w+i;
					int jj=j+top;
					int ii=i+left;
					if(jj<0||jj>=nh)
						continue;
					if(ii<0||ii>=nw)
						continue;
					int posnew=nw*nh*k+jj*nw+ii;
					pNewData[posnew]=pOldData[pos];
				}
			}
		}

		CImage *nImg=DataToImg(pNewData,3,nw,nh);
		saveImg(m_PicPath,nImg);
		//nImg->SaveJpeg(ImgPath,95);
		nImg->Destroy();
		delete nImg;
		delete []pNewData;
		delete []pOldData;
		img.Destroy();
	}
	
	
}


void CWisdomLabelDlg::OnPicstovideo()
{
	// TODO: 在此添加命令处理程序代码
	CMTextDlg pDlg;
	pDlg.Size=4;
	pDlg.Title="多个图片转视频:";
	pDlg.Titles[0]="请输入视频文件地址:";
	pDlg.Titles[1]="请输入图片目录:";
	pDlg.bLl[0]=1;
	pDlg.bLl[1]=2;
	pDlg.Titles[2]="请输入帧率(1秒转几帧):";
	pDlg.Texts[2]="15";
	pDlg.Titles[3]="请输入当前图片前缀:";
	pDlg.TextWidth=800;
	if(pDlg.DoModal()!=IDOK)
		return;
	/*CMTextDlg pDlg;
	pDlg.Size=2;
	pDlg.Title="请选择时间";
	pDlg.Titles[0]="请选择时间(秒):";
	pDlg.Texts[0]="1";
	pDlg.Titles[1]="请输入摄像头ID:";
	pDlg.Texts[1]="0";
	if(pDlg.DoModal()!=IDOK)
		return;*/
	//ffmpeg -f image2 -i /home/test/images/image%d.jpg  -vcodec libx264  -r 20 test.h264
	CStringA strPath=GetModuleDir()+"\\ffmpeg\\";
	//ffmpeg -i e:\wb\2.mp4 -r 1 -f image2 e:\wb\2-%05d.jpg
	//CStringA OutPath=GetModuleDir()+"\\"+CStringA(pDlg.Texts[1])+".jpg";//tmp.jpg";
		//ffmpeg -i test.asf -y -f image2 -t 0.001 -s 352x240 a.jpg
	//ffmpeg -i \"%s\" -r 15 -f image2 %d-%s.jpg
	CStringA cmdLine=strPath+"ffmpeg.exe"+" -r "+pDlg.Texts[2]+" -i  \""+pDlg.Texts[1]+"\\"+pDlg.Texts[3]+"%05d.jpg\""+" -vcodec libx264 "+pDlg.Texts[0];
	//CStringA cmdLine=strPath+"ffmpeg.exe -i "+dlg.GetPathName()+" -y -f image2 -t 0.001 "+OutPath;
	CTextDlg dlg2;
	dlg2.m_Text=cmdLine;
	dlg2.DoModal();
	STARTUPINFO si={sizeof(si)};
	PROCESS_INFORMATION pif;
		si.dwFlags=STARTF_USESHOWWINDOW;
		si.wShowWindow=TRUE;//TRUE表示显示创建的进程的窗口
		BOOL bRet=::CreateProcess(
						NULL,//TEXT("c://programfiles//internetexplorer//iexplore.exe"),
						CString(cmdLine).GetBuffer(0),//cmdline,//在Unicode版本中此参数不能为常量字符串，因为此参数会被修改
						NULL,
						NULL,
						FALSE,
						CREATE_NEW_CONSOLE,
						NULL,
						CString(strPath),
						&si,
						&pif);
	
	if(bRet)
	{
		WaitForSingleObject( pif.hProcess, INFINITE );
		::CloseHandle(pif.hThread);
		::CloseHandle(pif.hProcess);
	}
	else
	{
		//int error=GetLastError();
		AfxMessageBoxA("获取视频的图片失败");
		return;
	}
}


void CWisdomLabelDlg::OnPddxname()
{
	// TODO: 在此添加命令处理程序代码
	CTextDlg dlg;
	if(dlg.DoModal()!=IDOK){
		return;
	}
	PddxNames=SplitStr(CStringA(dlg.m_Text),",");

}


void CWisdomLabelDlg::OnDeletelabpixel()
{
	// TODO: 在此添加命令处理程序代码


	int bModify=0;
	for(int i=0;i<m_LkList.size();i++){
		Lk lk=m_LkList.at(i);
		if(lk.DxType=="delete")
			continue;
		CRect Rc(lk.Pts.at(0),lk.Pts.at(1));
		DeletePixel(m_pImg,Rc);
		//lk.DxType="delete";
		vector<Lk >::iterator it=m_LkList.begin()+i;
		it->DxType="delete";
		bModify=1;
		
	}
	if(bModify){
		saveImg(m_PicPath,m_pImg);
		SaveXml(GetXmlFileName(),m_PicName,m_PicPath,m_FolderPath,m_pImg->GetWidth(),m_pImg->GetHeight(),3,&m_LkList);
		ReDrawItem();
	}
}


void CWisdomLabelDlg::OnPideletepixel()
{
	// TODO: 在此添加命令处理程序代码
	PCmdList.clear();
	PCmdList.push_back(ID_DELETELABPIXEL);
	PCmdList.push_back(IDNEXTPIC);
	SetTimer(3,10,NULL);
}


void CWisdomLabelDlg::OnMzcopyto()
{
	// TODO: 在此添加命令处理程序代码
	CMTextDlg pDlg;
	static CStringA newBzPath="";
	static CStringA newJpgPath="";
	pDlg.Size=5;
	pDlg.Title="复制参数设置:";
	pDlg.Titles[0]="请输入图片目录:";
	pDlg.Titles[1]="请输入标注目录:";
	pDlg.Titles[2]="请输入新的图片目录:";
	pDlg.Titles[3]="请输入新的标注目录:";
	pDlg.Titles[4]="复制填0，移动填1:";
	pDlg.bLl[0]=2;
	pDlg.bLl[1]=2;
	pDlg.bLl[2]=2;
	pDlg.bLl[3]=2;
	pDlg.Texts[0]=m_FolderPath;
	pDlg.Texts[1]=m_LabelPath;
	pDlg.Texts[2]=newBzPath;
	pDlg.Texts[3]=newJpgPath;

	
	pDlg.TextWidth=800;
	if(pDlg.DoModal()!=IDOK)
		return;
	newBzPath=pDlg.Texts[2];
	newJpgPath=pDlg.Texts[3];
	vector<CString> bzFileList;
	vector<CString> jpgFileList;
	{
		CFileFind ff;
		int res=ff.FindFile(CString(pDlg.Texts[0]+"\\*.*"));
		while(res){
			res=ff.FindNextFile();
			if(ff.IsDots() || ff.IsDirectory())
				continue;
			CStringA fileName=CStringA(ff.GetFilePath());
			if(fileName.Right(4)==".jpg"){
				jpgFileList.push_back(ff.GetFilePath());
			}
		}
		
	}
	{
		CFileFind ff;
		int res=ff.FindFile(CString(pDlg.Texts[1]+"\\*.*"));
		while(res){
			res=ff.FindNextFile();
			if(ff.IsDots() || ff.IsDirectory())
				continue;
			CStringA fileName=CStringA(ff.GetFilePath());
			if(fileName.Right(4)==".xml"){
				bzFileList.push_back(ff.GetFilePath());
			}
		}
		
	}
	for(int i=0;i<jpgFileList.size();i++){
		CString fileName=jpgFileList.at(i);
		CString newFileName=fileName;
		newFileName.Replace(CString(pDlg.Texts[0]),CString(pDlg.Texts[2]));
		if(pDlg.Texts[4]="0"){
			CopyFile(fileName,newFileName,1);
		}else{
			MoveFile(fileName,newFileName);
		}
	}
	for(int i=0;i<bzFileList.size();i++){
		CString fileName=bzFileList.at(i);
		CString newFileName=fileName;
		newFileName.Replace(CString(pDlg.Texts[1]),CString(pDlg.Texts[3]));
		if(pDlg.Texts[4]="0"){
			CopyFile(fileName,newFileName,1);
		}else{
			MoveFile(fileName,newFileName);
		}
	}
}


void CWisdomLabelDlg::Onwidefacetovoc()
{
	// TODO: 在此添加命令处理程序代码
		CMTextDlg pDlg;
	static CStringA newBzPath="";
	static CStringA newJpgPath="";
	pDlg.Size=4;
	pDlg.Title="复制参数设置:";
	pDlg.Titles[2]="请输入voc图片目录:";
	pDlg.Titles[3]="请输入voc标注目录:";
	pDlg.Titles[0]="请输入Wideface标注文件:";
	pDlg.Titles[1]="请输入Wideface图片根目录:";
	pDlg.bLl[0]=1;
	pDlg.bLl[1]=2;
	pDlg.bLl[2]=2;
	pDlg.bLl[3]=2;
	pDlg.TextWidth=800;
	if(pDlg.DoModal()!=IDOK)
		return;
	CStdioFile fi;
	if(!fi.Open(CString(pDlg.Texts[0]),CFile::modeRead))
		return;
	CString ss;
	while(fi.ReadString(ss)){
		vector<CStringA> fns=SplitStr(CStringA(ss),"/");
		if(fns.size()<2){
			int a=3;
		}
		fi.ReadString(ss);
		int n=atoi(CStringA(ss));
		if(n==0){
			fi.ReadString(ss);//为0时也有一行0 0 0 0 0 0 0 0 0 0
		}
		vector<Lk>lks;
		for(int i=0;i<n;i++){
			fi.ReadString(ss);
			vector<CStringA> bzinfo=SplitStr(CStringA(ss)," ");
			int left=atoi(bzinfo.at(0));
			int top=atoi(bzinfo.at(1));
			int right=left+atoi(bzinfo.at(2));
			int bottom=top+atoi(bzinfo.at(3));
			Lk lk;
			lk.Pts.push_back(CPoint(left,top));
			lk.Pts.push_back(CPoint(right,bottom));
			lk.DxType="face";
			lk.LkType=TYPERECT;
			lks.push_back(lk);
		}
		if(n>6)
			continue;
		CStringA Folder=pDlg.Texts[1]+"\\"+fns.at(0);
		CStringA FilePath=Folder+"\\"+fns.at(1);
		CImage img;
		img.Load(CString(FilePath));
		CStringA XmlPath=pDlg.Texts[3]+"\\"+fns.at(1);
		XmlPath=XmlPath.Left(XmlPath.GetLength()-3)+"xml";
		SaveXml(XmlPath,fns.at(1),FilePath,Folder,img.GetWidth(),img.GetHeight(),img.GetBPP()/8,&lks);
		img.Destroy();
		CStringA newJpgPath=pDlg.Texts[2]+"\\"+fns.at(1);
		CopyFile(CString(FilePath),CString(newJpgPath),1);
		int a=3;
	}
	fi.Close();
}


void CWisdomLabelDlg::OnLabelscale()
{
	// TODO: 在此添加命令处理程序代码
	//static int times=0;
	//static int needtimes=0;
	static int ThisCmd=ID_LABELSCALE;
	static float scale=1.0;
	static int save=1;


	if(m_PiTimes==0){
		int piID=2;
		CMTextDlg dlg;
		dlg.Title="标签缩放";
		dlg.Size=piID+2;

		dlg.Titles[0]="请输入缩放比例";
		dlg.Titles[1]="是否保存结果,0为不保存,1为保存:";
		dlg.Titles[piID]="是否批批缩放,0为只缩放当前,1为批缩放:";
		dlg.Titles[piID+1]="批缩放数量:";
		//dlg.bLl[1]=2;
		dlg.Texts[0]=FloatToStr(scale);
		dlg.Texts[1]=IntToStr(save);
		dlg.Texts[piID]="0";
		dlg.Texts[piID+1]=IntToStr(m_pImgListCtrl.GetItemCount());
		if(dlg.DoModal()){
			scale=atof(dlg.Texts[0]);
			save=atoi(dlg.Texts[1]);
			m_PiNeedTimes=1;
			if(dlg.Texts[piID]=="1"){
				m_PiNeedTimes=atoi(dlg.Texts[piID+1]);
				m_PiCmd=ThisCmd;
				//SetTimer(10,10,NULL);
			}
		}
	}
	if(m_PiTimes<m_PiNeedTimes){
		vector<Lk>::iterator it=m_LkList.begin();
		for(;it!=m_LkList.end();it++){
			if(it->LkType!=TYPERECT){
				continue;
			}
			vector<CPoint>::iterator itp1=it->Pts.begin();
			vector<CPoint>::iterator itp2=itp1+1;
			int w=itp2->x-itp1->x;
			int h=itp2->y-itp1->y;
			float ww=w*scale-w;
			ww/=2;
			float hh=h*scale-h;
			hh/=2;
			itp1->x=Limit(itp1->x-ww,0,m_pImg->GetWidth()-1);
			itp1->y=Limit(itp1->y-hh,0,m_pImg->GetHeight()-1);
			itp2->x=Limit(itp2->x+ww,0,m_pImg->GetWidth()-1);
			itp2->y=Limit(itp2->y+hh,0,m_pImg->GetHeight()-1);
		}
		ReDrawItem();
		if(save)
			SaveXml(GetXmlFileName(),m_PicName,m_PicPath,m_FolderPath,m_pImg->GetWidth(),m_pImg->GetHeight(),3,&m_LkList);
		//times++;
	}
	PiChuli();
	
	//dlg.TextWidth=800;
}


void CWisdomLabelDlg::PiChuli(void)
{
	m_PiTimes++;
	if(m_PiTimes<m_PiNeedTimes){
		int id=GetSelID();
		id++;
		if(id>=m_pImgListCtrl.GetItemCount()-1)
			id=0;
		m_pImgListCtrl.SetSelectItem(id);
	}else{
		m_PiTimes=0;
		m_PiNeedTimes=0;
		m_PiCmd=-1;
	}
	if(m_PiTimes==1 && m_PiNeedTimes>1){
		SetTimer(10,10,NULL);
	}
}


void CWisdomLabelDlg::OnScaleto()
{
	// TODO: 在此添加命令处理程序代码
	static int ThisCmd=ID_SCALETO;
	static int nw=0;
	static int nh=0;
	static int save=1;


	if(m_PiTimes==0){
		int piID=3;
		CMTextDlg dlg;
		dlg.Title="标签缩放";
		dlg.Size=piID+2;

		dlg.Titles[0]="请输入新的宽度";
		dlg.Titles[1]="请输入新的高度";
		dlg.Titles[2]="是否保存图像,0为不保存,1为保存:";
		dlg.Titles[piID]="是否批执行,0为只执行当前,1为批执行:";
		dlg.Titles[piID+1]="批缩放数量:";
		
		//dlg.bLl[1]=2;
		dlg.Texts[0]=IntToStr(nw);
		dlg.Texts[1]=IntToStr(nh);
		dlg.Texts[2]="1";
		dlg.Texts[piID]="0";
		dlg.Texts[piID+1]=IntToStr(m_pImgListCtrl.GetItemCount());
		if(dlg.DoModal()){
			nw=atoi(dlg.Texts[0]);
			nh=atoi(dlg.Texts[1]);
			save=atoi(dlg.Texts[2]);
			m_PiNeedTimes=1;
			if(dlg.Texts[piID]=="1"){
				m_PiNeedTimes=atoi(dlg.Texts[piID+1]);
				m_PiCmd=ThisCmd;
				//SetTimer(10,10,NULL);
			}
		}
	}
	if(m_PiTimes<m_PiNeedTimes){
		int ow,oh;
		int cn=3;
		unsigned char *pOrData=GetImgData(m_pImg,cn,ow,oh);
		unsigned char *pNewData=new unsigned char[nw*nh*cn];
		ScaleData(pOrData,ow,oh,cn,pNewData,nw,nh);
		m_pImg->Destroy();
		delete m_pImg;
		m_pImg=DataToImg(pNewData,cn,nw,nh);
		delete []pOrData;
		delete []pNewData;
		ReDrawItem();
		if(save)
			saveImg(m_PicPath,m_pImg);
			
		//times++;
	}
	PiChuli();
}


void CWisdomLabelDlg::OnBnClickedIdcreatedate59()
{
	// TODO: 在此添加控件通知处理程序代码
	CMTextDlg pDlg;
	pDlg.Size=4;
	pDlg.Title="多视频转多个图片:";
	pDlg.Titles[0]="请输入视频文件目录:";
	pDlg.Titles[1]="请输入图片目录:";
	pDlg.bLl[0]=2;
	pDlg.bLl[1]=2;
	pDlg.Titles[2]="请输入帧率(1秒转几帧):";
	pDlg.Texts[2]="15";
	pDlg.Titles[3]="请输入前缀:";
	pDlg.TextWidth=800;
	if(pDlg.DoModal()!=IDOK)
		return;
	int k=0;
	CFileFind ff;
	int res=ff.FindFile(CString(pDlg.Texts[0]+"\\*.*"));
	while(res){
		res=ff.FindNextFile();
		if(ff.IsDots() || ff.IsDirectory())
			continue;
		CStringA fileName=CStringA(ff.GetFilePath());
		if(fileName.Right(4)==".mp4"||fileName.Right(4)==".abi"){
			k++;
			CStringA strPath=GetModuleDir()+"\\ffmpeg\\";
			//ffmpeg -i e:\wb\2.mp4 -r 1 -f image2 e:\wb\2-%05d.jpg
			//CStringA OutPath=GetModuleDir()+"\\"+CStringA(pDlg.Texts[1])+".jpg";//tmp.jpg";
				//ffmpeg -i test.asf -y -f image2 -t 0.001 -s 352x240 a.jpg
			//ffmpeg -i \"%s\" -r 15 -f image2 %d-%s.jpg
			CStringA Qz;
			Qz.Format("%s%03d",pDlg.Texts[3],k);
			CStringA cmdLine=strPath+"ffmpeg.exe -i \""+fileName+"\" -f image2 -r "+pDlg.Texts[2]+"  \""+pDlg.Texts[1]+"\\"+Qz+"%05d.jpg\"";
			//CStringA cmdLine=strPath+"ffmpeg.exe -i "+dlg.GetPathName()+" -y -f image2 -t 0.001 "+OutPath;
			/*CTextDlg dlg2;
			dlg2.m_Text=cmdLine;
			dlg2.DoModal();*/
			STARTUPINFO si={sizeof(si)};
			PROCESS_INFORMATION pif;
				si.dwFlags=STARTF_USESHOWWINDOW;
				si.wShowWindow=TRUE;//TRUE表示显示创建的进程的窗口
				BOOL bRet=::CreateProcess(
								NULL,//TEXT("c://programfiles//internetexplorer//iexplore.exe"),
								CString(cmdLine).GetBuffer(0),//cmdline,//在Unicode版本中此参数不能为常量字符串，因为此参数会被修改
								NULL,
								NULL,
								FALSE,
								CREATE_NEW_CONSOLE,
								NULL,
								CString(strPath),
								&si,
								&pif);
	
			if(bRet)
			{
				WaitForSingleObject( pif.hProcess, INFINITE );
				::CloseHandle(pif.hThread);
				::CloseHandle(pif.hProcess);
			}
			else
			{
				//int error=GetLastError();
				AfxMessageBoxA("获取视频的图片失败");
				return;
			}
		}
	}
	/*CMTextDlg pDlg;
	pDlg.Size=2;
	pDlg.Title="请选择时间";
	pDlg.Titles[0]="请选择时间(秒):";
	pDlg.Texts[0]="1";
	pDlg.Titles[1]="请输入摄像头ID:";
	pDlg.Texts[1]="0";
	if(pDlg.DoModal()!=IDOK)
		return;*/
	
	OpenDir(pDlg.Texts[1]);
}


void CWisdomLabelDlg::OnOpenrgb()
{
	// TODO: 在此添加命令处理程序代码
	CMTextDlg pDlg;
	pDlg.Size=4;
	pDlg.Title="打开RGB裸数据:";
	pDlg.Titles[0]="请输入数据文件地址:";
	pDlg.Titles[1]="请输入宽度：";
	pDlg.bLl[0]=1;
	pDlg.Titles[2]="请输入高度:";
	pDlg.Titles[3]="请输入通道数:";
	pDlg.Texts[1]="1920";
	pDlg.Texts[2]="1088";
	pDlg.Texts[3]="4";
	if(pDlg.DoModal()!=IDOK){
		return;
	}
	int w=atoi(pDlg.Texts[1]);
	int h=atoi(pDlg.Texts[2]);
	int cn=atoi(pDlg.Texts[3]);
	unsigned char *pData=new unsigned char[w*h*cn];
	CFile f;
	if(!f.Open(CString(pDlg.Texts[0]),CFile::modeRead)){
		return;
	}
	f.Read(pData,w*h*cn);
	f.Close();
	if(m_pImg &&!m_pImg->IsNull()){
		m_pImg->Destroy();
	}
	if(m_pImg)
		delete m_pImg;
	m_pImg=DatajxToImg(pData,4,1920,1088);
	m_SrcRect=FRect(0,0,m_pImg->GetWidth(),m_pImg->GetHeight());
	ReDrawItem();
}


void CWisdomLabelDlg::OnPideletepixeldbc()
{
	// TODO: 在此添加命令处理程序代码
	PCmdList.clear();
	PCmdList.push_back(IDIDCREATEDATE5);
	PCmdList.push_back(IDNEXTPIC);
	SetTimer(3,10,NULL);
}


void CWisdomLabelDlg::OnBnClickedDeletelabel()
{
	// TODO: 在此添加控件通知处理程序代码
	CStringA Str=GetXmlFileName();
	DeleteFile(CString(Str));
	m_LkList.clear();
	ReDrawItem();
}


void CWisdomLabelDlg::OnCleanlabel()
{
	// TODO: 在此添加命令处理程序代码
	PCmdList.clear();
	PCmdList.push_back(IDC_DELETELABEL);
	PCmdList.push_back(IDNEXTPIC);
	SetTimer(3,10,NULL);
}


void CWisdomLabelDlg::OnCaldbcrect()
{
	// TODO: 在此添加命令处理程序代码
	int dbcn=0;;
	int rcn=0;
	for(int i=0;i<m_LkList.size();i++){
		Lk lk=m_LkList.at(i);
		if(lk.LkType==TYPERECT)
			rcn++;
		else if(lk.LkType==TYPEDBC)
			dbcn++;
	}
	if(rcn!=dbcn){
		//KillTimer(3);
		PCmdList.clear();
		CStringA Str;
		Str.Format("%d,%d",rcn,dbcn);
		AfxMessageBoxA(Str);
	}
}


void CWisdomLabelDlg::OnPidbcnorect()
{
	// TODO: 在此添加命令处理程序代码
	PCmdList.clear();
	PCmdList.push_back(ID_CALDBCRECT);
	PCmdList.push_back(IDNEXTPIC);
	SetTimer(3,10,NULL);
}


void CWisdomLabelDlg::OnStopp()
{
	// TODO: 在此添加命令处理程序代码
	PCmdList.clear();
}


void CWisdomLabelDlg::OnBnClickedTogray()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_pImg){
		static BOOL bSave=0;
		if(!m_IsPCl){
			CMTextDlg Dlg;
			Dlg.Title="是否保存结果";
			Dlg.Size=1;
			Dlg.Titles[0]="是否保存结果(0:不保存,1:保存):";
			Dlg.Texts[0]=IntToStr(bSave);
			if(Dlg.DoModal()!=IDOK){
				return;
			}
			bSave=atoi(Dlg.Texts[0]);
		}
		ToGray(m_pImg);
		if(bSave){
			int cn=3;
			int w,h;
			unsigned char *Data=GetImgDatajx(m_pImg,3,w,h);
			saveImg(m_PicPath.GetBuffer(),Data,w,h,cn);
			//m_pImg->SaveJpeg(CString(m_PicPath),0.95);
		}
		ReDrawItem();
	}
}


void CWisdomLabelDlg::OnBnClickedTogray2()
{
	// TODO: 在此添加控件通知处理程序代码
	//static int BtnState=0;
	 StartPi(IDTOGRAYPI,IDTOGRAY);
	
}


UINT CWisdomLabelDlg::PiThread(void* p)
{
	CWisdomLabelDlg *pDlg=(CWisdomLabelDlg*)p;
	while(pDlg->m_PiThreadRun){
		pDlg->SendMessage(PIMESSAGE,0,0);
		Sleep(1);
	}

	return 0;
}
LRESULT CWisdomLabelDlg::OnPiMessage(WPARAM wp,LPARAM lp){
	if(m_pImgListCtrl.GetItemCount()>GetSelID()&&PCmdList.size()>0){
		if(m_PTimes++>0)
			m_IsPCl=1;
		for(int i=0;i<PCmdList.size();i++){
			SendMessage(WM_COMMAND,MAKEWPARAM(PCmdList.at(i),BN_CLICKED),0);
		}
		
		//OnBnClickedOpenpic2();
		m_IsPCl=0;
		if(m_pImgListCtrl.GetItemCount()-1>GetSelID())
			m_pImgListCtrl.SetSelectItem(GetSelID()+1);
		else{
			GetDlgItem(m_PiBtnId)->SetWindowText(CString("批"));
			m_PiThreadRun=0;
		}
	}else{
		m_PiThreadRun=0;
	}
	return 1;
}

void CWisdomLabelDlg::StartPi(int PiBtnID , int PiCmdID)
{
	m_PiBtnId=PiBtnID;
	CStringA Title=GetWndTxtA(GetDlgItem(m_PiBtnId));
	if(Title=="批"){
		m_PTimes=0;
		PCmdList.clear();
		m_pImgListCtrl.SetSelectItem(0);
		PCmdList.push_back(PiCmdID);
		GetDlgItem(m_PiBtnId)->SetWindowText(CString("停"));
		m_PiThreadRun=1;
		AfxBeginThread((AFX_THREADPROC)PiThread,this);
		//SetTimer(3,10,NULL);
		
	}else{
		GetDlgItem(m_PiBtnId)->SetWindowText(CString("批"));
		m_PiThreadRun=0;
		//KillTimer(3);
		
	}
}


void CWisdomLabelDlg::OnBnClickedTozfx()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_pImg){
		static BOOL bSave=0;
		if(!m_IsPCl){
			CMTextDlg Dlg;
			Dlg.Title="是否保存结果";
			Dlg.Size=1;
			Dlg.Titles[0]="是否保存结果(0:不保存,1:保存):";
			Dlg.Texts[0]=IntToStr(bSave);
			if(Dlg.DoModal()!=IDOK){
				return;
			}
			bSave=atoi(Dlg.Texts[0]);
		}
		int cn=3;
		int w,h;
		unsigned char *pData=GetImgData(m_pImg,cn,w,h);
		int w1=w;
		int h1=h;
		
		if(w1<h1)
			w1=h1;
		else
			h1=w1;
		
		int x=(w-w1)/2;
		int y=(h-h1)/2;
		if(x==0 && y==0)
			return ;
		
		
		unsigned char *pData1=CopyData3(pData,w,h,cn,x,y,w1,h1);
		m_pImg->Destroy();
		delete m_pImg;
		m_pImg=DataToImg(pData1,cn,w1,h1);
		
		
		delete []pData;
		delete []pData1;



		for(int j=0;j<m_LkList.size();j++){
			vector<Lk >::iterator it=m_LkList.begin()+j;
			for(vector<CPoint>::iterator it1=it->Pts.begin();it1!=it->Pts.end();it1++){
				it1->x-=x;
				it1->y-=y;
				
			}
		}
		OnBnClickedSaveAnnotation();
		m_SrcRect=FRect(0,0,m_pImg->GetWidth(),m_pImg->GetHeight());
		ReDrawItem();
		if(bSave)
			saveImg(m_PicPath,m_pImg);
	}
}


void CWisdomLabelDlg::OnBnClickedTozfxpi()
{
	// TODO: 在此添加控件通知处理程序代码
	StartPi(IDTOZFXPI,IDTOZFX);
}


void CWisdomLabelDlg::OnBnClickedRenamepic()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_pImg){
		static CStringA Qz;
		static int isWholeRename=0;
		static int Picn=0;
		if(!m_IsPCl){
			CMTextDlg dlg;

			dlg.Title="重命名设置";
			dlg.Size=2;
			dlg.Titles[0]="请输入前缀:";
			dlg.Titles[1]="是否完全重命名:";
			dlg.Texts[1]="0";
			if(dlg.DoModal()!=IDOK)
				return;

			Qz=dlg.Texts[0];
			isWholeRename=atoi(dlg.Texts[1]);
			Picn=0;
		}
	
		CStringA newName;
		CStringA newName2;
		while(1)
		{
			if(isWholeRename){
				newName.Format("%s\\%s%05d.jpg",m_FolderPath,Qz,Picn);
				newName2.Format("%s%05d.jpg",Qz,Picn);
				CFileStatus status;
				if(CFile::GetStatus(CString(newName),status)){
					Picn++;
				}else{
					break;
				}
			}else{
				newName.Format("%s\\%s%s.jpg",m_FolderPath,Qz,m_PicTitle);
				newName2.Format("%s%s.jpg",Qz,m_PicTitle);
				break;
			}
		
		}
		CStringA FilePath1=this->GetXmlFileName();
		
		
		int bOk=1;
		vector<Lk> Lks;
		CFileStatus status;
		if(CFile::GetStatus(CString(FilePath1),status))
		{
			OpenXml(FilePath1,&Lks);
			if(!DeleteFile(CString(FilePath1))){
				AfxMessageBoxA(FilePath1+"重命名失败,请检查是否被其他程序占用");
				bOk=0;
			}
		}
		if(bOk){
			if(rename(m_PicPath,newName)==0){
				
				CStringA newName1;
				if(isWholeRename){
					newName1.Format("%s\\%s%05d.xml",m_LabelPath,Qz,Picn);
				}else{
					newName1.Format("%s\\%s%s.xml",m_LabelPath,Qz,m_PicTitle);
				}
				if(Lks.size()>0){
					CImage img;
					img.Load(CString(newName));
					SaveXml(newName1,newName2,newName,m_LabelPath,img.GetWidth(),img.GetHeight(),img.GetBPP()/8,&Lks);
					img.Destroy();
				}
				m_PicPath=newName;
				m_PicName=newName2;
				m_PicTitle=newName2.Left(newName2.GetLength()-4);
				m_pImgListCtrl.SetItemText(GetSelID(),0,CString(m_PicName));
				m_pImgListCtrl.SetItemText(GetSelID(),1,CString(m_PicPath));
				//rename(FilePath1,newName);
			}else{
				AfxMessageBoxA("重命名失败,请检查是否被其他程序占用");
			}
		}
	}
}


void CWisdomLabelDlg::OnBnClickedRenamepic2()
{
	// TODO: 在此添加控件通知处理程序代码
	StartPi(IDRENAMEPICPI,IDRENAMEPIC);
}


void CWisdomLabelDlg::OnBnClickedCuttozfx()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_pImg){
		static BOOL bSave=0;
		if(!m_IsPCl){
			CMTextDlg Dlg;
			Dlg.Title="是否保存结果";
			Dlg.Size=1;
			Dlg.Titles[0]="是否保存结果(0:不保存,1:保存):";
			Dlg.Texts[0]=IntToStr(bSave);
			if(Dlg.DoModal()!=IDOK){
				return;
			}
			bSave=atoi(Dlg.Texts[0]);
		}
		int cn=3;
		int w,h;
		unsigned char *pData=GetImgData(m_pImg,cn,w,h);
		int w1=w;
		int h1=h;
		
		if(w1>h1)
			w1=h1;
		else
			h1=w1;
		
		int x=(w-w1)/2;
		int y=(h-h1)/2;
		if(x==0 && y==0)
			return ;
		
		
		unsigned char *pData1=CopyData3(pData,w,h,cn,x,y,w1,h1);
		m_pImg->Destroy();
		delete m_pImg;
		m_pImg=DataToImg(pData1,cn,w1,h1);
		
		
		delete []pData;
		delete []pData1;



		for(int j=0;j<m_LkList.size();j++){
			vector<Lk >::iterator it=m_LkList.begin()+j;
			for(vector<CPoint>::iterator it1=it->Pts.begin();it1!=it->Pts.end();it1++){
				it1->x-=x;
				it1->y-=y;
				
			}
		}
		OnBnClickedSaveAnnotation();
		m_SrcRect=FRect(0,0,m_pImg->GetWidth(),m_pImg->GetHeight());
		ReDrawItem();
		if(bSave)
			saveImg(m_PicPath,m_pImg);
	}
}


void CWisdomLabelDlg::OnBnClickedCuttozfxpi()
{
	// TODO: 在此添加控件通知处理程序代码
	StartPi(IDCUTTOZFXPI,IDCUTTOZFX);
}


void CWisdomLabelDlg::OnBnClicked9()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_pImg){
		static BOOL bSave=0;
		if(!m_IsPCl){
			CMTextDlg Dlg;
			Dlg.Title="是否保存结果";
			Dlg.Size=1;
			Dlg.Titles[0]="是否保存结果(0:不保存,1:保存):";
			Dlg.Texts[0]=IntToStr(bSave);
			if(Dlg.DoModal()!=IDOK){
				return;
			}
			bSave=atoi(Dlg.Texts[0]);
		}
		int cn=3;
		int w,h;
		unsigned char *pData=GetImgData(m_pImg,cn,w,h);
		int w1=w;
		int h1=h;
		
		if(w1*9<h1*16)
			w1=h1*16/9;
		else
			h1=w1*9/16;
		
		int x=(w-w1)/2;
		int y=(h-h1)/2;
		if(x==0 && y==0)
			return ;
		
		
		unsigned char *pData1=CopyData3(pData,w,h,cn,x,y,w1,h1);
		m_pImg->Destroy();
		delete m_pImg;
		m_pImg=DataToImg(pData1,cn,w1,h1);
		
		
		delete []pData;
		delete []pData1;



		for(int j=0;j<m_LkList.size();j++){
			vector<Lk >::iterator it=m_LkList.begin()+j;
			for(vector<CPoint>::iterator it1=it->Pts.begin();it1!=it->Pts.end();it1++){
				it1->x-=x;
				it1->y-=y;
				
			}
		}
		OnBnClickedSaveAnnotation();
		m_SrcRect=FRect(0,0,m_pImg->GetWidth(),m_pImg->GetHeight());
		ReDrawItem();
		if(bSave)
			saveImg(m_PicPath,m_pImg);
	}
}


void CWisdomLabelDlg::OnBnClicked9pi()
{
	// TODO: 在此添加控件通知处理程序代码
	StartPi(IDTO16_9pi,IDTO16_9);
}


void CWisdomLabelDlg::SelectAreaBrightnessReduction(CImage * pImg)
{
	if(m_TmpGraphRect.right>0){
					CRect tRc=ToTar(m_TmpGraphRect);
					if(m_TmpGraphRect.Width()>0 && m_TmpGraphRect.Height()>0){
						if(tRc.Width()>0 && tRc.Height()>0){
							BrightnessReduction1(pImg,m_TmpGraphRect);
						}
					}
			}
			if(m_TmpLk[1].Pts.size()>1){
					CRect nRc=LkToRect(m_TmpLk[1]);
					CRect tRc=ToTar(nRc);
					if(m_TmpLk[1].LkType==TYPERECT&&nRc.Width()>0 && nRc.Height()>0 && tRc.Width()>0 && tRc.Height()>0){
						BrightnessReduction1(pImg,nRc);
					}
			}
			if((m_MouseState==SELECTDX)){
				if(m_CurLk.LkType==TYPERECT &&m_CurPt.x>=0||m_CurLk.Pts.size()>0){
					if(m_CurLk.Pts.size()>0)
					{
						CRect nRc=LkToRect(m_CurLk);
						CRect tRc=ToTar(nRc);
						if(nRc.Width()>0 && nRc.Height()>0 && tRc.Width()>0 && tRc.Height()>0){
							BrightnessReduction1(pImg,nRc);
						}
					}
				}
			}
			if(m_MouseState==GAIBIOQIAN||m_MouseState==DELETEDX||m_MouseState==SETCOPYBZDX
				||m_MouseState==SELCROPDBC||ERZHIHUA==m_MouseState){
				if(m_CurLk.LkType==TYPERECT&&m_CurLk.Pts.size()>0)
				{
					CRect nRc=LkToRect(m_CurLk);
					CRect tRc=ToTar(nRc);
					if(nRc.Width()>0 && nRc.Height()>0 && tRc.Width()>0 && tRc.Height()>0){
						BrightnessReduction1(pImg,nRc);
					}
				}
			}
}


void CWisdomLabelDlg::OnBnClickedBrightnessaddpi()
{
	// TODO: 在此添加控件通知处理程序代码
	StartPi(IDBRIGHTNESSADDPI,IDBRIGHTNESSADD);
}


void CWisdomLabelDlg::OnBnClickedBrightnesssubpi()
{
	// TODO: 在此添加控件通知处理程序代码
	StartPi(IDBRIGHTNESSSUBPI,IDBRIGHTNESSSUB);
}


void CWisdomLabelDlg::OnBnClickedRedblueexchangepi()
{
	// TODO: 在此添加控件通知处理程序代码
	StartPi(IDREDBLUEEXCHANGEPI,IDREDBLUEEXCHANGE);
}


void CWisdomLabelDlg::OnBnClickedExpandtopi()
{
	// TODO: 在此添加控件通知处理程序代码
	StartPi(IDEXPANDTOPI,IDEXPANDTO);
}


void CWisdomLabelDlg::OnBnClickedAutolabelpi()
{
	// TODO: 在此添加控件通知处理程序代码
	StartPi(IDAUTOLABELPI,IDAUTOLABEL);
}



