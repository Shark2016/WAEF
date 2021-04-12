#pragma once
#include <afxhtml.h>			// MFC HTML view support

class CWAEFHtmlView :
	public CHtmlView
{
public:
	CWAEFHtmlView(void);
	~CWAEFHtmlView(void);
	DECLARE_MESSAGE_MAP()
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
};

