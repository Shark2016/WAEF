// HttpDebugView.cpp : 实现文件
//

#include "stdafx.h"
#include "WAEF.h"
#include "HttpDebugView.h"


// CHttpDebugView

IMPLEMENT_DYNCREATE(CHttpDebugView, CFormView)

CEdit			CHttpDebugView::m_HttpRequest;
CEdit			CHttpDebugView::m_HttpResponse;
HANDLE			CHttpDebugView::_hMutex;
int				CHttpDebugView::_reqIndex;
CListCtrl		CHttpDebugView::m_HttpRequestList;

CHttpDebugView::CHttpDebugView()
	: CFormView(CHttpDebugView::IDD)
{
	_first = true;
	_hMutex=CreateMutex(NULL, FALSE, NULL);
}

CHttpDebugView::~CHttpDebugView()
{
}

void CHttpDebugView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_HTTP_REQUEST, m_HttpRequest);
	DDX_Control(pDX, IDC_HTTP_RESPONSE, m_HttpResponse);
	DDX_Control(pDX, IDC_HTTP_REQUEST_LIST, m_HttpRequestList);
}

BEGIN_MESSAGE_MAP(CHttpDebugView, CFormView)
	ON_BN_CLICKED(IDC_HTTP_START_PROXY, &CHttpDebugView::OnBnClickedHttpStartProxy)
	ON_BN_CLICKED(IDC_HTTP_SEND_REQUEST, &CHttpDebugView::OnBnClickedHttpSendRequest)
	ON_MESSAGE(MSG_PROXY_NOTIFY, OnProxyNotify)
END_MESSAGE_MAP()


// CHttpDebugView 诊断

#ifdef _DEBUG
void CHttpDebugView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CHttpDebugView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CHttpDebugView 消息处理程序


void CHttpDebugView::OnBnClickedHttpStartProxy()
{
	// TODO: 在此添加控件通知处理程序代码
	httpproxy.initproxy();
	httpproxy.setopt_port(8080);
	httpproxy.setopt_owner(GetSafeHwnd());
	httpproxy.setopt_notify(NotifyProc);
	httpproxy.setopt_mode(httpproxy_mode_httpdebug);
	httpproxy.startproxy();
}

LRESULT CALLBACK CHttpDebugView::NotifyProc(string& request, httpproxy_mode mode)
{
	if(mode == httpproxy_mode_httpdebug)
	{
		m_HttpRequest.SetWindowText(request.c_str());
		WaitForSingleObject(_hMutex, INFINITE);
		CString req;
		m_HttpRequest.GetWindowText(req);
		request = req;
		m_HttpRequest.SetWindowText("");
		//m_HttpResponse.SetWindowText("");
		CString reqInx;
		reqInx.Format("%d", _reqIndex+1);
		m_HttpRequestList.InsertItem(_reqIndex, reqInx);
		m_HttpRequestList.SetItemText(_reqIndex++, 1, req.Left(req.Find("\r\n")));
		ReleaseMutex(_hMutex);
	}
	else
	{
		m_HttpResponse.SetWindowText(request.c_str());
		m_HttpRequestList.SetItemText(_reqIndex-1, 2, request.substr(0,request.find("\r\n")).c_str());
	}

	return 0;
}


void CHttpDebugView::OnBnClickedHttpSendRequest()
{
	// TODO: 在此添加控件通知处理程序代码
	ReleaseMutex(_hMutex);
	::EnableWindow(GetDlgItem(IDC_HTTP_SEND_REQUEST)->GetSafeHwnd(), false);
}

LRESULT CHttpDebugView::OnProxyNotify(WPARAM wParam, LPARAM lParam)
{
	WaitForSingleObject(_hMutex, INFINITE);
	::EnableWindow(GetDlgItem(IDC_HTTP_SEND_REQUEST)->GetSafeHwnd(), true);
	return 0;
}

BOOL CHttpDebugView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if( IsDialogMessage( pMsg ) )
		return TRUE;
	else
		return CFormView::PreTranslateMessage(pMsg);
}


void CHttpDebugView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类
	if(_first)		//不知道为什么要调用两次，所以加个判断
	{
		m_HttpRequestList.InsertColumn(0,_T("ID"),LVCFMT_LEFT,30);
		m_HttpRequestList.InsertColumn(1,_T("请求"),LVCFMT_LEFT,130);
		m_HttpRequestList.InsertColumn(2,_T("响应"),LVCFMT_LEFT,130);	
		m_HttpRequestList.SetExtendedStyle(LVS_EX_FULLROWSELECT);
		_first = false;
	}
}
