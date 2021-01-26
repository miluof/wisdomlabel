#pragma once
class FRect
{
public:
	FRect(void);
	FRect(int _left,int _top,int _right,int _bottom);
	~FRect(void);
	double left;
	double right;
	double top;
	double bottom;
	double Width(void);
	double Height(void);
	CRect ToCRect(void);
	void OffsetRect(double x, double y);
};

