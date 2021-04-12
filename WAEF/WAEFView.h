
// WAEFView.h : CWAEFView 类的接口
//

#pragma once

#include "InfoCollView.h"
#include "SiteSpiderView.h"
#include "WeakScanView.h"
#include "DeepScanView.h"
#include "WeakReportView.h"
#include "AssisToolView.h"
#include "CodeAuditView.h"
#include "HttpDebugView.h"
#include "WeakPluginView.h"

class CWAEFSet;

class CWAEFView : public COleDBRecordView
{
protected: // 仅从序列化创建
	CWAEFView();
	DECLARE_DYNCREATE(CWAEFView)

public:
	enum{ IDD = IDD_WAEF_VIEW };
	CWAEFSet* m_pSet;

// 特性
public:
	CWAEFDoc* GetDocument() const;

// 操作
public:

// 重写
public:
	virtual CRowset<>* OnGetRowset();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual void OnInitialUpdate(); // 构造后第一次调用

// 实现
public:
	virtual ~CWAEFView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CFormView* _view_array[VIEW_COUNT];

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
public:
	CInfoCollView* _info_collect_view;
	CSiteSpiderView* _site_spider_view;
	CWeakScanView* _weak_scan_view;
	CDeepScanView* _deep_scan_view;
	CWeakReportView* _weak_report_view;
	CAssisToolView* _assis_tool_view;
	CCodeAuditView* _code_audit_view;
	CHttpDebugView* _http_debug_view;
	CWeakPluginView* _weak_plugin_view;
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnSwitchView(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetSpiderTarget(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetDeepScanTarget(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetPluginTarget(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnAddScanUrl(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnAddReportItem(WPARAM wParam, LPARAM lParam);
};

#ifndef _DEBUG  // WAEFView.cpp 中的调试版本
inline CWAEFDoc* CWAEFView::GetDocument() const
   { return reinterpret_cast<CWAEFDoc*>(m_pDocument); }
#endif

