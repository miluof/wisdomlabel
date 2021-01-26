
// WisdomLabel.h : 头文件
//

#pragma once
#include "FRect.h"
#include <vector>
#include <map>
#include "afxwin.h"
#include "explorer3.h"
#include "PicListCtrl.h"
#include "SelectBtn.h"
#include "KjjDlg.h"
using namespace std;
static CStringA MSName[]={
	"修改图形,按左键选择修改对象,按右键切换到增加矩形",
	"设置矩形第一个对角点，按左键确定第一个点，按右键切换到修改图形",
	"设置矩形第二个对角点，按左键确定第二个点，按右键切换到修改图形",
	"设置多边形",
	"修改图形,移动到目标位置按左键，按右键切换到增加矩形",
	"删除标注",
	"二值化",
	"修改标签",
	"放置被粘贴的裁剪",
	"选择要与裁剪一起复制的标注,右键下一步",
	"选择多边形用来裁剪,右键下一步"
};
enum{
	MOUSESTATENONE,
	SETFACERECTFIRSTPOINT,
	SETFACERECTSECONDPOINT,
	SETDBX,
	GAIDIAN,
	DELETEDX,
	ERZHIHUA,
	GAIBIOQIAN,
	SETCOPYIMGPOS,
	SETCOPYBZDX,
	SELCROPDBC,
	SETFACECENTER,
	SETDBXMIN,
	SETPOINT,
	SET1POINT,
	SET2POINT,
	SET3POINT,
	SET4POINT,
	SET5POINT,
	FGPICX,
	FGPICY,
	SETLK,
	JUAN,
	MOHU1,
	MOHU2
};
enum{
	CMDNONE,
	CMDDELETEPIXEL
};
#define SELECTDX MOUSESTATENONE
#define PIMESSAGE WM_USER+101
enum{
	TYPERECT,
	TYPESPLINE,
	TYPEDBC,
	TYPEPOINT
};
struct Lk{
	int LkType;
	CStringA DxType;
	int isCopy;
	//int IsBh;
	vector<CPoint> Pts;
	CRect BqRc;
	CRect BqPicRc;
	Lk(){
		isCopy=0;
		BqPicRc=BqRc=CRect(-1000,-1000,-1000,-1000);
	}
};
struct fPoint{
double x,y,z;
	fPoint(){
	}
	fPoint(double xx,double yy,double zz=0){
		x=xx;
		y=yy;
		z=zz;
	}
};

struct fLine{
fPoint p1,p2;
	fLine(){
	}
	fLine(fPoint pa,fPoint pb){
		p1=pa;
		p2=pb;
	}
};
// CWisdomLabelDlg 对话框
class CWisdomLabelDlg : public CDialogEx
{
// 构造
public:
	CWisdomLabelDlg(CWnd* pParent = NULL);	// 标准构造函数
	map<CStringA,CBitmap *>m_LabMap;
	map<CStringA,CStringA>m_LabMap1;
	map<CStringA,CStringA>m_LabMap2;
	map<CStringA,Kjj>m_KjjMap;

	vector<int>PCmdList;
	int m_IsPCl;
	int m_PTimes;
	int m_bInit;
	int m_LabSize;
	//CBitmap m_LabPic;
// 对话框数据
	enum { IDD = IDD_WISDOMLABEL };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	void CWisdomLabelDlg::DrawLk(CDC *pDC,Lk *lk,int bSel=0,int bDrawHead=0);
	void CWisdomLabelDlg::DrawLk1(CDC *pDC,Lk *lk,CStringA Hz);
	void DrawRect2(CDC *pDC,CRect Rc);
	void DrawRect1(CDC *pDC,CRect Rc,COLORREF Col,int LineType,int LineWidth);
	CRect ToTar(CRect rcSrc);
	CPoint ToTar(CPoint ptSrc);
	CPoint ToSrc(CPoint ptTar);
	CRect ToSrc(CRect Rc);
	COLORREF m_TextColor;
	int m_ChangePic;
	int m_MaxNum;
	int m_MinNum;

	CStringA m_AutoBzDxName;
	CStringA m_AutoBzPicSize;
	CStringA m_AutoBzSizeType;
	CStringA m_AutoBzModelP;
	CStringA m_AutoBzModelB;

	CStringA m_TmpDir;

	int m_PiCmd;
	int m_PiTimes;
	int m_PiNeedTimes;
	int m_PiThreadRun;
	int m_PiBtnId;

// 实现
protected:
	HICON m_hIcon;

	//int m_DrawLeft;
	//int m_DrawTop;
	CStringA m_CurPath;
	CRect m_TarRect;
	//CRect m_TarRect1;
	FRect m_SrcRect;
	float m_DrawScale;
	CImage *m_pImg;
	CImage *m_CropImg;
	float *m_CropMb;
	CPoint m_CropPos;
	CPoint m_CropNewPos;
	int m_MouseState;
	int m_CmdState;
	int m_PtId;
	int m_EmptyID;
	CPoint m_CurPt;
	Lk     m_CurLk;
	int    m_SelLineID;
	int    m_SelLkID;
	int    m_SelPtID;
	CRect m_GraphRect;
	CRect m_TmpGraphRect;
	float m_Angle;
	CStringA m_RectType;
	vector<CPoint>m_OldDbx;
	vector<CPoint>m_OldDbxMin;
	vector<CPoint>m_Dbx;
	vector<CPoint>m_DbxMin;
	vector<CPoint>m_TmpDbx;
	Lk m_TmpLk[2];
	vector<Lk >m_LkList;
	vector<Lk >m_FzList;
	vector<Lk >m_CropLkList;
	vector<int>m_BackList;
	vector<int>m_FrontList;
	int m_BackOFront;
	int m_CopyOrWidth;
	int m_CopyOrHeight;
	//vector<Lk>m_CropFzList;
	CPoint m_Point[5];
	CPoint m_MousePos;
	CPoint m_CropMousePos;
	CStringA m_PicPath;
	CStringA m_PicName;
	CStringA m_PicTitle;
	CStringA m_FolderPath;
	CStringA m_LabelPath;
	CStringA m_ResPath;

	int m_LeftWidth;
	int m_RightWidth;
	
	//CStringA m_PicName;
	//CStringAList m_PicList;
	int m_IconW;
	int m_SectPtW;
	int m_DrawLk;

	//CStringA m_PicDir;
	//DrawRect(CDC *pDC,CRect Rc,COLORREF Col,int LineType,int LineWidth);
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOpenpic();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnBnClickedCropface();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedSaveAnnotation();
	afx_msg void OnBnClickedOpenAnnotation();
	afx_msg void OnBnClickedCreatelabel();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedCsetpoint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	CStringA GetRectType(void);
	afx_msg void OnCbnSelchangeCombo2();
	afx_msg void OnBnClickedOpenpic2();
	int OpenNewPic(CStringA PicPath);
	afx_msg void OnBnClickedOpenpic3();
	afx_msg void OnBnClickedCropface2();
	afx_msg void OnMouseHWheel(UINT nFlags, short zDelta, CPoint pt);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
//	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnBnClickedOpenpic4();
	afx_msg void OnBnClickedCreatelabel2();
	afx_msg void OnBnClickedCreatelabel3();
	afx_msg void OnBnClickedCreatelabel4();
	afx_msg void OnBnClickedCreatelabel5();
	afx_msg void OnBnClickedDbx();
	afx_msg void OnBnClickedCsetpoint2();
	afx_msg void OnBnClickedIdcreatedate();
	afx_msg void OnBnClickedIdcreatedate3();
	afx_msg void OnBnClickedIdcreatedate4();
	afx_msg void OnBnClickedIdcreatedate5();
	afx_msg void OnBnClickedIdcreatedate6();
	afx_msg void OnBnClickedOpenpic5();
	afx_msg void OnBnClickedOpenpic6();
	afx_msg void OnBnClickedIdcreatedate7();
	afx_msg void OnBnClickedIdcreatedate8();
	afx_msg void OnBnClickedIdcreatedate9();
	afx_msg void OnBnClickedOpenpic7();
	afx_msg void OnBnClickedOpenpic8();
	afx_msg void OnBnClickedOpenpic9();
	afx_msg void OnBnClickedIdcreatedate10();
	afx_msg void OnBnClickedDbx2();
	afx_msg void OnBnClickedOpenpic10();
	afx_msg void OnBnClickedIdcreatedate11();
	afx_msg void OnBnClickedDbx3();
	afx_msg void OnBnClickedIdcreatedate12();
	afx_msg void OnBnClickedIdcreatedate13();
	CStringA GetDataDir(void);
	afx_msg void OnBnClickedOpenpic11();
	afx_msg void OnBnClickedOpenpic12();
	afx_msg void OnBnClickedIdcreatedate14();
	afx_msg void OnBnClickedIdcreatedate15();
	afx_msg void OnBnClickedIdcreatedate16();
	afx_msg void OnBnClickedIdcreatedate17();
	afx_msg void OnBnClickedIdcreatedate19();
	afx_msg void OnBnClickedIdcreatedate20();
	afx_msg void OnFgTpX();
	afx_msg void OnFgTpY();
	afx_msg void OnBnClickedClk();
	afx_msg void OnBnClickedRuihua();
	afx_msg void OnBnClickedZzlb();
	afx_msg void OnBnClickedZzlb2();
	afx_msg void OnBnClickedJuan();
	afx_msg void OnBnClickedQuan();
	afx_msg void OnBnClickedCdisbiao();
	afx_msg void OnBnClickedQuan2();
	afx_msg void OnBnClickedClk2();
	afx_msg void OnBnClickedIdcreatedate21();
	afx_msg void OnBnClickedOpenpic13();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedCsetpoint3();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedOpenpic14();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedQuan3();
	afx_msg void OnBnClickedCdisbiao2();
	afx_msg void OnBnClickedButton7();
	void SaveXml(CStringA xmlName,CStringA picName,CStringA picPath,CStringA picDir,int w,int h,int cn,vector<Lk >*lks);
	afx_msg void OnBnClickedButton8();
	afx_msg void OnBnClickedIdcreatedate22();
	CButton m_AutoHdBtn;
	void LoadLabel(CStringA LabelPath);
	CButton m_ZdZfxBtn;
	afx_msg void OnBnClickedClk3();
	afx_msg void OnBnClickedCsetpoint4();
	void ReDrawItem(void);
	void AddLab(CStringA ss);
	CButton m_KeduBtn;
	CPicListCtrl m_pImgListCtrl;
	CComboBox m_FyTjBox;//翻页条件
	afx_msg void OnBnClickedOpenpicDir();
	afx_msg void OnBnClickedButton9();
	void UpdateImgFileList(void);
	CButton m_DisLabTextBtn;
	CButton m_DisLabIconBtn;
	afx_msg void OnBnClickedCheck5();
	afx_msg void OnBnClickedCheck6();
	afx_msg void OnBnClickedIdcreatedate24();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	void OpenXml(CStringA xmlName,vector<Lk >*lks);
	CStringA GetXmlFileName(void);
	afx_msg void OnBnClickedOpenpic17();
	afx_msg void OnBnClickedIdcreatedate2();
	afx_msg void OnBnClickedIdcreatedate18();
	afx_msg void OnBnClickedVideoToOnePic();
	afx_msg void OnBnClickedButton12();
	afx_msg void OnBnClickedButton13();
	CExplorer3 m_ie;
	afx_msg void OnBnClickedButton15();
	afx_msg void OnBnClickedQuan4();
	afx_msg void OnBnClickedQuan5();
	afx_msg void OnBnClickedIdcreatedate25();
	afx_msg void OnBnClickedQuan6();
	void InitLab(void);
	CButton m_KeduBtn32;
	CButton m_KeBtn8;
	afx_msg void OnBnClickedIdcreatedate27();
	afx_msg void OnBnClickedIdcreatedate28();
	afx_msg void OnBnClickedIdcreatedate29();
	afx_msg void OnBnClickedIdcreatedate30();
	afx_msg void OnBnClickedButton17();
	afx_msg void OnBnClickedIdcreatedate31();
	afx_msg void OnBnClickedIdcreatedate32();
	afx_msg void OnBnClickedIdcreatedate33();
	afx_msg void OnBnClickedIdcreatedate34();
	afx_msg void OnBnClickedIdcreatedate35();
	int GetSelID(void);
	afx_msg void OnHdnItemchangedList3(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedList3(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickList3(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButton14();
	afx_msg void OnBnClickedButton11();
	afx_msg void OnBnClickedIdcreatedate36();
	afx_msg void OnBnClickedOpenAnnotationsDir();
	afx_msg void OnBnClickedIdcreatedate37();
	afx_msg void OnBnClickedIdcreatedate38();
	afx_msg void OnBnClickedIdcreatedate39();
	afx_msg void OnBnClickedIdcreatedate40();
	afx_msg void OnBnClickedIdcreatedate41();
	afx_msg void OnBnClickedIdcreatedate42();
	afx_msg void OnBnClickedIdcreatedate43();
	afx_msg void OnBnClickedIdcreatedate44();
	afx_msg void OnBnClickedIdcreatedate45();
	afx_msg void OnBnClickedButton18();
	afx_msg void OnBnClickedIdcreatedate46();
	afx_msg void OnBnClickedIdcreatedate47();
	afx_msg void OnBnClickedButton19();
	afx_msg void OnBnClickedButton20();
	afx_msg void OnBnClickedButton22();
	afx_msg void OnBnClickedButton23();
	afx_msg void OnBnClickedButton24();
	afx_msg void OnBnClickedButton25();
	afx_msg void OnBnClickedCdisbiao3();
	CButton m_AutoFzDx;
	CButton m_ZdZfxjBtn;
	afx_msg void OnBnClickedButton26();
	afx_msg void OnBnClickedCdisbiao4();
	afx_msg void OnBnClickedCdisbiao5();
	afx_msg void OnBnClickedCdisbiao6();
	afx_msg void OnBnClickedCdisbiao7();
	afx_msg void OnBnClickedCdisbiao8();
	afx_msg void OnBnClickedCdisbiao9();
	CButton m_Auto16b9jBtn;
	afx_msg void OnBnClickedIdcreatedate48();
	afx_msg void OnBnClickedIdcreatedate49();
	void OpenDir(CStringA Dir);
	afx_msg void OnBnClickedOpenpic18();
	afx_msg void OnBnClickedRuihua2();
	void AddToHisDir(CStringA Dir);
	afx_msg void OnBnClickedOpenpic19();
	afx_msg void OnBnClickedCdisbiao10();
	afx_msg void OnBnClickedCdisbiao11();
	afx_msg void OnBnClickedIdcreatedate50();
	afx_msg void OnGetpicbylable();
	afx_msg void OnCheckname();
	afx_msg void OnCheckbox();
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedRuihua3();
	afx_msg void OnRuihua3();
	afx_msg void OnCreatepicmodel();
	afx_msg void OnMhhytj();
	afx_msg void OnMhhy();
	afx_msg void OnClose();
	afx_msg void OnStopmhhytj();
	afx_msg void OnOpenvideoframe();
	afx_msg void On32789();
	afx_msg void OnBnClickedCdisbiao12();
	afx_msg void OnBnClickedIdcreatedate51();
	afx_msg void OnNextall();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedOpenpic20();
	afx_msg void OnBnClickedOpenpic21();
	afx_msg void OnFgjzlb();
	afx_msg void OnBnClickedCdisbiao13();
	afx_msg void OnBnClickedCdisbiao14();
	afx_msg void OnBnClickedIdcreatedate52();
	afx_msg void OnBnClickedIdcreatedate53();
	afx_msg void OnBnClickedIdcreatedate54();
	afx_msg void OnBnClickedIdcreatedate55();
	afx_msg void OnBnClickedButton21();
	afx_msg void OnBnClickedButton28();
	CSelectBtn m_BlSxXBtn;
	afx_msg void OnBnClickedOpenpic22();
	CSelectBtn m_KdBtn;
	afx_msg void OnBnClickedOpenpic23();
	CSelectBtn m_MrBzBtn;
	afx_msg void OnBnClickedOpenpic24();
	CSelectBtn m_TmpDirBtn;
	afx_msg void OnBnClickedOpenpic25();
	afx_msg void OnBnClickedOpenpic26();
	CSelectBtn m_FindNrBtn;
	CSelectBtn m_InputSizeBtn;
	afx_msg void OnBnClickedIdcreatedate56();
	CExplorer3 m_ie1;
	afx_msg void OnBnClickedIdcreatedate57();
	
	afx_msg void OnBnClickedOpenpic27();
	afx_msg void OnBnClickedIdcreatedate58();
	afx_msg void OnHand2auto();
	afx_msg void OnTestiou();
	afx_msg void OnOpenlistbytxt();
	afx_msg void OnYolotoxml();
	afx_msg void OnHeighterr();
	afx_msg void OnFindsame();
	afx_msg void OnRename();
	afx_msg void OnFindsame2();
	afx_msg void OnSearchpd();
	afx_msg void OnNopd();
	afx_msg void OnSizeto();
	afx_msg void OnPicstovideo();
	afx_msg void OnPddxname();
	afx_msg void OnDeletelabpixel();
	afx_msg void OnPideletepixel();
	afx_msg void OnMzcopyto();
	afx_msg void Onwidefacetovoc();
	afx_msg void OnLabelscale();
	void PiChuli(void);
	afx_msg void OnScaleto();
	afx_msg void OnBnClickedIdcreatedate59();
	afx_msg void OnOpenrgb();
	afx_msg void OnPideletepixeldbc();
	afx_msg void OnBnClickedDeletelabel();
	afx_msg void OnCleanlabel();
	afx_msg void OnCaldbcrect();
	afx_msg void OnPidbcnorect();
	afx_msg void OnStopp();
	afx_msg void OnBnClickedTogray();
	afx_msg void OnBnClickedTogray2();
	afx_msg LRESULT OnPiMessage(WPARAM wp,LPARAM lp);
	UINT static PiThread(void* p);
	void StartPi(int PiBtnID , int PiCmdID);
	afx_msg void OnBnClickedTozfx();
	afx_msg void OnBnClickedTozfxpi();
	afx_msg void OnBnClickedRenamepic();
	afx_msg void OnBnClickedRenamepic2();
	afx_msg void OnBnClickedCuttozfx();
	afx_msg void OnBnClickedCuttozfxpi();
	afx_msg void OnBnClicked9();
	afx_msg void OnBnClicked9pi();
	void SelectAreaBrightnessReduction(CImage * pImg);
	afx_msg void OnBnClickedBrightnessaddpi();
	afx_msg void OnBnClickedBrightnesssubpi();
	afx_msg void OnBnClickedRedblueexchangepi();
	afx_msg void OnBnClickedExpandtopi();
	afx_msg void OnBnClickedAutolabelpi();
	
};
CStringA GetModuleDir();