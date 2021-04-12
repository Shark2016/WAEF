// WeakPluginView.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "WAEF.h"
#include "WeakPluginView.h"


// CWeakPluginView

IMPLEMENT_DYNCREATE(CWeakPluginView, CFormView)

CWeakPluginView::CWeakPluginView()
	: CFormView(CWeakPluginView::IDD)
	, m_TargetHost(_T(""))
	, m_First(true)
{

}

CWeakPluginView::~CWeakPluginView()
{
}

void CWeakPluginView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_PLUG_TARGET_HOST, m_TargetHost);
	DDX_Control(pDX, IDC_PLUG_PLUGIN_LIST, m_pluginList);
	DDX_Control(pDX, IDC_PLUG_INTERACTIVE_BOX, m_pluginInteractiveBox);
	DDX_Control(pDX, IDC_PLUG_RESULT, m_pluginResultList);
}

BEGIN_MESSAGE_MAP(CWeakPluginView, CFormView)
	ON_NOTIFY(NM_DBLCLK, IDC_PLUG_PLUGIN_LIST, &CWeakPluginView::OnDblclkPlugPluginList)
	ON_BN_CLICKED(IDC_PLUG_LOAD_DESC, &CWeakPluginView::OnBnClickedPlugLoadDesc)
	ON_BN_CLICKED(IDC_PLUG_PARAM_STR, &CWeakPluginView::OnBnClickedPlugParamStr)
	ON_BN_CLICKED(IDC_PLUG_EXECUTE, &CWeakPluginView::OnBnClickedPlugExecute)
END_MESSAGE_MAP()


// CWeakPluginView ���

#ifdef _DEBUG
void CWeakPluginView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CWeakPluginView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CWeakPluginView ��Ϣ�������

int CWeakPluginView::SetTargetHost(CString target)
{
	m_TargetHost = target;
	_weak_plugin_mdl.SetTarget(m_TargetHost);
	UpdateData(FALSE);
	return 0;
}

void CWeakPluginView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: �ڴ����ר�ô����/����û���
	if(m_First)
	{
		m_TargetHost="http://localhost";
		UpdateData(FALSE);

		
		m_pluginList.InsertColumn(0, "ID", LVCFMT_LEFT, 30);
		m_pluginList.InsertColumn(1, "�������", LVCFMT_LEFT, 100);
		m_pluginList.InsertColumn(2, "���·��", LVCFMT_LEFT, 240);
		m_pluginList.SetExtendedStyle(LVS_EX_FULLROWSELECT);

		m_pluginResultList.InsertColumn(0, "ID", LVCFMT_LEFT, 30);
		m_pluginResultList.InsertColumn(1, "���", LVCFMT_LEFT, 300);
		m_pluginResultList.SetExtendedStyle(LVS_EX_FULLROWSELECT);

		_weak_plugin_mdl.InitModule(&m_pluginList, &m_pluginInteractiveBox, &m_pluginResultList);
		_weak_plugin_mdl.SetTarget(m_TargetHost);

		m_First = false;
	}
}


void CWeakPluginView::OnDblclkPlugPluginList(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	// TODO: �ڴ���ӿؼ�֪ͨ����������
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	_weak_plugin_mdl.LoadWAEFPlugin(phdr->iItem);
	*pResult = 0;
}


void CWeakPluginView::OnBnClickedPlugLoadDesc()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	_weak_plugin_mdl.ExecutePlugin();
}


void CWeakPluginView::OnBnClickedPlugParamStr()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	_weak_plugin_mdl.LoadPluginConfig();
}


void CWeakPluginView::OnBnClickedPlugExecute()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	_weak_plugin_mdl.ExecutePlugin();
}


BOOL CWeakPluginView::PreTranslateMessage(MSG* pMsg)
{
	if( IsDialogMessage( pMsg ) )
		return TRUE;
	else
		return CFormView::PreTranslateMessage(pMsg);
}
