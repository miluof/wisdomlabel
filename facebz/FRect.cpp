#include "StdAfx.h"
#include "FRect.h"


FRect::FRect(void)
{
}
FRect::FRect(int _left,int _top,int _right,int _bottom){
	left=_left;
	top=_top;
	right=_right;
	bottom=_bottom;
}

FRect::~FRect(void)
{
}


double FRect::Width(void)
{
	return right-left;
}


double FRect::Height(void)
{
	return bottom-top;
}


CRect FRect::ToCRect(void)
{
	return CRect(left,top,right,bottom);
}


void FRect::OffsetRect(double x, double y)
{
	left+=x;
	right+=x;
	top+=y;
	bottom+=y;
}
