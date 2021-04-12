#pragma once

#include "HttpProxy.h"
#include "afxwin.h"
#include "afxcmn.h"

// CHttpDebugView 窗体视图

class CHttpDebugView : public CFormView
{
	DECLARE_DYNCREATE(CHttpDebugView)

public:
	CHttpDebugView();           // 动态创建所使用的受保护的构造函数
	virtual ~CHttpDebugView();

public:
	enum { IDD = IDD_HTTP_DEBUG_VIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	static LRESULT CALLBACK NotifyProc(string& request, httpproxy_mode mode);

	afx_msg LRESULT OnProxyNotify(WPARAM wParam, LPARAM lParam);

private:
	bool					_first;
	CHttpProxy				httpproxy;
	static int				_reqIndex;
	static CEdit			m_HttpRequest;
	static CEdit			m_HttpResponse;
	static CListCtrl		m_HttpRequestList;
	static HANDLE			_hMutex;
	static HANDLE			_hMutex2;

public:
	afx_msg void OnBnClickedHttpStartProxy();
	afx_msg void OnBnClickedHttpSendRequest();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnInitialUpdate();
};


