// UpdateDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "WAEF.h"
#include "UpdateDlg.h"
#include "afxdialogex.h"


// CUpdateDlg 对话框

IMPLEMENT_DYNAMIC(CUpdateDlg, CDialogEx)

CUpdateDlg::CUpdateDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CUpdateDlg::IDD, pParent)
{

}

CUpdateDlg::~CUpdateDlg()
{
}

void CUpdateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS1, m_UpdateProcess);
	DDX_Control(pDX, IDC_STATIC_STATE, m_StateBox);
}


BEGIN_MESSAGE_MAP(CUpdateDlg, CDialogEx)
	ON_WM_TIMER()
	ON_BN_CLICKED(ID_UPDATE, &CUpdateDlg::OnBnClickedUpdate)
END_MESSAGE_MAP()


// CUpdateDlg 消息处理程序


void CUpdateDlg::OnBnClickedUpdate()
{
	// TODO: 在此添加控件通知处理程序代码
	m_UpdateProcess.SetRange(0, 100);
	m_UpdateProcess.SetStep(5);
	ShowState("正在连接更新服务器，请稍后...");
	::SetTimer(m_hWnd, 1, 100, 0);
}

void CUpdateDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_UpdateProcess.StepIt();
	switch(m_UpdateProcess.GetPos())
	{
	case 40:
		ShowState("连接更新服务器成功，正在检查插件是否更新");
		break;
	case 45:
		ShowState("检测到更新，正在更新插件");
		break;
	case 100:
		ShowState("更新完成，此次共更新2个模块和1个插件。");
		KillTimer(1);
		break;
	}
	CDialogEx::OnTimer(nIDEvent);
}

void CUpdateDlg::ShowState(CString State)
{
	m_StateBox.SetWindowText(State);
}
