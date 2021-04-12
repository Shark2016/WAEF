// UpdateDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "WAEF.h"
#include "UpdateDlg.h"
#include "afxdialogex.h"


// CUpdateDlg �Ի���

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


// CUpdateDlg ��Ϣ�������


void CUpdateDlg::OnBnClickedUpdate()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_UpdateProcess.SetRange(0, 100);
	m_UpdateProcess.SetStep(5);
	ShowState("�������Ӹ��·����������Ժ�...");
	::SetTimer(m_hWnd, 1, 100, 0);
}

void CUpdateDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	m_UpdateProcess.StepIt();
	switch(m_UpdateProcess.GetPos())
	{
	case 40:
		ShowState("���Ӹ��·������ɹ������ڼ�����Ƿ����");
		break;
	case 45:
		ShowState("��⵽���£����ڸ��²��");
		break;
	case 100:
		ShowState("������ɣ��˴ι�����2��ģ���1�������");
		KillTimer(1);
		break;
	}
	CDialogEx::OnTimer(nIDEvent);
}

void CUpdateDlg::ShowState(CString State)
{
	m_StateBox.SetWindowText(State);
}
