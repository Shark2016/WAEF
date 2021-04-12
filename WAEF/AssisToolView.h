#pragma once

#include "AssisToolMdl.h"
#include "afxwin.h"

// CAssisToolView 窗体视图

class CAssisToolView : public CFormView
{
	DECLARE_DYNCREATE(CAssisToolView)

public:
	CAssisToolView();           // 动态创建所使用的受保护的构造函数
	virtual ~CAssisToolView();

public:
	enum { IDD = IDD_ASSIS_TOOL_VIEW };
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

private:	
	CAssisToolMdl	_assis_tool_mdl;

	CComboBox m_ConvertType;
	CString m_Src;
	CString m_Dst;

	bool		m_First;

public:
	afx_msg void OnSelChangeAssisConvertType();
	afx_msg void OnChangeAssisSrcString();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};


