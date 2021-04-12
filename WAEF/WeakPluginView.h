#pragma once
#include "afxcmn.h"

#include "WeakPluginMdl.h"
#include "afxwin.h"



// CWeakPluginView 窗体视图

class CWeakPluginView : public CFormView
{
	DECLARE_DYNCREATE(CWeakPluginView)

public:
	CWeakPluginView();           // 动态创建所使用的受保护的构造函数
	virtual ~CWeakPluginView();

public:
	enum { IDD = IDD_WAEF_PLUGIN_VIEW };
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
	int SetTargetHost(CString target);
	CString m_TargetHost;
	virtual void OnInitialUpdate();

private:
	CWeakPluginMdl	_weak_plugin_mdl;
	bool			m_First;

public:
	CListCtrl m_pluginList;
	CEdit m_pluginInteractiveBox;
	CListCtrl m_pluginResultList;
	afx_msg void OnDblclkPlugPluginList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedPlugLoadDesc();
	afx_msg void OnBnClickedPlugParamStr();
	afx_msg void OnBnClickedPlugExecute();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};


