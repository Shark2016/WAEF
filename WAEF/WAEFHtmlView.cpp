#include "stdafx.h"
#include "WAEFHtmlView.h"


CWAEFHtmlView::CWAEFHtmlView(void)
{
}


CWAEFHtmlView::~CWAEFHtmlView(void)
{
}
BEGIN_MESSAGE_MAP(CWAEFHtmlView, CHtmlView)
	ON_WM_MOUSEACTIVATE()
END_MESSAGE_MAP()


int CWAEFHtmlView::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	/*
	 ��д WM_MOUSEACTIVATE ��Ϣ����������Ȼ�������Viewʱ�ᱨ��
	 ����ο�:
	 ����û���ĵ��������ʹ��CView���������ࡷ http://www.cnblogs.com/clever101/archive/2009/12/05/1617472.html
	 ���й��ڶԻ����ϴ�����ͼ�ϵ���̽����http://blog.csdn.net/gxj1680/article/details/3236628
	*/

	return CHtmlView::OnMouseActivate(pDesktopWnd, nHitTest, message);
	//return CWnd::OnMouseActivate(pDesktopWnd, nHitTest, message);
}