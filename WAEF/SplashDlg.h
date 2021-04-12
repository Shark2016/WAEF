#pragma once
#include "afxcmn.h"


// CSplashDlg 对话框

class CSplashDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSplashDlg)

public:
	CSplashDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSplashDlg();

// 对话框数据
	enum { IDD = IDD_SPLASH_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CProgressCtrl m_SplashProcess;
};
