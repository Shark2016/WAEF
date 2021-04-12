
// WAEFView.cpp : CWAEFView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "WAEF.h"
#endif

#include "WAEFSet.h"
#include "WAEFDoc.h"
#include "WAEFView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CWAEFView

IMPLEMENT_DYNCREATE(CWAEFView, COleDBRecordView)

BEGIN_MESSAGE_MAP(CWAEFView, COleDBRecordView)
	ON_WM_CREATE()
	ON_MESSAGE(MSG_SWITCH_VIEW, OnSwitchView)
	ON_MESSAGE(MSG_SET_SPIDER_TARGET, OnSetSpiderTarget)
	ON_MESSAGE(MSG_SET_DEEPSCAN_TARGET, OnSetDeepScanTarget)
	ON_MESSAGE(MSG_SET_PLUGIN_TARGET, OnSetPluginTarget)
	ON_MESSAGE(MSG_ADDSCAN_URL, OnAddScanUrl)
	ON_MESSAGE(MSG_ADDREPORT_ITEM, OnAddReportItem)
END_MESSAGE_MAP()

// CWAEFView 构造/析构

CWAEFView::CWAEFView()
	: COleDBRecordView(CWAEFView::IDD)
	, _info_collect_view(NULL)
{
	m_pSet = NULL;
	// TODO: 在此处添加构造代码

}

CWAEFView::~CWAEFView()
{
	//int i;
	//for(i=0;i<VIEW_COUNT;i++)
	//{
	//	delete [] _view_array[i];
	//}
}

void CWAEFView::DoDataExchange(CDataExchange* pDX)
{
	COleDBRecordView::DoDataExchange(pDX);
	// 可以插入 DDX_* 函数以及 SetDlgItem*/GetDlgItem* API 调用以将数据库链接到视图
	// ex. ::SetDlgItemText(m_hWnd, IDC_MYCONTROL, m_pSet->m_MyColumn);
	// 有关详细信息，请参阅 MSDN 和 OLEDB 示例
}

BOOL CWAEFView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return COleDBRecordView::PreCreateWindow(cs);
}

void CWAEFView::OnInitialUpdate()
{
	m_pSet = &GetDocument()->m_WAEFSet;
	{
		CWaitCursor wait;
		HRESULT hr = m_pSet->OpenAll();
		if (FAILED(hr))
		{
			// 未能打开记录集。如果此记录集是
			// 一个存储过程，则确保在调用
			// OpenAll() 方法之前已正确
			// 初始化所有的输入参数。

//			AfxMessageBox(_T("未能打开记录集。"), MB_OK);
			// 因为在没有打开行集的情况下试图
			// 更改当前记录将导致故障，
			// 所以禁用“下一个”和“上一个”记录命令
			m_bOnFirstRecord = TRUE;
			m_bOnLastRecord = TRUE;
		}
		if( hr == DB_S_ENDOFROWSET )
		{
			// 行集是空的(不包含任何行)
			AfxMessageBox(_T("已打开记录集，但未返回任何行。"), MB_OK);
			// 禁用“下一个”和“上一个”记录命令
			m_bOnFirstRecord = TRUE;
			m_bOnLastRecord = TRUE;
		}
	}
	COleDBRecordView::OnInitialUpdate();
	
	::PostMessage(GetParentFrame()->GetSafeHwnd(), MSG_INIT_VIEW, 0, (LPARAM)m_hWnd);
}


// CWAEFView 诊断

#ifdef _DEBUG
void CWAEFView::AssertValid() const
{
	COleDBRecordView::AssertValid();
}

void CWAEFView::Dump(CDumpContext& dc) const
{
	COleDBRecordView::Dump(dc);
}

CWAEFDoc* CWAEFView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CWAEFDoc)));
	return (CWAEFDoc*)m_pDocument;
}
#endif //_DEBUG


// CWAEFView 数据库支持
CRowset<>* CWAEFView::OnGetRowset()
{
	return m_pSet->GetRowsetBase();
}



// CWAEFView 消息处理程序


int CWAEFView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (COleDBRecordView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO:  在此添加您专用的创建代码
	int i=0;
	_info_collect_view = new CInfoCollView();
	((CWnd*)_info_collect_view)->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,CRect(0,0,1020,800), this, AFX_IDW_PANE_FIRST, NULL);
	_view_array[i++] = _info_collect_view;

	_site_spider_view = new CSiteSpiderView();
	((CWnd*)_site_spider_view)->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,CRect(0,0,1020,800), this, AFX_IDW_PANE_FIRST, NULL);
	_view_array[i++] = _site_spider_view;

	_weak_scan_view = new CWeakScanView();
	((CWnd*)_weak_scan_view)->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,CRect(0,0,1020,800), this, AFX_IDW_PANE_FIRST, NULL);
	_view_array[i++] = _weak_scan_view;

	_deep_scan_view = new CDeepScanView();
	((CWnd*)_deep_scan_view)->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,CRect(0,0,1020,800), this, AFX_IDW_PANE_FIRST, NULL);
	_view_array[i++] = _deep_scan_view;

	_weak_report_view = new CWeakReportView();
	((CWnd*)_weak_report_view)->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,CRect(0,0,1020,800), this, AFX_IDW_PANE_FIRST, NULL);
	_view_array[i++] = _weak_report_view;

	_code_audit_view = new CCodeAuditView();
	((CWnd*)_code_audit_view)->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,CRect(0,0,1020,800), this, AFX_IDW_PANE_FIRST, NULL);
	_view_array[i++] = _code_audit_view;

	_weak_plugin_view = new CWeakPluginView();
	((CWnd*)_weak_plugin_view)->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,CRect(0,0,1020,800), this, AFX_IDW_PANE_FIRST, NULL);
	_view_array[i++] = _weak_plugin_view;

	_http_debug_view = new CHttpDebugView();
	((CWnd*)_http_debug_view)->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,CRect(0,0,1020,800), this, AFX_IDW_PANE_FIRST, NULL);
	_view_array[i++] = _http_debug_view;

	_assis_tool_view = new CAssisToolView();
	((CWnd*)_assis_tool_view)->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,CRect(0,0,1020,800), this, AFX_IDW_PANE_FIRST, NULL);
	_view_array[i++] = _assis_tool_view;

	OnSwitchView(INFO_COLLECT_VIEW, 0);

	return 0;
}

LRESULT CWAEFView::OnSwitchView(WPARAM wParam, LPARAM lParam)
{
	int i;
	for(i=0;i<VIEW_COUNT;i++)
	{
		if(i==wParam)
			_view_array[i]->ShowWindow(SW_NORMAL);
		else
			_view_array[i]->ShowWindow(SW_HIDE);
	}
	return 0;
}

LRESULT CWAEFView::OnSetSpiderTarget(WPARAM wParam, LPARAM lParam)
{
	CString target = (char*)lParam;
	_site_spider_view->SetTarget(target);
	OnSwitchView(WEAK_SPIDER_VIEW, 0);
	return 0;
}

LRESULT CWAEFView::OnSetDeepScanTarget(WPARAM wParam, LPARAM lParam)
{
	CString target = (char*)lParam;
	_deep_scan_view->SetTargetHost(target);
	OnSwitchView(DEEP_SCANNER_VIEW, 0);
	return 0;
}

LRESULT CWAEFView::OnSetPluginTarget(WPARAM wParam, LPARAM lParam)
{
	CString target = (char*)lParam;
	_weak_plugin_view->SetTargetHost(target);
	OnSwitchView(WEAK_PLUGIN_VIEW, 0);
	return 0;
}

LRESULT CWAEFView::OnAddScanUrl(WPARAM wParam, LPARAM lParam)
{
	pstruct_scan_item pscan_item = (pstruct_scan_item)lParam;
	_weak_scan_view->AddScanUrl(pscan_item);

	return 0;
}

LRESULT CWAEFView::OnAddReportItem(WPARAM wParam, LPARAM lParam)
{
	CString url = (char*)wParam;
	pstruct_scan_item pscan_item = (pstruct_scan_item)lParam;
	_weak_report_view->AddReportItem(url, pscan_item);
	return 0;
}