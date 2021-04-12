
// MainFrm.cpp : CMainFrame 类的实现
//

#include "stdafx.h"
#include "WAEF.h"

#include "MainFrm.h"


#include "SettingDlg.h"
#include "UpdateDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_COMMAND(ID_INFO_VIEW, &CMainFrame::OnShowInfoView)
	ON_MESSAGE(MSG_INIT_VIEW, OnSetView)
	ON_COMMAND(ID_SPID_VIEW, &CMainFrame::OnShowSpiderView)
	ON_COMMAND(ID_SCAN_VIEW, &CMainFrame::OnShowScanView)
	ON_COMMAND(ID_REPT_VIEW, &CMainFrame::OnShowReportView)
	ON_COMMAND(ID_PLUG_VIEW, &CMainFrame::OnPlugView)
	ON_COMMAND(ID_TOOL_VIEW, &CMainFrame::OnToolView)
	ON_WM_SIZE()
	ON_COMMAND(ID_SETTING_DLG, &CMainFrame::OnSettingDlg)
	ON_COMMAND(ID_CHECK_UPDATE, &CMainFrame::OnCheckUpdate)
	ON_COMMAND(ID_DEEP_VIEW, &CMainFrame::OnShowDeepView)
	ON_COMMAND(ID_HTTP_DEBUG, &CMainFrame::OnDebugView)
	ON_COMMAND(ID_AUDIT_VIEW, &CMainFrame::OnAuditView)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 状态行指示器
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame 构造/析构

CMainFrame::CMainFrame()
{
	// TODO: 在此添加成员初始化代码
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("未能创建工具栏\n");
		return -1;      // 未能创建
	}

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("未能创建状态栏\n");
		return -1;      // 未能创建
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	// TODO: 如果不需要可停靠工具栏，则删除这三行
	//m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	//EnableDocking(CBRS_ALIGN_ANY);
	//DockControlBar(&m_wndToolBar);
	
	CenterWindow();

	return 0;
}

// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 消息处理程序

LRESULT CMainFrame::OnSetView(WPARAM wParam, LPARAM lParam)
{
	_hView = (HWND)lParam;
	return 0;
}

void CMainFrame::OnShowInfoView()
{
	// TODO: 在此添加命令处理程序代码
	::PostMessage(_hView,MSG_SWITCH_VIEW, INFO_COLLECT_VIEW,0);
}

void CMainFrame::OnShowSpiderView()
{
	// TODO: 在此添加命令处理程序代码
	::PostMessage(_hView,MSG_SWITCH_VIEW, WEAK_SPIDER_VIEW,0);
}


void CMainFrame::OnShowScanView()
{
	// TODO: 在此添加命令处理程序代码
	::PostMessage(_hView,MSG_SWITCH_VIEW, WEAK_SCANNER_VIEW,0);
}

void CMainFrame::OnShowDeepView()
{
	// TODO: 在此添加命令处理程序代码
	::PostMessage(_hView,MSG_SWITCH_VIEW, DEEP_SCANNER_VIEW,0);
}


void CMainFrame::OnShowReportView()
{
	// TODO: 在此添加命令处理程序代码
	::PostMessage(_hView,MSG_SWITCH_VIEW, WEAK_REPORT_VIEW,0);
}


void CMainFrame::OnPlugView()
{
	// TODO: 在此添加命令处理程序代码
	::PostMessage(_hView,MSG_SWITCH_VIEW, WEAK_PLUGIN_VIEW,0);
}


void CMainFrame::OnToolView()
{
	// TODO: 在此添加命令处理程序代码
	::PostMessage(_hView,MSG_SWITCH_VIEW, WEAK_ASSIS_VIEW,0);
}

void CMainFrame::OnDebugView()
{
	// TODO: 在此添加命令处理程序代码
	::PostMessage(_hView,MSG_SWITCH_VIEW, WEAK_DEBUG_VIEW,0);
}


void CMainFrame::OnAuditView()
{
	// TODO: 在此添加命令处理程序代码
	::PostMessage(_hView,MSG_SWITCH_VIEW, WEAK_AUDIT_VIEW,0);
}



BOOL CMainFrame::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle , const RECT& rect , CWnd* pParentWnd , LPCTSTR lpszMenuName , DWORD dwExStyle , CCreateContext* pContext)
{
	// TODO: 在此添加专用代码和/或调用基类
	lpszWindowName=_T("Web Application Exploitation Framework - 内测版");
	//this->SetWindowPos(0, 0,0,100,100, SWP_NOMOVE|SWP_NOZORDER);
	return CFrameWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, lpszMenuName, dwExStyle, pContext);
}


BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此添加专用代码和/或调用基类
	//cs.style &= ~WS_THICKFRAME;
	if(!CFrameWnd::PreCreateWindow(cs))
	{
		return FALSE;
	}

	
	WSAData wsaDat;
	WSAStartup(MAKEWORD(1,1),&wsaDat);
	
	//cs.style &= ~WS_OVERLAPPED;
	cs.style &= ~WS_THICKFRAME;
	cs.cy = 565;
	cs.cx = 1024;
	return TRUE;
}


void CMainFrame::OnSettingDlg()
{
	// TODO: 在此添加命令处理程序代码
	CSettingDlg dlg;
	dlg.DoModal();
}


void CMainFrame::OnCheckUpdate()
{
	// TODO: 在此添加命令处理程序代码
	CUpdateDlg dlg;
	dlg.DoModal();
}