#pragma once
#include "afxcmn.h"


// CSplashDlg �Ի���

class CSplashDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSplashDlg)

public:
	CSplashDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSplashDlg();

// �Ի�������
	enum { IDD = IDD_SPLASH_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CProgressCtrl m_SplashProcess;
};
