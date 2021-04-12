
// WAEFView.h : CWAEFView ��Ľӿ�
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
protected: // �������л�����
	CWAEFView();
	DECLARE_DYNCREATE(CWAEFView)

public:
	enum{ IDD = IDD_WAEF_VIEW };
	CWAEFSet* m_pSet;

// ����
public:
	CWAEFDoc* GetDocument() const;

// ����
public:

// ��д
public:
	virtual CRowset<>* OnGetRowset();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual void OnInitialUpdate(); // ������һ�ε���

// ʵ��
public:
	virtual ~CWAEFView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CFormView* _view_array[VIEW_COUNT];

// ���ɵ���Ϣӳ�亯��
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

#ifndef _DEBUG  // WAEFView.cpp �еĵ��԰汾
inline CWAEFDoc* CWAEFView::GetDocument() const
   { return reinterpret_cast<CWAEFDoc*>(m_pDocument); }
#endif

