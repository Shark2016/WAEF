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
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	/*
	 重写 WM_MOUSEACTIVATE 消息处理函数，不然鼠标点击该View时会报错
	 详情参考:
	 《在没有文档的情况下使用CView及其派生类》 http://www.cnblogs.com/clever101/archive/2009/12/05/1617472.html
	 《有关在对话框上创建视图上的再探索》http://blog.csdn.net/gxj1680/article/details/3236628
	*/

	return CHtmlView::OnMouseActivate(pDesktopWnd, nHitTest, message);
	//return CWnd::OnMouseActivate(pDesktopWnd, nHitTest, message);
}