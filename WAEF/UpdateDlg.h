#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CUpdateDlg �Ի���

class CUpdateDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CUpdateDlg)

public:
	CUpdateDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CUpdateDlg();

// �Ի�������
	enum { IDD = IDD_UPDATE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CProgressCtrl m_UpdateProcess;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedUpdate();
	void ShowState(CString State);
	CStatic m_StateBox;
};
