#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CUpdateDlg 对话框

class CUpdateDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CUpdateDlg)

public:
	CUpdateDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CUpdateDlg();

// 对话框数据
	enum { IDD = IDD_UPDATE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CProgressCtrl m_UpdateProcess;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedUpdate();
	void ShowState(CString State);
	CStatic m_StateBox;
};
