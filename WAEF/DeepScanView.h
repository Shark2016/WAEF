#pragma once

#include "afxwin.h"

#include "DeepScanMdl.h"
#include "afxcmn.h"

// CDeepScanView ������ͼ

class CDeepScanView : public CFormView
{
	DECLARE_DYNCREATE(CDeepScanView)

public:
	CDeepScanView();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��
	virtual ~CDeepScanView();

public:
	enum { IDD = IDD_DEEP_SCAN_VIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
	int SetTargetHost(CString targetHost);
	
private:
	CDeepScanMdl	_deep_scan_mdl;

	bool			m_First;
	CString			m_TargetHost;
	cms_type		m_TargetCmsType;
	DWORD			m_TargetServerType;

public:
	virtual void OnInitialUpdate();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnSelchangeDeepServer();
	afx_msg void OnSelchangeDeepCmssystem();
	afx_msg void OnBnClickedDeepLoadDesc();
	afx_msg void OnDblclkDeepModuleList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedDeepParamStr();
	afx_msg void OnBnClickedDeepExecute();

private:
	CEdit		m_deepInteractiveBox;
	CImageList	m_ImgList;
	CTreeCtrl m_ModuleTree;
};


