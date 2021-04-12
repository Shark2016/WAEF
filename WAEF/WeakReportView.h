#pragma once

#include "WeakScanMdl.h"
#include "afxcmn.h"

#include "WAEFHtmlView.h"


enum report_type {
	report_host,
	report_vul
};

typedef struct _struct_report_item
{
	_struct_report_item(string hostname)
	{
		type = report_host;
		hostname = hostname;
		high_count = 0;
		medium_count = 0;
		low_count = 0;
		info_count = 0;
	};
	_struct_report_item(pstruct_weakness_item _pweakness_item)
	{
		type = report_vul;
		pweakness_item = _pweakness_item;
	};
	report_type					type;
	int							high_count;
	int							medium_count;
	int							low_count;
	int							info_count;
	string						hostname;
	string						reportfile;
	pstruct_weakness_item		pweakness_item;
}struct_report_item, *pstruct_report_item;



// CWeakReportView 窗体视图

class CWeakReportView : public CFormView
{
	DECLARE_DYNCREATE(CWeakReportView)

public:
	CWeakReportView();           // 动态创建所使用的受保护的构造函数
	virtual ~CWeakReportView();

public:
	enum { IDD = IDD_WEAK_REPORT_VIEW };
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
	int AddReportItem(CString url, pstruct_scan_item pscan_item);

private:
	CWAEFHtmlView*	_htmlView;
	CTreeCtrl m_TreeCtrl;
	CImageList m_ImgList;
	bool _first;

public:
	virtual void OnInitialUpdate();
	afx_msg void OnSelchangedTreeReport(NMHDR *pNMHDR, LRESULT *pResult);
	
	CString	m_ReportPath;
	CString m_ReportTemplateA;
	CString m_ReportTemplateB;
	virtual BOOL PreTranslateMessage(MSG* pMsg);

private:
	void GenerateReport(pstruct_report_item preport_item);
public:
	afx_msg void OnBnClickedButton1();
};


