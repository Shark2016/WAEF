#pragma once
#include "afxcmn.h"
#include "InfoCollMdl.h"
#include "afxwin.h"


// CInfoCollView ������ͼ

class CInfoCollView : public CFormView
{
	DECLARE_DYNCREATE(CInfoCollView)

public:
	CInfoCollView();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��
	virtual ~CInfoCollView();

public:
	enum { IDD = IDD_INFO_COLL_VIEW };
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
	CListCtrl _info_list;
	afx_msg void OnBnClickedBtnGetInfo();
	afx_msg LRESULT OnDomainInfoItem(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnHeaderInfoItem(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSignatureItem(WPARAM wParam, LPARAM lParam);

private:
	CInfoCollMdl _info_coll_mdl;

	bool _first;

public:
	virtual void OnInitialUpdate();
//	CEdit _ip_address;
	CString _domain;
	CString _ipaddress;
	afx_msg void OnBnClickedBtnResolveIp();
	afx_msg void OnBnClickedBtnServerInfo();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnRclickInfoList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnInfopopOpenSite();
	afx_msg void OnInfopopSpiderSite();
	afx_msg void OnInfopopClearList();
	afx_msg void OnInfopopImportList();
	afx_msg void OnInfopopExportList();
	afx_msg void OnDblclkInfoList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnImportList();
	afx_msg void OnDeepScan();
	afx_msg void OnCollectPluginScan();
};


