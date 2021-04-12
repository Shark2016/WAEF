
// MainFrm.h : CMainFrame 类的接口
//

#pragma once

class CMainFrame : public CFrameWnd
{
	
protected: // 仅从序列化创建
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// 特性
public:

// 操作
public:

// 实现
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // 控件条嵌入成员
	CToolBar          m_wndToolBar;
	CStatusBar        m_wndStatusBar;

// 生成的消息映射函数
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnShowInfoView();
	afx_msg LRESULT OnSetView(WPARAM wParam, LPARAM lParam);

protected:
	HWND _hView;
public:
	afx_msg void OnShowSpiderView();
	afx_msg void OnShowScanView();
	afx_msg void OnShowReportView();
	afx_msg void OnPlugView();
	afx_msg void OnToolView();
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle = WS_OVERLAPPEDWINDOW, const RECT& rect = rectDefault, CWnd* pParentWnd = NULL, LPCTSTR lpszMenuName = NULL, DWORD dwExStyle = 0, CCreateContext* pContext = NULL);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	afx_msg void OnSettingDlg();
	afx_msg void OnCheckUpdate();
	afx_msg void OnShowDeepView();
	afx_msg void OnDebugView();
	afx_msg void OnAuditView();
};


