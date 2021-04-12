#pragma once
#include "afxcmn.h"

#include "CodeAuditMdl.h"


// CCodeAuditView ������ͼ

class CCodeAuditView : public CFormView
{
	DECLARE_DYNCREATE(CCodeAuditView)

public:
	CCodeAuditView();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��
	virtual ~CCodeAuditView();

public:
	enum { IDD = IDD_CODE_AUDIT_VIEW };
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
	virtual void OnInitialUpdate();
	afx_msg void OnBnClickedAuditLoadProject();
private:
	bool			m_First;
	CTreeCtrl		m_ProjectTree;
	CCodeAuditMdl	_codeAuditMdl;
public:
	afx_msg void OnBnClickedAuditStartAudit();
	CListCtrl m_ResultList;
	afx_msg void OnNMDblclkAuditResultList(NMHDR *pNMHDR, LRESULT *pResult);
};


