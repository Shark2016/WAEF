#pragma once
#include "afxwin.h"


#include "WeakScanMdl.h"
#include "afxcmn.h"

#include "HttpProxy.h"

// CWeakScanView 窗体视图

class CWeakScanView : public CFormView
{
	DECLARE_DYNCREATE(CWeakScanView)

public:
	CWeakScanView();           // 动态创建所使用的受保护的构造函数

protected:
	virtual ~CWeakScanView();

public:
	enum { IDD = IDD_WEAK_SCAN_VIEW };
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
	virtual void OnInitialUpdate();
	static int AddScanUrl(pstruct_scan_item pscan_item);
	//CListBox targetList;
	afx_msg void OnBnClickedScanStart();

private:
	bool _first;
	static CWeakScanMdl _weak_scan_mdl;
	static int	_itemCount;
	static CListCtrl _scanlist;
	CListBox _loglist;

	CHttpProxy				httpproxy;

public:
	void FreeScanItem(pstruct_scan_item pscan_item);
	static bool CompareUrl(CString url1, CString url2);
	static LRESULT CALLBACK NotifyProc(string& request, httpproxy_mode mode);
	afx_msg void OnBnClickedWeakReport();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnRclickScanList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLbnDblclkWeakLoglist();
	afx_msg void OnTreepopDownTree();
	afx_msg void OnScanClearList();
};


