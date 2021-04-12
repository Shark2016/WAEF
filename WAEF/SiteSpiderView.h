#pragma once

#include "NetSpider.h"
#include "afxcmn.h"

#include "HttpProxy.h"

// CSiteSpiderView 窗体视图

typedef LRESULT (CALLBACK* TRAV_NOTIFYPROC)(string url, url_type type, bool param);

class CSiteSpiderView : public CFormView
{
	DECLARE_DYNCREATE(CSiteSpiderView)

public:
	CSiteSpiderView();           // 动态创建所使用的受保护的构造函数
	virtual ~CSiteSpiderView();

public:
	enum { IDD = IDD_SITE_SPIDER_VIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:
	NetSpider m_spider;
	static HWND				_static_hWnd;
	static HWND				_hParenthWnd;
	bool _first;
	static CTreeCtrl		m_TreeCtrl;
	static CListCtrl		m_SessionList;
	static CString			m_strCookie;
	CHttpProxy				httpproxy;

	static string			_exportlist;

protected:
	CString GetTreeItemUrl(HTREEITEM hItem);
	void TraversalUrl(string preUrl, HTREEITEM hItem, url_type type, TRAV_NOTIFYPROC pNotify, bool notifyParent = false);

public:
	CImageList m_ImgList;
	CString m_strTargetUrl;

	static LRESULT CALLBACK NotifyProc(string url);
	static LRESULT CALLBACK NotifyProc(string& request, httpproxy_mode mode);

	static LRESULT CALLBACK ScanNotifyProc(string url, url_type type, bool param);
	static LRESULT CALLBACK ExportNotifyProc(string url, url_type type, bool param);

	afx_msg void OnBnClickedSpiderStop();
	virtual void OnInitialUpdate();
	afx_msg void OnRclickTreeResult(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedSpiderStart();
	afx_msg void OnBnClickedBtnSetSession();
	int _deepth;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	int SetTarget(CString target);
	afx_msg void OnTreepopOpenUrl();
	afx_msg void OnTreepopCopyUrl();
	afx_msg void OnTreepopCheckNode();
	afx_msg void OnTreepopClearNode();
	afx_msg void OnTreepopClearTree();
	afx_msg void OnNMDblclkSessionList(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnTreepopExportNode();
};


