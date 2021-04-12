// SplashDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "WAEF.h"
#include "SplashDlg.h"
#include "afxdialogex.h"


// CSplashDlg 对话框

IMPLEMENT_DYNAMIC(CSplashDlg, CDialogEx)

CSplashDlg::CSplashDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSplashDlg::IDD, pParent)
{

}

CSplashDlg::~CSplashDlg()
{
}

void CSplashDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SPLASH_PROGRESS, m_SplashProcess);
}


BEGIN_MESSAGE_MAP(CSplashDlg, CDialogEx)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CSplashDlg 消息处理程序


BOOL CSplashDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_SplashProcess.SetRange(0, 100);
	m_SplashProcess.SetStep(5);
	::SetTimer(m_hWnd, 0, 100, 0);
	SetWindowPos(&wndTop, 0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	return TRUE;  // return TRUE unless you set the focus to a control
}


void CSplashDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_SplashProcess.StepIt();
	if(100 == m_SplashProcess.GetPos())
	{
		SendMessage(WM_CLOSE);
	}
	CDialogEx::OnTimer(nIDEvent);
}
